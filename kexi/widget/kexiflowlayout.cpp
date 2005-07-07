/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "kexiflowlayout.h"

#include <kdebug.h>

/// Iterator class

class KexiFlowLayoutIterator : public QGLayoutIterator
{
	public:
		KexiFlowLayoutIterator( QPtrList<QLayoutItem> *list )
		  : m_idx(0), m_list( list )
		{}
		uint count() const;
		QLayoutItem *current();
		QLayoutItem *next();
		QLayoutItem *takeCurrent();

	private:
		int m_idx;
		QPtrList<QLayoutItem> *m_list;
};

uint
KexiFlowLayoutIterator::count() const
{
	return m_list->count();
}

QLayoutItem *
KexiFlowLayoutIterator::current()
{
	return (m_idx < (int)count()) ? m_list->at(m_idx) : 0;
}

QLayoutItem *
KexiFlowLayoutIterator::next()
{
	m_idx++;
	return current();
}

QLayoutItem *
KexiFlowLayoutIterator::takeCurrent()
{
	return (m_idx < (int)count()) ? m_list->take(m_idx) : 0;
}

//// The layout itself

KexiFlowLayout::KexiFlowLayout(QWidget *parent, int border, int space, const char *name)
 : QLayout(parent, border, space, name)
{
	m_orientation = Vertical;
	m_justify = false;
	m_cached_width = 0;
}

KexiFlowLayout::KexiFlowLayout(QLayout* parent, int space, const char *name)
 : QLayout( parent, space, name )
{
	m_orientation = Vertical;
	m_justify = false;
	m_cached_width = 0;
}

KexiFlowLayout::KexiFlowLayout(int space, const char *name)
 : QLayout(space, name)
 {
 	m_orientation = Vertical;
	m_justify = false;
	m_cached_width = 0;
 }

KexiFlowLayout::~KexiFlowLayout()
{
	deleteAllItems();
}

void
KexiFlowLayout::addItem(QLayoutItem *item)
{
	m_list.append(item);
}

QLayoutIterator
KexiFlowLayout::iterator()
{
	return QLayoutIterator( new KexiFlowLayoutIterator(&m_list) );
}

void
KexiFlowLayout::invalidate()
{
	QLayout::invalidate();
	m_cached_sizeHint = QSize();
	m_cached_minSize = QSize();
	m_cached_width = 0;
}

bool
KexiFlowLayout::isEmpty()
{
	return m_list.isEmpty();
}

bool
KexiFlowLayout::hasHeightForWidth() const
{
	return (m_orientation == Horizontal);
}

int
KexiFlowLayout::heightForWidth(int w) const
{
	if(m_cached_width != w) {
		// workaround to allow this method to stay 'const'
		KexiFlowLayout *mthis = (KexiFlowLayout*)this;
		int h = mthis->simulateLayout( QRect(0,0,w,0) );
		mthis->m_cached_hfw = h;
		mthis->m_cached_width = w;
		return h;
 	}
	return m_cached_hfw;
}

QSize
KexiFlowLayout::sizeHint() const
{
	if(m_cached_sizeHint.isEmpty()) {
		KexiFlowLayout *mthis = (KexiFlowLayout*)this;
		QRect r = geometry().isValid() ? geometry() : QRect(0, 0, 400, 400);
		mthis->simulateLayout(r);
	}
	return m_cached_sizeHint;
}

QSize
KexiFlowLayout::minimumSize() const
{
	if(m_cached_minSize.isEmpty()) {
		KexiFlowLayout *mthis = (KexiFlowLayout*)this;
		QRect r = geometry().isValid() ? geometry() : QRect(0, 0, 400, 400);
		mthis->simulateLayout(r);
	}
	return m_cached_minSize;
}

QSizePolicy::ExpandData
KexiFlowLayout::expanding() const
{
	if(m_orientation == Vertical)
		return QSizePolicy::Vertically;
	else
		return QSizePolicy::Horizontally;
}

void
KexiFlowLayout::setGeometry(const QRect &r)
{
	QLayout::setGeometry(r);
	if(m_orientation == Horizontal)
		doHorizontalLayout(r);
	else
		doVerticalLayout(r);
}

int
KexiFlowLayout::simulateLayout(const QRect &r)
{
	if(m_orientation == Horizontal)
		return doHorizontalLayout(r, true);
	else
		return doVerticalLayout(r, true);
}

int
KexiFlowLayout::doHorizontalLayout(const QRect &r, bool testOnly)
{
	int x = r.x();
	int y = r.y();
	int h = 0; // height of this line
	int availableSpace = r.width() + spacing();
	int expandingWidgets=0; // number of widgets in the line with QSizePolicy == Expanding
	QPtrListIterator<QLayoutItem> it(m_list);
	QPtrList<QLayoutItem> currentLine;
	QLayoutItem *o;
	QSize minSize, sizeHint(20, 20);
	int minSizeHeight = 0 - spacing();

	while ( (o = it.current()) != 0 ) {
		if(o->isEmpty()) { /// do not consider hidden widgets
			++it;
			continue;
		}

		if (x + o->sizeHint().width() > r.right() && h > 0) {
			// do the layout of current line
			QPtrListIterator<QLayoutItem> it2(currentLine);
			QLayoutItem *item;
			int wx = r.x();
			int minSizeWidth=0 - spacing(), lineMinHeight=0;
			while( (item = it2.current()) != 0 ) {
				QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
				QSize itemMinSize = item->minimumSize(); // a while to get them
				QSize s;
				if(m_justify) {
					if(expandingWidgets != 0) {
						if(item->expanding() == QSizePolicy::Horizontally || item->expanding() == QSizePolicy::BothDirections)
							s = QSize( QMIN(itemSizeHint.width() + availableSpace / expandingWidgets
								, r.width()), itemSizeHint.height() );
						else
							s = QSize( QMIN(itemSizeHint.width(), r.width()), itemSizeHint.height() );
					}
					else
						s = QSize( QMIN(itemSizeHint.width() + availableSpace / currentLine.count()
							, r.width()), itemSizeHint.height() );
				}
				else
					s = QSize ( QMIN(itemSizeHint.width(), r.width()), itemSizeHint.height() );
				if(!testOnly)
					item->setGeometry( QRect(QPoint(wx, y), s) );
				wx = wx + s.width() + spacing();
				minSizeWidth = spacing() + itemMinSize.width();
				lineMinHeight = QMAX( lineMinHeight, itemMinSize.height() );
				++it2;
			}
			sizeHint = sizeHint.expandedTo( QSize(r.width() - availableSpace, 0) );
			minSize = minSize.expandedTo( QSize(minSizeWidth, 0) );
			minSizeHeight = minSizeHeight + spacing() + lineMinHeight;
			// start a new line
			y = y + spacing() + h;
			h = 0;
			x = r.x();
			currentLine.clear();
			expandingWidgets = 0;
			availableSpace = r.width() + spacing();
		}

		QSize oSizeHint = o->sizeHint(); // we cache these ones because it can take a while to get it (eg for child layouts)
		x = x + spacing() + oSizeHint.width();
		h = QMAX( h,  oSizeHint.height() );
		currentLine.append(o);
		if(o->expanding() == QSizePolicy::Horizontally || o->expanding() == QSizePolicy::BothDirections)
			++expandingWidgets;
		availableSpace = QMAX(0, availableSpace - spacing() - oSizeHint.width());
		++it;
	}

	// don't forget to layout the last line
	QPtrListIterator<QLayoutItem> it2(currentLine);
	QLayoutItem *item;
	int wx = r.x();
	int minSizeWidth=0 - spacing(), lineMinHeight=0;
	while( (item = it2.current()) != 0 ) {
		QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
		QSize itemMinSize = item->minimumSize(); // a while to get them
		QSize s;
		if(m_justify) {
			if(expandingWidgets != 0) {
				if(item->expanding() == QSizePolicy::Horizontally || item->expanding() == QSizePolicy::BothDirections)
					s = QSize( QMIN(itemSizeHint.width() + availableSpace / expandingWidgets
						, r.width()), itemSizeHint.height() );
				else
					s = QSize( QMIN(itemSizeHint.width(), r.width()), itemSizeHint.height() );
			}
			else
				s = QSize( QMIN(itemSizeHint.width() + availableSpace / currentLine.count()
					, r.width()), itemSizeHint.height() );
		}
		else
			s = QSize ( QMIN(itemSizeHint.width(), r.width()), itemSizeHint.height() );
		if(!testOnly)
			item->setGeometry( QRect(QPoint(wx, y), s) );
		wx = wx + s.width() + spacing();
		minSizeWidth = spacing() + itemMinSize.width();
		lineMinHeight = QMAX( lineMinHeight, itemMinSize.height() );
		++it2;
	}
	sizeHint = sizeHint.expandedTo( QSize(r.width() - availableSpace, y + spacing() + h) );
	minSizeHeight = minSizeHeight + spacing() + lineMinHeight;
	minSize = minSize.expandedTo( QSize(minSizeWidth, minSizeHeight) );

	// store sizeHint() and minimumSize()
	m_cached_sizeHint = sizeHint;
	m_cached_minSize = minSize;
	// return our height
	return y + h - r.y();
}

int
KexiFlowLayout::doVerticalLayout(const QRect &r, bool testOnly)
{
	int x = r.x();
	int y = r.y();
	int w = 0; // width of this line
	int availableSpace = r.height() + spacing();
	int expandingWidgets=0; // number of widgets in the line with QSizePolicy == Expanding
	QPtrListIterator<QLayoutItem> it(m_list);
	QPtrList<QLayoutItem> currentLine;
	QLayoutItem *o;
	QSize minSize, sizeHint(20, 20);
	int minSizeWidth = 0 - spacing();

	while ( (o = it.current()) != 0 ) {
		if(o->isEmpty()) { /// do not consider hidden widgets
			++it;
			continue;
		}

		if (y + o->sizeHint().height() > r.bottom() && w > 0) {
			// do the layout of current line
			QPtrListIterator<QLayoutItem> it2(currentLine);
			QLayoutItem *item;
			int wy = r.y();
			int minSizeHeight = 0 - spacing(), colMinWidth=0;
			while( (item = it2.current()) != 0 ) {
				QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
				QSize itemMinSize = item->minimumSize(); // a while to get them
				QSize s;
				if(m_justify) {
					if(expandingWidgets != 0) {
						if(item->expanding() == QSizePolicy::Vertically || item->expanding() == QSizePolicy::BothDirections)
							s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height() + availableSpace / expandingWidgets
								, r.height()) );
						else
							s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height(), r.height()) );
					}
					else
						s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height() + availableSpace / currentLine.count()
							, r.height()) );
				}
				else
					s = QSize (  itemSizeHint.width(), QMIN(itemSizeHint.height(), r.height()) );
				if(!testOnly)
					item->setGeometry( QRect(QPoint(x, wy), s) );
				wy = wy + s.height() + spacing();
				minSizeHeight = spacing() + itemMinSize.height();
				colMinWidth = QMAX( colMinWidth, itemMinSize.width() );
				++it2;
			}
			sizeHint = sizeHint.expandedTo( QSize(0, r.height() - availableSpace) );
			minSize = minSize.expandedTo( QSize(0, minSizeHeight) );
			minSizeWidth = minSizeWidth + spacing() + colMinWidth;
			// start a new column
			x = x + spacing() + w;
			w = 0;
			y = r.y();
			currentLine.clear();
			expandingWidgets = 0;
			availableSpace = r.height() + spacing();
		}

		QSize oSizeHint = o->sizeHint(); // we cache these ones because it can take a while to get it (eg for child layouts)
		y = y + spacing() + oSizeHint.height();
		w = QMAX( w,  oSizeHint.width() );
		currentLine.append(o);
		if(o->expanding() == QSizePolicy::Vertically || o->expanding() == QSizePolicy::BothDirections)
			++expandingWidgets;
		availableSpace = QMAX(0, availableSpace - spacing() - oSizeHint.height());
		++it;
	}

	// don't forget to layout the last line
	QPtrListIterator<QLayoutItem> it2(currentLine);
	QLayoutItem *item;
	int wy = r.y();
	int minSizeHeight = 0 - spacing(), colMinWidth=0;
	while( (item = it2.current()) != 0 ) {
		QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
		QSize itemMinSize = item->minimumSize(); // a while to get them
		QSize s;
		if(m_justify) {
			if(expandingWidgets != 0) {
				if(item->expanding() == QSizePolicy::Vertically || item->expanding() == QSizePolicy::BothDirections)
					s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height() + availableSpace / expandingWidgets
						, r.height()) );
				else
					s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height(), r.height()) );
			}
			else
				s = QSize( itemSizeHint.width(), QMIN(itemSizeHint.height() + availableSpace / currentLine.count()
					, r.height()) );
		}
		else
			s = QSize (  itemSizeHint.width(), QMIN(itemSizeHint.height(), r.height()) );
		if(!testOnly)
			item->setGeometry( QRect(QPoint(x, wy), s) );
		wy = wy + s.height() + spacing();
		minSizeHeight = spacing() + itemMinSize.height();
		colMinWidth = QMAX( colMinWidth, itemMinSize.width() );
		++it2;
	}
	sizeHint = sizeHint.expandedTo( QSize( x + spacing() + w, r.height() - availableSpace) );
	minSizeWidth = minSizeWidth + spacing() + colMinWidth;
	minSize = minSize.expandedTo( QSize(minSizeWidth, minSizeHeight) );

	// store sizeHint() and minimumSize()
	m_cached_sizeHint = sizeHint;
	m_cached_minSize = minSize;
	// return our width
	return x + w - r.x();
}

