#pragma once

#include "AbstractFileSystemModelFactory.h"

class Settings;

namespace Qml {
    class FileItemModelFactory : public AbstractFileSystemModelFactory {
        Q_OBJECT

    public:
        FileItemModelFactory(std::shared_ptr<::Settings> settings, QObject* parent = nullptr);
        ~FileItemModelFactory() override;

        Q_INVOKABLE QObject* createModel(const QString& root_path);

    private:
        std::shared_ptr<::Settings> _settings;
    };
}
