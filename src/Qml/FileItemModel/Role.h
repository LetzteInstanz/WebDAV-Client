#pragma once

#include <Qt>

namespace Qml {
    enum class FileItemModelRole {Name = Qt::UserRole, Extension, IconName, CreationTime, CreationTimeStr, ModTime, ModTimeStr, FileFlag, IsExit, Size};
}
