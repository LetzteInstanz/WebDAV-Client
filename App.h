#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ServerItemModel.h"
#include "ServerInfoManager.h"
#include "Settings.h"

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);

    void set_initial_properties(QQmlApplicationEngine& engine);

private:
    Settings _settings;
    ServerInfoManager _srv_info_manager;
    ServerItemModel _srv_item_model;
};
