// Copyright (c) 2021 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#include <argumentum/argparse.h>

#include <gtest/gtest.h>

using namespace argumentum;

TEST( Completion, shouldReturnOptionList )
{
   std::string str;
   long depth;

   auto parser = argument_parser{};
#if 1
   std::stringstream strout;
   parser.config().cout( strout );
#else
   // TODO: normal output and completion output are separate settings
   std::vector<std::string> completions;
   parser.config().complete_into( completions );
#endif

   auto params = parser.params();
   params.add_parameter( str, "-s" ).nargs( 1 ).help( "some string" );
   params.add_parameter( depth, "-d", "--depth" ).nargs( 1 ).help( "some depth" );

   // When ---complete is present at the end, completion starts.
   // Parsing is done without setting target values.
   // Errors are ignored.
   // Completions are written to stdout.  --- Option to write to a file.
   // The position of the cursor must be specified by the caller.  If it is not
   // specified, completion is done at the end.
   // Completion can complete a new word or extend the preceeding word.
   //   --complete-new=1      complete a new word after first word
   //   --complete-extend=1   complete by extending the first word
   auto res = parser.parse_args( { "-", "---complete-extend=1" } );

   EXPECT_TRUE( res );
#if 1
   auto pos = strout.str().find( "-s\n-d\n--depth" );
   EXPECT_NE( std::string::npos, pos );
#else
   ASSERT_EQ( 2, completions.size() );
   EXPECT_EQ( "-s", completions[0] );
   EXPECT_EQ( "-d", completions[0] );
   EXPECT_EQ( "--depth", completions[1] );
#endif
}

#if 0
TEST( Completion, shouldRedirectCompletions )
{
   std::vector<std::string> completions;
   auto parser = argument_parser{};
   parser.config().complete_into( completions );
}
#endif

#if 0
TEST( Completion, shouldCompleteShortOptions )
{
   std::string str;
   long depth;
   std::vector<std::string> completions;

   auto parser = argument_parser{};
   parser.config().complete_into( completions );

   auto params = parser.params();
   params.add_parameter( str, "-s" ).nargs( 1 ).help( "some string" );
   params.add_parameter( depth, "-d", "--depth" ).nargs( 1 ).help( "some depth" );

   // This serves more like help (what short options are available) since the
   // uset has to press the desired letter, anyway.
   auto res = parser.parse_args( { "-s", "---complete-extend=1" } );

   EXPECT_TRUE( res );
   ASSERT_EQ( 1, completions.size() );
   EXPECT_EQ( "-sd", completions[0] );
}
#endif