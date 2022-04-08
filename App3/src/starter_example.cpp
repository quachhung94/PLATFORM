/*
Platform - A C++ framework
*/

#include "starter_example.h"
#include "platform/core/SystemStatistics.h"
#include "spdlog/spdlog.h"

namespace starter_example {


void ATask::tick()
{
    this->wait();
}

void ATask::onConnect( int socketID )
{
    // Give this connection a random user ID
    const std::string& handle = "User " + toString( socketID );
    SPDLOG_INFO( "New connection: {}", handle );
    
    // Associate this handle with the connection
    this->setValue( socketID, "handle", handle );

    // Let everyone know the new user has connected
    this->broadcast( handle + " has connected." );
}

void ATask::onMessage( int socketID, const std::string& data )
{
    // Send the received message to all connected clients in the form of 'User XX: message...'
    const std::string& handle = "User " + toString( socketID );
    SPDLOG_INFO( "Received: " + data );
    const std::string& message = this->getValue( socketID, "handle" ) + ": " + data;
    SPDLOG_INFO( "message: " + message );
    this->broadcast( handle + " has Reply." );
    // this->send( socketID, message );
}

void ATask::onDisconnect( int socketID )
{
    const std::string& handle = this->getValue( socketID, "handle" );
    SPDLOG_INFO( "Disconnected: {}", handle );
    
    // Let everyone know the user has disconnected
    const std::string& message = handle + " has disconnected.";
    for( std::map<int,WS::WebSocketServer::Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        if( it->first != socketID )
            // The disconnected connection gets deleted after this function runs, so don't try to send to it
            // (It's still around in case the implementing class wants to perform any clean up actions)
            this->send( it->first, message );
}

void ATask::onError( int socketID, const std::string& message )
{
    SPDLOG_INFO( "Error: {}", message );
}

App::App() : Application(platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1800)),
            m_timerEventQueue(ExpiredQueue::create(30, *this, *this)),
            timers()
{

}

void App::init()
{
    SPDLOG_INFO("App::Init Starting...");
    // platform::core::SystemStatistics::instance().dump();
    // a_instance.start();
    // this->m_wsMonitorTimeout = 10;
    this->a_instance = std::make_shared<ATask>();
    
    this->a_instance->start();
    SPDLOG_INFO("Websocket Server runs ...");
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    // create_timer(RB_FSM_WS_MONITOR_TIMER_ID, std::chrono::milliseconds(1000));
    // create_timer(2, std::chrono::milliseconds(10000));
    // create_timer(3, std::chrono::milliseconds(100000));
    // create_timer(4, std::chrono::milliseconds(1000000));
    // this->create_timer(std::chrono::milliseconds(10000));
    // for (auto& t : this->timers)
    // {
    //     t.timer->start();
    // }
    // timers.front().timer->stop();
    // timers.front().timer->start();
    // timers.at(1).timer->stop();
    // timers.at(1).timer->start();


}

void App::tick()
{
    SPDLOG_INFO("App Hello world!");
    // SystemStatistics::instance().dump();
}

void App::event(const platform::core::timer::TimerExpiredEvent& event)
{
    auto& info = timers[static_cast<decltype(timers)::size_type>(event.getId())];
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - info.last);
    info.last = std::chrono::steady_clock::now();
    info.count++;
    info.total += duration;

    SPDLOG_INFO("Timer ID {} ({}ms): {}ms, avg: {}",
                    event.getId(),
                    info.interval.count(),
                    duration.count(),
                    static_cast<double>(info.total.count()) / info.count);
}

void App::create_timer(int id, std::chrono::milliseconds interval)
{
    TimerInfo t;
    t.timer = platform::core::timer::Timer::create(static_cast<int32_t>(id), m_timerEventQueue, true, interval);
    t.interval = interval;
    timers.push_back(t);
}

} // namespace starter_example
