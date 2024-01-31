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

        Q_INVOKABLE void setServerInfo(const QString& addr, const uint16_t port, const QString& path);
        Q_INVOKABLE void requestFileList();
        Q_INVOKABLE void stop();

    signals:
        void maxProgressEnabled(const bool enabled);
        void progressChanged(const float value);
        void maxProgressChanged(const float max);
        void progressTextChanged(const QString& text);
        void errorOccurred(const QString& text);
        void replyGot();

    private:
        void handle_error(::FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error);

    private:
        std::shared_ptr<::FileSystemModel> _fs_model;
    };
}
