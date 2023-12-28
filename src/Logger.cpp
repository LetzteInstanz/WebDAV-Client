#include "Logger.h"

namespace {
    bool operator>(QtMsgType lhs, QtMsgType rhs) {
        if (lhs != QtInfoMsg && rhs != QtInfoMsg)
            return static_cast<int>(lhs) > static_cast<int>(rhs);

        return lhs == QtInfoMsg && rhs == QtDebugMsg;
    }

    bool operator>=(QtMsgType lhs, QtMsgType rhs) { return lhs == rhs || lhs > rhs; }

    bool operator<(QtMsgType lhs, QtMsgType rhs) { return !(lhs >= rhs); }
}

QtMessageHandler Logger::_default_handler = nullptr;

Logger::Logger() noexcept { _default_handler = qInstallMessageHandler(&Logger::message_handler); }

void Logger::message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    _default_handler(type, context, msg);
    std::shared_ptr<Logger> logger = Logger::get_instance();
    if (type >= logger->get_max_level())
        logger->append_msg(type, msg);
}

std::shared_ptr<Logger> Logger::get_instance() {
    static std::atomic<std::shared_ptr<Logger>> logger(std::shared_ptr<Logger>(new Logger()));
    return logger.load(std::memory_order::relaxed);
}

void Logger::set_max_level(const QtMsgType level) {
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

QString Logger::get_log() const {
    QString log;
    {
        const std::lock_guard<std::mutex> locker(_mutex);
        for (const std::pair<QtMsgType, QString>& pair : _log)
            log.append(pair.second);
    }
    return log;
}

void Logger::append_msg(const QtMsgType type, const QString& msg) {
    const std::lock_guard<std::mutex> locker(_mutex);
    if (type < get_max_level()) // note: The value of _max_level may change in the main thread between calls get_max_level() and append_msg() in message_handler() in another thread
        return;

    _log.push_back(std::make_pair(type, msg));
    if (_notification_func)
        _notification_func(msg);
}

void Logger::set_notification_func(const std::function<void (const QString&)> func) {
    const std::lock_guard<std::mutex> locker(_mutex);
    _notification_func = func;
}
