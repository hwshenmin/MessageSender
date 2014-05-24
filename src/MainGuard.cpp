#include "stdafx.h"
#include "MainGuard.h"
#include <boost/algorithm/string/case_conv.hpp>

#pragma comment( lib, "AttNotification416_rtd.lib")
#pragma comment( lib, "COSNotify416_rtd.lib")
#pragma comment( lib, "omniORB416_rtd.lib")
#pragma comment( lib, "omniDynamic416_rtd.lib")
#pragma comment( lib, "COS416_rtd.lib")
#pragma comment( lib, "omnithread34_rtd.lib")
#pragma comment( lib, "oci.lib")
#pragma comment( lib, "aced.lib")
#pragma comment( lib, "Rpcrt4.lib")
#pragma comment( lib, "odbc32.lib")
#pragma comment( lib, "odbccp32.lib")
#pragma comment( lib, "stracelib.lib")
#pragma comment( lib, "TA_mcl_d.lib")
#pragma comment( lib, "TA_Message_d.lib")
#pragma comment( lib, "TA_Utilities_d.lib")
#pragma comment( lib, "TA_CorbaUtil_d.lib")
#pragma comment( lib, "TA_Base_Core_d.lib")


MainGuard::MainGuard(int argc, char* argv[])
{
    TA_Base_Core::RunParams::getInstance().set( "EnableSubChangeUpdates", "" );
    TA_Base_Core::RunParams::getInstance().parseCmdLine( argc, argv );
    TA_Base_Core::gSetDebugUtilFromRunParams();
    TA_Base_Core::CorbaUtil::getInstance().initialise();
    TA_Base_Core::CorbaUtil::getInstance().activate();
}


MainGuard::~MainGuard()
{
    TA_Base_Core::CorbaUtil::getInstance().shutdown();
    TA_Base_Core::CorbaUtil::cleanup();
}


void MainGuard::wait_for_quit()
{
    while ( true )
    {
        std::string command;
        std::cin >> command;

        std::string upper_command = boost::to_upper_copy(command);

        if ( upper_command == "Q" || "QUIT" == upper_command || "EXIT" == upper_command )
        {
            m_command_observers.clear();
            return;
        }
        else if ( "CLS" == upper_command )
        {
            ::system( "cls" );
            continue;
        }

        for ( std::set<ICommandObserver*>::iterator it = m_command_observers.begin(); it != m_command_observers.end(); ++it )
        {
            (*it)->process_command( command );
        }
    }
}


void MainGuard::register_command_observer( ICommandObserver* command_observer )
{
    if ( command_observer != NULL )
    {
        m_command_observers.insert( command_observer );
    }
}


void MainGuard::deregister_command_observer( ICommandObserver* command_observer )
{
    if ( command_observer != NULL )
    {
        m_command_observers.erase( command_observer );
    }
}

