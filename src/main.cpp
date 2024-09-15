#include "App.h"
#include "pch.h"

int main(int argc, char* argv[]) {
    App app(argc, argv);
    QQmlApplicationEngine engine;
    app.initialize_engine(engine);
    engine.loadFromModule("WebDavClient", "Main");
    return app.exec();
}
