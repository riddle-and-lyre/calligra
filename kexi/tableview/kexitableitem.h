/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEITEM_H
#define KEXITABLEITEM_H

#include <qstring.h>
#include <qdatetime.h>
#include <qvaluevector.h>
#include <qvariant.h>

#include <kexidb/connection.h>

//class KexiTableView;

typedef KexiDB::RowData KexiTableItemBase;

class KEXIDATATABLE_EXPORT KexiTableItem : public KexiTableItemBase
{

public:
	KexiTableItem(int numCols);
//js	KexiTableItem(KexiTableView *tableView);

	/*! Clears exisiting column values and inits new \a numCols 
	 columns with empty values. ist of values is resized to \a numCols. */
	void init(int numCols);

	/*! Clears exisiting column values, current number of columns is preserved. */
	void clearValues();

	~KexiTableItem();

//js	void attach(KexiTableView *tableView, bool sorted=false);
//js	void attach(KexiTableView *tableView, int position);

/*js	void setTable(KexiTableView *table) { m_pTable = table; }

	void setHint(QVariant hint)
	{
		m_hint = hint;
	}

	QVariant getHint()
	{
		return m_hint;
	}

	QVariant getValue(const int col) const
	{
		return m_columns[col];
	}

	void setValue(const int col, QVariant value)
	{
		m_columns[col] = value;
	}

	void setInsertItem(bool insertItem);

	bool isInsertItem()
	{
		return m_insertItem;
	}

	int position()
	{
		return m_position;
	}

	QString getText(const int col) const
	{
		return m_columns[col].toString();
	}

	void setText(const int col, const QString &t)
	{
		m_columns[col] = t;
	}

	void setInt(const int col, const int val)
	{
		m_columns[col] = val;
	}

	void setDate(const int col, const QDate &val)
	{
		m_columns[col] = val;
	}

	void setUInt(const int col, const unsigned int val)
	{
		m_columns[col] = val;
	}

	void setFloat(const int col, const float val)
	{
		m_columns[col] = val;
	}

	int getInt(const int col) const
	{
		return m_columns[col].toInt();
	}

	QDate getDate(const int col) const
	{
		return m_columns[col].toDate();
	}

	unsigned int getUInt(const int col) const
	{
		return m_columns[col].toUInt();
	}

	float getFloat(const int col) const
	{
		return (float)m_columns[col].toDouble();
	}
*/
//js	void setUserData(void* userData) {m_userData=userData;}
//js	void* getUserData() const {return m_userData;}

//js	QValueVector<QVariant>	 m_columns;

protected:
//js	QVariant		m_hint;
//js	KexiTableView		*m_pTable;

//js	bool			m_insertItem;

//js	int			m_position;

private:
//js	void *			m_userData;
};

#endif
