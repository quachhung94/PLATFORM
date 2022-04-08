/** --------------------------------------------------------------------------
 *  WebSocketServer.cpp
 *
 *  Base class that WebSocket implementations must inherit from.  Handles the
 *  client connections and calls the child class callbacks for connection
 *  events like onConnect, onMessage, and onDisconnect.
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  --------------------------------------------------------------------------
 **/

#include <stdlib.h>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <fcntl.h>
#include "libwebsockets.h"
#include "WebSocketServer.h"
#include "spdlog/spdlog.h"

// 0 for unlimited

namespace WS {
// Nasty hack because certain callbacks are statically defined
WebSocketServer *self;

static int callback_main(   struct lws *wsi,
                            enum lws_callback_reasons reason,
                            void *user,
                            void *in,
                            size_t len )
{
    int fd;
    const char * message = "QUACH THE HUNG";
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    SPDLOG_INFO("Server callback reason: {}", reason);
    switch( reason ) {
        case LWS_CALLBACK_ESTABLISHED:
            self->onConnectWrapper( lws_get_socket_fd( wsi ) );
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = lws_get_socket_fd( wsi );
            lws_write( wsi, &self->connections[fd]->received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], self->connections[fd]->received_payload.len, LWS_WRITE_TEXT );
            // while( !self->connections[fd]->buffer.empty( ) )
            // {
            //     // const char * message = self->connections[fd]->buffer.front( );
                
                
            //     int msgLen = strlen(message);
            //     SPDLOG_INFO("Server callback write: len: {} -> {}", msgLen, message);
            //     int charsSent = lws_write( wsi, (unsigned char *)message, msgLen, LWS_WRITE_TEXT );
            //     if( charsSent < msgLen )
            //     {
            //         self->onErrorWrapper( fd, std::string( "Error writing to socket" ) );
            //     }
            //     else
            //     {
            //         // Only pop the message if it was sent successfully.
            //         // self->connections[fd]->buffer.pop_front( );
            //         SPDLOG_INFO("here Server callback write: len: {} -> {}", msgLen, message);
            //     }
            // }
            // lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_RECEIVE:
            // memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
            // self->onMessage( lws_get_socket_fd( wsi ), std::string( (const char *)in, len ) );
            // received_payload.len = len;
			// lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
            
            // memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
			// received_payload.len = len;
			lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
            self->onMessage( lws_get_socket_fd( wsi ), std::string( (const char *)in, len ) );
            break;

        case LWS_CALLBACK_CLOSED:
            self->onDisconnectWrapper( lws_get_socket_fd( wsi ) );
            break;

        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "example-protocol",
        callback_main,
        0, // user data struct not used
        MAX_BUFFER_SIZE,
    },{ NULL, NULL, 0, 0 } // terminator
};


#if defined(LWS_ROLE_WS) && !defined(LWS_WITHOUT_EXTENSIONS)
static const struct lws_extension exts[] = {
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate"
	},
	{ NULL, NULL, NULL /* terminator */ }
};
#endif

WebSocketServer::WebSocketServer( int port, const std::string certPath, const std::string& keyPath )
{
    this->_port     = port;
    this->_certPath = certPath;
    this->_keyPath  = keyPath;

    lws_set_log_level( 0, lwsl_emit_syslog ); // We'll do our own logging, thank you.
    struct lws_context_creation_info info;
    memset( &info, 0, sizeof info );
    info.port = this->_port;
    info.iface = "lo";
    info.protocols = protocols;
#if defined(LWS_ROLE_WS) && !defined(LWS_WITHOUT_EXTENSIONS)
	info.extensions = exts;
#endif

    if( !this->_certPath.empty( ) && !this->_keyPath.empty( ) )
    {
        SPDLOG_INFO( "Using SSL certPath= {} keyPath= {}", this->_certPath, this->_keyPath);
        info.ssl_cert_filepath        = this->_certPath.c_str( );
        info.ssl_private_key_filepath = this->_keyPath.c_str( );
    }
    else
    {
        SPDLOG_INFO( "Not using SSL" );
        info.ssl_cert_filepath        = NULL;
        info.ssl_private_key_filepath = NULL;
    }
    info.gid = -1;
    info.uid = -1;
    info.options = 0;

    // keep alive
    info.ka_time = 60; // 60 seconds until connection is suspicious
    info.ka_probes = 10; // 10 probes after ^ time
    info.ka_interval = 10; // 10s interval for sending probes
    this->_context = lws_create_context( &info );
    if( !this->_context )
        throw "libwebsocket init failed";
    SPDLOG_INFO( "Server started on port {} ", this->_port);

    // Some of the libwebsocket stuff is define statically outside the class. This
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private.
    self = this;
}

WebSocketServer::~WebSocketServer( )
{
    // Free up some memory
    for( std::map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
    {
        Connection* c = it->second;
        this->connections.erase( it->first );
        delete c;
    }
}

void WebSocketServer::onConnectWrapper( int socketID )
{
    Connection* c = new Connection;
    c->createTime = time( 0 );
    this->connections[ socketID ] = c;
    this->onConnect( socketID );
}

void WebSocketServer::onDisconnectWrapper( int socketID )
{
    this->onDisconnect( socketID );
    this->_removeConnection( socketID );
}

void WebSocketServer::onErrorWrapper( int socketID, const std::string& message )
{
    SPDLOG_INFO( "Error: {} on {}", message, socketID);
    this->onError( socketID, message );
    this->_removeConnection( socketID );
}

void WebSocketServer::send( int socketID, std::string data )
{
    // Push this onto the buffer. It will be written out when the socket is writable.
    SPDLOG_INFO("Server Send {}", data);
    this->connections[socketID]->buffer.push_back( data.c_str() );
    // this->connections[socketID]->received_payload
    memcpy( &(this->connections[socketID]->received_payload.data[LWS_SEND_BUFFER_PRE_PADDING]), data.c_str(), strlen(data.c_str()));
    this->connections[socketID]->received_payload.len = strlen(data.c_str());
}

void WebSocketServer::broadcast(std::string data )
{
    SPDLOG_INFO("BROADCAST {}", data);
    for( std::map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        this->send( it->first, data );
}

void WebSocketServer::setValue( int socketID, const std::string& name, const std::string& value )
{
    this->connections[socketID]->keyValueMap[name] = value;
}

std::string WebSocketServer::getValue( int socketID, const std::string& name )
{
    return this->connections[socketID]->keyValueMap[name];
}
int WebSocketServer::getNumberOfConnections( )
{
    return this->connections.size( );
}

void WebSocketServer::run( uint64_t timeout )
{
    while( 1 )
    {
        this->wait( timeout );
    }
}

void WebSocketServer::wait( uint64_t timeout )
{
    if( lws_service( this->_context, timeout ) < 0 )
        throw "Error polling for socket activity.";
}

void WebSocketServer::_removeConnection( int socketID )
{
    Connection* c = this->connections[ socketID ];
    this->connections.erase( socketID );
    delete c;
}
}