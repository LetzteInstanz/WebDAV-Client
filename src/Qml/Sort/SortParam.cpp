#include "SortParam.h"

using namespace Qml;

namespace {
    constexpr SortParam::CompResult left_is_less_if_ascending(bool left_is_less, bool descending) {
        if (descending)
            return left_is_less ? SortParam::CompResult::Greater : SortParam::CompResult::Less;
        else
            return left_is_less ? SortParam::CompResult::Less : SortParam::CompResult::Greater;
    }
}

bool SortParam::operator==(const SortParam& rhs) const noexcept { return role == rhs.role && descending == rhs.descending; }

SortParam::CompResult SortParam::compare_file_flag(const QVariant& lhs, const QVariant& rhs, bool descending) {
    assert(lhs.canConvert<bool>());
    assert(rhs.canConvert<bool>());
    const auto left_is_file = lhs.toBool();
    const auto right_is_file = rhs.toBool();
    return left_is_file == right_is_file ? CompResult::Equal : left_is_less_if_ascending(right_is_file, descending);
}

SortParam::CompResult SortParam::compare_qstring(const QVariant& lhs, const QVariant& rhs, bool descending) {
    assert(lhs.canConvert<QString>());
    assert(rhs.canConvert<QString>());
    const int result = QString::compare(lhs.toString(), rhs.toString());
    return result == 0 ? CompResult::Equal : left_is_less_if_ascending(result < 0, descending);
}

SortParam::CompResult SortParam::compare_extension(const QVariant& lhs, const QVariant& rhs, bool descending) {
    const auto left_is_null = lhs.isNull();
    const auto right_is_null = rhs.isNull();
    if (left_is_null || right_is_null)
        return left_is_null == right_is_null ? CompResult::Equal : left_is_less_if_ascending(!left_is_null, descending);

    return compare_qstring(lhs, rhs, descending);
}

SortParam::CompResult SortParam::compare_time_t(const QVariant& lhs, const QVariant& rhs, bool descending) {
    assert(lhs.canConvert<time_t>());
    assert(rhs.canConvert<time_t>());
    const auto left_time = lhs.value<time_t>();
    const auto right_time = rhs.value<time_t>();
    const auto left_is_lowest = left_time == std::numeric_limits<time_t>::lowest();
    const auto right_is_lowest = right_time == std::numeric_limits<time_t>::lowest();
    if (left_is_lowest || right_is_lowest)
        return left_is_lowest == right_is_lowest ? CompResult::Equal : left_is_less_if_ascending(!left_is_lowest, descending);

    const double diff = std::difftime(right_time, left_time);
    return diff == 0 ? CompResult::Equal : left_is_less_if_ascending(diff > 0, descending);
}
