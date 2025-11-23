#pragma once

#include "../FileSystem/FileSystemModel.h"

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

    class MainFileSystemModel : public QObject {
        Q_OBJECT

    public:
        explicit MainFileSystemModel(std::shared_ptr<::FileSystemModel> model);
        ~MainFileSystemModel() override;

        Q_INVOKABLE std::uint32_t requestFullData(const QString& path, bool recursive);
        Q_INVOKABLE std::uint32_t requestBasicData(const QString& path, bool recursive);
        Q_INVOKABLE void remove(std::uint32_t file_system_id);
        Q_INVOKABLE void abortRequest(std::uint32_t file_system_id);
        Q_INVOKABLE void abortAllRequests();
        Q_INVOKABLE QString getCurrentPath(std::uint32_t file_system_id) const;
        Q_INVOKABLE void count(std::uint32_t file_system_id, FileSystemInfo* source) const;

    signals:
        void maxProgressEnabled(bool enabled);
        void progressChanged(float value);
        void maxProgressChanged(float max);
        void progressTextChanged(const QString& text);
        void errorOccurred(std::uint32_t file_system_id, const QString& text, bool is_critical);
        void ready(std::uint32_t file_system_id);

    private:
        void handle_error(std::uint32_t file_system_id, ::FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error, bool is_critical);

    private:
        std::shared_ptr<::FileSystemModel> _fs_model;
    };
}
