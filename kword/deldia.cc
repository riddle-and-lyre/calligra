/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kword_doc.h"
#include "kword_frame.h"
#include "kword_page.h"
#include "deldia.h"
#include "deldia.moc"

#include <klocale.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpainter.h>

#include <stdlib.h>

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

/*================================================================*/
KWDeleteDia::KWDeleteDia( QWidget *parent, const char *name, KWGroupManager *_grpMgr, KWordDocument *_doc, DeleteType _type, KWPage *_page )
    : QTabDialog( parent, name, true )
{
    type = _type;
    grpMgr = _grpMgr;
    doc = _doc;
    page = _page;

    setupTab1();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    resize( 300, 150 );
}

/*================================================================*/
void KWDeleteDia::setupTab1()
{
    tab1 = new QWidget( this );

    grid1 = new QGridLayout( tab1, 2, 1, 15, 7 );

    rc = new QLabel( type == ROW ? i18n( "Delete Row:" ) : i18n( "Delete Column:" ), tab1 );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( AlignLeft | AlignBottom );
    grid1->addWidget( rc, 0, 0 );

    value = new QSpinBox( 1, type == ROW ? grpMgr->getRows() : grpMgr->getCols(), 1, tab1 );
    value->resize( value->sizeHint() );
    value->setValue( type == ROW ? grpMgr->getRows() : grpMgr->getCols() );
    grid1->addWidget( value, 1, 0 );

    grid1->addRowSpacing( 0, rc->height() );
    grid1->addRowSpacing( 1, value->height() );
    grid1->setRowStretch( 0, 1 );
    grid1->setRowStretch( 1, 0 );

    grid1->addColSpacing( 0, rc->width() );
    grid1->addColSpacing( 0, value->width() );
    grid1->setColStretch( 0, 1 );

    grid1->activate();

    addTab( tab1, type == ROW ? i18n( "Delete Row" ) : i18n( "Delete Column" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( doDelete() ) );

    resize(minimumSize());
}

/*================================================================*/
void KWDeleteDia::doDelete()
{
    QPainter p;
    p.begin( page );

    if ( type == ROW )
        grpMgr->deleteRow( value->value() - 1 );
    else
        grpMgr->deleteCol( value->value() - 1 );

    page->getCursor()->setFrameSet( doc->getFrameSetNum( grpMgr->getFrameSet( 0, 0 ) ) + 1 );
    doc->drawMarker( *page->getCursor(), &p, page->contentsX(), page->contentsY() );
    page->getCursor()->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( page->getCursor()->getFrameSet() - 1 ) )->getFirstParag(), true );
    page->getCursor()->gotoStartOfParag();
    page->getCursor()->cursorGotoLineStart();
    p.end();

    doc->recalcFrames();
    doc->updateAllFrames();
    doc->updateAllViews( 0L );
    page->recalcCursor();
}
