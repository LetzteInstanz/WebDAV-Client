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
    using Message = std::pair<QtMsgType, QString>;
    using Log = std::vector<Message>;
    using NotificationFunc = std::function<void(QtMsgType, const QString&)>;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static std::shared_ptr<Logger> get_instance();
    QtMsgType get_max_level() const { return _max_level.load(std::memory_order::relaxed); }
    void set_max_level(QtMsgType level);
    Log get_log() const;
    void append_msg(QtMsgType type, const QString& msg);
    void set_notification_func(NotificationFunc&& func);

private:
    Logger();

    static void message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    static QtMessageHandler _default_handler;
    bool _filtered = false;
    mutable bool _enable_signal = false;
    std::atomic<QtMsgType> _max_level = QtDebugMsg;
    Log _log;
    mutable std::mutex _mutex;
    NotificationFunc _notification_func;
};
