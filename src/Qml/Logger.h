#pragma once

#include <memory>
#include <unordered_map>

#include <QChar>
#include <QObject>
#include <QString>
#include <QtLogging>

class Logger;

namespace Qml {
    class Logger : public QObject {
        Q_OBJECT

    public:
        explicit Logger(std::shared_ptr<::Logger> logger, QObject* parent = nullptr);
        ~Logger() override;

        Q_INVOKABLE QString getLog() const;

    signals:
        void msgReceived(const QString& msg);

    private:
        static QString escape(const QString& text);
        static QString wrap(QtMsgType type, QString&& msg);
        void emit_signal(QtMsgType type, const QString& msg);

    private:
        static const std::unordered_map<QChar, QString> _escaped_by_char_map;
        std::shared_ptr<::Logger> _logger;
    };
}
