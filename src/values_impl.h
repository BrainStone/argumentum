// Copyright (c) 2018, 2019 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#pragma once

#include "convert.h"
#include "notifier.h"
#include "values.h"
#include <functional>
#include <string>

namespace argparse {

CPPARGPARSE_INLINE void Value::setValue( std::string_view value, AssignAction action, Environment& env )
{
   ++mAssignCount;
   ++mOptionAssignCount;
   if ( action == nullptr )
      action = getDefaultAction();
   if ( action )
      action( *this, std::string{ value }, env );
}

CPPARGPARSE_INLINE void Value::setDefault( AssignDefaultAction action )
{
   if ( action ) {
      ++mAssignCount;
      action( *this );
   }
}

CPPARGPARSE_INLINE void Value::markBadArgument()
{
   // Increase the assign count so that flagValue will not be used.
   ++mOptionAssignCount;
   mHasErrors = true;
}

CPPARGPARSE_INLINE int Value::getAssignCount() const
{
   return mAssignCount;
}

CPPARGPARSE_INLINE int Value::getOptionAssignCount() const
{
   return mOptionAssignCount;
}

CPPARGPARSE_INLINE void Value::onOptionStarted()
{
   mOptionAssignCount = 0;
}

CPPARGPARSE_INLINE void Value::reset()
{
   mAssignCount = 0;
   mOptionAssignCount = 0;
   mHasErrors = false;
   doReset();
}

CPPARGPARSE_INLINE void Value::doReset()
{}

CPPARGPARSE_INLINE AssignAction VoidValue::getDefaultAction()
{
   return {};
}

}   // namespace argparse