#include "App.h"

int main(int argc, char* argv[]) {
    //try {
        App app(argc, argv);
        QQmlApplicationEngine engine;
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
        app.set_initial_properties(engine);
        engine.loadFromModule("WebDavClient", "Main");
        return app.exec();
    /*}
    catch (const std::runtime_error& e) {
        e.what();
    }*/
}
