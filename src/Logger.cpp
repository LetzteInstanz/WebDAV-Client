#include "Logger.h"

#include "Util.h"
#include "pch.h"

namespace {
    bool operator>(QtMsgType lhs, QtMsgType rhs) {
        if (lhs != QtInfoMsg && rhs != QtInfoMsg)
            return to_int(lhs) > to_int(rhs);

        return lhs == QtInfoMsg && rhs == QtDebugMsg;
    }

    bool operator>=(QtMsgType lhs, QtMsgType rhs) { return lhs == rhs || lhs > rhs; }

    bool operator<(QtMsgType lhs, QtMsgType rhs) { return !(lhs >= rhs); }
}

#ifndef NDEBUG
QtMessageHandler Logger::_default_handler = nullptr;
#endif

Logger::Logger() = default;

void Logger::message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
#ifndef NDEBUG
    _default_handler(type, context, msg);
#endif
    static auto logger = Logger::get_instance();
    if (type >= logger->get_max_level())
        logger->append_message(std::make_pair(type, msg));
}

std::shared_ptr<Logger> Logger::get_instance() {
#ifdef __cpp_lib_atomic_shared_ptr
    static std::atomic<std::shared_ptr<Logger>> logger(std::shared_ptr<Logger>(new Logger()));
    return logger.load(std::memory_order::relaxed);
#else
    static std::shared_ptr<Logger> logger(new Logger());
    return std::atomic_load_explicit(&logger, std::memory_order::relaxed);
#endif
}

void Logger::install_handler() {
#ifndef NDEBUG
    _default_handler = qInstallMessageHandler(&Logger::message_handler);
#else
    qInstallMessageHandler(&Logger::message_handler);
#endif
}

QtMsgType Logger::get_max_level() const noexcept { return _max_level.load(std::memory_order::relaxed); }

void Logger::set_max_level(QtMsgType level) {
    const auto old = _max_level.load(std::memory_order::relaxed);
    if (old == level)
        return;

    _max_level.store(level, std::memory_order::relaxed);
    if (_filtered || old > level)
        return;

    {
        const std::lock_guard<std::mutex> locker(_mutex);
        const auto end = std::end(_log);
        const auto it = std::remove_if(std::begin(_log), end, [level](const std::pair<QtMsgType, QString>& pair) { return pair.first < level; });
        _log.erase(it, end);
    }
    _filtered = true;
}

Logger::Log Logger::get_log() {
    const std::lock_guard<std::mutex> locker(_mutex);
    _enable_func = _notification_func != nullptr;
    return _log;
}

void Logger::append_message(Message&& msg) {
    const std::lock_guard<std::mutex> locker(_mutex);
    if (msg.first < get_max_level()) // note: The value of _max_level may change in the main thread between calls get_max_level() and append_msg() in message_handler() in another thread
        return;

    _log.emplace_back(msg);
    if (_enable_func)
        _notification_func(std::move(msg));
}

void Logger::set_notification_func(NotificationFunc&& func) {
    const std::lock_guard<std::mutex> locker(_mutex);
    _enable_func = false;
    _notification_func = std::move(func);
}
