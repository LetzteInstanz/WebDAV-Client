#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QtLogging>

class Logger;

namespace Qml {
    class Logger : public QObject {
        Q_OBJECT

    public:
        explicit Logger(std::shared_ptr<::Logger> logger, QObject* parent = nullptr);
        ~Logger();

        Q_INVOKABLE QString getLog() const;

    signals:
        void msgReceived(const QString& msg);

    private:
        static QString wrap(QtMsgType type, QString&& msg);
        void emit_signal(QtMsgType type, const QString& msg);

    private:
        std::shared_ptr<::Logger> _logger;
    };
}
