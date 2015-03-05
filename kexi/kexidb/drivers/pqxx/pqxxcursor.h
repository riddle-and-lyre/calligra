/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <adam@piggz.co.uk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDB_CURSOR_PQXX_H
#define KEXIDB_CURSOR_PQXX_H

#include <db/cursor.h>
#include <db/connection.h>
#include <db/utils.h>

#include <pqxx/pqxx>
#include <pqxx/binarystring>

#ifndef _MSC_VER
#  define _MSC_VER 0 // avoid warning in server/c.h
#  include <postgres.h>
#  undef _MSC_VER
#else
#  include <postgres.h>
#endif
#include <catalog/pg_type.h>

namespace KexiDB
{

class pqxxSqlCursor: public Cursor
{
public:
    virtual ~pqxxSqlCursor();

    virtual QVariant value(uint i);
    virtual const char **rowData() const;
    virtual bool drv_storeCurrentRow(RecordData &data) const;

//! @todo virtual const char *** bufferData()
//! @todo virtual int serverResult() const;
//! @todo virtual QString serverResultName() const;
//! @todo virtual QString serverErrorMsg() const;

protected:
    pqxxSqlCursor(Connection *conn, const QString &statement = QString(),
                  uint options = NoOptions);
    pqxxSqlCursor(Connection *conn, QuerySchema &query, uint options = NoOptions);
    virtual void drv_clearServerResult();
    virtual void drv_appendCurrentRecordToBuffer();
    virtual void drv_bufferMovePointerNext();
    virtual void drv_bufferMovePointerPrev();
    virtual void drv_bufferMovePointerTo(qint64 to);
    virtual bool drv_open();
    virtual bool drv_close();
    virtual void drv_getNextRecord();
    virtual void drv_getPrevRecord();

private:
    pqxx::result *m_res;
    pqxx::connection *my_conn;
    QVariant pValue(uint pos)const;
    bool m_implicityStarted;
    friend class pqxxSqlConnection;
};

inline QVariant pgsqlCStrToVariant(const pqxx::result::field &r)
{
    switch (r.type()) {
    case BOOLOID:
        return QString::fromLatin1(r.c_str(), r.size()) == "true"; //!< @todo check formatting
    case INT2OID:
    case INT4OID:
    case INT8OID:
        return r.as(int());
    case FLOAT4OID:
    case FLOAT8OID:
    case NUMERICOID:
        return r.as(double());
    case DATEOID:
        return QString::fromUtf8(r.c_str(), r.size()); //!< @todo check formatting
    case TIMEOID:
        return QString::fromUtf8(r.c_str(), r.size()); //!< @todo check formatting
    case TIMESTAMPOID:
        return QString::fromUtf8(r.c_str(), r.size()); //!< @todo check formatting
    case BYTEAOID:
        return KexiDB::pgsqlByteaToByteArray(r.c_str(), r.size());
    case BPCHAROID:
    case VARCHAROID:
    case TEXTOID:
        return QString::fromUtf8(r.c_str(), r.size()); //utf8?
    default:
        return QString::fromUtf8(r.c_str(), r.size()); //utf8?
    }
}

}

#endif
