#pragma once

#include <stdint.h>
#include <map>
#include <string>
#include <list>
#include <stdio.h>
#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include "libwebsockets.h"
#define MAX_BUFFER_SIZE 1024

namespace WS {

class WebSocketServer
{
public:
    // Represents a client connection
    struct payload
    {
        unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + MAX_BUFFER_SIZE + LWS_SEND_BUFFER_POST_PADDING];
        size_t len;
    };
    
    struct Connection
    {
        std::list<const char*>       buffer;     // Ordered list of pending messages to flush out when socket is writable
        payload received_payload;
        std::map<std::string,std::string> keyValueMap;
        time_t             createTime;
    };

    

    // Manages connections. Unfortunately this is public because static callback for
    // libwebsockets is defined outside the instance and needs access to it.
    std::map<int,Connection*> connections;

    // Constructor / Destructor
    WebSocketServer( int port, const std::string certPath = "", const std::string& keyPath = "" );
    ~WebSocketServer( );

    void run(       uint64_t timeout = 50     );
    void wait(      uint64_t timeout = 50     );
    void send(      int socketID, std::string data );
    void broadcast( std::string data               );

    // Key => value storage for each connection
    std::string getValue( int socketID, const std::string& name );
    void   setValue( int socketID, const std::string& name, const std::string& value );
    int    getNumberOfConnections( );

    // Overridden by children
    virtual void onConnect(    int socketID                        ) = 0;
    virtual void onMessage(    int socketID, const std::string& data    ) = 0;
    virtual void onDisconnect( int socketID                        ) = 0;
    virtual void   onError(    int socketID, const std::string& message ) = 0;


    // Wrappers, so we can take care of some maintenance
    void onConnectWrapper(    int socketID );
    void onDisconnectWrapper( int socketID );
    void onErrorWrapper( int socketID, const std::string& message );

protected:
    // Nothing, yet.

private:
    int                  _port;
    std::string               _keyPath;
    std::string               _certPath;
    struct lws_context  *_context;

    void _removeConnection( int socketID );
};

}

