#include "stdafx.h"
#include "MessageSender.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/ConnectionChecker.h"


namespace
{

    std::vector<TA_Base_Core::NameValuePair> extract_name_value_pairs( const std::string& str )
    {
        std::vector<TA_Base_Core::NameValuePair> pair_list;

        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep( ":,;" );
        tokenizer tokens( str, sep );

        for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
        {
            std::string pair_string = *tok_iter;
            size_t equal_pos = pair_string.find( "=" );

            if ( equal_pos != std::string::npos )
            {
                std::string name = pair_string.substr( 0, equal_pos );
                std::string value = pair_string.substr( equal_pos + 1, std::string::npos );
                TA_Base_Core::NameValuePair the_pair( name, TA_Base_Core::NameValuePair::EQUALS, value );
                pair_list.push_back( the_pair );
            }
            else
            {
                std::cout << str << std::endl;
                TA_ASSERT(false, "NameValuePair requestes an evaluation type.")
            }
        }

        return pair_list;
    }


    void add_to_filter_data( TA_Base_Core::FilterData& filter_data, const std::vector<TA_Base_Core::NameValuePair>& name_value_pairs )
    {
        for ( size_t i = 0; i < name_value_pairs.size(); ++i )
        {
            filter_data.push_back( &const_cast<TA_Base_Core::NameValuePair&>(name_value_pairs[i]) );
        }
    }

}


namespace TA_Base_Core
{

    MessageSender::MessageSender( unsigned long location_key,
                                  unsigned long entity_key,
                                  unsigned long interval,
                                  std::string message_load,
                                  const std::string& channel_name,
                                  const std::string& domain_name,
                                  const std::string& type_name,
                                  const std::string& filter_data )
        : m_location_key( location_key ),
          m_entity_key( entity_key ),
          m_interval( interval ),
          m_message_load( message_load.c_str() ),
          m_channel_name( channel_name ),
          m_domain_name( domain_name ),
          m_type_name( type_name ),
          m_filter_data( filter_data ),
          m_running( true ),
          m_supplier( TA_Base_Core::gGetStructuredEventSupplierForChannel( m_channel_name ) ),
          IChannelObserver( channel_name, location_key )
    {
        static TA_Base_Core::NameValuePair priorityField( CosNotification::Priority, "0" );
        m_var_header.push_back( &priorityField );

        //static TA_Base_Core::NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, boost::lexical_cast<std::string>( m_location_key ) );
        // static TA_Base_Core::NameValuePair messageTypeFilter( TA_Base_Core::MESSAGETYPE_FILTER_NAME, m_type_name.c_str() );
        //m_filterable_data.push_back( &regionFilter );
        // m_filterable_data.push_back( &messageTypeFilter );

        if ( false == m_filter_data.empty() )
        {
            std::vector<TA_Base_Core::NameValuePair>* name_value_pairs = new std::vector<TA_Base_Core::NameValuePair>;
            *name_value_pairs = extract_name_value_pairs( m_filter_data );
            add_to_filter_data( m_filterable_data, *name_value_pairs );
        }

        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().attach(this);

        start();
    }


    MessageSender::~MessageSender()
    {
        terminateAndWait();
    }


    void MessageSender::run()
    {
        send_message();
    }


    void MessageSender::terminate()
    {
        m_running = false;
    }


    void MessageSender::send_message()
    {
        unsigned long seq_num = 1;
        std::stringstream strm;

        while ( m_running )
        {
            if ( false == m_avaliable_channels.empty() )
            {
                strm.clear();
                strm.str("");
                strm << std::setw(10) << seq_num++ << ": " << m_message_load;

                send_message_impl( strm.str() );

                if ( ( 0 == m_interval && seq_num % 1000 == 0 ) || (0 < m_interval && seq_num % 100 == 0 ) )
                {
                    std::cout << "\rsend messages total number:" << seq_num << std::endl;
                }
                else
                {
                    std::cout << ".";
                }
            }

            if ( true == m_running && 0 < m_interval  )
            {
                for ( size_t i = 0; true == m_running && i < m_interval; i+= 10 )
                {
                    TA_Base_Core::Thread::sleep( 10 );
                }
            }

            if ( true == m_running && m_avaliable_channels.empty() )
            {
                TA_Base_Core::Thread::sleep( 100 );
            }
        }

        m_running = true; // for next time
    }



    void MessageSender::send_message_impl( const std::string& messageString )
    {
        try
        {
            CORBA::Any data;
            data <<= messageString.c_str();

            TA_Base_Core::CommsMessageCorbaDef* commsMessage = new TA_Base_Core::CommsMessageCorbaDef;

            commsMessage->createTime = time( NULL );
            commsMessage->messageTypeKey = m_type_name.c_str();
            commsMessage->assocEntityKey = m_entity_key;
            commsMessage->messageState = data;

            CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
            event->remainder_of_body <<= commsMessage;

            TA_Base_Core::gPopulateStructuredEventHeader( *event,
                                                          m_domain_name,
                                                          m_type_name,
                                                          &m_var_header,
                                                          &m_filterable_data );

            TA_ASSERT(m_supplier, "m_supplier != NULL");
            m_supplier->publishEvent(*event);
        }
        catch (...)
        {
            std::cout << "MessageSender::send_message_impl - caught exception." << std::endl;
        }
    }


    void MessageSender::process_command( const std::string& command )
    {
        try
        {
            unsigned long new_interval = boost::lexical_cast<unsigned long>(command);
            std::cout << "\rInterval changed from " << m_interval << " to " << new_interval << std::endl;
            m_interval = new_interval;
        }
        catch (...)
        {
        }
    }


    bool MessageSender::onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr channel, const TA_Base_Core::IChannelLocator_ptr channelLocator )
    {
        m_avaliable_channels.push_back( serviceAddr );
        return true;
    }


    void MessageSender::onChannelUnavailable( const std::string& serviceAddr )
    {
        std::vector<std::string>::iterator findIt = std::find( m_avaliable_channels.begin(), m_avaliable_channels.end(), serviceAddr );

        if ( findIt != m_avaliable_channels.end() )
        {
            m_avaliable_channels.erase( findIt );
        }
    }

}
