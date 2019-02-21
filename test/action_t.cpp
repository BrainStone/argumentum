// Copyright (c) 2018, 2019 Marko Mahnič
// License: MIT. See LICENSE in the root of the project.

#include "../src/argparser.h"

#include <gtest/gtest.h>

using namespace argparse;

TEST( ArgumentParserActionTest, shouldModifyArgumentWithAction )
{
   class TestAction: public argparse::ArgumentParser::Action
   {
      std::optional<std::string> exec( argparse::ArgumentParser::Value& target,
            const std::string &value ) override
      {
         if ( value.find( "1" ) != std::string::npos )
            return value + " Has One";
         return value;
      }
   };

   std::string result;
   std::stringstream strout;
   auto parser = ArgumentParser::create();
   parser.add_argument( result, "-v" ).nargs(1).action( std::make_shared<TestAction>() );

   auto res = parser.parse_args( { "-v", "1" } );
   EXPECT_TRUE( res.errors.empty() );
   EXPECT_EQ( "1 Has One", result );

   res = parser.parse_args( { "-v", "2" } );
   EXPECT_TRUE( res.errors.empty() );
   EXPECT_EQ( "2", result );

   res = parser.parse_args( { "-v", "212" } );
   EXPECT_TRUE( res.errors.empty() );
   EXPECT_EQ( "212 Has One", result );
}

