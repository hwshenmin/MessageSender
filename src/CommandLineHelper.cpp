#include "stdafx.h"
#include "CommandLineHelper.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{

    CommandLineHelper::CommandLineHelper()
    {
        unsigned long number            = TA_Base_Core::getRunParamValue( "Number", 1 );
        unsigned long location_key      = TA_Base_Core::getRunParamValue( RPARAM_LOCATIONKEY, 100 );
        std::string channel_name        = TA_Base_Core::RunParams::getInstance().get( "Channel" );
        std::string domain_name         = TA_Base_Core::RunParams::getInstance().get( "Domain" );
        std::string type_name           = TA_Base_Core::RunParams::getInstance().get( "Type" );
        std::string filterable_data     = TA_Base_Core::RunParams::getInstance().get( "FilterableData" );
        std::string region              = TA_Base_Core::RunParams::getInstance().get( "Region" );
        unsigned long interval          = TA_Base_Core::getRunParamValue( "Interval", 1000 );
        std::string data                = TA_Base_Core::RunParams::getInstance().get( "Data" );

        if ( region.empty() )
        {
            for ( size_t i = 0; i < number; ++i )
            {
                m_parameters.push_back( ParameterPtr( new Parameter( channel_name, domain_name, type_name, filterable_data, data, interval ) ) );
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
                m_parameters.push_back( ParameterPtr( new Parameter( channel_name, domain_name, type_name, filterable_data_strm.str(), data, interval ) ) );
            }
        }
    }

}
