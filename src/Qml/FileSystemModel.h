#pragma once

#include <cstdint>
#include <memory>

#include <QObject>
#include <QString>

class FileSystemModel;

namespace Qml {
    class FileSystemModel : public QObject {
        Q_OBJECT

    public:
        FileSystemModel(std::shared_ptr<::FileSystemModel> model);

        Q_INVOKABLE void setServerInfo(const QString& addr, const uint16_t port, const QString& path);
        Q_INVOKABLE void requestFileList();
        Q_INVOKABLE void stop();

    signals:
        void maxProgressEnabled(const bool enabled);
        void progressChanged(const float value);
        void maxProgressChanged(const float max);
        void progressTextChanged(const QString& text);
        void error_occurred();

    private:
        std::shared_ptr<::FileSystemModel> _fs_model;
    };
}
