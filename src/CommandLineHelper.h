#pragma once
#include "Parameter.h"


struct CommandLineHelper
{
    CommandLineHelper();
    ParameterPtrList get_parameters() { return m_parameters; }

private:

    ParameterPtrList m_parameters;
};
