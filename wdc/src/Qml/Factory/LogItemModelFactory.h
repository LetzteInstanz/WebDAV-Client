#pragma once

class Logger;

namespace Qml {
    class LogItemModelFactory : public QObject {
        Q_OBJECT

    public:
        LogItemModelFactory(std::shared_ptr<Logger> logger, QObject* parent = nullptr);
        ~LogItemModelFactory() override;

        Q_INVOKABLE QObject* createModel();

    private:
        std::shared_ptr<Logger> _logger;
    };
}
