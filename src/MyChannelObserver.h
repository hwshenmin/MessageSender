#pragma once
#include "core/message/src/EventChannelUser.h"


namespace TA_Base_Core
{

    struct MyChannelObserver : TA_Base_Core::IChannelObserver
    {
        MyChannelObserver( const std::string& channel_name );

        bool ready();

        virtual bool onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr, const TA_Base_Core::IChannelLocator_ptr );
        virtual void onChannelUnavailable( const std::string& serviceAddr );

    private:

        std::set<std::string> m_channels;
        volatile bool m_is_ready;
    };

    typedef boost::shared_ptr<MyChannelObserver> MyChannelObserverPtr;

}
