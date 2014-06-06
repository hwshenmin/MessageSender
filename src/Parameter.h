#pragma once


struct Parameter
{
    Parameter( const std::string& channel_name, const std::string& domain_name, const std::string& type_name, const std::string& filterable_data, const std::string& data, unsigned long interval  )
        : m_channel_name( channel_name ),
          m_domain_name( domain_name ),
          m_type_name( type_name ),
          m_filterable_data( filterable_data ),
          m_data( data ),
          m_interval( interval )
    {
    }

    std::string m_channel_name;
    std::string m_domain_name;
    std::string m_type_name;
    std::string m_filterable_data;
    std::string m_data;
    unsigned long m_interval;
};

typedef boost::shared_ptr<Parameter> ParameterPtr;
typedef std::vector<ParameterPtr> ParameterPtrList;
