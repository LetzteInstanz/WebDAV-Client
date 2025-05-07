#include "SortParamItemModel.h"

#include "../Json/SettingsJsonFile.h"
#include "../Util.h"
#include "SortParam.h"

using namespace Qml;

SortParamItemModel::SortParamItemModel(std::shared_ptr<SettingsJsonFile> settings, QObject* parent) : QAbstractListModel(parent), _settings(std::move(settings)) {
    qDebug().noquote() << QObject::tr("The sort parameter item model is being created");
    resetChanges();
}

SortParamItemModel::~SortParamItemModel() { qDebug().noquote() << QObject::tr("The sort parameter item model is being destroyed"); }

int SortParamItemModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : _data.size(); }

QVariant SortParamItemModel::data(const QModelIndex& index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return _data[index.row()].description;

        case to_int(Role::Descending):
            return _data[index.row()].descending;
    }
    return QVariant();
}

bool SortParamItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role != to_int(Role::Descending))
        return false;

    SortParam& param = _data[index.row()];
    assert(value.canConvert<bool>());
    const auto new_value = value.toBool();
    if (param.descending == new_value)
        return false;

    param.descending = new_value;
    dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> SortParamItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Descending), "descending");
    return names;
}

bool SortParamItemModel::hasChanges() const { return _data != _settings->get_sort_params(); }

void SortParamItemModel::moveUp(int row) {
    assert(row > 0);
    const auto higher = row - 1;
    std::swap(_data[higher], _data[row]);
    dataChanged(index(higher, 0), index(row, 0), {Qt::DisplayRole, to_int(Role::Descending)});
}

void SortParamItemModel::invert() {
    const std::size_t true_sz = _data.size();
    const auto sz = true_sz / 2;
    if (sz == 0)
        return;

    for (std::size_t i = 0; i < sz; ++i)
        std::swap(_data[i], _data[true_sz - 1 - i]);

    dataChanged(index(0, 0), index(rowCount() - 1, 0), {Qt::DisplayRole, to_int(Role::Descending)});
}

void SortParamItemModel::moveDown(int row) {
    assert(row < _data.size() - 1);
    const auto lower = row + 1;
    std::swap(_data[lower], _data[row]);
    dataChanged(index(row, 0), index(lower, 0), {Qt::DisplayRole, to_int(Role::Descending)});
}

void SortParamItemModel::save() { _settings->set_sort_params(_data); }

void SortParamItemModel::resetChanges() {
    beginResetModel();
    _data = _settings->get_sort_params();
    endResetModel();
}
