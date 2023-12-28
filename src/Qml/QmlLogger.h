#pragma once

#include <memory>

#include <QObject>
#include <QString>

class Logger;

class QmlLogger : public QObject {
    Q_OBJECT

public:
    explicit QmlLogger(std::shared_ptr<Logger> logger, QObject* parent = nullptr);
    ~QmlLogger();

    Q_INVOKABLE QString getLog() const;
    void emit_signal(const QString& msg);

signals:
    void msgReceived(const QString& msg);

private:
    std::shared_ptr<Logger> _logger;
};
