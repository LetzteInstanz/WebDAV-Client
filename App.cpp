#include "App.h"

App::App(int& argc, char** argv) : QGuiApplication(argc, argv), _srv_item_model(_srv_info_manager) {}

void App::set_initial_properties(QQmlApplicationEngine& engine) {
    engine.rootContext()->setContextProperty("srvItemModel", &_srv_item_model);
    //engine.setInitialProperties({{ "connListModel", QVariant::fromValue(static_cast<QObject*>(&_conn_list_model)) }}); // note: replace with that, when fixed https://bugreports.qt.io/browse/QTBUG-114403
}
