/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_DRIVER_P_H
#define KEXIDB_DRIVER_P_H

#ifndef __KEXIDB__
# error "Do not include: this is KexiDB internal file"
#endif

#include <qstring.h>
#include <qmap.h>
#include <qptrdict.h>
#include <qvariant.h>

namespace KexiDB {

/*! Detailed definition of driver's default behaviour.
 Note for driver developers: 
 Change these defaults in you Driver subclass
 constructor, if needed.
*/
class KEXI_DB_EXPORT DriverBehaviour
{
	public:
		DriverBehaviour();
			
	//! "UNSIGNED" by default
	QString UNSIGNED_TYPE_KEYWORD;
	
	//! "AUTO_INCREMENT" by default, used as add-in word to field definition
	//! May be also used as full definition if SPECIAL_AUTO_INCREMENT_DEF is true.
	QString AUTO_INCREMENT_FIELD_OPTION; 
	
	/*! True if autoincrement field has special definition 
	 e.g. like "INTEGER PRIMARY KEY" for SQLite.
	 Special definition string should be stored in AUTO_INCREMENT_FIELD_OPTION.
	 False by default. */
	bool SPECIAL_AUTO_INCREMENT_DEF : 1;
	
	/*! Name of a field (or built-in function) with autoincremented unique value,
	 typically returned by Connection::drv_lastInsertRowID().

	 Examples:
	 - PostgreSQL and SQLite engines use 'OID' field 
	 - MySQL uses LAST_INSERT_ID() built-in function
	*/
	QString ROW_ID_FIELD_NAME;
	
	/*! True if the value (fetched from field (or function) 
	 defined by ROW_ID_FIELD_NAME member) is EXACTLY the value if autoincremented field,
	 not just an implicit (internal) row number. Default value is false.
	 
	 Examples:
	 - PostgreSQL and SQLite engines have this flag set to false ('OID' field has 
	    it's own implicit value)
	 - MySQL has this flag set to true (LAST_INSERT_ID() returns real value 
	 of last autoincremented field)
	*/
	bool ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE : 1;

	/*! Name of any (e.g. first found) database for this connection that 
	 typically always exists. This can be not set if we want to do some magic checking
	 what database name is availabe by reimplementing
	 Connection::anyAvailableDatabaseName().
	 Example: for PostgreSQL this is "template1".
	 
	 \sa Connection::SetAvailableDatabaseName()
	*/
	QString ALWAYS_AVAILABLE_DATABASE_NAME;
	
	/*! Quotation marks used for escaping identifier (see Driver::escapeIdentifier()).
	 Default value is '"'. Change it for your driver.
	*/
	QChar QUOTATION_MARKS_FOR_IDENTIFIER;
	
	/*! True if using database is requied to perform real connection.
	 This is true for may engines, e.g. for PostgreSQL, where connections 
	 string should contain a database name.
	 This flag is unused for file-based db drivers,
	 by default set to true and used for all other db drivers.
	*/
	bool USING_DATABASE_REQUIRED_TO_CONNECT : 1;

	/*! True if before we know whether the fetched result of executed query
	 is empty or not, we need to fetch first record. Particularly, it's true for SQLite.
	 The flag is used in Cursor::open(). By default this flag is false. */
	bool _1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY : 1;
};

/*! Private driver's data members. Available for implementation. */
class DriverPrivate
{
	public:
		DriverPrivate();

		QPtrDict<Connection> connections;

//(js)now QObject::name() is reused:
//		/*! The name equal to the service name (X-Kexi-DriverName) 
//		 stored in given service .desktop file. Set this in subclasses. */
//		QString m_driverName;
		
		/*! Name of MIME type of files handled by this driver 
		 if it is a file-based database's driver 
		 (equal X-Kexi-FileDBDriverMime service property) */
		QString fileDBDriverMime;

		/*! Info about the driver as a service. */
		KService *service;

		/*! Internal constant flag: Set this in subclass if driver is a file driver */
		bool isFileDriver : 1;

		/*! Internal constant flag: Set this in subclass if after successfull
		 drv_createDatabased() database is in opened state (as after useDatabase()). 
		 For most engines this is not true. */
		bool isDBOpenedAfterCreate : 1;

		/*! List of system objects names, eg. build-in system tables that 
		 cannot be used by user, and in most cases user even shouldn't see these.
		 The list contents is driver dependent (by default is empty) 
		 - fill this in subclass ctor. */
//		QStringList m_systemObjectNames;

		/*! List of system fields names, build-in system fields that cannot be used by user,
		 and in most cases user even shouldn't see these.
		 The list contents is driver dependent (by default is empty) - fill this in subclass ctor. */
//		QStringList m_systemFieldNames;

		/*! Features (like transactions, etc.) supported by this driver
		 (sum of selected  Features enum items). 
		 This member should be filled in driver implementation's constructor 
		 (by default m_features==NoFeatures). */
		int features;

		//! real type names for this engine
		QValueVector<QString> typeNames;

		/*! Driver properties dictionary (indexed by name), 
		 useful for presenting properties to the user. 
		 Set available properties here in driver implementation. */
		QMap<QCString,QVariant> properties;

		/*! i18n'd captions for properties. You do not need 
		 to set predefined properties' caption in driver implementation 
		 -it's done automatically. */
		QMap<QCString,QString> propertyCaptions;

	protected:
		/*! Used by driver manager to initialize properties taken using internal driver flags. */
		void initInternalProperties();
	
	friend class DriverManagerInternal;
};

}

//! driver's static version information (impementation), 
//! with KLibFactory symbol declaration
#define KEXIDB_DRIVER_INFO( class_name, internal_name, internal_string ) \
	int class_name::versionMajor() const { return KEXIDB_VERSION_MAJOR; } \
	int class_name::versionMinor() const { return KEXIDB_VERSION_MINOR; } \
	K_EXPORT_COMPONENT_FACTORY(kexidb_ ## internal_name ## driver, KGenericFactory<KexiDB::class_name>( "kexidb_" internal_string "driver" ))

#endif
