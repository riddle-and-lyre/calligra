/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <qsizepolicy.h>
#include <qpainter.h>
#include <qdom.h>
#include <qvariant.h>

#include <kdebug.h>

#include "objecttree.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "widgetlibrary.h"

#include "spacer.h"

Spacer::Spacer(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
	m_edit = true;
	m_orient = Horizontal;
	setSizeType((SizeType)QSizePolicy::Expanding);
}

void
Spacer::setOrientation(Orientation orient)
{
	SizeType type = sizeType();
	m_orient = orient;
	setSizeType(type);
}

Spacer::SizeType
Spacer::sizeType() const
{
	if(m_orient == Vertical)
		return (SizeType)sizePolicy().verData();
	else
		return (SizeType)sizePolicy().horData();
}

void
Spacer::setSizeType(SizeType size)
{
	if(m_orient == Vertical)
		setSizePolicy(QSizePolicy::Minimum, (QSizePolicy::SizeType)size);
	else
		setSizePolicy( (QSizePolicy::SizeType)size, QSizePolicy::Minimum);
}

void
Spacer::paintEvent(QPaintEvent *ev)
{
	if(!m_edit)
		return;

	QPainter p(this);
	p.eraseRect(0,0,width(), height());
	p.drawLine(0, 0, width()-1, height()-1);
	p.drawLine(0, height()-1, width()-1, 0);
}

bool
Spacer::showProperty(const QString &name)
{
	if((name == "name") || (name == "sizeType") || (name == "orientation") || (name == "geometry"))
		return true;
	else
		return false;
}


void
Spacer::saveSpacer(KFormDesigner::ObjectTreeItem *item, QDomElement &parentNode, QDomDocument &domDoc, bool insideGridLayout)
{
	QDomElement tclass = domDoc.createElement("spacer");
	parentNode.appendChild(tclass);

	if(insideGridLayout)
	{
		tclass.setAttribute("row", item->gridRow());
		tclass.setAttribute("column", item->gridCol());
		if(item->spanMultipleCells())
		{
			tclass.setAttribute("rowspan", item->gridRowSpan());
			tclass.setAttribute("colspan", item->gridColSpan());
		}
	}

	KFormDesigner::FormIO::prop(tclass, domDoc, "name", item->widget()->property("name"), item->widget());

	if(parentNode.tagName() == "widget")
		KFormDesigner::FormIO::prop(tclass, domDoc, "geometry", item->widget()->property("geometry"), item->widget());

	if(!item->widget()->sizeHint().isValid())
		KFormDesigner::FormIO::prop(tclass, domDoc, "sizeHint", item->widget()->property("size"), item->widget());
	else
		KFormDesigner::FormIO::prop(tclass, domDoc, "sizeHint", item->widget()->property("sizeHint"), item->widget());

	KFormDesigner::FormIO::prop(tclass, domDoc, "orientation", item->widget()->property("orientation"), item->widget());
	KFormDesigner::FormIO::prop(tclass, domDoc, "sizeType", item->widget()->property("sizeType"), item->widget());
}

void
Spacer::loadSpacer(QWidget *w, const QDomElement &el)
{
	QString parentTag = el.parentNode().toElement().tagName();
	if(parentTag == "hbox")
	{
		QRect r;
		QDomElement node = el.previousSibling().toElement();
		for(QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
			{
				r = KFormDesigner::FormIO::readProp(n.firstChild().toElement(), w, "geometry").toRect();
				break;
			}
		}
		w->move(r.x() + r.width() +1, 0);
	}
	else if(parentTag == "vbox")
	{
		QRect r;
		QDomElement node = el.previousSibling().toElement();
		for(QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
			{
				r = KFormDesigner::FormIO::readProp(n.firstChild().toElement(), w, "geometry").toRect();
				break;
			}
		}
		w->move(0, r.y() + r.height() +1);
	}
}


#include "spacer.moc"

