#include "stdafx.h"
#include "MyChannelObserver.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/utilities/src/RunParams.h"


MyChannelObserver::MyChannelObserver( const std::string& channel_name )
    : IChannelObserver( channel_name, TA_Base_Core::getRunParamValue( RPARAM_LOCATIONKEY, 100 ) ),
      m_is_ready( false )
{
    TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().attach(this);
}


bool MyChannelObserver::onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr, const TA_Base_Core::IChannelLocator_ptr )
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


bool MyChannelObserver::ready()
{
    return m_is_ready;
}
