#include "stdafx.h"
#include "MainGuard.h"
#include "MessageSender.h"
#include "Parameter.h"
#include "CommandLineHelper.h"
#include "MyChannelObserver.h"
#include "core/message/src/MessagePublicationManager.h"


int main(int argc, char* argv[])
{
    MainGuard main_guard( argc, argv );

    CommandLineHelper command_line_helper;
    ParameterPtrList parameters = command_line_helper.get_parameters();
    boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> supplier( TA_Base_Core::gGetStructuredEventSupplierForChannel( TA_Base_Core::RunParams::getInstance().get( "Channel" ) ) );
    MyChannelObserverPtr channel_observer( new MyChannelObserver( TA_Base_Core::RunParams::getInstance().get( "Channel" ) ) );
    std::vector<MessageSenderPtr> m_senders;

    for ( size_t i = 0; i < parameters.size(); ++i )
    {
        MessageSenderPtr message_sender( new MessageSender( parameters[i], supplier, channel_observer ) );
        main_guard.register_command_observer( message_sender.get() );
        m_senders.push_back( message_sender );
    }

    main_guard.wait_for_user_command();

    for ( size_t i = 0; i < m_senders.size(); ++i )
    {
        m_senders[i]->terminate();
    }

    return 0;
}
