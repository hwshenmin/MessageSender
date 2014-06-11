#include "stdafx.h"
#include "CommandLineHelper.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/PublicationManager.h"

#define RPARAM_LocationKey                  "LocationKey"
#define RPARAM_Number                       "Number"
#define RPARAM_SupplierNumber               "SupplierNumber"
#define RPARAM_Channel                      "Channel"
#define RPARAM_Domain                       "Domain"
#define RPARAM_Type                         "Type"
#define RPARAM_FilterableData               "FilterableData"
#define RPARAM_Region                       "Region"
#define RPARAM_Interval                     "Interval"
#define RPARAM_Data                         "Data"


CommandLineHelper::CommandLineHelper()
{
    unsigned long number            = TA_Base_Core::getRunParamValue( RPARAM_Number, 1 );
    unsigned long supplier_number   = TA_Base_Core::getRunParamValue( RPARAM_SupplierNumber, 1 );
    unsigned long location_key      = TA_Base_Core::getRunParamValue( RPARAM_LocationKey, 100 );
    unsigned long interval          = TA_Base_Core::getRunParamValue( RPARAM_Interval, 1000 );
    std::string channel_name        = TA_Base_Core::RunParams::getInstance().get( RPARAM_Channel );
    std::string domain_name         = TA_Base_Core::RunParams::getInstance().get( RPARAM_Domain );
    std::string type_name           = TA_Base_Core::RunParams::getInstance().get( RPARAM_Type );
    std::string filterable_data     = TA_Base_Core::RunParams::getInstance().get( RPARAM_FilterableData );
    std::string region              = TA_Base_Core::RunParams::getInstance().get( RPARAM_Region );
    std::string data                = TA_Base_Core::RunParams::getInstance().get( RPARAM_Data );
    MyChannelObserverPtr channel_observer( new MyChannelObserver( channel_name ) );
    std::vector< boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> > suppliers;

    if ( 1 == supplier_number )
    {
        boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> supplier( TA_Base_Core::gGetStructuredEventSupplierForChannel( channel_name) );

        for ( size_t i = 0; i < number; ++i )
        {
            suppliers.push_back( supplier );
        }
    }
    else
    {
        supplier_number = std::min<unsigned long>( supplier_number, number );

        for ( size_t i = 0; i < supplier_number; ++i )
        {
            boost::shared_ptr<TA_Base_Core::StructuredEventSupplier> supplier( TA_Base_Core::gGetStructuredEventSupplierForChannel( channel_name) );
            suppliers.push_back( supplier );
        }
    }

    if ( region.empty() )
    {
        for ( size_t i = 0; i < number; ++i )
        {
            m_parameters.push_back( ParameterPtr( new Parameter( channel_name, domain_name, type_name, filterable_data, data, interval, suppliers[i % supplier_number], channel_observer ) ) );
        }
    }
    else
    {
        for ( size_t i = 0; i < number; ++i )
        {
            std::stringstream filterable_data_strm;

            if ( false == filterable_data.empty() )
            {
                filterable_data_strm << filterable_data << ",";
            }

            filterable_data_strm << "Region=" << boost::lexical_cast<size_t>( region ) + i;
            m_parameters.push_back( ParameterPtr( new Parameter( channel_name, domain_name, type_name, filterable_data_strm.str(), data, interval, suppliers[i % supplier_number], channel_observer ) ) );
        }
    }
}
