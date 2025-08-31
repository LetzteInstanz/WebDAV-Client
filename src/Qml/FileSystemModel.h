#pragma once

#include "../FileSystem/FileSystemModel.h"

namespace Qml {
    class FileSystemModel : public QObject {
        Q_OBJECT

    public:
        explicit FileSystemModel(std::shared_ptr<::FileSystemModel> model);
        ~FileSystemModel() override;

        Q_INVOKABLE void requestFileList(const QString& path);
        Q_INVOKABLE void abortRequest();
        Q_INVOKABLE void disconnect();
        Q_INVOKABLE QString getCurrentPath() const;

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
