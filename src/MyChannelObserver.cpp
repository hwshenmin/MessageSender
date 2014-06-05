#include "stdafx.h"
#include "MyChannelObserver.h"
#include "core\message\src\ConnectionChecker.h"


namespace TA_Base_Core
{

    MyChannelObserver::MyChannelObserver( const std::string& channel_name, unsigned long location_key )
        : IChannelObserver( channel_name, location_key ),
          m_is_ready( false )
    {
        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().attach(this);
    }


    bool MyChannelObserver::onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr channel, const TA_Base_Core::IChannelLocator_ptr channelLocator )
    {
        m_channels.insert( serviceAddr );
        m_is_ready = true;
        return true;
    }


    void MyChannelObserver::onChannelUnavailable( const std::string& serviceAddr )
    {
        m_channels.erase( serviceAddr );
        m_is_ready = ( false == m_channels.empty() );
    }


    bool MyChannelObserver::is_channel_ready()
    {
        return m_is_ready;
    }

}
