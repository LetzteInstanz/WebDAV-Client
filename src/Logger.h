#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>

#include <QString>
#include <QtLogging>

class Logger {
public:
    using Message = std::pair<QtMsgType, QString>;
    using Log = std::deque<Message>;
    using NotificationFunc = std::function<void (Message&&)>;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static std::shared_ptr<Logger> get_instance();
    void install_handler();
    QtMsgType get_max_level() const noexcept;
    void set_max_level(QtMsgType level);
    Log get_log();
    void append_message(Message&& msg);
    void set_notification_func(NotificationFunc&& func);

private:
    Logger();

    static void message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
#ifndef NDEBUG
    static QtMessageHandler _default_handler;
#endif
    bool _filtered = false;
    std::atomic<QtMsgType> _max_level = QtDebugMsg;
    std::mutex _mutex;
    bool _enable_func = false;
    Log _log;
    NotificationFunc _notification_func;
};
