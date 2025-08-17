#include "FileSortFilterItemModel.h"

#include "../../Settings/Settings.h"
#include "../../Util.h"
#include "../FileItemModel/FileItemModel.h"
#include "../FileItemModel/Role.h"
#include "SortParam.h"

using namespace Qml;

namespace Qml {
    using Role = FileItemModelRole;
}

FileSortFilterItemModel::FileSortFilterItemModel(std::shared_ptr<::Settings> settings, std::unique_ptr<FileItemModel, QScopedPointerDeleteLater>&& source, QObject* parent)
    : QSortFilterProxyModel(parent), _settings(std::move(settings)), _source(std::move(source))
{
    qDebug().noquote() << QObject::tr("The file sort filter item model is being created");
    _settings->set_sort_param_changed_notif_func([this]() { update(); });
    _params = _settings->get_sort_params();
    setSourceModel(_source.get());
    sort(0);
    _timer.setSingleShot(true);
    const auto search = [this]() {
        _case_sensitive = _settings->get_search_cs_flag();
        invalidateRowsFilter();
    };
    connect(&_timer, &QTimer::timeout, this, search);
}

FileSortFilterItemModel::~FileSortFilterItemModel() {
    qDebug().noquote() << QObject::tr("The file sort filter item model is being destroyed");
    setSourceModel(nullptr);
    _settings->set_sort_param_changed_notif_func(nullptr);
}

void FileSortFilterItemModel::search(const QString& text) {
    if (_text == text)
        return;

    _text = text;
    repeatSearch(0);
}

void FileSortFilterItemModel::searchWithTimer(const QString& text) {
    if (_text == text)
        return;

    _text = text;
    repeatSearch(600);
}

void FileSortFilterItemModel::repeatSearch(int msec) {
    _timer.stop();
    _timer.start(msec);
}

bool FileSortFilterItemModel::areAllItemsCheckedToDownload() const {
    const auto sz = rowCount();
    for (auto i = sz > 0 && index(0, 0).data(to_int(Role::IsExit)).toBool() ? 1 : 0; i < sz; ++i) {
        const QModelIndex index = this->index(i, 0);
        const QVariant data = index.data(to_int(Role::IsReadyToDownload));
        if (!data.toBool())
            return false;
    }
    return true;
}

void FileSortFilterItemModel::checkAllToDownloadItems(bool check) {
    const auto sz = rowCount();
    for (auto i = sz > 0 && index(0, 0).data(to_int(Role::IsExit)).toBool() ? 1 : 0; i < sz; ++i) {
        const QModelIndex index = this->index(i, 0);
        setData(index, check, to_int(Role::IsReadyToDownload));
    }
}

bool FileSortFilterItemModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    if (_text.isEmpty())
        return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    auto role = to_int(Role::IsExit);
    assert(index.data(role).canConvert<bool>());
    if (index.data(role).toBool())
        return false;

    role = to_int(Role::Name);
    assert(index.data(role).canConvert<QString>());
    const auto name = index.data(to_int(Role::Name)).toString();
    return name.indexOf(_text, 0, _case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) != -1;
}

bool FileSortFilterItemModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const {
    const QVariant left_data = source_left.data(to_int(Role::IsExit));
    const QVariant right_data = source_right.data(to_int(Role::IsExit));
    assert(left_data.canConvert<bool>());
    assert(right_data.canConvert<bool>());
    const auto left_is_exit = left_data.toBool();
    if (left_is_exit || right_data.toBool())
        return left_is_exit;

    assert(!_params.empty());
    for (const auto& param : _params) {
        const QVariant left_data = source_left.data(to_int(param.role));
        const QVariant right_data = source_right.data(to_int(param.role));
        assert(param.comp_func);
        const SortParam::CompResult result = param.comp_func(left_data, right_data, param.descending);
        if (result != SortParam::CompResult::Equal)
            return result == SortParam::CompResult::Less;
    }
    return false;
}

void FileSortFilterItemModel::update() {
    _params = _settings->get_sort_params();
    invalidate();
}
