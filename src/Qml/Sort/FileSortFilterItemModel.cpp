#include "FileSortFilterItemModel.h"

#include "../../Json/SettingsJsonFile.h"
#include "../FileItemModel/FileItemModel.h"
#include "../FileItemModel/Role.h"
#include "../Util.h"
#include "SortParam.h"

using namespace Qml;

FileSortFilterItemModel::FileSortFilterItemModel(std::shared_ptr<SettingsJsonFile> settings, std::unique_ptr<FileItemModel, QScopedPointerDeleteLater>&& source, QObject* parent)
    : QSortFilterProxyModel(parent), _settings(std::move(settings)), _source(std::move(source))
{
    qDebug().noquote() << QObject::tr("The file sort filter item model is being created");
    setSourceModel(_source.get());
    sort(0);
}

FileSortFilterItemModel::~FileSortFilterItemModel() {
    qDebug().noquote() << QObject::tr("The file sort filter item model is being destroyed");
    setSourceModel(nullptr);
}

bool FileSortFilterItemModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const {
    using Role = FileItemModelRole;
    const QVariant left_data = source_left.data(to_int(Role::IsExit));
    assert(left_data.canConvert<bool>());
    assert(source_right.data(to_int(Role::IsExit)).canConvert<bool>());
    const auto left_is_exit = left_data.toBool();
    if (left_is_exit || source_right.data(to_int(Role::IsExit)).toBool())
        return left_is_exit;

    const std::vector<SortParam> params = _settings->get_sort_params();
    assert(!params.empty());
    for (const auto& param : params) {
        const QVariant left_data = source_left.data(to_int(param.role));
        const QVariant right_data = source_right.data(to_int(param.role));
        assert(param.comp_func);
        const SortParam::CompResult result = param.comp_func(left_data, right_data, param.descending);
        if (result != SortParam::CompResult::Equal)
            return result == SortParam::CompResult::Less;
    }
    return false;
}
