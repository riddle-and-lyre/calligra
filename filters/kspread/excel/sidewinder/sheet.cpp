/* Sidewinder - Portable library for spreadsheet 
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
   Boston, MA 02111-1307, US
*/

#include "cell.h"
#include "sheet.h"
#include "workbook.h"
#include "ustring.h"

#include <iostream>
#include <map>

namespace Sidewinder
{

class Sheet::Private
{
public:
  Workbook* workbook;
  UString name;
    
  // hash to store cell, FIXME replace with quad-tree
  std::map<unsigned,Cell*> cells;
  unsigned maxRow;
  unsigned maxColumn;
  
  bool visible;
  bool protect;
  double leftMargin;
  double rightMargin;
  double topMargin;
  double bottomMargin;};
};

using namespace Sidewinder;

Sheet::Sheet( Workbook* wb )
{
  d = new Sheet::Private();
  d->workbook = wb;
  d->name = "Sheet"; // FIXME better name ?
  d->maxRow = 0;
  d->maxColumn = 0;
  d->visible      = true;
  d->protect      = false;
  d->leftMargin   = 1.0;
  d->rightMargin  = 1.0;
  d->topMargin    = 1.0;
  d->bottomMargin = 1.0;
}

Sheet::~Sheet()
{
  // TODO delete all cells;
  delete d;
}

Workbook* Sheet::workbook()
{
  return d->workbook;
}

UString Sheet::name() const
{
  return d->name;
}

void Sheet::setName( const UString& name )
{
  d->name = name;
}

Cell* Sheet::cell( unsigned column, unsigned row, bool autoCreate )
{
  unsigned hashed = (row+1)*1024 + column + 1;
  Cell* c = d->cells[ hashed ];
  
  // create cell if necessary
  if( !c && autoCreate )
  {
    c = new Cell( this, column, row );
    d->cells[ hashed ] = c;
    
    if( row > d->maxRow ) d->maxRow = row;
    if( column > d->maxColumn ) d->maxColumn = column;
  }
  
  return c;
}

unsigned Sheet::maxRow() const
{
  return d->maxRow;
}

unsigned Sheet::maxColumn() const
{
  return d->maxColumn;
}

bool Sheet::visible() const
{
  return d->visible;
}

void Sheet::setVisible( bool v )
{
  d->visible = v;
}

bool Sheet::protect() const
{
  return d->protect;
}

void Sheet::setProtect( bool p )
{
  d->protect = p;
}

double Sheet::leftMargin() const
{
  return d->leftMargin;
}

void Sheet::setLeftMargin( double m ) 
{
  d->leftMargin = m;
}

double Sheet::rightMargin() const
{
  return d->rightMargin;
}

void Sheet::setRightMargin( double m ) 
{
  d->rightMargin = m;
}

double Sheet::topMargin() const
{
  return d->topMargin;
}

void Sheet::setTopMargin( double m ) 
{
  d->topMargin = m;
}

double Sheet::bottomMargin() const
{
  return d->bottomMargin;
}

void Sheet::setBottomMargin( double m ) 
{
  d->bottomMargin = m;
}

