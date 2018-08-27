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
#ifndef REPORTPARSER_H
#define REPORTPARSER_H

#include "ncreport.h"

#include <qxml.h>
#include <qstringlist.h>
#include <QXmlErrorHandler>
#include <QXmlDefaultHandler>

/*!
Derived XML SAX parser class for parsing report definition XML structure.
*/
class ReportParser : public QXmlDefaultHandler
{
public:
    ReportParser(NCReport * rep );
    virtual ~ReportParser();

    // return the list of quotes
    QStringList quotes();

    // return the error protocol if parsing failed
    QString errorProtocol();

    // overloaded handler functions
    bool startDocument();
    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );

    QString errorString();

    bool fatalError( const QXmlParseException& exception );
	bool error( const QXmlParseException& exception );
	//QString getErrorMsg() { return errorProt; }
	void appendElementObjectList( report_ElementObject *eo );

private:
    QStringList quoteList;
    QString errorProt;

    QString author;
    QString reference;
	QString curText;	// current parsed characters

	// These pointers need, because used by both startElement() and endElement()
	// The text saved in characters() can be process in endElement() only.
	report_Query *curQuery;
	report_Label *curLabel;
	report_Field *curField;
	report_Variable *curVariable;
	//report_Group *curGroup;
	//report_groupHeader *curGroupHeader;
	//report_groupFooter *curGroupFooter;


	int QNum;

    enum State {
		StateInit,
		StateDocument,
		StatePageHeader,
		StatePageFooter,
		StateGroups,
		StateGroup,
		StateGroupHeader,
		StateGroupFooter,
		StateQueries,
		StateQuery,
		StateGroupFooterQueries,
		StateGroupFooterQuery,
		StateDetail,
		StateElement,
		StateVariables,
		StateVariable,
	StateQuote,
	StateLine,
	StateHeading,
	StateP

    };
    State state, prevstate;		//prevstate: because of saving the last state

	NCReport* Report;
};

/*!
Derived XML SAX error handler for parser
*/
class ReportParserErrorHandler : public QXmlErrorHandler
{
public:
	virtual ~ReportParserErrorHandler() {}
	bool warning( const QXmlParseException & exception );
	bool error( const QXmlParseException & exception );
	bool fatalError( const QXmlParseException & exception );
	QString errorString() const;
private:
	QString errorProt;
};

#endif //REPORTPARSER_H
