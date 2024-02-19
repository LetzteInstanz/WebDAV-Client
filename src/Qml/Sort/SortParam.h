#pragma once

#include <functional>

#include <QString>
#include <QVariant>

namespace Qml {
    enum class FileItemModelRole;

    struct SortParam {
        enum class CompResult {Less, Equal, Greater};

        FileItemModelRole role;
        QString description;
        bool descending;
        std::function<CompResult(const QVariant&, const QVariant&, bool)> comp_func;

        bool operator==(const SortParam& rhs) const noexcept;

        static CompResult compare_file_flag(const QVariant& lhs, const QVariant& rhs, bool descending);
        static CompResult compare_qstring(const QVariant& lhs, const QVariant& rhs, bool descending);
        static CompResult compare_extension(const QVariant& lhs, const QVariant& rhs, bool descending);
        static CompResult compare_time_t(const QVariant& lhs, const QVariant& rhs, bool descending);
        static CompResult compare_uint64_t(const QVariant& lhs, const QVariant& rhs, bool descending);
    };
}
