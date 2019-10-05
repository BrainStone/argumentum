﻿// Copyright (c) 2018, 2019 Marko Mahnič
// License: MPL2. See LICENSE in the root of the project.

#pragma once

#include "helpformatter_i.h"

#include "argparser.h"

#include <regex>

namespace argparse {

class Writer
{
   std::ostream& stream;
   size_t position = 0;
   size_t lastWritePosition = 0;
   size_t width = 80;
   bool startOfParagraph = true;
   std::string indent;

public:
   Writer( std::ostream& outStream, size_t widthBytes = 80 )
      : stream( outStream )
      , width( widthBytes )
   {}

   void setIndent( size_t indentBytes )
   {
      if ( indentBytes > width )
         indentBytes = width;
      indent = indentBytes == 0 ? "" : std::string( indentBytes, ' ' );
   }

   void write( std::string_view text )
   {
      auto blocks = splitIntoParagraphs( text );
      for ( auto block : blocks ) {
         if ( block.empty() )
            startParagraph();
         else {
            write_paragraph( block );
            startOfParagraph = false;
         }
      }
   }

   void startLine()
   {
      if ( position > 0 )
         stream << "\n";
      position = 0;
      lastWritePosition = 0;
      startOfParagraph = false;
   }

   void skipToColumnOrNewLine( size_t column )
   {
      if ( column >= width || column < position )
         startLine();
      else if ( column > position ) {
         stream << std::string( column - position, ' ' );
         position = column;
      }
      startOfParagraph = false;
   }

   void startParagraph()
   {
      if ( !startOfParagraph ) {
         startLine();
         stream << "\n";
         startOfParagraph = true;
      }
   }

   static std::vector<std::string_view> splitIntoWords( std::string_view text )
   {
      std::vector<std::string_view> words;

      size_t pos = 0;
      while ( pos < text.size() ) {
         while ( pos < text.size() && iswspace( text[pos] ) )
            ++pos;

         size_t end = pos;
         while ( end < text.size() && !iswspace( text[end] ) )
            ++end;

         if ( end > pos )
            words.push_back( text.substr( pos, end - pos ) );

         pos = end;
      }

      return words;
   }

   // Paragraphs are delimited by two or more consecutive newlines intermixed
   // with other whitespace. The paragraph delimiters are returned as empty blocks.
   static std::vector<std::string_view> splitIntoParagraphs( std::string_view text )
   {
      auto rxParagraph = std::regex( "[ \t]*\n[ \t]*\n\\s*" );
      std::vector<std::string_view> res;

      auto it = std::cregex_iterator( text.data(), text.data() + text.size(), rxParagraph );
      auto iend = std::cregex_iterator();
      auto lastPosition = 0;
      for ( ; it != iend; ++it ) {
         auto match = std::cmatch( *it );
         if ( match.position() == 0 )
            res.emplace_back();
         else {
            res.push_back( text.substr( lastPosition, match.position() - lastPosition ) );
            res.emplace_back();
         }
         lastPosition = match.position() + match.length();
      }

      if ( lastPosition < text.size() )
         res.push_back( text.substr( lastPosition ) );
      return res;
   }

private:
   void write_paragraph( std::string_view text )
   {
      auto words = splitIntoWords( text );
      for ( auto word : words ) {
         auto newpos = position + ( position == 0 ? indent.size() : 1 ) + word.size();
         if ( newpos > width )
            startLine();
         else if ( position > 0 && position == lastWritePosition ) {
            stream << " ";
            ++position;
         }

         if ( position == 0 && indent.size() > 0 ) {
            stream << indent;
            position = indent.size();
         }

         stream.write( word.data(), word.size() );
         position += word.size();
         lastWritePosition = position;
      }
   }
};

inline void HelpFormatter::format( const argument_parser& parser, std::ostream& out )
{
   auto config = parser.getConfig();
   auto args = parser.describe_arguments();
   auto iendpos = std::stable_partition(
         std::begin( args ), std::end( args ), []( auto&& d ) { return d.is_positional(); } );
   auto iendreq = std::stable_partition(
         iendpos, std::end( args ), []( auto&& d ) { return d.is_required(); } );
   auto hasPositional = std::begin( args ) != iendpos;
   auto hasRequired = iendpos != iendreq;
   auto hasOptional = iendreq != std::end( args );
   auto desctiptionIndent = deriveMaxArgumentWidth( args ) + mArgumentIndent + 1;
   if ( desctiptionIndent > mMaxDescriptionIndent )
      desctiptionIndent = mMaxDescriptionIndent;

   auto writeArguments = [&]( auto&& writer, auto&& start, auto&& end ) {
      writer.startLine();
      for ( auto it = start; it != end; ++it ) {
         writer.setIndent( mArgumentIndent );
         writer.write( formatArgument( *it ) );
         writer.skipToColumnOrNewLine( desctiptionIndent );
         writer.setIndent( desctiptionIndent );
         writer.write( it->help );
         writer.startLine();
      }
      writer.startParagraph();
      writer.setIndent( 0 );
   };

   Writer writer( out, mTextWidth );
   if ( !config.usage.empty() ) {
      writer.write( "usage: " );
      writer.write( config.usage );
      writer.startParagraph();
   }

   if ( !config.description.empty() ) {
      writer.write( config.description );
      writer.startParagraph();
   }

   if ( hasPositional ) {
      writer.write( "positional arguments:" );
      writeArguments( writer, std::begin( args ), iendpos );
   }

   if ( hasRequired ) {
      writer.write( "required arguments:" );
      writeArguments( writer, iendpos, iendreq );
   }

   if ( hasOptional ) {
      writer.write( "optional arguments:" );
      writeArguments( writer, iendreq, std::end( args ) );
   }

   if ( !config.epilog.empty() ) {
      writer.write( config.epilog );
      writer.startParagraph();
   }
}

}   // namespace argparse
