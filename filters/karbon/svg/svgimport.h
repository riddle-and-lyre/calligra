/* This file is part of the KDE project
   Copyright (C) 2002, Sven L�ppken <sven@kde.org>

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

#ifndef __SVGIMPORT_H__
#define __SVGIMPORT_H__

#include <koFilter.h>
#include <qdom.h>
#include <qptrstack.h>
#include <core/vfill.h>
#include <core/vpath.h>
#include <core/vdocument.h>

class VGroup;

class SvgImport : public KoFilter
{
	Q_OBJECT

public:
	SvgImport(KoFilter *parent, const char *name, const QStringList&);
	virtual ~SvgImport();

	virtual KoFilter::ConversionStatus convert(const QCString& from, const QCString& to);

protected:
	class GraphicsContext
	{
	public:
		GraphicsContext()
		{
			stroke.setType( VStroke::none ); // default is no stroke
		}
		VFill	fill;
		VStroke	stroke;
	};

	void parseGroup( VGroup *, const QDomElement & );
	void parseStyle( VObject *, const QDomElement & );
	QDomDocument inpdoc;
	QDomDocument outdoc;
	void convert();

private:
	VDocument m_document;
	QPtrStack<GraphicsContext> m_gc;
};

#endif
