#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ServerItemModel.h"
#include "ServerInfoManager.h"
#include "SettingsJsonFile.h"

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);

    void set_initial_properties(QQmlApplicationEngine& engine);

private:
    SettingsJsonFile _settings;
    ServerInfoManager _srv_info_manager;
    ServerItemModel _srv_item_model;
};
