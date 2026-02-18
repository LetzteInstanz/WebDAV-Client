#include "LogItemModelFactory.h"

#include "../Logger.h"
#include "Logger/LogItemModel.h"

Qml::LogItemModelFactory::LogItemModelFactory(std::shared_ptr<Logger> logger, QObject* parent) : QObject(parent), _logger(std::move(logger)) {}

Qml::LogItemModelFactory::~LogItemModelFactory() = default;

QObject* Qml::LogItemModelFactory::createModel() { return new LogItemModel(_logger); }
