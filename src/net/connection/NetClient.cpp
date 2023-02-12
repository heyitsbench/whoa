#include "net/connection/NetClient.hpp"
#include "net/connection/WowConnection.hpp"
#include <cstdlib>
#include <cstring>
#include <new>
#include <common/Prop.hpp>
#include <storm/Error.hpp>
#include <storm/String.hpp>

HPROPCONTEXT s_propContext;

int32_t NetClient::s_clientCount;

void InitializePropContext() {
    if (PropGetSelectedContext() != s_propContext) {
        PropSelectContext(s_propContext);
    }
}

void NetClient::Connect(const char* addrStr) {
    if (this->m_netState != NS_INITIALIZED) {
        SErrDisplayAppFatal("Expected (m_netState == NS_INITIALIZED), got %d", this->m_netState);
    }

    uint16_t port = 9090;

    char host[1024];
    SStrCopy(host, addrStr, sizeof(host));

    auto portDelim = SStrChr(host, ':');
    if (portDelim) {
        *portDelim = '\0';
        port = atoi(portDelim + 1);
    }

    this->m_serverConnection->SetEncryptionType(WC_ENCRYPT_0);
    this->m_netState = NS_INITIALIZED;
    this->ConnectInternal(host, port);
}

int32_t NetClient::ConnectInternal(const char* host, uint16_t port) {
    if (this->m_netState != NS_INITIALIZED) {
        SErrDisplayAppFatal("Expected (m_netState == NS_INITIALIZED), got %d", this->m_netState);
    }

    this->m_netState = NS_GETTING_REALMS;
    this->m_serverConnection->Connect(host, port, -1);

    // TODO

    return 1;
}

int32_t NetClient::Initialize() {
    STORM_ASSERT(this->m_netState == NS_UNINITIALIZED);

    if (NetClient::s_clientCount == 0) {
        s_propContext = PropGetSelectedContext();

        if (!WowConnection::InitOsNet(nullptr, InitializePropContext, 1, false)) {
            return 0;
        }
    }

    NetClient::s_clientCount++;

    auto queueMem = SMemAlloc(sizeof(NETEVENTQUEUE), __FILE__, __LINE__, 0x0);
    auto queue = new (queueMem) NETEVENTQUEUE(this);
    this->m_netEventQueue = queue;

    memset(this->m_handlers, 0, sizeof(this->m_handlers));
    memset(this->m_handlerParams, 0, sizeof(this->m_handlerParams));

    auto connectionMem = SMemAlloc(sizeof(WowConnection), __FILE__, __LINE__, 0x0);
    auto connection = new (connectionMem) WowConnection(this, nullptr);
    this->m_serverConnection = connection;

    this->m_netState = NS_INITIALIZED;

    return 1;
}

void NetClient::SetLoginData(LoginData* loginData) {
    memcpy(&this->m_loginData, loginData, sizeof(this->m_loginData));
}

void NetClient::SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param) {
    this->m_handlers[msgId] = handler;
    this->m_handlerParams[msgId] = param;
}

void NetClient::WCCantConnect(WowConnection* conn, uint32_t timeStamp, NETCONNADDR* addr) {
    // TODO
}

void NetClient::WCConnected(WowConnection* conn, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) {
    // TODO
}

void NetClient::WCDataReady(WowConnection* conn, uint32_t timeStamp, uint8_t* data, int32_t len) {
    // TODO
}

void NetClient::WCDisconnected(WowConnection* conn, uint32_t timeStamp, NETCONNADDR* addr) {
    // TODO
}
