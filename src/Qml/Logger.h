#pragma once

#include <memory>

#include <QObject>
#include <QString>

class Logger;

namespace Qml {
    class Logger : public QObject {
        Q_OBJECT

    public:
        explicit Logger(std::shared_ptr<::Logger> logger, QObject* parent = nullptr);
        ~Logger();

        Q_INVOKABLE QString getLog() const;
        void emit_signal(const QString& msg);

    signals:
        void msgReceived(const QString& msg);

    private:
        std::shared_ptr<::Logger> _logger;
    };
}
