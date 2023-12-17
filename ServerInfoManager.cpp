#include "ServerInfoManager.h"

ServerInfo ServerInfoManager::get(ServerInfo::Id id) const {
    return _get(_info_hash.find(id), id)->second;
}

ServerInfo::Id ServerInfoManager::add_new(const ServerInfo& info) {
    ++_max_id;
    _info_hash.emplace(_max_id, info);
    return _max_id;
}

ServerInfo& ServerInfoManager::operator[](ServerInfo::Id id) {
    const auto it = _info_hash.find(id);
    return const_cast<ServerInfo&>(_get(it, id)->second);
}

const ServerInfoManager::IdServerInfoHash::const_iterator ServerInfoManager::_get(IdServerInfoHash::const_iterator it, ServerInfo::Id id) const {
    if (it == _info_hash.end()) {
        const QString err(QObject::tr("Server item with the ID ") + QString::number(id) + QObject::tr(" doesn't exist"));
        throw std::runtime_error(err.toStdString());
    }
    return it;
}
