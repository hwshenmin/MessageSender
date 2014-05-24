#ifndef ICOMMANDOBSERVER_H_INCLUDED
#define ICOMMANDOBSERVER_H_INCLUDED

#include <string>


struct ICommandObserver
{
    virtual ~ICommandObserver() {}
    virtual void process_command( const std::string& command ) {}
};


#endif
