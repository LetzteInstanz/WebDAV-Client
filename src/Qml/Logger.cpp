#include "Logger.h"

#include "../Logger.h"

Qml::Logger::Logger(std::shared_ptr<::Logger> logger, QObject* parent) : QObject(parent), _logger(logger) {
    _logger->set_notification_func(std::bind(&Logger::emit_signal, this, std::placeholders::_1));
}

Qml::Logger::~Logger() { _logger->set_notification_func(nullptr); }

QString Qml::Logger::getLog() const { return _logger->get_log(); }

void Qml::Logger::emit_signal(const QString& msg) { QTimer::singleShot(0, this, [this, msg]() { msgReceived(msg); }); }
