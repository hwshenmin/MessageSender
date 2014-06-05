#include "stdafx.h"
#include "MainGuard.h"
#include "MessageSender.h"
#include "Parameter.h"
#include "CommandLineHelper.h"
#include "core/message/src/MessagePublicationManager.h"


int main(int argc, char* argv[])
{
    MainGuard main_guard( argc, argv );

    TA_Base_Core::CommandLineHelper command_line_helper;
    TA_Base_Core::ParameterPtrList parameters = command_line_helper.get_parameters();
    boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> supplier( TA_Base_Core::gGetStructuredEventSupplierForChannel( TA_Base_Core::RunParams::getInstance().get( "Channel" ) ) );
    std::vector<TA_Base_Core::MessageSenderPtr> m_senders;

    for ( size_t i = 0; i < parameters.size(); ++i )
    {
        TA_Base_Core::MessageSenderPtr message_sender( new TA_Base_Core::MessageSender( parameters[i], supplier ) );
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
