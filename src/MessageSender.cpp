#include "stdafx.h"
#include "MessageSender.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"


MessageSender::MessageSender( ParameterPtr parameter )
    : m_parameter( parameter ),
      m_data( NULL ),
      m_running( true )
{
    m_supplier = m_parameter->m_supplier;
    m_channel_observer = m_parameter->m_channel_observer;

    m_data = new char[m_parameter->m_data.size() + 11];
    ::sprintf( m_data, "%10d%s", 0, m_parameter->m_data.c_str() );

    populate_half_done_event();

    start();
}


MessageSender::~MessageSender()
{
    terminateAndWait();

    delete m_data;
    m_data = NULL;
}


void MessageSender::run()
{
    while ( m_running )
    {
        while ( false == m_channel_observer->ready() && true == m_running )
        {
            TA_Base_Core::Thread::sleep( 10 );
        }

        if ( false == m_running )
        {
            break;
        }

        send_message( next_data() );
        my_sleep();
    }

    m_running = true; // for next time
}


void MessageSender::terminate()
{
    m_running = false;
}


void MessageSender::send_message( const char* data )
{
    try
    {
        TA_Base_Core::CommsMessageCorbaDef* comms_message = new TA_Base_Core::CommsMessageCorbaDef;

        comms_message->createTime = time( NULL );
        comms_message->messageTypeKey = m_parameter->m_type_name.c_str();
        comms_message->assocEntityKey = 0;
        comms_message->messageState <<= data;

        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent( m_half_done_event );
        event->remainder_of_body <<= comms_message;

        m_supplier->publishEvent( *event );
    }
    catch ( std::exception& ex )
    {
        std::cout << "MessageSender::send_message - std::exception: " << ex.what() << std::endl;
        TA_Base_Core::Thread::sleep( 100 );
    }
    catch ( ... )
    {
        std::cout << "MessageSender::send_message - exception." << std::endl;
        TA_Base_Core::Thread::sleep( 100 );
    }
}


const char* MessageSender::next_data()
{
    static size_t counter = 0;
    static TA_Base_Core::NonReEntrantThreadLockable lock;

    {
        TA_THREADGUARD( lock );
        ++counter;
    }

    std::string counter_str = boost::lexical_cast<std::string>( counter );

    for ( size_t i = 0; i < counter_str.size() || i < 10; ++i )
    {
        m_data[i] = ( i < counter_str.size() ? counter_str[i] : ' ' );
    }

    if ( 0 == counter % 100 )
    {
        std::cout << "\r" << "TOTAL: " << counter << std::endl;
    }
    else if ( 0 == counter % 20 )
    {
        std::cout << "\r                                        \r.";
    }
    else
    {
        std::cout << ".";
    }

    return m_data;
}


void MessageSender::process_command( const std::string& command )
{
    try
    {
        unsigned long interval = boost::lexical_cast<unsigned long>(command);
        std::cout << "\r" << "INTERVAL CHANGE: FROM " << m_parameter->m_interval << " TO " << interval << std::endl;
        m_parameter->m_interval = interval;
    }
    catch (...)
    {
    }
}


void MessageSender::my_sleep()
{
    if ( 0 == m_parameter->m_interval )
    {
        return;
    }

    size_t STEP = m_parameter->m_interval;

    if ( 1000 < STEP )
    {
        STEP = 100;
    }

    for ( size_t i = 0; i < m_parameter->m_interval && true == m_running; i += STEP )
    {
        TA_Base_Core::Thread::sleep( STEP );
    }
}


void MessageSender::populate_half_done_event()
{
    // Fixed Header
    m_half_done_event.header.fixed_header.event_type.domain_name = m_parameter->m_domain_name.c_str();
    m_half_done_event.header.fixed_header.event_type.type_name = m_parameter->m_type_name.c_str();

    // Variable Header
    m_half_done_event.header.variable_header.length(2);
    m_half_done_event.header.variable_header[0].name = CosNotification::Priority;
    m_half_done_event.header.variable_header[0].value <<= static_cast<const char*>( "0" );
    m_half_done_event.header.variable_header[1].name = TA_Base_Core::SENDER_ENTITY_FIELDNAME;
    m_half_done_event.header.variable_header[1].value <<= TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

    // Filterable Body
    m_filterable_data = parse_filterable_date( m_parameter->m_filterable_data );
    m_half_done_event.filterable_data.length( m_filterable_data.size() );

    for ( size_t i = 0; i < m_filterable_data.size(); ++i )
    {
        m_half_done_event.filterable_data[i].name = m_filterable_data[i].first.c_str();
        m_half_done_event.filterable_data[i].value <<= m_filterable_data[i].second.c_str();
    }
}


std::vector< std::pair<std::string, std::string> > MessageSender::parse_filterable_date( const std::string& filterable_data_str )
{
    std::vector< std::pair<std::string, std::string> > filterable_data;

    const char* filterable_data_regex_str = "(?x) ([^ ,;=]+) = ([^ ,;=]+)";
    const boost::regex filterable_data_regex( filterable_data_regex_str );
    boost::sregex_iterator it( filterable_data_str.begin(), filterable_data_str.end(), filterable_data_regex );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        filterable_data.push_back( std::make_pair( it->str(1), it->str(2) ) );
    }

    return filterable_data;
}
