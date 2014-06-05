#pragma once
#include "Parameter.h"


namespace TA_Base_Core
{

    struct CommandLineHelper
    {
        CommandLineHelper();
        ParameterPtrList get_parameters() { return m_parameters; }

    private:

        ParameterPtrList m_parameters;
    };

}
