// Copyright (c) 2021 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace argumentum {

// An external application (like Bash) passes the completion params in
// ---complete-* options.
struct CompletionParams
{
   // The arguments associated with completion. These are turned into options
   // for the completion parser.
   std::vector<std::string> completeArgs;

   // The remaining arguments after completion arguments are removed.
   // Completion is done on this list.
   std::vector<std::string_view> programArgs;

   // The index of the argument to complete.  A negative index denotes the last
   // argument.  This is an index into programArgs, not into the original
   // arguments.  If all ---complete options are at the end of the command
   // line, these indices are the same.
   int argumentIndex = -1;

   // The position of the cursor inside the argument.  A negative value denotes
   // the end of the argument.
   int byteOffset = -1;

   // Set to true if --complete-new is used.  In this case the cursor is between
   // arguments and there is whitespace before and after the cursor.
   bool isNewArgument = false;

public:
   void splitArguments( std::vector<std::string>::const_iterator ibegin,
         std::vector<std::string>::const_iterator iend );

   void parseCompletionArguments();

   std::string getPrefix() const;
};

}   // namespace argumentum
