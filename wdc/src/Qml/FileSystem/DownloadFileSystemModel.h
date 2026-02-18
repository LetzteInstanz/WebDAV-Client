#pragma once

#include "../FileSystem/FileSystemModel.h"
#include "AbstractFileSystemModel.h"

class FileSystemModel;

namespace Qml {
    class FileSystemInfo : public QObject {
        Q_OBJECT
        Q_PROPERTY(std::size_t directoryCount MEMBER directoryCount)
        Q_PROPERTY(std::size_t fileCount MEMBER fileCount)
        Q_PROPERTY(std::uint64_t size MEMBER size)
        Q_PROPERTY(bool areValidAllSizes MEMBER areValidAllSizes)
        Q_PROPERTY(bool areInvalidAllSizes MEMBER areInvalidAllSizes)

    public:
        using QObject::QObject;
        ~FileSystemInfo() override;

        std::size_t directoryCount = 0;
        std::size_t fileCount = 0;
        std::uint64_t size = 0;
        bool areValidAllSizes = true;
        bool areInvalidAllSizes = true;

        Q_INVOKABLE QString getSizeStr() const;
    };

    class DownloadFileSystemModel : public AbstractFileSystemModel {
        Q_OBJECT

    public:
        explicit DownloadFileSystemModel(std::shared_ptr<FileSystemModel> model);
        ~DownloadFileSystemModel() override;

        Q_INVOKABLE void remove(FileSystemModel::Id id);
        Q_INVOKABLE void count(FileSystemModel::Id id, FileSystemInfo* source) const;
    };
}
