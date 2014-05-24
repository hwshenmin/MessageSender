#if !defined(AFX_TESTSENDER_H__8D684DED_C14B_4E2F_91F6_029482DB0D1F__INCLUDED_)
#define AFX_TESTSENDER_H__8D684DED_C14B_4E2F_91F6_029482DB0D1F__INCLUDED_

#include "ICommandObserver.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/NameValuePair.h"
#include <boost/shared_ptr.hpp>


namespace TA_Base_Core
{
    class MessageSender : public TA_Base_Core::Thread,
                          public ICommandObserver,
                          public TA_Base_Core::IChannelObserver
    {
    public:

        MessageSender( unsigned long location_key,
                       unsigned long entity_key,
                       unsigned long interval,
                       std::string message_load,
                       const std::string& channel_name,
                       const std::string& domain_name,
                       const std::string& type_name,
                       const std::string& filter_data );

        ~MessageSender();

    public:

        virtual void run();
        virtual void terminate();

    private:

        void send_message();
        void send_message_impl( const std::string& messageString );

    public:

        virtual void process_command( const std::string& command );

    public:

        virtual bool onChannelAvailable( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr channel, const TA_Base_Core::IChannelLocator_ptr channelLocator );
        virtual void onChannelUnavailable( const std::string& serviceAddr );
        std::vector<std::string> m_avaliable_channels;

    private:

        unsigned long m_sender_number;
        unsigned long m_location_key;
        volatile unsigned long m_interval;
        unsigned long m_entity_key;
        std::string m_message_load;
        std::string m_channel_name;
        std::string m_domain_name;
        std::string m_type_name;
        std::string m_filter_data;
        TA_Base_Core::FilterData m_var_header;
        TA_Base_Core::FilterData m_filterable_data;
        boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> m_supplier;
        volatile bool m_running;
    };

    typedef boost::shared_ptr<MessageSender> MessageSenderPtr;
}

#endif // !defined(AFX_TESTSENDER_H__8D684DED_C14B_4E2F_91F6_029482DB0D1F__INCLUDED_)
