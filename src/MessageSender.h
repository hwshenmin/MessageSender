#pragma once
#include "ICommandObserver.h"
#include "Parameter.h"
#include "MyChannelObserver.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


class MessageSender : public TA_Base_Core::Thread,
                      public ICommandObserver
{
public:

    MessageSender( ParameterPtr parameter );
    ~MessageSender();

public:

    virtual void run();
    virtual void terminate();
    virtual void process_command( const std::string& command );

private:

    void send_message();
    void send_message( const char* data );
    const char* next_data();
    void my_sleep();
    void populate_half_done_event();
    void print_status( size_t counter );
    size_t next_id();
    static std::vector< std::pair<std::string, std::string> > parse_filterable_date( const std::string& filterable_data_str );

private:

    ParameterPtr m_parameter;
    MyChannelObserverPtr m_channel_observer;
    boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> m_supplier;
    volatile bool m_running;
    char* m_data;
    std::vector< std::pair<std::string, std::string> > m_filterable_data;
    CosNotification::StructuredEvent m_half_done_event;

    bool m_is_multiple_thread;
    static size_t s_counter;
    static TA_Base_Core::NonReEntrantThreadLockable s_counter_lock;
    static TA_Base_Core::NonReEntrantThreadLockable s_cout_lock;
};

typedef boost::shared_ptr<MessageSender> MessageSenderPtr;
typedef std::vector<MessageSenderPtr> MessageSenderPtrList;
