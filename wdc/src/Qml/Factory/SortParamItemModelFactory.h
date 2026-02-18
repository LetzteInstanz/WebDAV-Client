#pragma once

class Settings;

namespace Qml {
    class SortParamItemModelFactory : public QObject {
        Q_OBJECT

    public:
        SortParamItemModelFactory(std::shared_ptr<::Settings> settings, QObject* parent = nullptr);
        ~SortParamItemModelFactory() override;

        Q_INVOKABLE QObject* createModel();

    private:
        std::shared_ptr<::Settings> _settings;
    };
}
