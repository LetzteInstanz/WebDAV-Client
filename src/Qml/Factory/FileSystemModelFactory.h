#pragma once

#include "AbstractFileSystemModelFactory.h"

namespace Qml {
    class FileSystemModelFactory : public AbstractFileSystemModelFactory {
        Q_OBJECT

    public:
        using AbstractFileSystemModelFactory::AbstractFileSystemModelFactory;
        ~FileSystemModelFactory() override;

        Q_INVOKABLE QObject* createModel(const QString& addr, std::uint16_t port);
        Q_INVOKABLE QObject* createModel();
    };
}
