/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/tableschema.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>

#include <assert.h>

#include <kdebug.h>

using namespace KexiDB;

TableSchema::TableSchema(const QString& name)
	: FieldList(true)
	, SchemaData(KexiDB::TableObjectType)
	, m_conn(0)
	, m_query(0)
	, m_isKexiDBSystem(false)
{
	m_name = name;
	init();
}

TableSchema::TableSchema(const SchemaData& sdata)
	: FieldList(true)
	, SchemaData(sdata)
	, m_conn(0)
	, m_query(0)
	, m_isKexiDBSystem(false)
{
	init();
}

TableSchema::TableSchema()
	: FieldList(true)
	, SchemaData(KexiDB::TableObjectType)
	, m_conn(0)
	, m_query(0)
	, m_isKexiDBSystem(false)
{
	init();
}

void TableSchema::init()
{
	m_indices.setAutoDelete( true );
	m_pkey = new IndexSchema(this);
	m_indices.append(m_pkey);
}

TableSchema::TableSchema(const TableSchema& ts)
	: FieldList(static_cast<const FieldList&>(ts))
	, SchemaData(static_cast<const SchemaData&>(ts))
	, m_conn( ts.m_conn )
	, m_query(0) //not cached
	, m_isKexiDBSystem(false)
{
	m_name = ts.m_name;
	m_indices.setAutoDelete( true );
	m_pkey = 0; //will be copied

	//deep copy all members
	IndexSchema::ListIterator idx_it(ts.m_indices);
	for (;idx_it.current();++idx_it) {
		IndexSchema *idx = new IndexSchema(*idx_it.current());
		idx->m_tableSchema = this;
		if (idx->isPrimaryKey()) {//assign pkey
			m_pkey = idx;
		}
		m_indices.append(idx);
	}
}

// used by Connection
TableSchema::TableSchema(Connection *conn, const QString & name)
	: FieldList(true)
	, SchemaData(KexiDB::TableObjectType)
	, m_conn( conn )
	, m_query(0)
	, m_isKexiDBSystem(false)
{
	assert(conn);
	m_name = name;
	m_indices.setAutoDelete( true );
	m_pkey = new IndexSchema(this);
	m_indices.append(m_pkey);
}

TableSchema::~TableSchema()
{
	if (m_conn)
		m_conn->removeMe( this );
	delete m_query;
}

void TableSchema::setPrimaryKey(IndexSchema *pkey)
{
	if (m_pkey && m_pkey!=pkey) {
		if (m_pkey->fieldCount()==0) {//this is empty key, probably default - remove it
			m_indices.remove(m_pkey);
		}
		else {
			m_pkey->setPrimaryKey(false); //there can be only one pkey..
			//thats ok, the old pkey is still on indices list, if not empty
		}
//		m_pkey=0; 
	}
	
	if (!pkey) {//clearing - set empty pkey
		pkey = new IndexSchema(this);
	}
	m_pkey = pkey; //todo
	m_pkey->setPrimaryKey(true);
}

KexiDB::FieldList& TableSchema::addField(KexiDB::Field* field)
{
	assert(field);
	FieldList::addField(field);
	field->setTable(this);
	field->m_order = m_fields.count();
	//Check for auto-generated indices:

	IndexSchema *idx = 0;
	if (field->isPrimaryKey()) {// this is auto-generated single-field unique index
		idx = new IndexSchema(this);
		idx->setAutoGenerated(true);
		idx->addField( field );
		setPrimaryKey(idx);
	}
	if (field->isUniqueKey()) {
		if (!idx) {
			idx = new IndexSchema(this);
			idx->setAutoGenerated(true);
			idx->addField( field );
		}
		idx->setUnique(true);
	}
	if (idx)
		m_indices.append(idx);
	return *this;
}


void TableSchema::clear()
{
	m_indices.clear();
	FieldList::clear();
	SchemaData::clear();
	m_conn = 0;
}

/*
void TableSchema::addPrimaryKey(const QString& key)
{
	m_primaryKeys.append(key);
}*/

/*QStringList TableSchema::primaryKeys() const
{
	return m_primaryKeys;
}

bool TableSchema::hasPrimaryKeys() const
{
	return !m_primaryKeys.isEmpty();
}
*/

//const QString& TableSchema::name() const
//{
//	return m_name;
//}

//void TableSchema::setName(const QString& name)
//{
//	m_name=name;
/*	ListIterator it( m_fields );
	Field *field;
	for (; (field = it.current())!=0; ++it) {
	
	int fcnt=m_fields.count();
	for (int i=0;i<fcnt;i++) {
		m_fields[i].setTable(name);
	}*/
//}

/*KexiDB::Field TableSchema::field(unsigned int id) const
{
	if (id<m_fields.count()) return m_fields[id];
	return KexiDB::Field();
}

unsigned int TableSchema::fieldCount() const
{
	return m_fields.count();
}*/

void TableSchema::debug() const
{
	KexiDBDbg << "TABLE " << schemaDataDebugString() << endl;
	FieldList::debug();
}

void TableSchema::setKexiDBSystem(bool set)
{
	if (set)
		m_native=true;
	m_isKexiDBSystem = set;
}

void TableSchema::setNative(bool set)
{
	if (m_isKexiDBSystem && !set) {
		KexiDBDbg << "TableSchema::setNative(): cannot set native off"
			" when KexiDB system flag is set on!" << endl;
		return;
	}
	m_native=set;
}

QuerySchema* TableSchema::query()
{
	if (m_query)
		return m_query;
	m_query = new QuerySchema( this ); //it's owned by me
	return m_query;
}

//----------------------------------------------------

/*
TableDef::TableDef(const QString& name)
	: Table(name)
{
}

TableDef::TableDef()
	: Table()
{
}

TableDef::~TableDef()
{
}

KexiDB::FieldDef
TableDef::field(unsigned int id) const
{
	if (id<m_fields.count()) return m_fields[id];
	return KexiDB::FieldDef();
}

void TableDef::addField(KexiDB::FieldDef field)
{
	field.setTable(m_name);
	m_fields.append(field);
}
*/

