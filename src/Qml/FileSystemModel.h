#pragma once

#include <cstdint>
#include <memory>

#include <QNetworkReply>
#include <QObject>
#include <QString>

#include "../FileSystem/FileSystemModel.h"

namespace Qml {
    class FileSystemModel : public QObject {
        Q_OBJECT

    public:
        explicit FileSystemModel(std::shared_ptr<::FileSystemModel> model);
        ~FileSystemModel();

        Q_INVOKABLE void setServerInfo(const QString& addr, uint16_t port);
        Q_INVOKABLE void setRootPath(const QString& absolute_path);
        Q_INVOKABLE void requestFileList(const QString& relative_path);
        Q_INVOKABLE void stop();

    signals:
        void maxProgressEnabled(bool enabled);
        void progressChanged(float value);
        void maxProgressChanged(float max);
        void progressTextChanged(const QString& text);
        void errorOccurred(const QString& text);
        void replyGot();

    private:
        void handle_error(::FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error);

    private:
        std::shared_ptr<::FileSystemModel> _fs_model;
    };
}
