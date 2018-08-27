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
#include "reportparser.h"
#include "ncutils.h"

using namespace NCUtils;

ReportParser::ReportParser(NCReport* rep)
{
	Report = rep;
}


ReportParser::~ReportParser()
{
}


QStringList ReportParser::quotes()
{
    return quoteList;
}


QString ReportParser::errorProtocol()
{
    return errorProt;
}


bool ReportParser::startDocument()
{
    // at the beginning of parsing: do some initialization
    quoteList.clear();
    errorProt = "";
    state = StateInit;
	QNum = 0;

    return TRUE;
}


bool ReportParser::startElement( const QString&, const QString&, const QString& qName, const QXmlAttributes& atts )
{
	bool isContainer = (state == StatePageHeader ||
				state == StatePageFooter || 
				state == StateGroupHeader || 
				state == StateGroupFooter || 
				state == StateDetail );

	// do different actions depending on the name of the tag and the
    // state you are in
	#ifdef DEBUG_ON
		qDebug("...startElement : %s", qName.latin1() );
	#endif
	if ( qName == "NCReport" && state == StateInit ) {
		state = StateDocument;

		Report->name = atts.value("name");
		if ( atts.index("pageSize")>=0 )
			Report->pageSize = atts.value("pageSize");
		if ( atts.index("orientation")>=0 )
			Report->pageOrientation = (atts.value("orientation").toUpper()=="L" ? NCReport::Landscape : NCReport::Portrait);
		if ( atts.index("pageWidth")>=0 )
			Report->pageWidth = atts.value("pageWidth").toDouble();
		if ( atts.index("pageHeight")>=0 )
			Report->pageHeight = atts.value("pageHeight").toDouble();
		if ( atts.index("printerMode")>=0 ) {
			QString pm = atts.value("printerMode");
			if ( pm == "screen")
				Report->printerMode = QPrinter::ScreenResolution;
			else if ( pm == "printer")
				Report->printerMode = QPrinter::PrinterResolution;
			else if ( pm == "high")
				Report->printerMode = QPrinter::HighResolution;
		}
		if ( atts.index("columnCount")>=0 )
			Report->columnCount = atts.value("columnCount").toInt();
		if ( atts.index("columnWidth")>=0 )
			Report->columnWidth = atts.value("columnWidth").toDouble();
		if ( atts.index("columnSpacing")>=0 )
			Report->columnSpacing = atts.value("columnSpacing").toDouble();
		if ( atts.index("topMargin")>=0 )
			Report->topMargin = atts.value("topMargin").toDouble();
		if ( atts.index("bottomMargin")>=0 )
			Report->bottomMargin = atts.value("bottomMargin").toDouble();
		if ( atts.index("leftMargin")>=0 )
			Report->leftMargin = atts.value("leftMargin").toDouble();
		if ( atts.index("rightMargin")>=0 )
			Report->rightMargin = atts.value("rightMargin").toDouble();
		if ( atts.index("defaultFontName")>=0 )
			Report->defaultFontName = atts.value("defaultFontName");
		if ( atts.index("defaultFontSize")>=0 )
			Report->defaultFontSize = atts.value("defaultFontSize").toInt();
		if ( atts.index("backcolor")>=0 )
			Report->backcolor = webColor( atts.value("backcolor") );
		// SQL options
		if ( atts.index("Hostname")>=0 )
			Report->sqlHostname = atts.value("Hostname");
		if ( atts.index("Database")>=0 )
			Report->sqlDatabase = atts.value("Database");
		if ( atts.index("User")>=0 )
			Report->sqlUser = atts.value("User");
		if ( atts.index("Password")>=0 )
			Report->sqlPassword = atts.value("Password");
		if ( atts.index("masterAlias")>=0 )
			Report->masterAlias = atts.value("masterAlias");
		//
		if ( atts.index("dataSource")>=0 ) {
			QString ds = atts.value("dataSource").toUpper();
			if ( ds.isEmpty() || ds == "DB" )
				Report->dataSource = NCReport::Database;
			else if ( ds == "TEXT" )
				Report->dataSource = NCReport::Text;
		}
		if ( atts.index("resourceTextFile")>=0 )
			Report->resourceTextFile = atts.value("resourceTextFile");
		if ( atts.index("textDataDelimiter")>=0 ) {
			QString d = atts.value("textDataDelimiter").toUpper();
			Report->textDataDelimiter = '\t';	//tab
			if ( d == "TAB" ) Report->textDataDelimiter = '\t';
			else if ( d == "COMMA" ) Report->textDataDelimiter = ',';
			else if ( d == "SEMICOLON" ) Report->textDataDelimiter = ';';
		}

    } else if ( qName == "queries" && state == StateDocument ) {
		// Queries
		state = StateQueries;

    } else if ( qName == "queries" && state == StateGroupFooter ) {
		// Queries
		state = StateGroupFooterQueries;
		
    } else if ( qName == "query" && state == StateQueries ) {
		// Query
		state = StateQuery;
		curQuery = new report_Query( Report->dataDef );
		if ( atts.index("alias")>=0 )
			curQuery->alias = atts.value("alias");

    } else if ( qName == "query" && state == StateGroupFooterQueries ) {
		// Query
		state = StateGroupFooterQuery;
		curQuery = new report_Query( Report->dataDef );
		if ( atts.index("alias")>=0 )
			curQuery->alias = atts.value("alias");
			
    } else if ( qName == "pageHeader" && state == StateDocument ) {
		// page header
		state = StatePageHeader;
		if ( atts.index("height")>=0 )
			Report->dataDef->pageHeader->height = atts.value("height").toDouble();

    } else if ( qName == "pageFooter" && state == StateDocument ) {
		// page footer
		state = StatePageFooter;
		if ( atts.index("height")>=0 )
			Report->dataDef->pageFooter->height = atts.value("height").toDouble();
		if ( atts.index("resetVariables")>=0 )
			Report->dataDef->pageFooter->resetVariables = atts.value("resetVariables");

    } else if ( qName == "groups" && state == StateDocument ) {
		// groups
		state = StateGroups;



    } else if ( qName == "group" && state == StateGroups ) {
		// a group
		state = StateGroup;


		report_Group *gr = new report_Group( Report->dataDef );
		gr->name = atts.value("name");
		
		//curGroup = new report_Group();
		//curGroup->name = atts.value("name");
		//curGroup->groupExpression = atts.value("groupExpression");
		gr->groupExpression = atts.value("groupExpression");
		if ( atts.index("startOnNewPage")>=0 )
			gr->startOnNewPage = (atts.value("startOnNewPage")=="true");
		if ( atts.index("startOnNewPageWL")>=0 )
			gr->startOnNewPageWL = atts.value("startOnNewPageWL").toInt();
		if ( atts.index("resetVariables")>=0 )
			gr->resetVariables = atts.value("resetVariables");
		//if ( atts.index("resetPageno")>=0 )
		//	gr->resetPageno = (atts.value("resetPageno")=="true");
		if ( atts.index("reprintHeader")>=0 )
			gr->reprintHeader = (atts.value("reprintHeader") == "true");

		Report->dataDef->Groups->groups.append( gr );
		Report->dataDef->Groups->groupCount +=1;

    } else if ( qName == "groupHeader" && state == StateGroup ) {
		// a group header in a group
		state = StateGroupHeader;
		
		report_groupHeader *gh = Report->dataDef->Groups->groups.last()->groupHeader;
		gh->height = atts.value("height").toDouble();


    } else if ( qName == "groupFooter" && state == StateGroup ) {
		// a group footer in a group
		state = StateGroupFooter;

		report_groupFooter *gf = Report->dataDef->Groups->groups.last()->groupFooter;
		gf->height = atts.value("height").toDouble();



		//Report->Groups->groups.current()->groupFooter = gf;

    
	} else if ( qName == "detail" && state == StateDocument ) {
		// THE DETAIL!
		state = StateDetail;

		if ( atts.index("height")>=0 )
			Report->dataDef->Detail->height = atts.value("height").toDouble();
		if ( atts.index("expandable")>=0 )
			Report->dataDef->Detail->expandable = (atts.value("expandable")=="true");

    } else if ( qName == "variables" && state == StateDocument ) {
		// Queries
		state = StateVariables;


	} else if ( qName == "variable" && state == StateVariables ) {
		// Variables!
		state = StateVariable;
		curVariable = new report_Variable( Report->dataDef ); 

		if ( atts.index("name")>=0 )
			curVariable->name = atts.value("name");

		if ( atts.index("type")>=0 ) {

			report_Field::baseTypes btype = report_Field::Text;
			QString att = atts.value("type");

			if (att == "text")
				btype = report_Field::Text;
			else if (att == "numeric")
				btype = report_Field::Numeric;
			else if (att == "date")
				btype = report_Field::Date;


			curVariable->type = btype;
		}

		if ( atts.index("funcType")>=0 ) {

			report_Variable::funcTypes type = report_Variable::nothing;
			QString vt = atts.value("funcType");

			if ( vt == "nothing" ) 
				type = report_Variable::nothing;
			else if ( vt == "count" )
				type = report_Variable::count;
			else if ( vt == "sum" )
				type = report_Variable::sum;
			else if ( vt == "average" )
				type = report_Variable::average;

			curVariable->funcType = type;
		}

		if ( atts.index("initValue")>=0 )
			curVariable->initValue = atts.value("initValue");

		if ( atts.index("resetAt")>=0 ) {
			report_Variable::resetTypes resetat = report_Variable::eofreport;
			QString vt = atts.value("resetAt");


			if ( vt == "eofreport" ) 
				resetat = report_Variable::eofreport;
			else if ( vt == "eofpage" )
				resetat = report_Variable::eofpage;
			else if ( vt == "group" )
				resetat = report_Variable::group;
			
			curVariable->resetAt = resetat;
		}
		if ( atts.index("resetGroup")>=0 )
			curVariable->resetGroupName = atts.value("resetGroup");

		//save to list
		//Report->Variables.variables.append( var );
		//Report->Variables.insert( var->name, var );
		
    
	} else if ( qName == "label" || qName == "field" && isContainer ) {
		
		//if ( !(state == StatePageHeader || state == StatePageFooter || state == StateGroupHeader || state == StateGroupFooter || state == StateDetail ) )
		bool isField = (qName == "field");
		prevstate = state;		// previous state
		state = StateElement;	// Element state set
		report_Label *lbl;
		// possible states: 
		// StatePageHeader, StatePageFooter, StateGroupHeader, StateGroupFooter, StateDetail
		if (isField) {
			curField = new report_Field( Report->dataDef );	
			lbl = curField;
			if ( prevstate == StateGroupFooter )
				curField->refreshType = report_Field::atGroupFooter;	// refresh mode
			
		} else {
			curLabel = new report_Label( Report->dataDef );	
			lbl = curLabel;
		}

		if ( atts.index("printWhen")>=0 )
			lbl->printWhen = atts.value("printWhen");
		if ( atts.index("posX")>=0 )
			lbl->posX = atts.value("posX").toDouble();
		if ( atts.index("posY")>=0 )
			lbl->posY = atts.value("posY").toDouble();
		if ( atts.index("height")>=0 )
			lbl->height = atts.value("height").toDouble();
		if ( atts.index("width")>=0 )
			lbl->width = atts.value("width").toDouble();
		if ( atts.index("fontName")>=0 )
			lbl->fontName = atts.value("fontName");
		if ( atts.index("fontSize")>=0 )
			lbl->fontSize = atts.value("fontSize").toInt();
		if ( atts.index("fontWeight")>=0 ) {
			QFont::Weight w = QFont::Normal;
			QString att = atts.value("fontWeight");
			if (att == "light")
				w = QFont::Light;
			else if (att == "normal")
				w = QFont::Normal;
			else if (att == "demibold")
				w = QFont::DemiBold;
			else if (att == "bold")
				w = QFont::Bold;
			else if (att == "black")
				w = QFont::Black;

			lbl->fontWeight = w;
		}
		if ( atts.index("fontItalic")>=0 ) 
			lbl->fontItalic = ( atts.value("fontItalic")=="true" );
		if ( atts.index("fontUnderline")>=0 )
			lbl->fontUnderline = ( atts.value("fontUnderline")=="true" );
		if ( atts.index("fontStrikeOut")>=0 )
			lbl->fontStrikeOut = ( atts.value("fontStrikeOut")=="true" );
		if ( atts.index("rotation")>=0 )
			lbl->rotation = atts.value("rotation").toInt();
		if ( atts.index("alignmentH")>=0 ) {
			int al= Qt::AlignLeft;
			QString att = atts.value("alignmentH");
			
			if (att == "left")
				al = Qt::AlignLeft;
			else if (att == "right")
				al = Qt::AlignRight;
			else if (att == "center")
				al = Qt::AlignHCenter;
			
			lbl->alignmentH = al;

		}
		if ( atts.index("alignmentV")>=0 ) {
			int al= Qt::AlignVCenter;
			QString att = atts.value("alignmentV");
			
			if (att == "top")
				al = Qt::AlignTop;
			else if (att == "bottom")
				al = Qt::AlignBottom;
			else if (att == "center")
				al = Qt::AlignVCenter;
			
			lbl->alignmentV = al;


		}
		if ( atts.index("wordbreak")>=0 )
			lbl->wordbreak = ( atts.value("wordbreak")=="true" );
		if ( atts.index("forecolor")>=0 )
			lbl->forecolor = webColor( atts.value("forecolor") );
		if ( atts.index("backcolor")>=0 )
			lbl->backcolor = webColor( atts.value("backcolor") );
		if ( atts.index("dynamicHeight")>=0 )
			lbl->dynamicHeight = ( atts.value("dynamicHeight")=="true" );
		if ( atts.index("richText")>=0 )
			lbl->isRichText = ( atts.value("richText")=="true" );
		if ( atts.index("loadFromFile")>=0 )
			lbl->loadFromFile = ( atts.value("loadFromFile")=="true" );

		//--------------------------- FIELD -----------------------------------
		
		if (isField) {
			if ( atts.index("type")>=0 ) {
				report_Field::baseTypes btype = report_Field::Text;
				QString att = atts.value("type");
				
				if (att == "text")
					btype = report_Field::Text;
				else if (att == "numeric")
					btype = report_Field::Numeric;
				else if (att == "date")
					btype = report_Field::Date;
			
				
				curField->type = btype;
			}
			if ( atts.index("ftype")>=0 ) {
				report_Field::fieldTypes ftype = report_Field::invalid;
				//variable=0, sqlcolumn, parameter, invalid
				QString att = atts.value("ftype");

				if (att == "var")
					ftype = report_Field::variable;
				else if (att == "par")
					ftype = report_Field::parameter;
				else if (att == "sql")
					ftype = report_Field::sqlcolumn;
				else if (att == "sys")
					ftype = report_Field::sysvar;
				else if (att == "lookup")
					ftype = report_Field::lookup;
				else if (att == "text")
					ftype = report_Field::textsection;
				//else 
				//	ftype = report_Field::invalid;

				curField->fieldType = ftype;
			}
			//if ( atts.index("numDigitNo")>=0 )
			//	curField->numDigitNo = atts.value("numDigitNo").toInt();
			if ( atts.index("numSeparation")>=0 )
				curField->numSeparation = (atts.value("numSeparation") == "true" );
			if ( atts.index("numSeparator")>=0 )
				curField->numSeparator = atts.value("numSeparator").at(0).toAscii();
			if ( atts.index("numDigitPoint")>=0 )
				curField->numDigitPoint = atts.value("numDigitPoint").at(0).toAscii();
			if ( atts.index("numBlankIfZero")>=0 )
				curField->numBlankIfZero = (atts.value("numBlankIfZero") == "true" );
			if ( atts.index("numFormat")>=0 )
				curField->numFormat = atts.value("numFormat");
			if ( atts.index("dateFormat")>=0 )
				curField->dateFormat = atts.value("dateFormat");
			if ( atts.index("embedString")>=0 )
				curField->embedString = atts.value("embedString");
			if ( atts.index("callFunction")>=0 )
				curField->callFunction = atts.value("callFunction");
			if ( atts.index("lookupClass")>=0 )
				curField->lookupClass = atts.value("lookupClass");
			if ( atts.index("numDecimals")>=0 )
				curField->numDecimals = atts.value("numDecimals").toInt();

		}
		
    } else if ( qName == "line" && isContainer ) {
		
		prevstate = state;		// last state
		state = StateElement;	// 
		report_Line *obj = new report_Line( Report->dataDef );	

		if ( atts.index("printWhen")>=0 )
			obj->printWhen = atts.value("printWhen");
		if ( atts.index("fromX")>=0 )
			obj->fromX = atts.value("fromX").toDouble();
		if ( atts.index("fromY")>=0 )
			obj->fromY = atts.value("fromY").toDouble();
		if ( atts.index("toX")>=0 )
			obj->toX = atts.value("toX").toDouble();
		if ( atts.index("toY")>=0 )
			obj->toY = atts.value("toY").toDouble();
		//if ( atts.index("lineStyle")>=0 )
			//obj->lineStyle = atts.value("lineStyle");
		if ( atts.index("lineWidth")>=0 )
			obj->lineWidth = atts.value("lineWidth").toInt();
		if ( atts.index("lineColor")>=0 )
			obj->lineColor = webColor( atts.value("lineColor") );
		if ( atts.index("lineStyle")>=0 ) {
			Qt::PenStyle ps = Qt::SolidLine;
			QString ls = atts.value("lineStyle");

			if (ls == "solid")
				ps = Qt::SolidLine;
			else if (ls == "dash")
				ps = Qt::DashLine;
			else if (ls == "dot")
				ps = Qt::DotLine;
			else if (ls == "dashdotdot")
				ps = Qt::DashDotDotLine;
			else if (ls == "dashdot")
				ps = Qt::DashDotLine;
			else if (ls == "nopen")
				ps = Qt::NoPen;

			obj->lineStyle = ps;
		}

		appendElementObjectList( obj );

    } else if ( qName == "rectangle" && isContainer ) {
		
		prevstate = state;		
		state = StateElement;	
		report_Rectangle *obj = new report_Rectangle( Report->dataDef );	

		if ( atts.index("printWhen")>=0 )
			obj->printWhen = atts.value("printWhen");
		if ( atts.index("posX")>=0 )
			obj->posX = atts.value("posX").toDouble();
		if ( atts.index("posY")>=0 )
			obj->posY = atts.value("posY").toDouble();
		if ( atts.index("width")>=0 )
			obj->width = atts.value("width").toDouble();
		if ( atts.index("height")>=0 )
			obj->height = atts.value("height").toDouble();
		if ( atts.index("lineWidth")>=0 )
			obj->lineWidth = atts.value("lineWidth").toInt();
		if ( atts.index("lineColor")>=0 )
			obj->lineColor = webColor( atts.value("lineColor") );
		if ( atts.index("lineStyle")>=0 ) {
			Qt::PenStyle ps = Qt::SolidLine;
			QString ls = atts.value("lineStyle");

			if (ls == "solid")
				ps = Qt::SolidLine;
			else if (ls == "dash")
				ps = Qt::DashLine;
			else if (ls == "dot")
				ps = Qt::DotLine;
			else if (ls == "dashdotdot")
				ps = Qt::DashDotDotLine;
			else if (ls == "dashdot")
				ps = Qt::DashDotLine;
			else if (ls == "nopen")
				ps = Qt::NoPen;

			obj->lineStyle = ps;
		}
		if ( atts.index("fillStyle")>=0 ) {
			if ( atts.value("fillStyle") == "filled" )
				obj->fillStyle = report_GeometricObject::filled;

		}
		if ( atts.index("fillColor")>=0 )
			obj->fillColor = webColor( atts.value("fillColor") );
		if ( atts.index("roundValue")>=0 )
			obj->roundValue = atts.value("roundValue").toInt();

		appendElementObjectList( obj );

    } else if ( qName == "ellipse" && isContainer ) {
		
		prevstate = state;		
		state = StateElement;	

		report_Ellipse *obj = new report_Ellipse( Report->dataDef );	

		if ( atts.index("printWhen")>=0 )
			obj->printWhen = atts.value("printWhen");
		if ( atts.index("posX")>=0 )
			obj->posX = atts.value("posX").toDouble();
		if ( atts.index("posY")>=0 )
			obj->posY = atts.value("posY").toDouble();
		if ( atts.index("width")>=0 )
			obj->width = atts.value("width").toDouble();
		if ( atts.index("height")>=0 )
			obj->height = atts.value("height").toDouble();
		if ( atts.index("lineWidth")>=0 )
			obj->lineWidth = atts.value("lineWidth").toInt();
		if ( atts.index("lineColor")>=0 )
			obj->lineColor = webColor( atts.value("lineColor") );
		if ( atts.index("lineStyle")>=0 ) {

			Qt::PenStyle ps = Qt::SolidLine;
			QString ls = atts.value("lineStyle");

			if (ls == "solid")
				ps = Qt::SolidLine;
			else if (ls == "dash")
				ps = Qt::DashLine;
			else if (ls == "dot")
				ps = Qt::DotLine;
			else if (ls == "dashdotdot")
				ps = Qt::DashDotDotLine;
			else if (ls == "dashdot")
				ps = Qt::DashDotLine;
			else if (ls == "nopen")
				ps = Qt::NoPen;

			obj->lineStyle = ps;
		}
		if ( atts.index("fillStyle")>=0 ) {
			if ( atts.value("fillStyle") == "filled" )
				obj->fillStyle = report_GeometricObject::filled;
		}
		if ( atts.index("fillColor")>=0 )
			obj->fillColor = webColor( atts.value("fillColor") );

		appendElementObjectList( obj );

    } else if ( qName == "pixmap" && isContainer ) {
		
		prevstate = state;		
		state = StateElement;	
		report_Pixmap *obj = new report_Pixmap( Report->dataDef );	

		if ( atts.index("printWhen")>=0 )

			obj->printWhen = atts.value("printWhen");
		if ( atts.index("posX")>=0 )
			obj->posX = atts.value("posX").toDouble();
		if ( atts.index("posY")>=0 )
			obj->posY = atts.value("posY").toDouble();
		if ( atts.index("width")>=0 )
			obj->width = atts.value("width").toDouble();
		if ( atts.index("height")>=0 )
			obj->height = atts.value("height").toDouble();

		if ( atts.index("resource")>=0 )
			obj->resource = atts.value("resource");

		appendElementObjectList( obj );

	} else {
		// error
		errorProt = "Invalid tag: " + qName;
		return FALSE;
		//return TRUE;

    }

    return TRUE;
}

bool ReportParser::endElement( const QString&, const QString&, const QString& qName)
{
    // "pop" the state and do some actions
	#ifdef DEBUG_ON
		qDebug("...endElement : %s", qName.latin1() );
	#endif
    switch ( state ) {
	case StateQueries:

		state = StateDocument;
		break;

	case StateQuery:
		state = StateQueries;
		curQuery->queryString = curText;
		Report->dataDef->Queries->queries.append( curQuery );
		Report->dataDef->Queries->queryCount +=1;
		break;

	case StateGroupFooterQueries:
		state = StateGroupFooter;
		break;
		
	case StateGroupFooterQuery:
		state = StateGroupFooterQueries;
		curQuery->queryString = curText;
		Report->dataDef->Groups->groups.last()->groupFooter->Queries->queries.append( curQuery );
		Report->dataDef->Groups->groups.last()->groupFooter->Queries->queryCount +=1;
		break;
		
	case StatePageHeader:
		state = StateDocument;
		break;
	
	case StatePageFooter:
		state = StateDocument;
		break;

	case StateGroups:
		state = StateDocument;
		break;

	case StateGroup:
		state = StateGroups;
		//curGroup->queryString = curQueryString;
		//Report->Groups->groups.append( curGroup );
		//QNum +=1;
		break;

	case StateGroupHeader:
		state = StateGroup;
		break;

	case StateGroupFooter:
		state = StateGroup;
		break;

	case StateDetail:
		state = StateDocument;
		break;

	case StateVariables:
		state = StateDocument;
		break;

	case StateVariable:
		state = StateVariables;
		curVariable->varExp = curText;
		Report->dataDef->Variables.insert( curVariable->name, curVariable );

		break;

	case StateElement:

		state = prevstate;	//restore state

		if (qName == "label") {
			curLabel->text = curText;
			appendElementObjectList( curLabel );
		} else if (qName == "field") {
			curField->text = curText;
			/* 2003 02 06
			if ( curText.startsWith("$") )	// variable or sqlcolumn
				curField->fieldType = report_Field::variable;
			else
				curField->fieldType = report_Field::sqlcolumn;
			*/	
			appendElementObjectList( curField );
			Report->dataDef->Flds.append( curField );
		}
		break;

	default:
	    // do nothing
	    break;
    }
    return TRUE;
}


bool ReportParser::characters( const QString& ch )
{
    // we are not interested in whitespaces
    QString ch_simplified = ch.trimmed();
    if ( ch_simplified.isEmpty() )

		return TRUE;

    switch ( state ) {
	case StateQuery:
	case StateGroupFooterQuery:
	case StateVariable:
	case StateElement:
		curText = ch_simplified;
		break;
	default:
	    //return FALSE;
		return TRUE;

    }

    return TRUE;
}


QString ReportParser::errorString()
{
    //return "The document is not in the NCReport file format.";

	return QXmlDefaultHandler::errorString();
}


bool ReportParser::fatalError( const QXmlParseException& exception )
{
    errorProt += QString( "fatal parsing error: %1 in line %2, column %3\n" )
	.arg( exception.message() )
	.arg( exception.lineNumber() )
	.arg( exception.columnNumber() );

    return QXmlDefaultHandler::fatalError( exception );
}

bool ReportParser::error( const QXmlParseException& exception )
{
    errorProt += QString( "Parsing error: %1 in line %2, column %3\n" )
	.arg( exception.message() )
	.arg( exception.lineNumber() )
	.arg( exception.columnNumber() );

    return QXmlDefaultHandler::error( exception );
}

void ReportParser::appendElementObjectList( report_ElementObject *eo )
{
	switch ( prevstate ) {
	case StatePageHeader:
		Report->dataDef->pageHeader->objects.append( eo );

		break;
	case StatePageFooter:
		Report->dataDef->pageFooter->objects.append( eo );
		break;
	case StateGroupHeader:
		Report->dataDef->Groups->groups.last()->groupHeader->objects.append( eo );
		//Report->pageFooter->objects.append( eo );
		break;
	case StateGroupFooter:
		Report->dataDef->Groups->groups.last()->groupFooter->objects.append( eo );
		break;
	case StateDetail:

		Report->dataDef->Detail->objects.append( eo );
		break;
	default:
		//semmi
		break;
	}
}

bool ReportParserErrorHandler::error( const QXmlParseException & exception )
{
    errorProt += QString( "Parsing error: %1 in line %2, column %3\n" )
	.arg( exception.message() )
	.arg( exception.lineNumber() )
	.arg( exception.columnNumber() );

	return true;	//QXmlErrorHandler::error( exception );
}
bool ReportParserErrorHandler::fatalError( const QXmlParseException & exception )
{
    errorProt += QString( "Fatal parsing error: %1 in line %2, column %3\n" )
		.arg( exception.message() )
		.arg( exception.lineNumber() )
		.arg( exception.columnNumber() );


	return true;	//QXmlErrorHandler::fatalError( exception );
}
bool ReportParserErrorHandler::warning( const QXmlParseException & exception )
{
    errorProt += QString( "Warning: %1 in line %2, column %3\n" )
		.arg( exception.message() )
		.arg( exception.lineNumber() )
		.arg( exception.columnNumber() );

	return true;	//QXmlErrorHandler::warning( exception );
}
QString ReportParserErrorHandler::errorString() const
{
    //return "The document is not in the NCReport file format.";

	//return QXmlDefaultHandler::errorString();
	return errorProt;
}





