#include "stdafx.h"
#include "MainGuard.h"
#include "MessageSender.h"


int main(int argc, char* argv[])
{
    MainGuard main_guard( argc, argv );

    unsigned long location_key = TA_Base_Core::getRunParamValue( "LocationKey", LONG_MAX );
    unsigned long interval = TA_Base_Core::getRunParamValue( "Interval", 1000 );
    std::string msg_load = TA_Base_Core::RunParams::getInstance().get( "Message" );
    std::string channel_name = TA_Base_Core::RunParams::getInstance().get( "Channel" );
    std::string domain_name = TA_Base_Core::RunParams::getInstance().get( "Domain" );
    std::string type_name = TA_Base_Core::RunParams::getInstance().get( "Type" );
    std::string filter_data = TA_Base_Core::RunParams::getInstance().get( "FilterData" );
    unsigned long region_filter_start = TA_Base_Core::getRunParamValue( "RegionFilterStart", location_key );
    unsigned long region_filter_number = TA_Base_Core::getRunParamValue( "RegionFilterNumber", 1 );
    unsigned long entity_key = 1;

    std::vector<TA_Base_Core::MessageSender*> m_senders;

    for ( size_t i = 0; i < region_filter_number; ++i )
    {
        if ( true == TA_Base_Core::RunParams::getInstance().get( "FilterData" ).empty() && ( TA_Base_Core::RunParams::getInstance().isSet( "RegionFilterStart" ) || TA_Base_Core::RunParams::getInstance().isSet( "RegionFilterNumber" ) )  )
        {
            filter_data = "Region=";
            filter_data += boost::lexical_cast<std::string>(i + region_filter_start);
        }

        TA_Base_Core::MessageSender* message_sender = new TA_Base_Core::MessageSender( location_key,
                                                                                       entity_key,
                                                                                       interval,
                                                                                       msg_load,
                                                                                       channel_name,
                                                                                       domain_name,
                                                                                       type_name,
                                                                                       filter_data );
        main_guard.register_command_observer( message_sender );
        m_senders.push_back( message_sender );
    }

    main_guard.wait_for_user_command();

    for ( size_t i = 0; i < region_filter_number; ++i )
    {
        m_senders[i]->terminate();
    }

    for ( size_t i = 0; i < region_filter_number; ++i )
    {
        delete m_senders[i];
        m_senders[i] = NULL;
    }

    return 0;
}
