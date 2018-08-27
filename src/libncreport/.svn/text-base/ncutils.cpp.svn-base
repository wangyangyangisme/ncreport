/***************************************************************************
                          ncutils.cpp  -  description
                             -------------------
    begin                : Wed Oct 9 2002
    copyright            : (C) 2002 by Noci
    email                : noci@helta.hu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ncutils.h"

#include <QTextStream>
#include <QFile>
#include <QApplication>
#include <QStringList>
#include <QDesktopWidget>


static const char *enctable[] = {
"  q1ayza",
". sad2e3",
"+ pxcvm3",
"@ kabqa2",
"# xpx751",
"$ JrFlk4",
"% 3KVBeE",
"& IurxCc",
"* mmjjbb",
"= eYoY9Y",
"- mFL0l2",
"; G9w4Hz",
"> asWaDq",
", ToO9dC",
"' 224511",
") 313233",
"! 343536",
"~ 414243",
"{ 555659",
"] 141711",
"^ 6g6j6p",
"/ 616263",
"( 696867",
"_ 737271",
": 858483",
"< 999897",
"[ 969594",
"} 939291",
"| 010307",
"1 0Y0x0c",
"2 u8u5u9",
"3 h7H7h6",
"4 505152",
"5 4000DD",
"6 rAfAcA",
"7 bIsImX",
"8 !c!s!e",
"9 !q!w!E",
"0 !a.Y!d",
"a !r!t!z",
"b #y#x#c",
"c 6f6g6h",
"d /D/f/e",
"e +v+n!m",
"f .vb.w.",
"g Q.E.R.",
"h x.v.n.",
"i u.i.p.",
"j BaBsBd",
"k B3B4B6",
"l VqVaVy",
"m VwVsVx",
"n VeVdVc",
"o WeWdWc",
"p WttpTP",
"q FFGpGG",
"r HaCaMg",
"s hAcAmG",
"t rRtTzZ",
"u iIlLL9",
"v v7C8b8",
"w PamnNu",
"x CrackU",
"y MrLoLj",
"z kAkSkD",
"A x)rYeM",
"B shJwql",
"C ckdpqh",
"D bdaLJa",
"E AzEzOS",
"F soseEn",
"G IsMeRs",
"H h32d4T",
"I 8U9i9I",
"J 5f5c5b",
"K 6G2y3V",
"L 1a9P9J",
"M 7g6h5l",
"N LaLihg",
"O hgFdSa",
"P pOiUzT",
"Q rEwQlK",
"R jHgFdS",
"S mNbVcX",
"T 0Pj0b0",
"U ttbbxx",
"V O1YXX4",
"W D2F6J6",
"X r7h7H0",
"Y iqQIPI",
"Z e5g7WW",
"�obBICR",
" sNdmMD",
"�barATH",
"�efghij",
" klmnop",
"�Rqx0X0",
"�cdWeqw",
" gUGlDr",
"�WljWZ8",
"�TjV63v",
"�3D8D1D",
"�fsDyOv",
"�D8UwJo",
"�XUqgwu",
"�ctNeDe",
"�fygqOK"};


QColor NCUtils::webColor( const QString & colorstring )
{
	QColor color;
	bool ok;
	colorstring.right(6).toInt( &ok, 16 );

	if ( ok && colorstring.startsWith("#") && colorstring.length() == 7 ) {

		color = QColor( colorstring.mid(1,2).toInt( &ok, 16),
						colorstring.mid(3,2).toInt( &ok, 16),
						colorstring.mid(5,2).toInt( &ok, 16) );

	}

	return color;
}

QString NCUtils::formatNumber( const double number, const char* frm, bool sep, const char separator, const char decpoint )
{
	QString fn;
	fn.setNum( number,'f',2 );
	
	//fn.sprintf( frm );
	
	//return fn;
	return formatNumber( fn, sep, separator, decpoint  );
}

QString NCUtils::formatNumber( const QString & numberString, bool sep, const char separator, const char decpoint )
{
	QString fn = numberString.trimmed();
	if ( fn == "NULL" )
		return QString::null;

	int decPPos = fn.indexOf('.');

	if (decpoint != '.') {	//decpoint csere
		if ( decPPos < 0 ) {}
		else {
			QString dp;
			dp = decpoint;
			fn.replace( decPPos, 1, dp );
		}
	}

	if (sep) {
		if (decPPos < 0)	// nincs benne "."
			decPPos = fn.length();

		if (decPPos >= 4)
			fn.insert( decPPos-3, separator );

		if (decPPos >= 7)
			fn.insert( decPPos-6, separator );

		if (decPPos >= 10)
			fn.insert( decPPos-9, separator );

		if (decPPos >= 13)
			fn.insert( decPPos-12, separator );
//		fn = fn.insert( decPPos-6, separator );
//		fn = fn.insert( decPPos-9, separator );
	}

	return fn;
}

QString NCUtils::readConfig( const QString &filename, const QString &group, const QString &option )
{
	/**************
	reads configuration file
	**************/
	//#ifdef NCAPP_DEBUGMODE
		//qDebug( "NCUtils::readConfig( %s, %s, %s )", filename.latin1(), group.latin1(), option.latin1() );
	//#endif
	QStringList list;
	QString retVal;
	int groupfoundLine = -1;
	int optfoundLine = -1;
	if ( !_loadConfig( filename, list, group, option, groupfoundLine, optfoundLine ) )
		return "";

	if ( optfoundLine < 0 )		// option not found
		return "";

	//QStringList::const_iterator it = list.at( optfoundLine );
	retVal = list.at( optfoundLine );
	
	int eTokenPos = -1;
	//if ( it != list.end() ) {
	if ( !retVal.isEmpty() ) {
		//retVal = *it;	// option line
		eTokenPos = retVal.indexOf("=");
		if ( eTokenPos < 0 )
			retVal = "";
		else
			retVal = retVal.mid( eTokenPos+1 ).trimmed();		// everything after "=" token
	}
	else
		retVal = "";

	return retVal;
}

bool NCUtils::writeConfig( const QString &filename, const QString &group, const QString &option, const QString &value )
{
	/**************
	reads configuration file
	**************/
	QStringList list;
	QString retVal;
	int groupfoundLine = -1;
	int optfoundLine = -1;
	if ( !QFile::exists( filename ) ) { //not exists, create new.
		//groupfoundLine = 0;
		//optfoundLine = 0;
	} else {
		if ( !_loadConfig( filename, list, group, option, groupfoundLine, optfoundLine ) )
			return false;
	}

	if ( optfoundLine < 0 )	{	// option not found, we need inser as new line
		if ( groupfoundLine < 0 ) {	// group not found also.
			list += "[" + group + "]";
			list += option + " = " + value;
		} else {	// group exists
			//QStringList::Iterator it = list.at( groupfoundLine );
		
			list.insert( groupfoundLine, option + " = " + value );
		}
	} else {	// option found
		
		list.insert( optfoundLine, option + " = " + value );
		/*
		QStringList::Iterator it = list.at( optfoundLine );
		if ( it != list.end() ) {
			it = list.remove( it );	//remove line
			if ( it != list.end() )
				list.insert( it, option + " = " + value );
			else	// we were at end of file.
				list += option + " = " + value;		// append file;
		}
		
		else
			return false;
		*/
	}

	return _saveConfig( filename, list );
}

bool NCUtils::_saveConfig( const QString &filename, const QStringList & sl )
{

    QFile f( filename );
    if ( !f.open( QIODevice::WriteOnly ) ) {
    	qDebug( "Cannot save config to %s", qPrintable(filename) );
        return false;
	}

    QTextStream t( &f );

	for ( QStringList::ConstIterator it = sl.begin(); it != sl.end(); ++it ) {
		t << (*it) << "\r\n";
	}

	f.close();
	return true;
}

bool NCUtils::_loadConfig(const QString &filename, QStringList & sl, const QString &group, const QString &option, int &groupLineNo, int &optLineNo )
{
    QFile f( filename );
    if ( !f.open( QIODevice::ReadOnly ) ) {
		qDebug( "Cannot open %s", qPrintable(filename) );
        return false;
	}

    QTextStream t( &f );
	sl.clear();		// empty string list
	QString curLine;
	bool groupExists = false;
	//bool optExists = false;
	int line=0;
	groupLineNo = -1;	// group not found
	optLineNo = -1;	// option not found	
	while ( !t.atEnd() ) {
		curLine = t.readLine().trimmed();
		sl += curLine;

		if ( curLine.startsWith( "["+group+"]") ) {
			groupExists = true;
			groupLineNo = line;
		}

		if ( groupExists ) {
			if ( curLine.startsWith( option ) )
				optLineNo = line;
				//break;
		}
		line++;
    }

    f.close();
    return true;
}

QString NCUtils::dateToString( const QDate & dt )
{
	return QString("%1-%2-%3").arg( dt.year()).arg(dt.month()).arg(dt.day());
}

QDate NCUtils::stringToDate( const QString & dts )
{
	int year = dts.left(4).toInt();
	int month = dts.mid(5,2).toInt();
	int day = dts.right(2).toInt();
	//if ( QDate::isValid( year, month ,day ) )
	//		return
	return QDate( year, month ,day );
}

/*************************************
	SAY A NUMBER (hungary)
***************************************/
QString NCUtils::sayNumber( double numb )
{
	QString pretext;
	if ( numb==0 )
		return "nulla";
	else if ( numb < 0 ) {
		pretext="minusz ";
		numb = -numb;
	}
	else if ( numb > 0 && numb < 1000000000 )
		pretext="";
	else
		return QString::null;
	
	//dimension j1(9),j10(9),ert(9),kb(9)
	const QString thousand ="ezer";
	const QString million ="millio";

	QString osz;
	osz = osz.sprintf( "%9.2f", numb ).rightJustified( 12, ' ');
	//fill('000000000'+ltrim(str(numb)),9)

	QString cents = osz.right( 3 );
	QString p1 = osz.mid( 6,3 );
	QString p2 = osz.mid( 3,3 );
	QString p3 = osz.left( 3 );
	QString mi, ez;

	if ( p3.toInt() == 0 )
		mi = "";
	else
		mi = million+"-";
	
	if ( p2.toInt() == 0 )
		ez = "";
	else
		ez = thousand+"-";

	QString sayCents;
	if ( cents > ".00" )
		sayCents = " + 0"+cents;
	
	QString say3 = sayNumber_part( p3 );
	QString say2 = sayNumber_part( p2 );
	QString say1 = sayNumber_part( p1 );
			
	return pretext + say3 + mi + say2 + ez + say1 + sayCents;
}

QString NCUtils::sayNumber_part( const QString& part )
{
   	//qDebug( "Number Part: %s", part.latin1() );
	const QString j1[9] = { "egy", "ketto", "harom", "negy", "ot", "hat", "het", "nyolc", "kilenc" };
	const QString j10[9] ={ "tizen", "huszon", "harminc", "negyven", "otven", "hatvan", "hetven", "nyolcvan", "kilencven" };
	const QString ten ="tiz";
	const QString twenty ="husz";
	const QString hundred ="szaz";
	int ert[3] = { 0, 0, 0 };
	QString kb[3];
	
	int a=0;
	while ( a < 3 ) {
		ert[a] = part.mid( 2-a, 1 ).toInt();
		a += 1;
	}
	QString szel = part.right( 2 );

	int x=0;
	// egyesek elnevez�e
	while ( x < 3 ) {
		if ( ert[x] == 0 )
			kb[x] = "";
		else
			kb[x] = j1[ ert[x]-1 ];
			
		x += 1;
	}
	// tizesek elnevez�e
	if ( ert[1] == 0 )
		kb[1] = "";
	else
		kb[1] = j10[ ert[1]-1 ];

	QString fine, szazas;
	if ( szel == "10" )
		fine = ten;
	else if ( szel == "20" )
		fine = twenty;
	else
		fine = kb[1] + kb[0];
		
	if ( part.left( 1 ) == "0" || part.left( 1 ) == " " )
		szazas = "";
	else {
		szazas = hundred;
		if ( ert[2] == 1 )	// avoid egysz�
			kb[2] = "";
	}
		
	return kb[2] + szazas + fine;
}


double NCUtils::round( const double value, const short int digits )
{
	QString sval;
	QString format = "%12." + QString::number( digits )+ 'f';
	double roundval = sval.sprintf( format.toLatin1(), value ).toDouble();
	return roundval;
}


void NCUtils::EncodeStringHTML( QString& s )
{
	//qDebug( "�betk sz�a: %i", s.contains("�) );
	s.replace( QRegExp("[�"), "&Otilde;" );
	s.replace( QRegExp("[�"), "&otilde;" );
	s.replace( QRegExp("[�"), "&Ucirc;" );
	s.replace( QRegExp("[]"), "&ucirc;" );
}

void NCUtils::EncodeStringSQL( QString& s )
{
	//qDebug( "EncodeStringSQL(): %s", s.latin1() );
	QString bs = QString("\\");
	QString qm = QString("\'");
	QString qm2 = QString("\"");

	s.replace( bs, bs+bs );
	s.replace( qm, bs+qm );
	s.replace( qm2, bs+qm2 );
	/*
	s.replace( "'", bs+"'" );
	s.replace( '\"', bs+'\"' );
	s.replace( '\\', bs+bs );
	*/
	/*
	s.replace( QRegExp("\'"), bs+"\'" );
	s.replace( QRegExp( "\\" ), bs+bs );
	*/
	//s.replace( QRegExp("%"), bs+"%" );
	//s.replace( QRegExp("_"), bs+"_" );
	//s.replace( QRegExp( "[\"]", "\\" + "\"" );
	//s.replace( QRegExp("'"), QString("\')+"'" );
	//s.replace( QRegExp("[\"]"), "\"" );
}

void NCUtils::EncodeStringX( QString& s )
{
	QString encstring;
	const int len = s.length();
	if (len ==0 )
		return;
    for ( int sc=0; sc < len; sc++ ) {
		const QString encrow = QString( findInEncTable( s[sc] ) );
		if ( !encrow.isNull() ) {
			//int pos =2;
			// megtal�ta
			encstring += encrow.mid( (sc%3)*2 +2, 2 );
			/*
			switch ( sc%3 ) {
			case 0: pos = 2;
			case 1: pos = 4;
			case 2: pos = 6;
			}
			*/
		}
	}
	s = encstring;
}
void NCUtils::DecodeStringX( QString& s )
{
	QString decstring;
	const int n = sizeof(enctable)/sizeof(enctable[0]);
	const int len = s.length();
	if (len < 2 )
		return;
	QString codedLetter, letter;
    for ( int sc=0; sc < len/2; sc++ ) {
		codedLetter = s.mid( sc*2, 2 );
		letter = QString::null;
		// keres� a k�t�l�an
		QString encrow;
		for ( int i =0; i<n; i++ ) {
			encrow = enctable[i];
			if ( codedLetter == encrow.mid( (sc%3)*2 +2, 2 ) ) {
				letter = encrow.left(1);
				break;
			}
		}
		if ( letter.isNull() )			// nem tal�ta a kifejez�t
			return;
        decstring += letter;
	}
	s = decstring;
}

const char* NCUtils::findInEncTable( const QChar ch )
{
	const int n = sizeof(enctable)/sizeof(enctable[0]);
	// keres� a k�t�l�an
	for ( int i =0; i<n; i++ ) {
		if ( ch == enctable[i][0] )
			return enctable[i];
	}
	return 0;
}

void NCUtils::concatStringWith( QString& s, const QString& s1, const QString& delim )
{
	if ( s.isEmpty() )
		s += s1;
	else {
		s += ( s1.isEmpty() ? QString::null : delim ) +s1;
	}
}


QPoint NCUtils::popupPosition( QWidget *hostwidget, QWidget* popupwidget )
{
    int w = qApp->desktop()->width();                   // returns screen width
    int h = qApp->desktop()->height();                  // returns screen height
    int eh = popupwidget->height();
    int ew = popupwidget->width();
	QPoint p_bottom = hostwidget->mapToGlobal( QPoint( 0, hostwidget->height() ) );
	QPoint p_top = hostwidget->mapToGlobal( QPoint( 0, 0 ) );
	int x=0, y=0;

	if ( p_bottom.y() + eh > h )	//  tl lent van
		y = p_top.y() - eh;
		//sp = QPoint( p_top.x(), p_top.y() - calHeight );
	else
		y = p_bottom.y();

	if ( p_top.x() + ew > w )	//  tl jobbra van
		x = w - ew;
		//sp = QPoint( p_top.x(), p_top.y() - calHeight );
	else
		x = p_top.x();


	return QPoint( x, y );
}

double NCUtils::abs( const double d )
{
	if ( d<0 )
		return -1*d;
	else
		return d;
}

