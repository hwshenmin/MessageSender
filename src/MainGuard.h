#ifndef INCLUDE_MAIN_GUARD_H
#define INCLUDE_MAIN_GUARD_H

#include "ICommandObserver.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/RunParams.h"


class MainGuard
{
public:

    MainGuard(int argc, char* argv[]);
    ~MainGuard();

    void wait_for_quit();


    void register_command_observer( ICommandObserver* command_observer );
    void deregister_command_observer( ICommandObserver* command_observer );
    std::set<ICommandObserver*> m_command_observers;
};


#endif
