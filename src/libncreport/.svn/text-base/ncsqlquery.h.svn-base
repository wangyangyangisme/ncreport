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
#ifndef NCSQLQUERY_H
#define NCSQLQUERY_H

#include <QSqlQuery>

/*!
Wrapper class used by me for SQL data manipulation derived from QSqlQuery. 
It has capability for name resolving of fields in 
SQL result. This can log sql errors and debug sql execution events.
*/

class NCSqlQuery : public QSqlQuery
{
public:
    //NCSqlQuery();
	NCSqlQuery();
	NCSqlQuery( QSqlDatabase db );
	NCSqlQuery( const QString & query, QSqlDatabase db );
	virtual ~NCSqlQuery();

	virtual bool query( const QString &qs );
	virtual bool exec( const QString &qs = QString::null );
	QString getErrorMsg();
	int getError();
	int getRowCount();
	int getLastInsertedPK();
	QString fieldName(int);
	
	QString stringValue(const QString& columnname);
	QString stringValue(int columnno);
	QVariant value(const QString& columnname);
	QVariant value( int i );
	int getFieldCount();
	QSqlDatabase defaultDatabase();
	bool beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();
protected:
	void saveColumns();
	void logSqlError( const QString& qs, const QString& error );
	//QSqlRecord rec;

};

#endif
