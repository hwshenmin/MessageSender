#pragma once
#include "core/message/src/EventChannelUser.h"


namespace TA_Base_Core
{

    struct MyChannelObserver : TA_Base_Core::IChannelObserver
    {
        MyChannelObserver( const std::string& channel_name, unsigned long location_key );

        virtual bool onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr channel, const TA_Base_Core::IChannelLocator_ptr channelLocator );
        virtual void onChannelUnavailable( const std::string& serviceAddr );

        bool is_channel_ready();

    private:

        std::set<std::string> m_channels;
        volatile bool m_is_ready;
    };

    typedef boost::shared_ptr<MyChannelObserver> MyChannelObserverPtr;

}
