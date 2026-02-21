#pragma once

#include <FileSystem/FileSystemModel.h>

#include "AbstractFileSystemModel.h"
#include "FileSystemInfo.h" // note: Forward declaration breaks integration with QML

namespace Qml {
    class DownloadFileSystemModel : public AbstractFileSystemModel {
        Q_OBJECT

    public:
        explicit DownloadFileSystemModel(std::shared_ptr<FileSystemModel> model);
        ~DownloadFileSystemModel() override;

        Q_INVOKABLE void remove(FileSystemModel::Id id);
        Q_INVOKABLE void count(FileSystemModel::Id id, FileSystemInfo* source) const;
    };
}
