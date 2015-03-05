/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2014 Wojciech Kosowicz <pcellix@gmail.com>

   This file has been automatically generated from
   calligra/kexi/tools/sql_keywords/sql_keywords.sh and
   mysql-5.6.17/sql/lex.h.

   Please edit the sql_keywords.sh, not this file!

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
#include <mysqldriver.h>

namespace KexiDB
{
const char *const MySqlDriver::keywords[] = {
    "ACCESSIBLE",
    "ACTION",
    "ADD",
    "AGAINST",
    "AGGREGATE",
    "ALGORITHM",
    "ALTER",
    "ANALYSE",
    "ANALYZE",
    "ANY",
    "ASCII",
    "ASENSITIVE",
    "AT",
    "AUTO_INCREMENT",
    "AUTOEXTEND_SIZE",
    "AVG",
    "AVG_ROW_LENGTH",
    "BACKUP",
    "BIGINT",
    "BINARY",
    "BINLOG",
    "BIT",
    "BLOB",
    "BLOCK",
    "BOOL",
    "BOOLEAN",
    "BOTH",
    "BTREE",
    "BYTE",
    "CACHE",
    "CALL",
    "CASCADED",
    "CATALOG_NAME",
    "CHAIN",
    "CHANGE",
    "CHANGED",
    "CHAR",
    "CHARACTER",
    "CHARSET",
    "CHECKSUM",
    "CIPHER",
    "CLASS_ORIGIN",
    "CLIENT",
    "CLOSE",
    "COALESCE",
    "CODE",
    "COLLATION",
    "COLUMN",
    "COLUMN_FORMAT",
    "COLUMN_NAME",
    "COLUMNS",
    "COMMENT",
    "COMMITTED",
    "COMPACT",
    "COMPLETION",
    "COMPRESSED",
    "CONCURRENT",
    "CONDITION",
    "CONNECTION",
    "CONSISTENT",
    "CONSTRAINT_CATALOG",
    "CONSTRAINT_NAME",
    "CONSTRAINT_SCHEMA",
    "CONTAINS",
    "CONTEXT",
    "CONTINUE",
    "CONVERT",
    "CPU",
    "CUBE",
    "CURRENT",
    "CURRENT_DATE",
    "CURRENT_TIME",
    "CURRENT_TIMESTAMP",
    "CURRENT_USER",
    "CURSOR",
    "CURSOR_NAME",
    "DATA",
    "DATABASES",
    "DATAFILE",
    "DATE",
    "DATETIME",
    "DAY",
    "DAY_HOUR",
    "DAY_MICROSECOND",
    "DAY_MINUTE",
    "DAY_SECOND",
    "DEALLOCATE",
    "DEC",
    "DECIMAL",
    "DECLARE",
    "DEFAULT_AUTH",
    "DEFINER",
    "DELAYED",
    "DELAY_KEY_WRITE",
    "DESCRIBE",
    "DES_KEY_FILE",
    "DETERMINISTIC",
    "DIAGNOSTICS",
    "DIRECTORY",
    "DISABLE",
    "DISCARD",
    "DISK",
    "DISTINCTROW",
    "DIV",
    "DO",
    "DOUBLE",
    "DUAL",
    "DUMPFILE",
    "DUPLICATE",
    "DYNAMIC",
    "EACH",
    "ELSEIF",
    "ENABLE",
    "ENCLOSED",
    "ENDS",
    "ENGINE",
    "ENGINES",
    "ENUM",
    "ERROR",
    "ERRORS",
    "ESCAPE",
    "ESCAPED",
    "EVENT",
    "EVENTS",
    "EVERY",
    "EXCHANGE",
    "EXECUTE",
    "EXISTS",
    "EXIT",
    "EXPANSION",
    "EXPORT",
    "EXPIRE",
    "EXTENDED",
    "EXTENT_SIZE",
    "FALSE",
    "FAST",
    "FAULTS",
    "FETCH",
    "FIELDS",
    "FILE",
    "FIRST",
    "FIXED",
    "FLOAT",
    "FLOAT4",
    "FLOAT8",
    "FLUSH",
    "FORCE",
    "FORMAT",
    "FOUND",
    "FULLTEXT",
    "FUNCTION",
    "GENERAL",
    "GEOMETRY",
    "GEOMETRYCOLLECTION",
    "GET_FORMAT",
    "GET",
    "GLOBAL",
    "GRANT",
    "GRANTS",
    "HANDLER",
    "HASH",
    "HELP",
    "HIGH_PRIORITY",
    "HOST",
    "HOSTS",
    "HOUR",
    "HOUR_MICROSECOND",
    "HOUR_MINUTE",
    "HOUR_SECOND",
    "IDENTIFIED",
    "IF",
    "IGNORE_SERVER_IDS",
    "IMPORT",
    "INDEXES",
    "INFILE",
    "INITIAL_SIZE",
    "INOUT",
    "INSENSITIVE",
    "INSERT_METHOD",
    "INSTALL",
    "INT",
    "INT1",
    "INT2",
    "INT3",
    "INT4",
    "INT8",
    "INTERVAL",
    "IO",
    "IO_AFTER_GTIDS",
    "IO_BEFORE_GTIDS",
    "IO_THREAD",
    "IPC",
    "ISOLATION",
    "ISSUER",
    "ITERATE",
    "INVOKER",
    "KEYS",
    "KEY_BLOCK_SIZE",
    "KILL",
    "LANGUAGE",
    "LAST",
    "LEADING",
    "LEAVE",
    "LEAVES",
    "LESS",
    "LEVEL",
    "LINEAR",
    "LINES",
    "LINESTRING",
    "LIST",
    "LOAD",
    "LOCAL",
    "LOCALTIME",
    "LOCALTIMESTAMP",
    "LOCK",
    "LOCKS",
    "LOGFILE",
    "LOGS",
    "LONG",
    "LONGBLOB",
    "LONGTEXT",
    "LOOP",
    "LOW_PRIORITY",
    "MASTER",
    "MASTER_AUTO_POSITION",
    "MASTER_BIND",
    "MASTER_CONNECT_RETRY",
    "MASTER_DELAY",
    "MASTER_HOST",
    "MASTER_LOG_FILE",
    "MASTER_LOG_POS",
    "MASTER_PASSWORD",
    "MASTER_PORT",
    "MASTER_RETRY_COUNT",
    "MASTER_SERVER_ID",
    "MASTER_SSL",
    "MASTER_SSL_CA",
    "MASTER_SSL_CAPATH",
    "MASTER_SSL_CERT",
    "MASTER_SSL_CIPHER",
    "MASTER_SSL_CRL",
    "MASTER_SSL_CRLPATH",
    "MASTER_SSL_KEY",
    "MASTER_SSL_VERIFY_SERVER_CERT",
    "MASTER_USER",
    "MASTER_HEARTBEAT_PERIOD",
    "MAX_CONNECTIONS_PER_HOUR",
    "MAX_QUERIES_PER_HOUR",
    "MAX_ROWS",
    "MAX_SIZE",
    "MAX_UPDATES_PER_HOUR",
    "MAX_USER_CONNECTIONS",
    "MAXVALUE",
    "MEDIUM",
    "MEDIUMBLOB",
    "MEDIUMINT",
    "MEDIUMTEXT",
    "MEMORY",
    "MERGE",
    "MESSAGE_TEXT",
    "MICROSECOND",
    "MIDDLEINT",
    "MIGRATE",
    "MINUTE",
    "MINUTE_MICROSECOND",
    "MINUTE_SECOND",
    "MIN_ROWS",
    "MOD",
    "MODE",
    "MODIFIES",
    "MODIFY",
    "MONTH",
    "MULTILINESTRING",
    "MULTIPOINT",
    "MULTIPOLYGON",
    "MUTEX",
    "MYSQL_ERRNO",
    "NAME",
    "NAMES",
    "NATIONAL",
    "NDB",
    "NDBCLUSTER",
    "NCHAR",
    "NEW",
    "NEXT",
    "NO",
    "NO_WAIT",
    "NODEGROUP",
    "NONE",
    "NO_WRITE_TO_BINLOG",
    "NUMBER",
    "NUMERIC",
    "NVARCHAR",
    "OLD_PASSWORD",
    "ONE",
    "ONLY",
    "OPEN",
    "OPTIMIZE",
    "OPTIONS",
    "OPTION",
    "OPTIONALLY",
    "OUT",
    "OUTFILE",
    "OWNER",
    "PACK_KEYS",
    "PARSER",
    "PAGE",
    "PARTIAL",
    "PARTITION",
    "PARTITIONING",
    "PARTITIONS",
    "PASSWORD",
    "PHASE",
    "PLUGIN",
    "PLUGINS",
    "PLUGIN_DIR",
    "POINT",
    "POLYGON",
    "PORT",
    "PRECISION",
    "PREPARE",
    "PRESERVE",
    "PREV",
    "PRIVILEGES",
    "PROCEDURE",
    "PROCESS",
    "PROCESSLIST",
    "PROFILE",
    "PROFILES",
    "PROXY",
    "PURGE",
    "QUARTER",
    "QUERY",
    "QUICK",
    "RANGE",
    "READ",
    "READ_ONLY",
    "READ_WRITE",
    "READS",
    "REAL",
    "REBUILD",
    "RECOVER",
    "REDO_BUFFER_SIZE",
    "REDOFILE",
    "REDUNDANT",
    "REGEXP",
    "RELAY",
    "RELAYLOG",
    "RELAY_LOG_FILE",
    "RELAY_LOG_POS",
    "RELAY_THREAD",
    "RELEASE",
    "RELOAD",
    "REMOVE",
    "RENAME",
    "REORGANIZE",
    "REPAIR",
    "REPEATABLE",
    "REPLICATION",
    "REPEAT",
    "REQUIRE",
    "RESET",
    "RESIGNAL",
    "RESTORE",
    "RESUME",
    "RETURNED_SQLSTATE",
    "RETURN",
    "RETURNS",
    "REVERSE",
    "REVOKE",
    "RLIKE",
    "ROLLUP",
    "ROUTINE",
    "ROW_COUNT",
    "ROWS",
    "ROW_FORMAT",
    "RTREE",
    "SAVEPOINT",
    "SCHEDULE",
    "SCHEMA",
    "SCHEMA_NAME",
    "SCHEMAS",
    "SECOND",
    "SECOND_MICROSECOND",
    "SECURITY",
    "SENSITIVE",
    "SEPARATOR",
    "SERIAL",
    "SERIALIZABLE",
    "SESSION",
    "SERVER",
    "SHARE",
    "SHOW",
    "SHUTDOWN",
    "SIGNAL",
    "SIGNED",
    "SIMPLE",
    "SLAVE",
    "SLOW",
    "SNAPSHOT",
    "SMALLINT",
    "SOCKET",
    "SOME",
    "SONAME",
    "SOUNDS",
    "SOURCE",
    "SPATIAL",
    "SPECIFIC",
    "SQL",
    "SQLEXCEPTION",
    "SQLSTATE",
    "SQLWARNING",
    "SQL_AFTER_GTIDS",
    "SQL_AFTER_MTS_GAPS",
    "SQL_BEFORE_GTIDS",
    "SQL_BIG_RESULT",
    "SQL_BUFFER_RESULT",
    "SQL_CACHE",
    "SQL_CALC_FOUND_ROWS",
    "SQL_NO_CACHE",
    "SQL_SMALL_RESULT",
    "SQL_THREAD",
    "SQL_TSI_SECOND",
    "SQL_TSI_MINUTE",
    "SQL_TSI_HOUR",
    "SQL_TSI_DAY",
    "SQL_TSI_WEEK",
    "SQL_TSI_MONTH",
    "SQL_TSI_QUARTER",
    "SQL_TSI_YEAR",
    "SSL",
    "START",
    "STARTING",
    "STARTS",
    "STATS_AUTO_RECALC",
    "STATS_PERSISTENT",
    "STATS_SAMPLE_PAGES",
    "STATUS",
    "STOP",
    "STORAGE",
    "STRAIGHT_JOIN",
    "STRING",
    "SUBCLASS_ORIGIN",
    "SUBJECT",
    "SUBPARTITION",
    "SUBPARTITIONS",
    "SUPER",
    "SUSPEND",
    "SWAPS",
    "SWITCHES",
    "TABLE_NAME",
    "TABLES",
    "TABLESPACE",
    "TABLE_CHECKSUM",
    "TEMPTABLE",
    "TERMINATED",
    "TEXT",
    "THAN",
    "TIME",
    "TIMESTAMP",
    "TIMESTAMPADD",
    "TIMESTAMPDIFF",
    "TINYBLOB",
    "TINYINT",
    "TINYTEXT",
    "TRAILING",
    "TRIGGER",
    "TRIGGERS",
    "TRUE",
    "TRUNCATE",
    "TYPE",
    "TYPES",
    "UNCOMMITTED",
    "UNDEFINED",
    "UNDO_BUFFER_SIZE",
    "UNDOFILE",
    "UNDO",
    "UNICODE",
    "UNKNOWN",
    "UNLOCK",
    "UNINSTALL",
    "UNSIGNED",
    "UNTIL",
    "UPGRADE",
    "USAGE",
    "USE",
    "USER",
    "USER_RESOURCES",
    "USE_FRM",
    "UTC_DATE",
    "UTC_TIME",
    "UTC_TIMESTAMP",
    "VALUE",
    "VARBINARY",
    "VARCHAR",
    "VARCHARACTER",
    "VARIABLES",
    "VARYING",
    "WAIT",
    "WARNINGS",
    "WEEK",
    "WEIGHT_STRING",
    "WHILE",
    "VIEW",
    "WITH",
    "WORK",
    "WRAPPER",
    "WRITE",
    "X509",
    "XA",
    "XML",
    "YEAR",
    "YEAR_MONTH",
    "ZEROFILL",
    0
};
}
