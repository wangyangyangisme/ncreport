/*************************************
NCUtils Utility functions for Qt
**************************************/
#ifndef NCUTILS_H
#define NCUTILS_H

#include <qstring.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qwidget.h>
//Added by qt3to4:
//#include <Q3CString>

/*!
NC's utility collention
*/
namespace NCUtils {

	QColor webColor( const QString & );
	QString formatNumber( const double number,
			const char* frm="%8.2f",
			bool sep=false,
			const char separator=' ',
			const char decpoint='.' );

	QString formatNumber( const QString & numberString,
			bool sep=false,
			const char separator=' ',
			const char decpoint='.' );

	QString dateToString( const QDate & dt );
	QDate stringToDate( const QString & dts );
	QString sayNumber( double numb );
	QString sayNumber_part( const QString& part );
	double round( const double value, const short int digits =0 );
	double abs( const double d );
	
    QString readConfig( const QString &filename, const QString &group, const QString &option );
    bool writeConfig( const QString &filename, const QString &group, const QString &option, const QString &value );
	bool _saveConfig( const QString &filename, const QStringList & sl );
	bool _loadConfig(const QString &filename, QStringList & sl,
			const QString &group, const QString &option,
			int &groupLineNo, int &optLineNo );
	void EncodeStringHTML( QString& s );
	void EncodeStringSQL( QString& s );
	void EncodeStringX( QString& s );
	void DecodeStringX( QString& s );
	const char* findInEncTable( const QChar ch );
	/*! "intelligent" string concating */
	void concatStringWith( QString& s, const QString& s1, const QString& delim =", ");
	QPoint popupPosition( QWidget *hostwidget, QWidget* popupwidget );
}

#endif // NCUTILS_H



