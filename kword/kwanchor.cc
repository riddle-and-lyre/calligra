/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwanchor.h"
#include "kwcommand.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include <kdebug.h>
#include <qdom.h>

KWAnchor::KWAnchor( KWTextDocument *textdoc, KWFrame * frame )
    : KWTextCustomItem( textdoc ),
      // We store the frame as frameset+framenum for undo/redo purposes
      m_frameset( frame->getFrameSet() ),
      m_frameNum( m_frameset->getFrameFromPtr( frame ) )
{
}

KWAnchor::~KWAnchor()
{
    kdDebug() << "KWAnchor::~KWAnchor" << endl;
}

KWFrame * KWAnchor::frame() const
{
    ASSERT( !m_deleted );
    if ( m_deleted )
        return 0L;
    return m_frameset->getFrame( m_frameNum );
}

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg )
{
    ASSERT( !m_deleted );
    if ( m_deleted ) // can't happen !
        return;

    if ( placement() != PlaceInline ) {
        x = xpos;
        y = ypos;
    }

    int paragy = paragraph()->rect().y();
    //kdDebug(32001) << "KWAnchor::draw " << x << "," << y << " paragy=" << paragy
    //               << "  " << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
    KWDocument * doc = m_frameset->kWordDocument();
    KWTextFrameSet * fs = textDocument()->textFrameSet();

    // 1 - move frame. We have to do this here since QTextCustomItem doesn't
    // have a way to tell us when we are placed (during formatting)
    QPoint cPoint;
    if ( fs->internalToContents( QPoint( x, y+paragy ), cPoint ) )
    {
        //kdDebug(32001) << "KWAnchor::draw moving frame to [zoomed pos] " << cPoint.x() << "," << cPoint.y() << endl;
        // Move the frame to position x,y.
        frame()->moveTopLeft( KoPoint( cPoint.x() / doc->zoomedResolutionX(), cPoint.y() / doc->zoomedResolutionY() ) );
    }

    // 2 - draw

    p->save();
    // Determine crect in contents coords
    QRect crect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
    if ( fs->internalToContents( crect.topLeft(), cPoint ) )
        crect.moveTopLeft( cPoint );
    // and go back to contents coord system
    QPoint iPoint( 0, paragy );
    if ( fs->internalToContents( iPoint, cPoint ) )
    {
        //kdDebug(32002) << "translate " << -cPoint.x() << "," << -cPoint.y() << endl;
        p->translate( -cPoint.x(), -cPoint.y() );
    }
    // Draw the frame
    QColorGroup cg2( cg );
    m_frameset->drawContents( p, crect, cg2, false /*?*/, false /*?*/ );
    p->restore();
}

QSize KWAnchor::size() const
{
    KWDocument * doc = m_frameset->kWordDocument();
    KWFrame * f = frame();
    ASSERT( f );
    return QSize( doc->zoomItX( f->width() ), doc->zoomItY( f->height() ) );
}

void KWAnchor::resize()
{
    if ( m_deleted )
        return;
    QSize s = size();
    width = s.width();
    height = s.height();
    //kdDebug() << "KWAnchor::resize " << width << "x" << height << endl;
}

KWTextDocument * KWAnchor::textDocument() const
{
    return static_cast<KWTextDocument *>( parent );
}

void KWAnchor::addDeleteCommand( KMacroCommand * macroCmd )
{
    kdDebug() << "KWAnchor::addDeleteCommand" << endl;
    KWDeleteFrameCommand * cmd = new KWDeleteFrameCommand( QString::null, m_frameset->kWordDocument(), frame() );
    macroCmd->addCommand( cmd );
    cmd->execute(); // deletes the frame
}

void KWAnchor::save( QDomElement &formatElem )
{
    formatElem.setAttribute( "id", 6 ); // code for an anchor
    QDomElement anchorElem = formatElem.ownerDocument().createElement( "ANCHOR" );
    formatElem.appendChild( anchorElem );
    anchorElem.setAttribute( "type", "frameset" ); // the only possible value currently
    KWDocument * doc = textDocument()->textFrameSet()->kWordDocument();
    // ## TODO save the frame number as well ? Only the first frame ? to be determined
    // ## or maybe use len=<number of frames>. Difficult :}
    int num = doc->getFrameSetNum( frame()->getFrameSet() );
    anchorElem.setAttribute( "instance", num );
    return;
}
