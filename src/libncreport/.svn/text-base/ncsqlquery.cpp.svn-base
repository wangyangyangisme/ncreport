/***************************************************************************
 *   Copyright (C) 2005 by Norbert Szabo                                   *
 *   Hungary                                                               *
 *   nszabo@helta.hu                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ncsqlquery.h"

#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QTextCodec>
#include <QSqlRecord>
#include <QVariant>
//Added by qt3to4:

#define NCSQL_DEFAULTCODEC	"ISO 8859-2"
#define NCSQL_ERRORLOGFILE	"sqlerrors.log.txt"
//#define NCSQL_DEBUG_ON
//#define NCSQL_ALWAYSLOG


NCSqlQuery::NCSqlQuery() : QSqlQuery()
{
}

NCSqlQuery::NCSqlQuery( QSqlDatabase db ) : QSqlQuery( db )
{
}

NCSqlQuery::NCSqlQuery( const QString & query, QSqlDatabase db ) 
	: QSqlQuery( query, db )
{
	if ( !query.isEmpty() )
		saveColumns();
}

NCSqlQuery::~NCSqlQuery() 
{
	#ifdef NCSQL_DEBUG_ON
		qDebug( "NCSqlQuery object deleted." );
	#endif
}

bool NCSqlQuery::query( const QString & qs )
{
	return exec(qs);
}

bool NCSqlQuery::exec( const QString & qs )
{
	bool ok = false;
	QTextCodec* codec = QTextCodec::codecForName(NCSQL_DEFAULTCODEC);
	
	if ( qs.isEmpty() )
		ok = QSqlQuery::exec();
	else
		ok = QSqlQuery::exec( (const char*)(codec->fromUnicode( qs )) );

	if ( !ok )
		logSqlError( qs, getErrorMsg() );
		
	if ( isActive() )
		saveColumns();
	else
		ok = false;
		
	//recinfo = sqlquery->driver()->recordInfo( *sqlquery );

	#ifdef NCSQL_ALWAYSLOG
		logSqlError( qs, "OK." );
	#endif

		
	#ifdef NCSQL_DEBUG_ON
		qDebug( "**************************************" );
		qDebug( "Execute NCSqlQuery::exec(...)" );
		qDebug( "**************************************" );
		qDebug( "%s", qPrintable(qs) );
		qDebug( "**************************************" );
	#endif
		
	return ok;
}

int NCSqlQuery::getError( )
{
	return lastError().number();
}

QString NCSqlQuery::getErrorMsg( )
{
	QString errmsg = lastError().text();
	if ( errmsg.isEmpty() && !isActive() )
		errmsg = QString("Database connection lost!");
	return errmsg;
}

int NCSqlQuery::getRowCount( )
{
	//return numRowsAffected();
	return size();
}

int NCSqlQuery::getFieldCount( )
{
	return record().count();
}

QString NCSqlQuery::stringValue( const QString & columnname )
{
	if (!isValid() )
		first();
	
	int pos = record().indexOf( columnname );
	//int pos =1;
	if ( pos<0 ) {
		qWarning("Column '%s' not found", qPrintable(columnname) );
		return QString::null;
	}
	else
		return value(pos).toString();
		//return QObject::tr( value(pos).toString() );
}

QString NCSqlQuery::stringValue( int columnno )
{
	if (!isValid() )
		first();
	return value(columnno).toString();
}


QVariant NCSqlQuery::value( int i )
{
	if (!isValid() )
		first();
		
	return QSqlQuery::value(i);
}

QVariant NCSqlQuery::value( const QString & columnname )
{
	if (!isValid() )
		first();
	int pos = record().indexOf( columnname );
	
	if ( pos<0 ) {
		qWarning("Column '%s' not found", qPrintable(columnname) );
		return QVariant();
	}
	else
		return ( QSqlQuery::value(pos) );
}




void NCSqlQuery::logSqlError( const QString& qs, const QString& error )
{
	QFile f( NCSQL_ERRORLOGFILE );
	if ( !f.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
		return;
	QTextStream t( &f );
	t << "Error time: " << QDateTime::currentDateTime().toString() << "\n";
	t << error << "\n";
	t << "Sql command: \n" << qs << "\n";
	t << "************************************************************\n";
	f.close();
}

int NCSqlQuery::getLastInsertedPK()
{
	QString qry;
	int lastpk=0;
	qry = "SELECT LAST_INSERT_ID() as lastID";
	if ( exec(qry) )
		lastpk = value(0).toInt();
	
	#ifdef NCSQL_DEBUG_ON	
		qDebug( "LAST_INSERT_ID() = %i", lastpk );
	#endif
	
	return lastpk;
}

void NCSqlQuery::saveColumns( )
{
	//rec = driver()->record( *this );
}

QString NCSqlQuery::fieldName( int i )
{
	return record().fieldName(i);
}

bool NCSqlQuery::beginTransaction( )
{
	return defaultDatabase().transaction();
}

QSqlDatabase NCSqlQuery::defaultDatabase( )
{
	return QSqlDatabase::database();
}

bool NCSqlQuery::commitTransaction( )
{
	return defaultDatabase().commit();
}

bool NCSqlQuery::rollbackTransaction( )
{
	return defaultDatabase().rollback();
}







