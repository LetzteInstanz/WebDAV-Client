#pragma once

namespace Qml {
    Q_NAMESPACE

    enum class FileItemModelRole {Name = Qt::UserRole, Extension, IconName, WideImageWidthFlag, CreationTime, CreationTimeStr, ModTime, ModTimeStr, FileFlag, IsExit, Size, SizeStr, IsReadyToDownload, Path, EnumSize};
    Q_ENUM_NS(FileItemModelRole)
}
