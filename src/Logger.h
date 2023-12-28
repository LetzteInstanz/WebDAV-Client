#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include <QString>
#include <QtLogging>

class Logger {
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static std::shared_ptr<Logger> get_instance();
    QtMsgType get_max_level() const { return _max_level.load(std::memory_order::relaxed); }
    void set_max_level(const QtMsgType level);
    QString get_log() const;
    void append_msg(const QtMsgType type, const QString& msg);
    void set_notification_func(const std::function<void(const QString&)> func);

private:
    Logger() noexcept;

    static void message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    static QtMessageHandler _default_handler;
    bool _filtered = false;
    std::atomic<QtMsgType> _max_level = QtDebugMsg;
    std::vector<std::pair<QtMsgType, QString>> _log;
    mutable std::mutex _mutex;
    std::function<void(const QString&)> _notification_func;
};
