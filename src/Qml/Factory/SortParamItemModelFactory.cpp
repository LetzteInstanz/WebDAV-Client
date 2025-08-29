#include "SortParamItemModelFactory.h"

#include "../Settings/Settings.h"
#include "Sort/SortParamItemModel.h"

Qml::SortParamItemModelFactory::SortParamItemModelFactory(std::shared_ptr<::Settings> settings, QObject* parent) : QObject(parent), _settings(std::move(settings)) {}

Qml::SortParamItemModelFactory::~SortParamItemModelFactory() = default;

QObject* Qml::SortParamItemModelFactory::createModel() { return new SortParamItemModel(_settings); }
