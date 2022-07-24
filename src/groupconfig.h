// Copyright (c) 2018, 2019, 2020 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#pragma once

#ifndef ARGUMENTUM_BUILD_MODULE
#include "nomodule.h"

#include <memory>
#include <string_view>
#endif

namespace argumentum {

class OptionGroup;

ARGUMENTUM_EXPORT
class GroupConfig
{
   std::shared_ptr<OptionGroup> mpGroup;

public:
   GroupConfig( std::shared_ptr<OptionGroup> pGroup );

   // Set the title of the group that will be displayed in the generated help.
   GroupConfig& title( std::string_view title );

   // Set the description of the group that will be displayed in the generated
   // help.
   GroupConfig& description( std::string_view description );

   // Set to true if at least one option from the group must be present in the
   // input arguments.
   GroupConfig& required( bool isRequired = true );
};

}   // namespace argumentum
