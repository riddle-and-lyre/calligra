/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koxmlwriter.h"
#include <kglobal.h> // kMin

static const unsigned int s_indentBufferLength = 100;

KoXmlWriter::KoXmlWriter( QIODevice* dev )
    : m_dev( dev )
{
    m_indentBuffer = new char[ s_indentBufferLength ];
    memset( m_indentBuffer, ' ', s_indentBufferLength );
    *m_indentBuffer = '\n'; // write newline before indentation, in one go

    m_escapeBuffer = new char[s_escapeBufferLen];
}

KoXmlWriter::~KoXmlWriter()
{
    // just to do exactly like QDom does (newline at end of file).
    // This can be removed though.
    writeChar( '\n' );
    delete[] m_indentBuffer;
    delete[] m_escapeBuffer;
}

void KoXmlWriter::startElement( const char* tagName )
{
    // Tell parent that it has children
    if ( !m_tags.isEmpty() ) {
        Tag& parent = m_tags.top();
        if ( !parent.hasChildren ) {
            closeStartElement( parent );
            parent.hasChildren = true;
        }
        // this is for cases like <a>\n<b>\n<c>foo</c>\n</b>text</a> (no \n around text)
        if ( !parent.lastChildIsText ) {
            writeIndent();
        }
        parent.lastChildIsText = false;
    }

    m_tags.push( Tag( tagName ) );
    writeChar( '<' );
    writeCString( tagName );
}

void KoXmlWriter::endElement()
{
    Tag tag = m_tags.pop();
    if ( !tag.hasChildren ) {
        writeCString( "/>" );
    }
    else {
        if ( !tag.lastChildIsText ) {
            writeIndent();
        }
        writeCString( "</" );
        writeCString( tag.tagName );
        writeChar( '>' );
    }
}

void KoXmlWriter::addTextNode( const char* cstr )
{
    Tag& parent = m_tags.top();
    if ( !parent.hasChildren ) {
        closeStartElement( parent );
        parent.hasChildren = true;
    }
    parent.lastChildIsText = true;

    char* escaped = escapeForXML( cstr );
    writeCString( escaped );
    if(escaped != m_escapeBuffer)
        delete[] escaped;
}

void KoXmlWriter::writeDocType( const char* elemName )
{
    Q_ASSERT( m_tags.isEmpty() );
    writeCString( "<!DOCTYPE " );
    writeCString( elemName );
    writeCString( ">\n" );
}

void KoXmlWriter::addAttribute( const char* attrName, const char* value )
{
    writeChar( ' ' );
    writeCString( attrName );
    writeCString("=\"");
    char* escaped = escapeForXML( value );
    writeCString( escaped );
    if(escaped != m_escapeBuffer)
        delete[] escaped;
    writeChar( '"' );
}

void KoXmlWriter::writeIndent()
{
    // +1 because of the leading '\n'
    m_dev->writeBlock( m_indentBuffer, kMin( m_tags.size()+1, s_indentBufferLength ) );
}

void KoXmlWriter::writeString( const QString& str )
{
    // cachegrind says .utf8() is where most of the time is spent
    QCString cstr = str.utf8();
    m_dev->writeBlock( cstr.data(), cstr.size() - 1 );
}

// In case of a reallocation (ret value != m_buffer), the caller owns the return value,
// it must delete it (with [])
char* KoXmlWriter::escapeForXML( const char* source ) const
{
    // we're going to be pessimistic on char length; so lets make the outputLength less
    // the amount one char can take: 6
    char* destBoundary = m_escapeBuffer + s_escapeBufferLen - 6;
    char* destination = m_escapeBuffer;
    char* output = m_escapeBuffer;
    for ( ;; ) {
        if(destination >= destBoundary) {
            // When we come to realize that our escaped string is going to
            // be bigger than the escape buffer (this shouldn't happen very often...),
            // we drop the idea of using it, and we allocate a bigger buffer.
            uint len = qstrlen( source ); // expensive...
            uint newLength = len * 6 + 1; // worst case. 6 is due to &quot; and &apos;
            char* buffer = new char[ newLength ];
            destBoundary = buffer + newLength;
            uint amountOfCharsAlreadyCopied = destination - m_escapeBuffer;
            memcpy( buffer, m_escapeBuffer, amountOfCharsAlreadyCopied );
            output = buffer;
            destination = buffer + amountOfCharsAlreadyCopied;
        }
        switch( *source ) {
        case 60: // <
            memcpy( destination, "&lt;", 4 );
            destination += 4;
            break;
        case 62: // >
            memcpy( destination, "&gt;", 4 );
            destination += 4;
            break;
        case 34: // "
            memcpy( destination, "&quot;", 6 );
            destination += 6;
            break;
#if 0 // needed?
        case 39: // '
            memcpy( destination, "&apos;", 6 );
            destination += 6;
            break;
#endif
        case 38: // &
            memcpy( destination, "&amp;", 5 );
            destination += 5;
            break;
        case 0:
            *destination = '\0';
            return output;
        default:
            *destination++ = *source++;
            continue;
        }
        ++source;
    }
    // NOTREACHED (see case 0)
    return output;
}
