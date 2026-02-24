#include "FileSortFilterItemModel.h"

#include <Common/QtUtil.h>
#include <Common/Util.h>

#include "../../Settings/Settings.h"
#include "../FileItemModel/FileItemModel.h"
#include "../FileItemModel/Role.h"
#include "SortParam.h"

using namespace Qml;

namespace Qml { using Role = FileItemModelRole; }

FileSortFilterItemModel::FileSortFilterItemModel(std::shared_ptr<::Settings> settings, std::unique_ptr<FileItemModel, QScopedPointerDeleteLater>&& source, QObject* parent)
    : QSortFilterProxyModel(parent), _settings(std::move(settings)), _source(std::move(source)), _params(_settings->get_sort_params()), _case_sensitive(_settings->get_filter_cs_flag())
{
    qDebug().noquote().nospace() << QObject::tr("The file sort filter item model is being created");
    _params = _settings->get_sort_params();
    setSourceModel(_source.get());
    sort(0);
    _timer.setSingleShot(true);
    _settings->add_notif_func_about_change(Settings::Sorting::Sequence, this, [this]() { update_sequence(); });
    _settings->add_notif_func_about_change(Settings::Filter::CaseSensitive, this, [this]() { apply_case_sensitivity(); });
}

FileSortFilterItemModel::~FileSortFilterItemModel() {
    qDebug().noquote().nospace() << QObject::tr("The file sort filter item model is being destroyed");
    setSourceModel(nullptr);
    _settings->remove_notif_func_about_change(Settings::Filter::CaseSensitive, this);
    _settings->remove_notif_func_about_change(Settings::Sorting::Sequence, this);
}

void FileSortFilterItemModel::filter(const QString& text) {
    if (_text == text)
        return;

    auto update = [this, text]() {
        beginFilterChange(); // note: Calling this code directly from QML results in invocation lessThan() with invalid QModelIndex'es
        _text = text;
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
    };
    QTimer::singleShot(0, std::move(update));
}

void FileSortFilterItemModel::filterWithTimer(const QString& text) {
    if (_text == text)
        return;

    _timer.stop();
    _timer.disconnect();
    connect(&_timer, &QTimer::timeout, [this, text]() { filter(text); });
    _timer.start(600);
}

bool FileSortFilterItemModel::areAllItemsCheckedToDownload() const {
    const auto start = std::chrono::steady_clock::now();
    const auto sz = rowCount();
    for (auto i = sz > 0 && index(0, 0).data(to_int(Role::IsUpDirRow)).toBool() ? 1 : 0; i < sz; ++i) {
        const QModelIndex index = this->index(i, 0);
        const QVariant data = index.data(to_int(Role::IsReadyToDownload));
        if (!data.toBool()) {
            log_duration(QObject::tr("areAllItemsCheckedToDownload: duration: "), start, std::chrono::steady_clock::now());
            return false;
        }
    }
    log_duration(QObject::tr("areAllItemsCheckedToDownload: duration: "), start, std::chrono::steady_clock::now());
    return true;
}

void FileSortFilterItemModel::checkAllToDownloadItems(bool check) {
    const auto start = std::chrono::steady_clock::now();
    const auto sz = rowCount();
    for (auto i = sz > 0 && index(0, 0).data(to_int(Role::IsUpDirRow)).toBool() ? 1 : 0; i < sz; ++i) {
        const QModelIndex index = this->index(i, 0);
        setData(index, check, to_int(Role::IsReadyToDownload));
    }
    log_duration(QObject::tr("checkAllToDownloadItems: duration: "), start, std::chrono::steady_clock::now());
}

bool FileSortFilterItemModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    if (_text.isEmpty())
        return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    assert(index.data(to_int(Role::IsUpDirRow)).canConvert<bool>());
    if (index.data(to_int(Role::IsUpDirRow)).toBool())
        return false;

    assert(index.data(to_int(Role::Name)).canConvert<QString>());
    const auto name = index.data(to_int(Role::Name)).toString();
    return name.indexOf(_text, 0, _case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) != -1;
}

bool FileSortFilterItemModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const {
    const QVariant left_data = source_left.data(to_int(Role::IsUpDirRow));
    const QVariant right_data = source_right.data(to_int(Role::IsUpDirRow));
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

void FileSortFilterItemModel::update_sequence() {
    _params = _settings->get_sort_params();
    invalidate();
}

void FileSortFilterItemModel::apply_case_sensitivity() {
    beginFilterChange();
    _case_sensitive = _settings->get_filter_cs_flag();
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}
