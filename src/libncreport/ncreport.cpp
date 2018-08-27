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
#include "ncreport.h"
#include "ncpreview.h"
#include "reportparser.h"
#include "ncutils.h"
#include "globals.h"

#include <QApplication>
#include <QPainter> 
#include <QImage>
#include <QMessageBox>
#include <QFontMetrics> 
#include <QPushButton>
#include <QTextCodec>
#include <QDateTime>
#include <QObject>
#include <QFileInfo>
#include <QPixmap>
#include <QTextStream>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPrintDialog>
#include <QPicture>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlField>

#define NCREPORT_DOT_DELIMITER	"."
#define NCREPORT_PARAM_BEGINS	"$P{"
#define NCREPORT_PARAM_ENDS		"}"
#define NCREPORT_GPARAM_BEGINS	"$G{"
#define NCREPORT_GPARAM_ENDS	"}"

#define NCREPORT_VARIABLE_BEGINS	"$"
#define NCREPORT_SYSVAR_BEGINS		"$@"

#define NCREPORT_CONFIGFILE		"reportconf.ini"
#define NCREPORT_LOGFILE	"reportlog.txt"

#define SCREENFACTOR	1.0

//#include "images.h"
using namespace NCUtils;

static const char* TOKENS[] = { "==", "<=", ">=", "!=", ">", "<", "\0" };
static const int TOKENSIZES[] = { 2, 2, 2, 2, 1, 1, 0 };

//#define REPORT_DEBUG_ON

report_ElementObject::report_ElementObject( QObject* parent, const char* nm )
	: QObject( parent )
{
		//etype=""; name=""; printWhen="";
	setObjectName( nm );
	#ifdef REPORT_DEBUG_ON
		qDebug( "report_ElementObject created.(%s)", qPrintable( objectName() ) );
	#endif
	printDone = false;
}
report_ElementObject::~report_ElementObject()
{
	#ifdef REPORT_DEBUG_ON
		qDebug( "report_ElementObject destroyed.(%s)", qPrintable( objectName() ) );
	#endif
}

////////////////////////////////////////////////////////////////////
//	NCREPORT
////////////////////////////////////////////////////////////////////

NCReport::NCReport(const QString & fileName, QObject * parent, const char* name ) : QObject( parent )
//NCReport::NCReport( const QString & fileName ) : QObject( 0, "valami")
{
	iconFactory = 0;
	name        = "";
	dataSource  = Database;
	//pageWidth= dm.width();
	//pageHeight= dm.height();

	printerPageSize = QPrinter::A4;
	//printerMode = QPrinter::ScreenResolution;
	printerMode = QPrinter::HighResolution;
	pageSize ="A4";
	pageWidth= 0;
	pageHeight= 0;
	pageWidth_screen= 0;
	pageHeight_screen= 0;
    dpiX =72;
    dpiY =72;
	showPrintDialog = true;
	alwaysShowPrintDialog = false;
	columnCount=1;
	columnWidth=0;
	columnSpacing=0;
	topMargin=0;
	bottomMargin=0;
	leftMargin=0;
	rightMargin=0;
	defaultFontName="Arial";
	defaultFontSize=10;
	textDataDelimiter = '\t';
	//backcolor;
	/*
	sqlHostname = "localhost";
	sqlDatabase = "xdatabase";
	sqlUser = "user";
	sqlPassword = "password";	
	*/
	//previewWidget = new NCPreview( pic, 0, "prw" );

	dpiFactor = 1;
	reportOutput = Preview;
	pageOrientation = Portrait;
	reportFileParsed = false;
	reportFileName = fileName;
	reccount = 0;
	columncount =0;
	recno = 0;
	
	/*
	Queries = new report_Queries( this );
	pageHeader = new report_pageHeader( this );
	pageFooter = new report_pageFooter( this );
	Groups = new report_Groups( this );
	Detail = new report_Detail( this );
	*/
	dataDef = 0;
	
	//Variables.setAutoDelete(TRUE);
	//Flds.setAutoDelete(TRUE);
	
	//picPages.setAutoDelete(TRUE);

	_pageno = 0;
	_numcopies =1;
	_numforcecopies =1;
	_currentforcecopy =1;
	isforcecopies = false;	// ciklusos copy
	trimFont_prn =0;
	trimLine_prn =0;
	trimFont_pvw =0;
	trimLine_pvw =0;
	backcolor = QColor( 255,255,255 );
	queryByParameter = false;
	queryListChanged = false;
	previewForm =0;
	previewismainform = false;
	deleteReportAfterPreview = false;
	printerSetupParent = 0;
	previewismaximized = false;
	_globalPosX =0;
	_globalPosY =0;
	currentSection =0;
	dynamicSectionIncrement = 0;
	
	if ( fileName.isNull() )
		pmode = fromDatabase;
	else
		pmode = fromFile;
		
	reportID =-1;
	database = 0;
	pQueries = new report_Queries( this );
	//pQueries->queries.setAutoDelete( true );
	//DataSession.setAutoDelete( TRUE );
	//Parameters.setAutoDelete( TRUE );
	logging = false;	// logging
	loadConfig();

	setPageSize( pageSize );
	#ifdef REPORT_DEBUG_ON
		qDebug("NCReport init done." );
	#endif

}

NCReport::~NCReport()
{}

bool NCReport::wasError() { return flagError; }
QString NCReport::ErrorMsg() { return errorMsg; }
void NCReport::setForceCopies( bool set ) { isforcecopies = set; }

void NCReport::setOutputFile( const QString& fn ) { outputFileName = fn; }

void NCReport::setReportFile( const QString& fn )
{
	if ( fn == reportFileName ) {
		QFileInfo fi1( fn );
		QFileInfo fi2( reportFileName );
		if ( fi1.lastModified() != fi2.lastModified() )
			reportFileParsed = false;
	
	} else {
		reportFileParsed = false;
	}
	reportFileName = fn;
}

void NCReport::setReportID( int id )
{
	if ( id != reportID )
		reportFileParsed = false;

	reportID = id;
}

void NCReport::setReportID( const QString & sid )
{
	if ( sid != reportStringID )
		reportFileParsed = false;

	reportStringID = sid;

}

void NCReport::setParseMode( parsemode pm ) { pmode = pm; }
void NCReport::setShowPrintDialog( bool set ) { showPrintDialog = set; }
void NCReport::setCopies( int num ) { _numcopies = num; }
void NCReport::setDatabase( QSqlDatabase* db ) { database = db; }
void NCReport::setReportQuery( const QString& st ) { reportQuery = st; }
void NCReport::setIconFactory( NCIconFactoryBase* factory ) { iconFactory = factory; }

void NCReport::addQuery( const QString& _query, const QString& _alias, bool resetList )
{
	/*
	dataDef->Queries->queries.append( _query );	
	dataDef->Queries->queryCount +=1;
	*/
	if ( resetList ) {
		pQueries->queries.clear();
	}
	
	report_Query *q = new report_Query( this );
	q->queryString = _query;
	q->alias = _alias;
	pQueries->queries.append( q );
	pQueries->queryCount +=1;
		
	queryByParameter = true;
	queryListChanged = true;
}

void NCReport::addParameter( const QString& pvalue, const QString& pname, const char* ptype )
{
	report_Parameter *par = Parameters[ pname ];
	bool addNew = false;
	if ( !par ) {
		par = new report_Parameter( this );
		addNew = true;
	}
	par->stringValue = pvalue;
	if ( ptype ) {
		if ( strcmp(ptype,"numeric")==0 )
			par->type = report_Parameter::Numeric;
		else if ( strcmp(ptype,"date")==0 )
			par->type = report_Parameter::Date;
	}
	if (addNew)
		Parameters.insert( pname, par );
	#ifdef REPORT_DEBUG_ON
		qDebug( "Add parameter: %s='%s'", qPrintable( pname), qPrintable(pvalue) );
	#endif
}

void NCReport::setOutput( NCReport::Output o ) { reportOutput = o; }
void NCReport::setOrientation( NCReport::Orientation po ) { pageOrientation = po; }
		
//************* SLOTS
void NCReport::runReportToPrinter()
{
	reportOutput = Printer;
	runReport();
}

void NCReport::runReportToPrinterFromPreview()
{
	reportOutput = Printer;
	printerSetupParent = previewForm;
	runReport();
	printerSetupParent = 0;
}

void NCReport::runReportToPreview()
{
	reportOutput = Preview;
	_numforcecopies =1;	// safety
	runReport();
}

void NCReport::runReportToPDF()
{
	reportOutput = Pdf;
	_numforcecopies =1;
	runReport();
}

bool NCReport::runReport()
{
	bool ok = true;
	flagError = false;
	//if (!reportFileParsed)
	// test: always parse: 2004 03 09
	if (!reportFileParsed)
		parseXMLResource();

	if ( reportFileParsed ) {
		//if ( queryByParameter && queryListChanged ) {	// query not from XML file, it's from qryList
		//	copyQueryListToDataDef();
		//	queryListChanged = false;
		//}
		/**********************
		  REPORT PROCESS!
		**********************/
		reportProcess();

	} else
		ok =  false;
		
	return ok;
}

bool NCReport::openXMLFile()
{
	//open  resource file
	if ( !QFile::exists( reportFileName ) )  {
		flagError = true;

		errorMsg = tr("%1 report file does not exists.").arg( reportFileName );
		return false;
	}
	return true;
}

void NCReport::resetObjContainers()
{
	if ( dataDef )
		delete dataDef;

	dataDef = new report_DataDef( this );
}

bool NCReport::parseXMLResource()
{

	reportLog( tr("Parse XML def...") );
	QTime t;
    t.start();                          // start clock

	//QString _xml;
	
	if ( pmode == fromFile ) { // get XML definition file from file
		_xml = QString::null;
		if ( !loadResourceFromFile( _xml, reportFileName ) ) 
			return false;
	} else if ( pmode == fromDatabase ) {
		// get XML definition file from SQL database
		_xml = QString::null;
		NCSqlQuery *sql = (database ? new NCSqlQuery(*database) : new NCSqlQuery() );	//add to QDict
		QString _query;
		if (reportQuery.isNull()) { 
			if ( reportID > 0 )		// get by integer pk
				_query = QString("select reportdef from " NCREPORT_REPORT_TABLE " where pk=%1").arg( reportID );
	
			else	// get by form_id
				_query = QString("select reportdef from "
								NCREPORT_REPORT_TABLE ", "
								NCREPORT_SYSTEM_DATABASE ".sys_reportforms where "
								"sys_reportforms.report_fk=sys_reports.pk and sys_reportforms.form_id='%1'")
							.arg( reportStringID );
		} else { 
			_query = reportQuery;  
		}
		
		if ( !sql->exec( _query ) ) {
			flagError = true;
			errorMsg = sql->getErrorMsg();
			return false;
		}

		if ( sql->getRowCount() <= 0 ) {
			flagError = true;
			errorMsg = tr("No report definition found!");
			return false;
		}
		_xml = sql->stringValue(0);
		delete sql;
	}
		
	if ( _xml.isEmpty() ) {
		flagError = true;
		errorMsg = tr("Report definition is empty!");
		return false;
	}
	
    QXmlInputSource source;

	source.setData( _xml );

	resetObjContainers();

	ReportParser parser( this );
	ReportParserErrorHandler errHand;

	QXmlSimpleReader reader;
	reader.setContentHandler( &parser );
	reader.setErrorHandler( &errHand );

	reportFileParsed = reader.parse( source );

	if ( !reportFileParsed ) {
		//QMessageBox::information( 0, "Parse error", parser->errorProtocol() );
		flagError = true;
		errorMsg = tr("Error in report definition: %1").arg( errHand.errorString() );
	}

	#ifdef REPORT_DEBUG_ON
	    qDebug( "Report file parsing time: %d\n", t.elapsed() );
	#endif
	reportLog( tr("Parsing time elapsed: %1 msec").arg( t.elapsed() ) );

	return reportFileParsed;
}

bool NCReport::loadResourceFromFile( QString& txt, const QString& filename )
{
	QFile txtFile( filename );
	if ( !txtFile.open( QIODevice::ReadOnly ) ) {
		flagError = true;
		errorMsg = tr("Cannot open file %1").arg( filename.left(60) );
		return false;
	}
			
	QTextStream ts( &txtFile );
	if ( !encoding.isNull() )	{ // set encoding
		QTextCodec *c = QTextCodec::codecForName( encoding.toLatin1() );
		if ( c ) ts.setCodec( c );
	}
	txt = ts.readAll();	
	txtFile.close();

	return true;

}

bool NCReport::openTextSource()
{
	resourceFile.setFileName( resourceTextFile );
	if ( !resourceFile.open( QIODevice::ReadOnly ) ) {
		flagError = true;
		errorMsg = tr("Cannot open file %1").arg( resourceTextFile );
		return false;
	}
	resourceStream.setDevice( &resourceFile );
	if ( !encoding.isNull() )	{ // set encoding
		QTextCodec *c = QTextCodec::codecForName( encoding.toLatin1() );
		if ( c ) resourceStream.setCodec( c );
	}
	return true;
}

bool NCReport::closeTextSource()
{
	resourceStream.setDevice(0);
	resourceFile.close();
	return true;
}

void NCReport::copyQueryListToDataDef()
{
/*
	dataDef->Queries->queries.setAutoDelete( true );
	dataDef->Queries->queries.clear();
	dataDef->Queries->queries.setAutoDelete( false );
	
//	for ( QStringList::Iterator it = qryList.begin(); it != qryList.end(); ++it ) {
//
//		report_Query *curQuery = new report_Query( dataDef );
//		curQuery->queryString = *it;
//		curQuery->alias = ??;
//
//		dataDef->Queries->queries.append( curQuery );
//		dataDef->Queries->queryCount +=1;
//
//	}
	
	report_Query *q;
	for ( q= pQueries->queries.first(); q != 0; q = pQueries->queries.next() ) {
		dataDef->Queries->queries.append( q );
		dataDef->Queries->queryCount +=1;
	}
*/
}
bool NCReport::printerSetup( QPrinter *pr )
{
	reportLog( tr("printerSetup() ...") );

	if ( reportOutput == Pdf ) {
		pr->setOutputFormat( QPrinter::PdfFormat );
		pr->setOutputFileName( outputFileName );
		//pr->setResolution( 600 );
	}
	pr->setFullPage( true ); 
	pr->setOrientation( pageOrientation == Portrait ? QPrinter::Portrait : QPrinter::Landscape );	
	pr->setPageSize( printerPageSize );
	if ( isforcecopies ) {
		_numforcecopies = _numcopies;
		pr->setNumCopies(1);
	}
	else {
		_numforcecopies = 1;
		pr->setNumCopies( _numcopies );
	}
		
   	//if ( showPrintDialog || alwaysShowPrintDialog ) {
	if ( reportOutput == Printer && showPrintDialog ) {
    	//if ( !pr->setup() )
		QPrintDialog pdia( pr, printerSetupParent );
		if (pdia.exec() == QDialog::Accepted) {
			setOrientation( pr->orientation() == QPrinter::Portrait ? NCReport::Portrait : NCReport::Landscape );
			_numcopies = ( isforcecopies ? _numforcecopies : pr->numCopies() );
		} else
			return false;
			
	} else {
		//pr->setMinMax(0,0);
		//pr->setFromTo(0,0);
		//pr->setPageSize( QPrinter::A4 );
	}

	return true;
}


void NCReport::reportProcess()
{
	if ( reportOutput == TXT ) {
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		runQueryToFile();
		QApplication::restoreOverrideCursor();
		return;
	}

	QPainter paint;
	pa = &paint;		// save QPainter pointer
	pr = 0;
	
	setPageSize( pageSize );	// size setting

	if ( reportOutput == Printer || reportOutput == Pdf ) {
		pr = new QPrinter( printerMode );

		if ( printerSetup( pr ) ) {
			setPageSize( pageSize );	// size setting
		} else {
			delete pr;
			return;
		}
	}
	else if ( reportOutput == XML )
	{
		outputDomDocument = QDomDocument( "report" );
		currentElement = outputDomDocument.createElement("report");
		outputDomDocument.appendChild( currentElement );
	}

	QTime t;
    t.start();                          // start clock
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//NCPictureList *piclist =0;
	//if (reportOutput == preview) {
	//	piclist = new NCPictureList();
	//}

	bool querysucc;

	if ( queryByParameter ) {
		QListIterator<report_Query*> it(pQueries->queries); // iterator for query objects list
		querysucc = execQueries( it );
	} else {
		QListIterator<report_Query*> it(dataDef->Queries->queries); // iterator for query objects list
		querysucc = execQueries( it );
	}

	if ( dataSource == Database ) {
		//-------------------
		// Database
		//-------------------
		if ( !querysucc ) {
			DataSession.clear();
			if ( reportOutput == Printer || reportOutput == Pdf )
				delete pr;
			qApp->restoreOverrideCursor();
			return;
		}
		// MASTER ALIAS PROPERTIES
		if ( masterAlias.isEmpty() || DataSession.isEmpty() ) {
			flagError = true;
			errorMsg = tr("Master alias is invalid!");
			DataSession.clear();
			if ( reportOutput == Printer || reportOutput == Pdf )
				delete pr;
			qApp->restoreOverrideCursor();
			return ;
		}
		NCSqlQuery *sql = DataSession[masterAlias];
		//bool isactive = sql->isActive();
		//bool isselect = sql->isSelect();
		reccount = sql->getRowCount();
		columncount = sql->getFieldCount();

		if (reccount == 0) {
			flagError = true;
			errorMsg = tr("Data not found in master query! (%1)").arg(masterAlias);
			DataSession.clear();
			if ( reportOutput == Printer || reportOutput == Pdf )
				delete pr;
			QApplication::restoreOverrideCursor();
			return ;
		}
	} else if ( dataSource == Text ) {
		//-------------------
		// Text
		//-------------------
		if ( !openTextSource() ) {
			QApplication::restoreOverrideCursor();
			if ( reportOutput == Printer || reportOutput == Pdf )
				delete pr;
			return;
		}
		
	}	
	// report begining
	_init = true;		
	/*****************************
	(Force) iteration
	*****************************/
	for ( int fcx=0; fcx < _numforcecopies; fcx++ ) {
		_currentforcecopy = fcx+1;
		reportLog( tr("Report cycle : %1").arg( _currentforcecopy ) );

		if ( dataSource == Database )
			DataSession[masterAlias]->first();
		recno =0;
		flagFirstRecord = true;

		//_updateFieldValues( Session[masterAlias] );	// we are on first record.
		flagNewPage = true; //page driver flag
		flagBeginReport = true;
		flagEndReport = false;
		_pageno =1;
		_resetAllVariable();
		//emit processTotal( reccount );		// useful for an indicator

		while ( flagNewPage ) {	// page cycle
			//_pageno +=1;
			// new page contents: pageHeader and pageFooter
			if ( flagBeginReport ) {
				// initial expression setup.
				_updateVariableExp();
				_updateFieldValues();
				_updateGroupExp();

				flagBeginReport = false;
			} else {
				//_updateFieldValues( true );		//update system variables only
			}
			initNewPage();
			drawOverPageObjects();
			
			handleGroupHeader();

			if ( flagEndReport ) { //2004 04 27
				// report end, finally finish report group
				handleGroupFooter();
			}

			while ( !flagNewPage && !flagEndReport && !flagError ) {
				emit onProcess( recno+1 ); 		// useful for an indicator

				if (!flagNewPage) {
					drawDetail();
				}

				if (!flagNewPage) {
					_updateGroupExp();
					handleGroupFooter();
				}

				if (!flagNewPage) {
					nextRecord();	// next master record
					_updateVariableExp();
					_updateFieldValues();
				}

				if (!flagNewPage && !flagEndReport )
					handleGroupHeader();

			}
			emit pageDone( _pageno-1 );
			qApp->processEvents();
		}	// end pages while
	}	// force copy end

	if ( reportOutput == Printer || reportOutput == Pdf ) {	//end printer
		paint.end();
		delete pr;
	}
	else if ( reportOutput == XML )
	{
		if ( !outputFileName.isEmpty() )
		{ // Possibly the user wants to deal with the DOM tree directly instead of a file.
			QFile f( outputFileName );
			f.open( QFile::Truncate | QFile::Text | QFile::WriteOnly );
			QTextStream stream( &f );
			stream.setCodec( "UTF8" );
			stream << outputDomDocument.toString();
		}
	}

	// session clear
	if ( dataSource == Database )
		DataSession.clear();
	else if ( dataSource == Text )
		closeTextSource();
	//emit onProcess( 0 ); 		// useful for an indicator
	QApplication::restoreOverrideCursor();
	//#ifdef REPORT_DEBUG_ON
	    qDebug( "Report process time: %d\n", t.elapsed() );
    //#endif
	reportLog( tr("****** Report finished. Full process time: %1 msec").arg( t.elapsed() ) );
	if (reportOutput == Preview) {
	    //NCPreview *previewForm = new NCPreview( pageHeight_screen, pageWidth_screen, piclist, AppMainWindow()->windowContainer(), "prw", Qt::WDestructiveClose );
		//previewForm = new NCPreview( pageHeight_screen, pageWidth_screen, piclist, 0, "prw", WDestructiveClose );
		previewForm = new NCPreview( pageHeight_screen, pageWidth_screen, piclist, 0, "prw", 0, iconFactory );		
		previewForm->setAttribute( Qt::WA_DeleteOnClose );
		previewForm->setWindowModality( Qt::ApplicationModal );
		previewForm->setReport( this );
		previewForm->setDeleteReportOnClose( deleteReportAfterPreview );
		//connect( previewForm->actionPrint, SIGNAL( activated() ), this, SLOT( runReportToPrinter() ) );

		//if ( previewismainform )
			//qApp->setMainWidget(previewForm);

		//if ( AppConf()->showMaximized )
		if ( previewismaximized )
    		previewForm->showMaximized();
		else
	    	previewForm->show();
	}
}

void NCReport::initNewPage()
{
#ifdef RICHTEXT_DEBUG_ON
	    qDebug( "Init new page ..." );
#endif

	if ( reportOutput == Preview || reportOutput == XML ) {
		// Print preview -> draw to QPicture
		//picPages.append( new QPicture );	//append new preview page
		QPicture *pic = new QPicture;
		piclist.append( pic );	//append new preview page
		
		if ( paintBegin )
			pa->end();
		paintBegin = pa->begin( pic );
		//pa->setBackgroundMode( Qt::OpaqueMode );
		//pa->setBackgroundColor( backcolor );
	} else if ( reportOutput == Printer || reportOutput == Pdf ) {
		// draw to printer
		if (_init) {
			pa->begin( pr );
			_init = false;
		} else
			pr->newPage();
	}

	flagNewPage = false;	// flag set back
	pa->resetMatrix();	// reset transformations
	_globalPosX =0;
	_globalPosY =0;
	startY =0;	//section coordinate
	
	// 2004.01.23 switched to metric system
	dpiX = pa->device()->logicalDpiX();
	dpiY = pa->device()->logicalDpiY();

	if ( reportOutput == Preview ) {
		//pa->setWindow( 0, 0, pageWidth_screen, pageHeight_screen );
		//QBrush brush( backcolor, Qt::SolidPattern );
		//pa->setBrush( brush );
		//pageWidth = 210;	//A4 teszt
		//pageHeight = 297;
	} else {
		//pageWidth = pdm.width();
		//pageHeight = pdm.height();
		//pa->setWindow( 0, 0, pageWidth, pageHeight );
	}

	//netto size without margins
	_pageWidth = pageWidth - leftMargin - rightMargin;

	_pageHeight = pageHeight - topMargin - bottomMargin;
	//pageFooterY = _pageHeight - dataDef->pageFooter->height;	// save pageFooter start coordinates, because must know where page footer starts
	pageFooterY = pageHeight - topMargin - dataDef->pageFooter->height;	// start Y position of pageFooter
	#ifdef REPORT_DEBUG_ON
		qDebug("pageWidth = %f  pageHeight = %f", pageWidth, pageHeight );
		qDebug("pageWidth = %f(%i)  pageHeight = %f(%i)", pageWidth, toPixelX(pageWidth), pageHeight, toPixelY(pageHeight) );
		qDebug("_pageWidth = %f(%i)  _pageHeight = %f(%i)", _pageWidth, toPixelX(_pageWidth), _pageHeight, toPixelY(_pageHeight) );
		qDebug("pageFooterY = %f(%i)", pageFooterY, toPixelY(pageFooterY) );
		qDebug("Logical dpiX=%i  dpiY=%i", dpiX, dpiY );
		qDebug("Physical dpiX=%i  dpiY=%i", pa->device()->physicalDpiX(), pa->device()->physicalDpiY() );
	#endif
	reportLog( QString("pageWidth = %1(%2)  pageHeight = %3(%4)").arg(pageWidth).arg(toPixelX(pageWidth)).arg(pageHeight).arg(toPixelY(pageHeight)) );
	reportLog( QString("pageFooterY = %1(%2)").arg( pageFooterY ).arg( toPixelY(pageFooterY) ) );
	reportLog( QString("dpiX = %1  dpiY = %2").arg(dpiX).arg(dpiY) );
	/*************
	  PAGE HEADER AND FOOTER
	**************/
	_updateSystemFieldValues();	// reload system variables

	_pageno++;
	drawPageHeader();
	drawPageFooter();

	translate_position( toPixelX( leftMargin ), toPixelY( topMargin + dataDef->pageHeader->height ), FALSE );  //absolute
}

void NCReport::drawOverPageObjects()
{
	//////////////////////////////////
	// Handle over paged objects!
	//////////////////////////////////
//	if ( overPageObjects.count() > 0 )
//		initNewPage();
	
#ifdef RICHTEXT_DEBUG_ON
	    qDebug( "drawOverPageObjects() ..." );
#endif
	report_Label *dynlabel;
	//QHash<report_Label*, report_Label*>::const_iterator i = overPageObjects.constBegin();
	//while (i != overPageObjects.constEnd()) {
	//	dynlabel = i.value();
	//	if ( !dynlabel->printDone ) { // print only valid printable overpage objects
	//		drawObject( dynlabel );
	//	}
	//}
	for (int i = 0; i < overPageObjects.size(); ++i) {
		dynlabel = overPageObjects.at(i);
		if ( !dynlabel->printDone ) { // print only valid printable overpage objects
			drawObject( dynlabel );
		} else {
			//overPageObjects.removeRef( dynlabel );
		}
	}
}

void NCReport::drawReport()
{
}

bool NCReport::drawPageHeader( )
{
	bool ok = true;
	
	//p->setViewport( leftMargin, topMargin, pageWidth, pageHeader->height );
	//p->setClipping( true );
	if ( dataDef->pageHeader->height > _pageHeight ) {	// if true, than something wrong (ex. bad height of page)
		flagError = true;
		errorMsg = tr("Invalid page header operation!");
		qDebug( qPrintable(errorMsg) );
		return false;	// something wrong.
	}

	// translate to top-left corner
	translate_position( toPixelX( leftMargin ), toPixelY( topMargin ), FALSE );  //relativ
	// sets viewable rectangle
	//p->setClipRect(leftMargin, topMargin, pageWidth, pageHeader->height);
	currentSection = dataDef->pageHeader;

	if ( dataDef->pageHeader->height>0 && dataDef->pageHeader->height < _pageHeight ) {
		//Page header tartalma:

		QDomElement prevElement = currentElement;
		if ( reportOutput == XML )
		{
			QDomElement newElement = outputDomDocument.createElement( "pageHeader" );
			currentElement.appendChild( newElement );
			currentElement = newElement;
		}

		for (int i = 0; i < dataDef->pageHeader->objects.size(); ++i)
			drawObject( dataDef->pageHeader->objects.at(i) );
		
		if ( reportOutput == XML )
			currentElement = prevElement;

		//QListIterator<report_ElementObject> it( dataDef->pageHeader->objects); // iterator for pageheader objects list
        //while ( it.hasNext() ) {
        //    //report_ElementObject *obj = it.current();
		//	drawObject( it.current() );
        //}


    } else {
		// nem fert el, vagy nincs pageheader
		//ok = false;


	}

	return ok;
}

bool NCReport::drawPageFooter( )
{
	bool ok = true;
	//int startY = _pageHeight - pageFooter->height;
	currentSection = dataDef->pageFooter;

	if ( dataDef->pageFooter->height>0 && dataDef->pageFooter->height < pageHeight - dataDef->pageHeader->height ) {

		translate_position( toPixelX( leftMargin ), toPixelY( pageFooterY ), FALSE );	//relative, 
		//Page footer tartalma:

		QDomElement prevElement = currentElement;
		if ( reportOutput == XML )
		{
			QDomElement newElement = outputDomDocument.createElement( "pageFooter" );
			currentElement.appendChild( newElement );
			currentElement = newElement;
		}

		for (int i = 0; i < dataDef->pageFooter->objects.size(); ++i)
			drawObject( dataDef->pageFooter->objects.at(i) );

		if ( reportOutput == XML )
			currentElement = prevElement;

    } else {
    	//ok = false;
	}

	_resetVariables( dataDef->pageFooter->resetVariables );		//reset variables at the end of page

	return ok;
}

void NCReport::handleGroupHeader( )
{
	if ( dataDef->Groups->groupCount==0)
		return;	// no grouping

	/*
	Group set by init value. The group lifecycle being while this initial value changes.
	(while it's stepping through records)
	After change, groupFooter must be drawn (the current level), but if only:
	- the higher level did not change. If yes than must draw the higher level group also
	*/
	for (int i = 0; i < dataDef->Groups->groups.size(); ++i) {
		report_Group *group = dataDef->Groups->groups.at(i);
			
		//if (group->needDrawGroupHeader ) {
		if ( group->state == report_Group::closed ) {
			#ifdef REPORT_DEBUG_ON
				qDebug("Draw groupheader... %s", qPrintable(group->name) );
			#endif
			reportLog( tr("Draw groupheader ... %1").arg(group->name) );
			if ( drawGroupHeader( group ) )		//if have enough space
				group->state = report_Group::onProcess;  //group in process
		}
	}
}

void NCReport::handleGroupFooter( )
{
	if ( dataDef->Groups->groupCount==0)
		return;	// no groupping

	// iterate groups
    QListIterator<report_Group*> itgr( dataDef->Groups->groups); // iterator for groups
	itgr.toBack();	// pointer to back

	while ( itgr.hasPrevious() ) {
		report_Group *group = itgr.previous();

		if ( group->state == report_Group::onProcess && (group->groupChanged || flagEndReport) ) {
			#ifdef REPORT_DEBUG_ON
			qDebug("Draw groupfooter: %s",qPrintable(group->name));
			#endif
			reportLog( tr("Draw Groupfooter ... %1").arg(group->name) );
			if ( drawGroupFooter( group ) )	{ //if have enough space, close previous group
    			//it kell nullazni a Variableket
				#ifdef REPORT_DEBUG_ON
				qDebug("Reset variables: %s", qPrintable(group->resetVariables) );
				#endif
				reportLog( tr("Reset variables: ... %1").arg(group->resetVariables) );
				_resetVariables( group->resetVariables );	//reset group's variables
				//_updateFieldValues();	// refresh fields (later should refresh only variable fields)

				group->state = report_Group::closed;
			} else {
            	// no space for footer
			}
		}
	}
}

bool NCReport::drawGroupFooter( report_Group *gr )
{
	bool ok = true;
	/**************
	  GROUP Footer
	***************/
	if ( pageFooterY < 0 || startY > pageFooterY ) {	// something wrong, e.x. page height
		flagError = true;
		errorMsg = tr("Invalid group footer position!");
		qDebug( qPrintable(errorMsg) );
		return false;	// something wrong.
	}

	report_groupFooter *gFooter = gr->groupFooter;
	currentSection = gFooter;

	if ( startY + gFooter->height < pageFooterY ) {
		//groupHeader tartalma:
		//gFooter->drawTryAgain = false;
		
		/**********************************
			2003.02.07	Run Footer queries !!
		***********************************/
		QListIterator<report_Query*> qit( gFooter->Queries->queries ); // iterator for query objects list

		execQueries( qit, 'G' );
		_updateFieldValues( true, report_Field::atGroupFooter );
		
		QDomElement prevElement = currentElement;
		if ( reportOutput == XML )
		{
			QDomElement newElement = outputDomDocument.createElement( "groupFooter" );
			currentElement.appendChild( newElement );
			currentElement = newElement;
		}

		for (int i = 0; i < gFooter->objects.size(); ++i) 
			drawObject( gFooter->objects.at(i) );

		if ( reportOutput == XML )
			currentElement = prevElement;

		// a kovetkezo elemhez a kezdopozicio meghatarozasa
		translate_position( 0, toPixelY( gFooter->height ) );
	} else {
		//nincs is, vagy nem fert el a groupheader!
		ok = false;
		flagNewPage = true;
		flagNPFGroupFooter = true;	//No place for groupfooter signal
		//gFooter->drawTryAgain = true;
	}

	return ok;
}

bool NCReport::drawGroupHeader( report_Group *gr )
{
	bool ok = true;
	/**************
	  GROUP HEADER
	***************/
	if ( pageFooterY < 0 || startY > pageFooterY ) {	// ha ez, van akkor rossz valami, pl az oldalmagassag!
		flagError = true;
		errorMsg = tr("Invalid group header position!");
		qDebug( qPrintable(errorMsg) );
		return false;	// something wrong.
	}

	report_groupHeader *gHeader = gr->groupHeader;
	currentSection = gHeader;

	if ( startY + gHeader->height <= pageFooterY ) {
		//groupHeader tartalma:
		//gHeader->drawTryAgain = false;

		QDomElement prevElement = currentElement;
		if ( reportOutput == XML )
		{
			QDomElement newElement = outputDomDocument.createElement( "groupHeader" );
			currentElement.appendChild( newElement );
			currentElement = newElement;
		}

		for (int i = 0; i < gHeader->objects.size(); ++i) 
			drawObject( gHeader->objects.at(i) );

		if ( reportOutput == XML )
			currentElement = prevElement;

		// translate to next groupheader
		translate_position( 0, toPixelY( gHeader->height ) );
	} else {
		//not exists or no place enough for groupheader!

		ok = false;

		flagNewPage = true;
		flagNPFGroupHeader = true;	//No place for groupheader signal
		//gHeader->drawTryAgain = true;
	}

	return ok;
}

bool NCReport::drawDetail( )
{
	bool ok = true;
#ifdef REPORT_DEBUG_ON
	qDebug("**** Draw detail ****");
#endif
	currentSection = dataDef->Detail;

	if ( startY + dataDef->Detail->height <= pageFooterY ) {
		//groupHeader tartalma:
		//Detail->drawTryAgain = false;
		QDomElement prevElement = currentElement;
		if ( reportOutput == XML )
		{
			QDomElement newElement = outputDomDocument.createElement( "detail" );
			currentElement.appendChild( newElement );
			currentElement = newElement;
		}

		for (int i = 0; i < dataDef->Detail->objects.size(); ++i) 
			drawObject( dataDef->Detail->objects.at(i), TRUE );

		if ( reportOutput == XML )
			currentElement = prevElement;

		//startY += dataDef->Detail->height + toMillimeterY( dynamicSectionIncrement );
		translate_position( 0, toPixelY( dataDef->Detail->height )+dynamicSectionIncrement );
		dynamicSectionIncrement = 0;	//dynamic section increment reset
	} else {
		//nincs is, vagy nem fer el a groupheader!
		ok = false;
		flagNewPage = true;
		flagNPFDetail = true;	//No place for detail
		//Detail->drawTryAgain = true;
	}


	return ok;
}

void NCReport::drawObject( report_ElementObject *obj, bool checkPrintDone )
{
	if ( checkPrintDone && obj->printDone ) { // printed object needn't print again
		#ifdef RICHTEXT_DEBUG_ON
		if (obj->etype == "label")
			qDebug( "Object [%s] obj->printDone = %i", ((report_Label*)obj)->text.latin1(), obj->printDone );
		#endif
		return;
	}
	if ( !obj->printWhen.isEmpty() ) {
		bool printable = expressionParser( obj->printWhen );
		if ( !printable )
			return;
	}
	//pa->save();
			
	if (obj->etype == "label") {
		_drawLabel( (report_Label *)obj );
	}
	if (obj->etype == "field") {
		_drawLabel( (report_Field *)obj, true );
	}
	else if (obj->etype == "line") {
		_drawLine( (report_Line *)obj );
	}
	else if (obj->etype == "rectangle") {
		_drawRectangle( (report_Rectangle *)obj );
	}
	else if (obj->etype == "ellipse") {
		_drawEllipse( (report_Ellipse *)obj );
	}
	else if (obj->etype == "pixmap") {
		_drawPixmap( (report_Pixmap *)obj );
	}
	//pa->restore();
}

void NCReport::_drawLabel( report_Label *obj, bool isField )
{
	bool continueFromLastPage = (obj->printedSnip > 0);
	QString printableText;
	
	if ( continueFromLastPage ) {
		printableText = ( isField ? obj->displayValue : obj->text);
	} else {
		if ( !isField )
			_evalParameters( obj->text );
		
		if ( !textFromResource( printableText, obj, isField ) )
			return;
	}
	
	if ( printableText.isEmpty() )
		return;

	if ( reportOutput == XML )
	{
		QDomElement e = outputDomDocument.createElement( isField ? "field" : "label" );
		if ( isField )
		{
			report_Field* f = dynamic_cast<report_Field*>( obj );
			if ( f )
			{
				QString fieldtype;
				switch( f->fieldType )
				{
				case report_Field::sysvar:		fieldtype = "sysvar";		break;
				case report_Field::sqlcolumn:	fieldtype = "sql";			break;
				case report_Field::variable:	fieldtype = "variable";		break;
				case report_Field::parameter:	fieldtype = "parameter";	break;
				case report_Field::invalid:
				default:
					break;
				}
				if ( !fieldtype.isEmpty() )
					e.setAttribute( "type", fieldtype );
				e.setAttribute( "name", f->text );
				switch( f->type )
				{
				case report_Field::Numeric:		fieldtype = "numeric";		break;
				case report_Field::Date:		fieldtype = "date";			break;
				default:
				case report_Field::Text:		fieldtype = "text";			break;
				}

				e.setAttribute( "datatype", fieldtype );
			}
		}
		e.appendChild( outputDomDocument.createTextNode(printableText) );
		currentElement.appendChild( e ); // Later on we can add formating information
		return;
	}

	if (isField)
		reportLog( tr("Draw Field %1 ='%2'").arg( obj->name ).arg( obj->displayValue ) );
	else
		reportLog( tr("Draw Label %1 ='%2'").arg( obj->name ).arg( obj->text ) );
	
	int trim = 0;
	if ( reportOutput == Printer || reportOutput == Pdf )
		trim = trimFont_prn;
	else
		trim = trimFont_pvw;
		
	QFont f;
	if ( obj->fontName.isNull() )
		f.setFamily( defaultFontName );
	else
		f.setFamily( obj->fontName );

	if ( obj->fontSize == 0 )
		f.setPointSize( defaultFontSize +trim );
	else
		f.setPointSize( obj->fontSize +trim );
	//f.setPixelSize( obj->fontSize + trim );

	f.setWeight( obj->fontWeight );
	f.setItalic( obj->fontItalic );
	f.setUnderline( obj->fontUnderline );
	f.setStrikeOut( obj->fontStrikeOut );
	pa->setFont( f );
	QFontMetrics fm( f, pa->device() );

	QPen pen( obj->forecolor );
	pa->setPen( pen );

	int w,h;
	//QRect br = fm.boundingRect( obj->text );

	if (obj->width >0)
		w = toPixelX( obj->width );
	else
		w = toPixelX(_pageWidth - obj->posX);
		//w = (int)(fm.width( obj->text )*1.3);
		//w = br.width();

	if (obj->height >0)
		h = toPixelY( obj->height );
	else {
		//h = fm.height();
		//h = br.height();
		if ( printerMode == QPrinter::ScreenResolution )
			h = fm.lineSpacing();
		else
			h = fm.height();	// high resolution
	}
	
	if (obj->rotation > 0) {	// rotation, other transformations
		//pa->save();
		//pa->rotate( obj->rotation );
	}
	int aflag = obj->alignmentH | obj->alignmentV;
	if ( obj->wordbreak )
		aflag = aflag | Qt::TextWrapAnywhere; //Qt::TextWordWrap

	#ifdef REPORT_DEBUG_ON
		qDebug("x=%i y=%i w=%i h=%i startY=%f", toPixelX( obj->posX ), toPixelY( obj->posY ), w, h, startY );
	#endif
	reportLog( tr("x=%1 y=%2 w=%3 h=%4 startY=%5").arg(toPixelX( obj->posX )).arg(toPixelY( obj->posY )).arg(w).arg(h).arg(startY) );
	//----------------------
	//
	//----------------------
	if ( obj->dynamicHeight ) {
		
		QRectF textView;
		bool overPage = false;   //object to be continued in the next page
		int printedHeight =0;
		int printableSnip = 0;
		int spaceToEOP = toPixelY(pageFooterY) - _globalPosY + toPixelY( obj->posY );

		if ( obj->isRichText ) {

			//RICHTEXT
			#ifdef RICHTEXT_DEBUG_ON
				qDebug("-------------- RichText printing... ---------------" );
			#endif
			
			//if ( obj->printedSnip == 0 ) {
			if ( !obj->richText ) {
				obj->richText = new QTextDocument;
				//obj->docLayout = obj->richText->documentLayout();
				obj->richText->setHtml( printableText );
				obj->richText->setDefaultFont( f );

				if ( reportOutput == Printer || reportOutput == Pdf )
					obj->richText->documentLayout()->setPaintDevice(pr);
				//else
				//	docLayout->setPaintDevice(pr);

				//obj->richText->setTextWidth( toPixelX(obj->width) );
				obj->richText->setTextWidth( w );
				obj->richText->setPageSize( QSizeF(w,spaceToEOP) );

			}
		
			printableSnip = qRound(obj->richText->size().height()) - obj->printedSnip;
			#ifdef RICHTEXT_DEBUG_ON
				qDebug("Text: %s...", printableText.left(40).latin1() );
				qDebug("RichText textView: x:%i y:%i w:%i h:%i", textView.x(), textView.y(), textView.width(), textView.height() );
				qDebug("RichText full height: %i", obj->richText->height() );
				qDebug("obj->printedSnip = %i printableSnip = %i", obj->printedSnip, printableSnip );
			#endif

			// check if have place in section
			if ( printableSnip + toPixelY( obj->posY ) > toPixelY(currentSection->height) ) {
				// no place in section. Check if have place in page
				if ( printableSnip > spaceToEOP ) {
					// no place in page!!!
					printedHeight = spaceToEOP + fixRichTextBreakPos( obj->richText, spaceToEOP ) ;
					overPage = true;
					//dynamicSectionIncrement = spaceToEOP;
					flagNewPage = true;		// new page flag on

					#ifdef RICHTEXT_DEBUG_ON
						qDebug("No place in page!\ntextView: x:%i y:%i w:%i h:%i - printedHeight=%i", textView.x(), textView.y(), textView.width(), textView.height(), printedHeight );
					#endif
					
				} else {
					// have place in page, higher than section
					printedHeight = printableSnip;
					
					// must expand section height by difference of section height and text real height
					//dynamicSectionIncrement = printedHeight - toPixelY(currentSection->height) + toPixelY( obj->posY );
					int dynamicSectionIncrement_last = dynamicSectionIncrement;
					if ( continueFromLastPage ) // it's a continue from last page
						//dynamicSectionIncrement = printedHeight;
						dynamicSectionIncrement = printedHeight - toPixelY(dataDef->Detail->height);
					else
						dynamicSectionIncrement = printedHeight - toPixelY(currentSection->height) + toPixelY( obj->posY );

					if ( dynamicSectionIncrement < dynamicSectionIncrement_last )
						dynamicSectionIncrement = dynamicSectionIncrement_last; //use maximum possible size.
					#ifdef RICHTEXT_DEBUG_ON
						qDebug("There is enough place in page, but text higher than section height. dynamicSectionIncrement=%i",dynamicSectionIncrement );
					#endif
				}
			} else {
				// have place in section
				printedHeight = printableSnip;
				#ifdef RICHTEXT_DEBUG_ON
					qDebug("There is enough place inside the section." );
				#endif
			}
			
			////////////////
			// DRAW
			////////////////
			int x = _globalPosX + toPixelX( obj->posX );
			int y = _globalPosY + toPixelY( obj->posY );
			if ( continueFromLastPage )
				y = _globalPosY;

			//pa->drawLine( x, 0, toPixelY( pageWidth-rightMargin ), 0 );
			//pa->drawLine( x, y, toPixelY( pageWidth-rightMargin ), y );
			//pa->drawRect( x, y, w, printedHeight );
			//pa->drawRect( _globalPosX, y, toPixelY( pageWidth-leftMargin-rightMargin ), printedHeight );

			textView.setRect( 0, 0, w, printedHeight );	// object view rectangle
			textView.translate( 0, obj->printedSnip );
			if ( continueFromLastPage )
				y -= obj->printedSnip;			
			//pa->resetMatrix();
			pa->translate( x, y );
			//obj->richText->draw( pa, _globalPosX + toPixelX( obj->posX ), _globalPosY+(continueFromLastPage ? 0 : toPixelY( obj->posY )), textView, cg );
			if ( reportOutput == Printer || reportOutput == Pdf ) {
				QAbstractTextDocumentLayout::PaintContext pctx;
				pctx.clip = textView;
				obj->richText->documentLayout()->draw( pa, pctx );
				//extern int qt_defaultDpi();
				//qreal sourceDpiX = qt_defaultDpi();
				//qreal sourceDpiY = sourceDpiX;
				//QPaintDevice *dev = obj->richText->documentLayout()->paintDevice();
				//if (dev) {
				//	sourceDpiX = dev->logicalDpiX();
				//	sourceDpiY = dev->logicalDpiY();
				//}
				//const qreal dpiScaleX = qreal(pr->logicalDpiX()) / sourceDpiX;
				//const qreal dpiScaleY = qreal(pr->logicalDpiY()) / sourceDpiY;
				//// scale to dpi
				//pa->scale(dpiScaleX, dpiScaleY);
			} else {
				obj->richText->drawContents( pa, textView );
			}
			//obj->richText->drawContents( pa );
			pa->translate( -x, -y );
			
			if ( flagNewPage ) {
				//object to be continued in the next page
				obj->printedSnip += printedHeight; // increase printed Snip
				if (overPageObjects.indexOf(obj)<0)	// find
					overPageObjects.append(obj);
				obj->printDone = false;

				#ifdef RICHTEXT_DEBUG_ON
				qDebug("New page flag set TRUE. obj->printedSnip=%i", obj->printedSnip );
				#endif
			} else {
				obj->printDone = true;
				obj->printedSnip = 0;
				overPageObjects.removeAll( obj );
				if ( obj->richText ) {
					delete obj->richText;
					obj->richText =0;
				}

			}
			
		} else {
			// non richtext, but dynamic
			// SIMPLE TEXT
			textView = fm.boundingRect( _globalPosX + toPixelX( obj->posX ), _globalPosY + toPixelY( obj->posY ), 
										w, h, aflag, printableText );
		}
		
		
	} else {
		if ( obj->isRichText ) {
			QTextDocument richText;
			richText.setHtml( printableText );
			richText.setDefaultFont( f );
			if ( reportOutput == Printer || reportOutput == Pdf )
				richText.documentLayout()->setPaintDevice(pr);

			richText.setTextWidth( toPixelX(obj->width) );
			int x = _globalPosX + toPixelX( obj->posX );
			int y = _globalPosY + toPixelY( obj->posY );
			QRect textView( x, y, w, h );
			//richText.drawContents( pa, textView );
			pa->translate( x, y );

			if ( reportOutput == Printer || reportOutput == Pdf ) {
				QAbstractTextDocumentLayout::PaintContext pctx;
				pctx.clip = textView;
				richText.documentLayout()->draw( pa, pctx );
			} else {
				richText.drawContents( pa );
			}
			pa->translate( -x, -y );
			obj->printDone = true;

		} else {
			// SIMPLE TEXT, NON Dynamic height
			pa->drawText( _globalPosX + toPixelX( obj->posX ), _globalPosY + toPixelY( obj->posY ), w, h,
					//( obj->alignmentH == Qt::AlignCenter || obj->alignmentV == Qt::AlignCenter ) ? Qt::AlignCenter : obj->alignmentH | obj->alignmentV,
						  aflag, printableText );
			obj->printDone = true;
		}
	}



	//----------------------
	//
	//----------------------
	if (obj->rotation > 0) {	// restore
		//pa->restore();
		//pa->resetMatrix();
	}
}

	//pa->drawText( _globalPosX + toPixelX( obj->posX ), _globalPosY + toPixelY( obj->posY ), w, h, 
	//				obj->wordbreak ? 
	//				obj->alignmentH | obj->alignmentV | Qt::TextWrapAnywhere //Qt::TextWordWrap 
	//				: 
	//				obj->alignmentH | obj->alignmentV ,
	//			//aflag, 
	//			( isField ) ? obj->displayValue : obj->text );

void NCReport::_drawRichText( report_Label *, bool )
{
}

void NCReport::_drawLine( report_Line *obj )
{
	if ( obj->width == 0)
		obj->width = _pageWidth;

	if ( reportOutput == XML )
	{
		QDomElement e = outputDomDocument.createElement("line");
		currentElement.appendChild( e );
	}

	_setPenAndStyle( obj );
	pa->drawLine( _globalPosX+toPixelX( obj->fromX ), _globalPosY+toPixelY( obj->fromY ), _globalPosX+toPixelX( obj->toX ), _globalPosY+toPixelY( obj->toY ) );
	obj->printDone = false;
}

void NCReport::_drawRectangle( report_Rectangle *obj )
{
	if ( obj->width == 0)
		obj->width = _pageWidth;

	_setPenAndStyle( obj );
/*
	// TESZT
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(300, 0));
	//QLinearGradient linearGrad;
	linearGrad.setColorAt(0, Qt::blue);
	linearGrad.setColorAt(1, Qt::white);
	QBrush b( linearGrad );
	pa->setBrush( b );
*/
		
	if ( obj->roundValue > 0 )
		pa->drawRoundRect( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), toPixelX( obj->width ), toPixelY( obj->height ), obj->roundValue, obj->roundValue );
	else
		pa->drawRect( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), toPixelX( obj->width ), toPixelY( obj->height ) );
		//pa->drawRect( QRectF( toPoint(obj->posX,obj->posY), toSize(obj->width,obj->height))  );
	obj->printDone = false;
}

void NCReport::_drawEllipse( report_Ellipse *obj )
{
	_setPenAndStyle( obj );
	pa->drawEllipse( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), toPixelX( obj->width ), toPixelY( obj->height ) );
	obj->printDone = false;
}


void NCReport::_drawPixmap( report_Pixmap *obj )
{
    if ( !QFile::exists( obj->resource ) )
        return;

    //if (reportOutput == preview) 	// avoid crash
    //	return;

     // teszt
	if (reportOutput == Preview) {
		//return;		
	}
	//-----------------------------
	// QT4 ?
	//-----------------------------
	QPixmap im;
	if ( im.load( obj->resource ) ) {
		#ifdef REPORT_DEBUG_ON
			qDebug("Paint pixmap: ...");
		#endif
		reportLog( tr("Paint pixmap ...") );
		
		#ifdef REPORT_DEBUG_ON
			qDebug("Scale pixmap: ...");
		#endif
		if ( obj->width==0 && obj->height>0 )
			im = im.scaledToHeight( toPixelY(obj->height) );
		else if ( obj->width>0 && obj->height==0 )
			im = im.scaledToWidth( toPixelX(obj->width) );
	
		#ifdef REPORT_DEBUG_ON
			qDebug("Draw pixmap: ...");
		#endif
		if ( obj->width>0 && obj->height>0 )
			pa->drawPixmap( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), toPixelX(obj->width), toPixelY(obj->height), im );
		else	
			pa->drawPixmap( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), im );
	}
	//QImage im;
	//if ( im.load( obj->resource ) ) {
	//	if ( reportOutput == Printer ) {
	//		//im.resize( im.size().width()*dpiX/72, im.size().height()*dpiY/72 );
	//		im = im.scale( qRound((im.size().width()*dpiX/72)/SCREENFACTOR), qRound((im.size().height()*dpiY/72)/SCREENFACTOR) );
	//		//im = im.scale( im.size().width()*(1+72/dpiX), im.size().height()*(1+72/dpiY) );
	//	}
	//	#ifdef REPORT_DEBUG_ON
	//		qDebug("Paint image: ...");
	//	#endif
	//	reportLog( tr("Paint image ...") );
	//	//pa->drawPixmap( toPixelX( obj->posX ), toPixelY( obj->posY ), im, 0, 0, toPixelX( obj->width ), toPixelY( obj->height ) );
	//	//pa->drawPixmap( toPixelX( obj->posX ), toPixelY( obj->posY ), im );
	//	pa->drawImage( _globalPosX+toPixelX( obj->posX ), _globalPosY+toPixelY( obj->posY ), im );
	//	obj->printDone = true;
	//}

}


void NCReport::_setPenAndStyle( report_GeometricObject *obj )
{
	qreal trim;
	qreal lWidth = obj->lineWidth;
	if ( reportOutput == Printer || reportOutput == Pdf )
		trim = trimLine_prn;
	else {
		lWidth = obj->lineWidth;
		trim = trimLine_pvw;
	}
	if (trim == 0.0) trim =1.0;
	lWidth *= trim;
	
	QPen pen( obj->lineColor, lWidth, obj->lineStyle);
	pa->setPen( pen );
	if ( obj->fillStyle == report_GeometricObject::transparent)
		pa->setBrush( Qt::NoBrush );
	else
		pa->setBrush( obj->fillColor );
}

void NCReport::nextRecord()
{
 	// step next record

	// this function for only masterAlias
	if ( dataSource == Database ) {
		if ( !DataSession[ masterAlias ] ) {
			flagError = true;
			errorMsg = tr(" Error: DataSession[ masterAlias ] is null.");
			qDebug( qPrintable(errorMsg) );
			reportLog( errorMsg );
			return;	// something wrong.
		}

			#ifdef REPORT_DEBUG_ON

				qDebug("Step to next record. current is: %i", recno);
			#endif
			reportLog( tr("Step to next record. current is: %1").arg(recno) );
		flagEndReport = !DataSession[ masterAlias ]->next();
		if ( !flagEndReport )
			recno++;
			flagFirstRecord = false;
	} else if ( dataSource == Text ) {
		
		if ( resourceStream.atEnd() )
			flagEndReport = true;
		if ( !flagEndReport ) {
			
			if ( prevRecordPending ) // step to previous occured. we musn't readLine() again
				currentTextRow = nextTextRow;
			else {
				if ( recno>0 )
					lastTextRow = currentTextRow;
				currentTextRow = resourceStream.readLine();
				recno++;
				flagFirstRecord = false;
			}
		}
	}
	prevRecordPending = false;
	
}


void NCReport::prevRecord()
{
 	// step previous record

	// this function for only masterAlias
	if ( dataSource == Database ) {
		if ( !DataSession[ masterAlias ] ) {

			flagError = true;
			errorMsg = tr(" Error: DataSession[ masterAlias ] is null.");
			qDebug( qPrintable(errorMsg) );
			reportLog( errorMsg );

			return;	// something wrong.
		}

		if ( recno <= 0 ) {
			flagFirstRecord = true ;
		} else {
			#ifdef REPORT_DEBUG_ON
				qDebug("Step to previous record. current is: %i", recno);
			#endif
			reportLog( tr("Step to previous record. current is: %1").arg(recno) );
			DataSession[ masterAlias ]->previous();

			recno--;
			flagEndReport = false;
		}
	} else if ( dataSource == Text ) {
		
		if ( recno <= 0 ) 
			flagFirstRecord = true;
		else {
			nextTextRow = currentTextRow;
			currentTextRow = lastTextRow;
			recno--;
			flagEndReport = false;
		}
	}
	prevRecordPending = true;

}

void NCReport::_updateFieldValues( bool aTypeOnly, report_Field::refreshTypes t ) //, const QString & aliasOnly )
{
#ifdef REPORT_DEBUG_ON
	qDebug("Update field values...");
#endif
	reportLog( tr("Update field values...") );
	// refresh all fields

	//QListIterator<report_Field> it( dataDef->Flds ); // iterator for fields
	//for ( ; it.current(); ++it ) {
	for (int i = 0; i < dataDef->Flds.size(); ++i) {
		report_Field *field = dataDef->Flds.at(i);

        if ( aTypeOnly ) {
        	if ( field->refreshType != t )
         		continue;
			//if ( !aliasOnly.isNull() && field->fieldType == report_Field::sqlcolumn ) {
			//	if ( !field->text.startsWith( aliasOnly ) )
			//		return;
			//}
		}
        
		_evalFieldExp( field );
	 }

}

void NCReport::_updateSystemFieldValues()
{
	for (int i = 0; i < dataDef->Flds.size(); ++i) {
		report_Field *field = dataDef->Flds.at(i);
		if ( field->fieldType == report_Field::sysvar )
			_evalFieldExp( field );
	}
}

void NCReport::_evalFieldExp( report_Field * fld )
{
	#ifdef REPORT_DEBUG_ON
		qDebug( "_evalFieldExp ... %s type: %i", qPrintable(fld->text), fld->fieldType );
	#endif
	
	bool setValueType = FALSE;

	switch ( fld->fieldType ) {
	case report_Field::sysvar :
		fld->displayValue = systemVariable( fld->text );
		break;	
	case report_Field::variable :
		{
			#ifdef REPORT_DEBUG_ON
				qDebug( "Variable %s num.value = %f", qPrintable(fld->name), fld->numValue );
			#endif
			
			report_Variable *var = dataDef->Variables[ fld->text ];		//get variable from Variables (QDict)

			if ( var ) {
	         	if (var->type == report_Field::Numeric ) {
					#ifdef REPORT_DEBUG_ON
						qDebug( "Numeric variable %s num.value = %f", qPrintable(var->name), var->numValue );
					#endif
					fld->numValue = var->numValue;
					_setFieldNumDisplayValue( fld, var->numValue );

				} else if (var->type == report_Field::Date ) {
				}
			}
			break;

		}
	case report_Field::sqlcolumn :
		{
		fld->displayValue = getSqlColumnStringValue( fld->text, Database );
		if ( fld->displayValue == "NULL" )
			fld->displayValue = QString::null;
		setValueType = TRUE;
		break;
		}
	case report_Field::parameter :
		{
		fld->displayValue = getParameterValue( fld->text );
		setValueType = TRUE;	
		
		break;
		}
	case report_Field::lookup :
		{
		// search lookup classes
		if ( !fld->lookupClass.isEmpty() ) {
			NCReportLookup *lc =0;
			lc = lookupClasses[fld->lookupClass];
			if (!lc)
				break;
			
			fld->displayValue = lc->lookupResult( getSqlColumnValue( fld->text, dataSource ), DataSession[ masterAlias ]->record() );	// execute lookup class
			setValueType = TRUE;	
			
		}
		break;
		}
	case report_Field::textsection :
		{
		// set data from textfile source, current row and specified position
			fld->displayValue = currentTextRow.section(textDataDelimiter,fld->text.toInt(),fld->text.toInt());
			setValueType = TRUE;
			break;
		}
	case report_Field::invalid :
		break;
		
	}	// end master switch

	//fld->displayValue = retVal;
	//return retVal;

	if ( setValueType ) {
		switch ( fld->type ) {
			case report_Field::Text : break;
			case report_Field::Numeric : {
				fld->numValue = fld->displayValue.toDouble();
				_setFieldNumDisplayValue( fld, fld->numValue );
				break;
			}
			case report_Field::Date :
				if ( !fld->dateFormat.isEmpty() )
					fld->displayValue = QDateTime::fromString( fld->displayValue, Qt::ISODate ).toString( fld->dateFormat );
				break;
		}
	}	
	if ( !fld->callFunction.isNull() )
		_evalFunction( fld );
	if ( !fld->embedString.isNull() )
		fld->displayValue = embed( fld->embedString, fld->displayValue );

	fld->printDone = false;		// set flag as non printed

	return;
}

QString NCReport::embed( const QString& es, const QString& valuelist )
{
	QString val = es;
	return val.arg( valuelist );
	/*
	QStringList list;
	list = QStringList::split( ',', valuelist );	// split list

	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		report_Variable *var = dataDef->Variables[ *it ];
	*/
}


QString NCReport::getParameterValue( const QString & pname )

{
	QString rv;
	report_Parameter *par = Parameters[ pname ];
	if ( par )
		rv = par->stringValue;
	else
		rv = "*?Invalid parameter name*";

	#ifdef REPORT_DEBUG_ON
		qDebug( "getParameterValue(%s)=%s", qPrintable(pname), qPrintable(rv) );
	#endif
	reportLog( tr("getParameterValue(%1)=%2").arg(pname).arg(rv) );
	return rv;	
}

void NCReport::_setFieldNumDisplayValue( report_Field *fld, double dVal )
{
	if ( fld->numBlankIfZero && dVal == 0 ) {
		fld->displayValue = "";
		return;
	} else {
		int fw = 0;
		char fmt = 'f';
		int prec = 2;
		
		if ( fld->numDecimals >=0 )
			prec =  fld->numDecimals;
		else {
			if ( dVal == (int)dVal )
				prec =0;		// avoid to show 0 decimals
		}
		
		if ( fld->numFormat.isEmpty() ) 
			fld->displayValue = QString::number( dVal, fw, prec );
		else {
			// Format rules by QString::arg(...)
			// FORMAT: %L1;0f2
			// %L1  : local number format
			// 0f2 : 0=fieldwidth f=float 2=decimals  
			// if decimals is empty this is automatic
			QString l;
			QString r;
			if (fld->numFormat.startsWith('%') ) {
				
				if ( fld->numFormat.contains(';') ) {
					l=fld->numFormat.section( ';', 0,0);
					r=fld->numFormat.section( ';', 1,1);
				} else {
					// Old format code like "%9.2f"
					fw = fld->numFormat.section('.', 0,0).mid(1).toInt();
					fmt = fld->numFormat.right(1).at(0).toAscii();
					prec = fld->numFormat.section('.', 1,1).left(1).toInt();	
				}
				
			} else 
				r = fld->numFormat;

			if ( !r.isEmpty() ) {
				fw = r.at(0).digitValue();
				fmt = r.at(1).toAscii();
				prec = ( r.at(2).isNull() ? -1 : r.at(2).digitValue() );
			}

			if ( l.isEmpty() ) 
				fld->displayValue = QString::number( dVal, fw, prec );
			else
				fld->displayValue = l.arg( dVal, fw, fmt, prec );
			
			
			//fld->displayValue = formatNumber( dVal, fld->numFormat.toLatin1(), fld->numSeparation, fld->numSeparator, fld->numDigitPoint );
			//fld->displayValue = QString( "%L1").arg(number,0,'f',2);	
			//fld->displayValue = QString(fld->numFormat).arg(number,0,'f',2);	
		}

	}

	if (fld->displayValue == "NULL")	// beautify NULL value
		fld->displayValue ="";
}



QString NCReport::systemVariable( const QString & name )
{
    QString retVal;

	if ( name=="pageno" )
		retVal.setNum( _pageno );
	else if ( name=="numcopies" )
		retVal.setNum( _numcopies );
	else if ( name=="currentcopy" )
		retVal.setNum( _currentforcecopy );
	else if ( name=="recno" )
		retVal.setNum( recno );

	else if ( name=="reccount" )
		retVal.setNum( reccount );
	else if ( name=="date" ) {
		retVal = NCUtils::dateToString( QDate::currentDate() );
		//QDate date = QDate::currentDate();
		//retVal = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
	}
	else if ( name=="time" )
		retVal = QTime::currentTime().toString();
	else if ( name=="datetime" )
		retVal = QDateTime::currentDateTime().toString();
	else if ( name=="appshortname" )
		retVal = NCAPP_NAME; //App()->appShortName();
	else if ( name=="applongname" )
		retVal = NCAPP_LONGNAME;//App()->appLongName();
	else if ( name=="appinfo" )
		retVal = NCAPP_NAME " " NCAPP_VERSION " " NCAPP_COPYRIGHT;//App()->appLongName() +" "+App()->appVersion()+" "+App()->appCopyright();
	else if ( name=="appversion" )
		retVal = NCAPP_VERSION;//App()->appVersion();
	else if ( name=="appcopyright" )
		retVal = NCAPP_COPYRIGHT;//;App()->appCopyright();
	else if ( name=="qtversion" )
		retVal = qVersion();
	else if ( name=="os" )
		//#ifdef _WS_WIN_
		#ifdef Q_WS_WIN
			retVal = "Windows";
		#else
		retVal = "Linux";
		#endif
	else
		retVal = "*?*";

	return retVal;

}

QVariant NCReport::getSqlColumnValue( const QString & exp, DataSource ds )
{
	QVariant f;
	QString _sessionname,_columnname;
	if ( ds == Database ) {
		if ( exp.contains(NCREPORT_DOT_DELIMITER)==1 ){
			// sql column with "." delimiter
			int dPos = exp.indexOf(NCREPORT_DOT_DELIMITER);
			_sessionname = exp.left( dPos );
			_columnname = exp.mid( dPos+1 );
		} else {
				// sql column without delimiter = master alias
			_sessionname = masterAlias;
			_columnname = exp;
		}
		if ( DataSession[ _sessionname ] )	// van -e ilyen session?  !!!!!!!!!!
			f = DataSession[ _sessionname ]->value( _columnname );
	
		if ( f.type() == QVariant::String )
			//return QVariant( tr(f.toString()) );
			return f;
	} else if ( ds == Text ) {
		int pos = exp.toInt();
		f = QVariant( currentTextRow.section(textDataDelimiter,pos,pos) );
	}		
	return f;
}

QString NCReport::getSqlColumnStringValue( const QString & exp, DataSource ds )
{
	QString val,_sessionname,_columnname;
	if ( ds == Database ) {
		if ( exp.contains(NCREPORT_DOT_DELIMITER)==1 ){
			// sql column with "." delimiter
			int dPos = exp.indexOf(NCREPORT_DOT_DELIMITER);
			_sessionname = exp.left( dPos );
			_columnname = exp.mid( dPos+1 );
		} else {
			// sql column without delimiter = master alias
			_sessionname = masterAlias;
			_columnname = exp;
		}
		if ( DataSession[ _sessionname ] )	// van -e ilyen session?  !!!!!!!!!!
			val = DataSession[ _sessionname ]->stringValue( _columnname );
		
	} else if ( ds == Text ) {
		int pos = exp.toInt();
		val = currentTextRow.section(textDataDelimiter,pos,pos);
	}		
	return val;
}

void NCReport::_updateGroupExp()
{
#ifdef REPORT_DEBUG_ON
	qDebug("Update group expressions...");
#endif
	reportLog( tr("Update group expressions...") );
	flagGroupChanged = false;

    //QListIterator<report_Group> itgr( dataDef->Groups->groups); // iterator for groups
	//for ( ; itgr.current(); ++itgr ) {
	for (int i = 0; i < dataDef->Groups->groups.size(); ++i) {
		 report_Group *group = dataDef->Groups->groups.at(i);

		if ( flagGroupChanged )  	// ha a magasabbrendu group valtozott, akkor az alacsonyabbrendek is mind valtoznak!
			group->groupChanged = true;	//
		else
			_evalGroupExp( group );
	}
}

void NCReport::_evalGroupExp( report_Group* grp )
{
	//QString retVal;
	QString newVal, curVal;
	if ( flagBeginReport )
		grp->groupChanged = true;
	else {
		if ( grp->groupExpression.isEmpty() || grp->groupExpression == "0" )
			grp->groupChanged = FALSE;
		else {
			curVal = getSqlColumnStringValue( grp->groupExpression, dataSource );
			//curVal = grp->currentValue;
			nextRecord();
			if ( flagEndReport ) {
				newVal = "**end**";
				//flagEndReport = false;
			} else {
				newVal = getSqlColumnStringValue( grp->groupExpression, dataSource );
				prevRecord();
			}
			grp->groupChanged = ( curVal != newVal );	// group value changed
		}
	}

	flagGroupChanged = grp->groupChanged;

	if (grp->groupChanged) {	// signal for changed group
		#ifdef REPORT_DEBUG_ON
			qDebug( qPrintable(QString("Group exp changed: from '%1' to '%2' ").arg( curVal ).arg( newVal )) );
		#endif
		reportLog( QString("Group exp changed: from '%1' to '%2' ").arg( curVal ).arg( newVal ) );
	}
	grp->needDrawGroupHeader = (grp->state == report_Group::closed);
	grp->needDrawGroupFooter = (grp->state == report_Group::onProcess && (grp->groupChanged || flagEndReport) );
	#ifdef REPORT_DEBUG_ON
		qDebug( "needDrawGroupFooter: %x", grp->needDrawGroupFooter);
		qDebug( "needDrawGroupHeader: %x", grp->needDrawGroupHeader);
	#endif
	reportLog( tr("needDrawGroupFooter: %1").arg( grp->needDrawGroupFooter) );
	reportLog( tr("needDrawGroupHeader: %1").arg( grp->needDrawGroupHeader) );
	return;
}


void NCReport::_updateVariableExp()
{
#ifdef REPORT_DEBUG_ON
	qDebug("Update variables...");
#endif
	reportLog( tr("Update variables...") );

	QHashIterator<QString,report_Variable*> it( dataDef->Variables ); // iterator for variables

	while ( it.hasNext() ) {
		it.next();
		_evalVariableExp( it.value() );
	}
}


void NCReport::_evalVariableExp( report_Variable* var )
{
	if (!var)
		return;
	bool ok = true;
	if ( var->funcType == report_Variable::nothing ) {
		// evaluate sql column string value
		var->stringValue = getSqlColumnStringValue( var->varExp, dataSource );
	}
	else if ( var->funcType == report_Variable::count ) {
		var->numValue +=1;	// increment count value
		#ifdef REPORT_DEBUG_ON
			qDebug("Count variable increment to %f",var->numValue);
		#endif
	}
	else if ( var->funcType == report_Variable::sum ) {
		var->stringValue = getSqlColumnStringValue( var->varExp, dataSource );
		double dVal = var->stringValue.toDouble( &ok );
		if (ok) {
			var->numValue += dVal;		// increment sum value

			#ifdef REPORT_DEBUG_ON
				qDebug("Sum variable increment to %f",var->numValue);
			#endif
			//var->displayValue = var->displayValue.setNum( var->sumValue,"f",10 );
			//QCString format = "%8.2f";
			//var->displayValue = var->displayValue.sprintf( format, var->sumValue );

			//var->displayValue = formatNumber( var->sumValue, "%9.2f", true, ' ', ',' );
		}
	}

	else if ( var->funcType == report_Variable::average ) {
		// most semmi
	}
}

void NCReport::_resetVariables( const QString & vars )
{
	QStringList list = vars.split(',');
	//list = QStringList::split( ',', vars );	// split variable list

	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		report_Variable *var = dataDef->Variables[ *it ];

		if (var) {	// valid variable
			//var->countValue =0;

			//var->sumValue =0;
			var->numValue = 0;
			var->stringValue ="";
		}
		//_evalVariableExp( var );	// jra eval
	}
	//_updateVariableExp();
}


void NCReport::_resetAllVariable()
{
	QHashIterator<QString,report_Variable*> it( dataDef->Variables ); // iterator for variables
	while ( it.hasNext() ) {
		it.next();
		report_Variable *var = it.value();
		_evalVariableExp( var );
		if ( var ) {
			it.value()->numValue = 0;
			it.value()->stringValue ="";
		}
	}
}                         			

bool NCReport::execQueries( QListIterator<report_Query*>& it, const char qmode )
{
	//QProgressDialog progress( "Running queries...", "Abort", dataDef->Queries->queries.count(),
    //                            0, "progress", FALSE );

	//for ( ; ( queryByParameter ? it.current() : it2.current() ); ( queryByParameter ? ++it : ++it2 ) ) {
	int commandCount =0;
	while (it.hasNext()) {
		report_Query *obj = it.next();
		commandCount ++;
		//progress.setProgress( commandCount );
		emit queryIsRunning( commandCount );
		//qApp->processEvents();
        /*
        if ( progress.wasCancelled() ) {
			DataSession.clear();
			delete oSql;
			return false;
		}
		*/

		//report_Query *obj = queryByParameter ? it.current() : it2.current() ;

        //if ( DataSession.find( obj->alias ) && obj->alias == masterAlias ) {	// master session is already exists
		if ( DataSession[obj->alias] && obj->alias == masterAlias ) {	// master session is already exists
			flagError = true;
			errorMsg = tr("Master alias already exists. Alias name: %1").arg( obj->alias );
			return false;
		}
        
		DataSession[obj->alias] = ( database ? new NCSqlQuery(*database) : new NCSqlQuery() );	//add to QDict
		QString _query = obj->queryString;
		_evalParameters( _query );
		if ( qmode == 'G' )
			_evalParameters( _query, 'G' );		// evaluate group parameters

		#ifdef REPORT_DEBUG_ON
			qDebug("Run report query : %s", qPrintable(_query) );
		#endif
		reportLog( tr("Run report query: ")+ _query );

		if ( !DataSession[obj->alias]->exec( _query ) ) {
			flagError = true;
			errorMsg = DataSession[obj->alias]->getErrorMsg();
			return false;

		} else {
			// query success.
		}
	}

	//progress.setProgress( dataDef->Queries->queries.count() );

	return true;
}

bool NCReport::runQueryToFile()
{
	emit queryIsRunning( 1 );
	report_Query *obj = 0;
	QListIterator<report_Query*> it(dataDef->Queries->queries);
	bool found=false;
	while ( it.hasNext() ) {
		obj = it.next();
		if ( obj->alias == masterAlias ) {
			found = true;
			break;
		}
	}
	if ( !found )
		return false;
    
	QString _query = obj->queryString;
	_evalParameters( _query );

	NCSqlQuery sql;
	if ( !sql.exec( _query ) ) {
		flagError = true;
		errorMsg = sql.getErrorMsg();
		return false;
	}
	int reccount = sql.getRowCount();
	if ( reccount == 0 )
		return true;
	emit processTotal( reccount );

	QFile f( outputFileName );
	if ( !f.open( QFile::Truncate | QFile::Text | QFile::WriteOnly ) )
		return false;
	QTextStream t( &f );
	if ( !encoding.isEmpty() ) // set encoding
		t.setCodec( QTextCodec::codecForName( encoding.toLatin1() ) );	
	
	int record=0;
	int fields=0;
	bool firstrec = true;
	
	while ( sql.next() ) {
		emit onProcess( record+1 ); // useful for an indicator
		if (firstrec) {
			fields = sql.getFieldCount();
			for ( int i=0; i<fields; ++i )
				t << sql.fieldName(i) << (i==fields-1?"":"\t");
			t << endl;
		}
		for ( int i=0; i<fields; ++i ) {
			//QVariant::Type _type = sql.record().field(i).type();
			//QString s = sql.stringValue(i);
			QVariant d = sql.record().field(i).value();
			switch ( d.type() ) {
				case QVariant::Double:
					t << d.toDouble(); break;
				case QVariant::Int:
				case QVariant::LongLong:
				case QVariant::ULongLong:
					t << d.toInt(); break;
				default:
					t << d.toString(); break;
			}
			if ( i < fields-1 )
				t << "\t";

			//if ( _type == QVariant::Double || _type == QVariant::LongLong || _type == QVariant::ULongLong )
			//	s.replace('.',',');
			//t << s << (i==fields-1?"":"\t");
		}
		t << endl;
		firstrec = false;
		record++;
		qApp->processEvents();
	}
	f.close();	
	emit onProcess( reccount );

	return true;
}

void NCReport::_evalParameters( QString & qry, const char emode )
{
	// fill query string with parameters.
	const char* _BTOKEN;
	const char* _ETOKEN;

	if ( emode == 'P' ) {
		_BTOKEN = NCREPORT_PARAM_BEGINS;
		_ETOKEN = NCREPORT_PARAM_ENDS;
	} else {
		//emode == 'G'
		_BTOKEN = NCREPORT_GPARAM_BEGINS;
		_ETOKEN = NCREPORT_GPARAM_ENDS;
	}
		
	
	int noo = qry.count( _BTOKEN );
	if (noo == 0)	// not found
		return;

	//int searchpos = 0;	
	for ( int i=1; i <= noo; i++ ) {
		int beginpos = qry.indexOf( _BTOKEN, 0 );
		int endpos = qry.indexOf( _ETOKEN, beginpos+1 );
		int brutLength = endpos-beginpos +1;
		QString parname = qry.mid( beginpos, brutLength );
		#ifdef REPORT_DEBUG_ON
			qDebug("_evalQuery: beginpos=%i endpos=%i brutLength=%i parname=%s", beginpos, endpos, brutLength, qPrintable(parname) );
		#endif
		
		parname = parname.mid( 3, brutLength - 4 );
		#ifdef REPORT_DEBUG_ON
			qDebug("_evalQuery: parname=%s", qPrintable(parname) );
		#endif
		QString pval;
		if ( emode == 'P' )
			pval = getParameterValue( parname );
		else
			pval = getSqlColumnStringValue( parname, dataSource );
		//searchpos = beginpos +1;
		qry.replace( beginpos, brutLength, pval );
		#ifdef REPORT_DEBUG_ON
			qDebug("Parameter value = %s", qPrintable(pval) );
		#endif
		#ifdef REPORT_DEBUG_ON
			qDebug("qry current value: %s", qPrintable(qry) );
		#endif
	}
	return;
}

bool NCReport::expressionParser( const QString & expr )
{
	// parser for two operandus logical expressions
	if ( expr.isEmpty() )
		return true;
	if ( expr == "%2" ) {
		//qDebug( "recno%2 = %i", recno%2 );
		return ( recno%2 );
	}

	expr.trimmed();
	// tokens:	==  <  >  <=  >=  !=
	bool found = false;
	const char* token ="****";
	int lentoken =0;
	int i=0;

	// we iterate TOKENS[] array with pointer
	for ( const char** p = TOKENS; **p!=0; p++ ) {
		token = *p;
		lentoken = TOKENSIZES[i];
		#ifdef REPORT_DEBUG_ON
			//qDebug("Token: %s",token);
		#endif

		if ( expr.contains( token ) == 1 ) {
			found = true;
			break;	// a token found
		}
        i++;
	}

	if ( !found ) {
		qDebug("Invalid expression: %s", qPrintable(expr) );
		return false;
	}


	int tPos = expr.indexOf( token );
	QString	leftOperand = expr.left( tPos ).trimmed();
	QString rightOperand = expr.mid( tPos + lentoken ).trimmed();	//.local8Bit();
	char type_left, type_right;
	type_left='U';
	type_right='U';
	double leftValue_numeric, rightValue_numeric;
	leftValue_numeric = 0;
	rightValue_numeric = 0;

	QString leftValue_string, rightValue_string;

	if ( !(_evalOperand( leftOperand, leftValue_string, leftValue_numeric, type_left ) &&
		 _evalOperand( rightOperand, rightValue_string, rightValue_numeric, type_right ) ) )
		return false;
		
	#ifdef REPORT_DEBUG_ON
		qDebug("Operator: %s", token );
		qDebug("Left operand: %s", qPrintable(leftOperand) );

		qDebug("Right operand: %s", qPrintable(rightOperand) );
		qDebug("Exp. values: %s, %s, %f, %f", qPrintable(leftValue_string), qPrintable(rightValue_string), leftValue_numeric, rightValue_numeric );
	#endif

	if (type_left != type_right) {
		qDebug( "Invalid operand type in expression.");
		return false;
	}


    if ( strcmp(token, "==") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric == rightValue_numeric );
		else
	   		return ( leftValue_string == rightValue_string );
    } else if ( strcmp(token,">=") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric >= rightValue_numeric );
		else

	   		return ( leftValue_string >= rightValue_string );
    } else if ( strcmp(token,">") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric > rightValue_numeric );
		else
	   		return ( leftValue_string > rightValue_string );
    } else if ( strcmp(token,"<=") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric <= rightValue_numeric );
		else
	   		return ( leftValue_string <= rightValue_string );
    } else if ( strcmp(token,"<") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric < rightValue_numeric );
		else
	   		return ( leftValue_string < rightValue_string );
    } else if ( strcmp(token,"!=") == 0 ) {
	   	if (type_left == 'N')
	   		return ( leftValue_numeric != rightValue_numeric );
		else
	   		return ( leftValue_string != rightValue_string );
    }
	
	return false;
}

bool NCReport::_evalOperand( const QString & operand, QString & sValue, double & nValue, char & type )
{
 	// Evaluates operand. if something invalid returns false
	// sValue, nValue  variable references : we store result to this

	// type = stores the type of the operand 
	QChar fl = operand.at(0);

	bool ok=false;
	sValue = "";
	nValue = 0;

	if ( fl.isDigit() || fl == '-' ) {
		// digit
		type = 'N';
		nValue = operand.toDouble( &ok );
		if (!ok)
			return false;
	} else {
     	// letter: variable or sql column or string exp.
		if ( operand.startsWith(NCREPORT_SYSVAR_BEGINS ) ) {
			//variable
			type = 'S';
			sValue = systemVariable( operand.mid(2) );
		} else if ( operand.startsWith(NCREPORT_VARIABLE_BEGINS ) ) {
			//variable
			report_Variable *var = dataDef->Variables[ operand.mid(1) ];		//get variable from Variables (QDict)
			if ( var ) {
				if (var->funcType == report_Variable::count || var->funcType == report_Variable::sum) {
					type = 'N';
					nValue = var->numValue;
				} else {
					type = 'S';
					sValue = var->stringValue;
				}
			} else {
				qDebug("Invalid variable: %s", qPrintable(operand) );
				return false;
			}
		} else if ( fl == '\'' ) {
			// String expression
			type = 'S';
			sValue = operand.mid(1, operand.length()-2 );

		} else {
			// sql column
			QVariant f = getSqlColumnValue( operand, dataSource );
			//QString _type = QString( f.type );
			
			switch ( f.type() ) {
			case QVariant::Int:			
			case QVariant::UInt:			
			case QVariant::Double:			
			case QVariant::LongLong: {
				type = 'N';
				nValue = f.toDouble();
				break;
				}
			default: {
				type = 'S';
				sValue = f.toString();
				}
			}

		}
	}
	return true;
}

void NCReport::setPageSize(  const QString& ps )
{
	int w=0, h=0;
	QString psize = ps.toUpper();
	QPrinter::PageSize pps = QPrinter::A4;
	if ( psize == "A0" ) {
		pps = QPrinter::A0;
		w = 841; h = 1189;
	}
	else if ( psize == "A1" ) {
		pps = QPrinter::A1;
		w = 594; h = 841;
	}
	else if ( psize == "A2" ) {
		pps = QPrinter::A2;
		w = 420; h = 594;
	}
	else if ( psize == "A3" ) {
		pps = QPrinter::A3;
		w = 297; h = 420;
	}
	else if ( psize == "A4" ) {
		pps = QPrinter::A4;
		w = 210; h = 297;
	}
	else if ( psize == "A5M" ) { // half A4 - portrait
		// felezett A4 = A5
		pps = QPrinter::A4;
		w = 210; h = 148;
	}
	else if ( psize == "A5" ) {
		pps = QPrinter::A5;
		w = 148; h = 210;
	}
	else if ( psize == "A6" ) {
		pps = QPrinter::A6;
		w = 105; h = 148;
	}
	else if ( psize == "A7" ) {
		pps = QPrinter::A7;
		w = 74; h = 105;
	}
	else if ( psize == "A8" ) {
		pps = QPrinter::A8;
		w = 52; h = 74;
	}
	else if ( psize == "A9" ) {
		pps = QPrinter::A9;
		w = 37; h = 52;
	}
	else if ( psize == "B0" ) {
		pps = QPrinter::B0;
		w = 1030; h = 1456;
	}
	else if ( psize == "B1" ) {
		pps = QPrinter::B1;

		w = 728; h = 1030;
	}
	else if ( psize == "B10" ) {
		pps = QPrinter::B10;
		w = 32; h = 45;
	}
	else if ( psize == "B2" ) {
		pps = QPrinter::B2;
		w = 515; h = 728;
	}
	else if ( psize == "B3" ) {
		pps = QPrinter::B3;
		w = 364; h = 515;
	}
	else if ( psize == "B4" ) {
		pps = QPrinter::B4;
		w = 257; h = 364;
	}
	else if ( psize == "B5" ) {
		pps = QPrinter::B5;
		w = 182; h = 257;
	}
	else if ( psize == "B6" ) {
		pps = QPrinter::B6;
		w = 128; h = 182;
	}
	else if ( psize == "B7" ) {

		pps = QPrinter::B7;
		w = 91; h = 128;
	}
	else if ( psize == "B8" ) {
		pps = QPrinter::B8;
		w = 64; h = 91;
	}
	else if ( psize == "B9" ) {
		pps = QPrinter::B9;
		w = 45; h = 764;
	}
	else if ( psize == "C5E" ) {
		pps = QPrinter::C5E;
		w = 163; h = 229;
	}
	else if ( psize == "COMM10E" ) {
		pps = QPrinter::Comm10E;
		w = 105; h = 241;
	}
	else if ( psize == "DLE" ) {
		pps = QPrinter::DLE;
		w = 110; h = 220;
	}
	else if ( psize == "EXECUTIVE" ) {
		pps = QPrinter::Executive;
		w = 191; h = 254;
	}
	else if ( psize == "FOLIO" ) {
		pps = QPrinter::Folio;
		w = 210; h = 330;
	}
	else if ( psize == "LEDGER" ) {
		pps = QPrinter::Ledger;
		w = 432; h = 279;
	}
	else if ( psize == "LEGAL" ) {
		pps = QPrinter::Legal;
		w = 216; h = 356;
	}
	else if ( psize == "LETTER" ) {
		pps = QPrinter::Letter;
		w = 216; h = 279;
	}
	else if ( psize == "TABLOID" ) {
		pps = QPrinter::Tabloid;
		w = 279; h = 432;
	}

	if ( reportOutput == Printer || reportOutput == Pdf ) {
		if ( pageOrientation == Landscape ) {
			//change width/heigt
			int _w = w;
			w = h;
			h = _w;
		}
			
	}
		
	if ( w>0 && h>0 ) {
		//pageWidth_screen= w*4;
		//pageHeight_screen= h*4;
		pageWidth = w;
		pageHeight = h;
		pageWidth_screen= toPixelX( w );
		pageHeight_screen= toPixelY( h );
		#ifdef REPORT_DEBUG_ON
			qDebug("setPageSize() ..." );
			qDebug("pageWidth = %f(%i)  pageHeight = %f(%i)", pageWidth, toPixelX(pageWidth), pageHeight, toPixelY(pageHeight) );
			qDebug("pageWidth_screen = %i  pageHeight_screen = %i", pageWidth_screen, pageHeight_screen );
		#endif
		reportLog( tr("setPageSize(%1) ...").arg(ps) );
		printerPageSize = pps;
	}

}

void NCReport::_evalFunction( report_Field* fld )
{
	const QString fname = fld->callFunction.toUpper();
	if ( fname == "SAYNUMBER" )
		fld->displayValue = sayNumber( fld->numValue );
}

void NCReport::setTrimmers(  int tfont_print, int tfont_prevw, int tline_print, int tline_prevw )
{
	trimFont_prn = tfont_print;
	trimFont_pvw = tfont_prevw;
	trimLine_prn = tline_print;
	trimLine_pvw = tline_prevw;
}


void NCReport::loadConfig()
{

	QString conf;
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "trimFontPrinter" );
	if ( !conf.isEmpty() )
		trimFont_prn = conf.toInt();
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "trimFontPreview" );
	if ( !conf.isEmpty() )
		trimFont_pvw = conf.toInt();
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "trimLinePrinter" );
	if ( !conf.isEmpty() )
		trimLine_prn = conf.toInt();
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "trimLinePreview" );
	if ( !conf.isEmpty() )
		trimLine_pvw = conf.toInt();
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "alwaysshowdialog" );
	if ( !conf.isEmpty() )
		alwaysShowPrintDialog = conf.toInt();
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "printerMode" );
	if ( !conf.isEmpty() ) {
		if ( conf == "screen")
			printerMode = QPrinter::ScreenResolution;
		else if ( conf == "printer")
			printerMode = QPrinter::PrinterResolution;
		else if ( conf == "high")
			printerMode = QPrinter::HighResolution;

	}
	conf = readConfig( NCREPORT_CONFIGFILE, "settings", "log");
	logging = ( conf == "1" );
}

QPointF NCReport::toPoint( qreal mm_x, qreal mm_y )
{
	if ( reportOutput == Printer || reportOutput == Pdf )
		return QPointF( _globalPosX + mm_x*dpiX/25.4, _globalPosY + mm_y*dpiY/25.4 );
	else {
		return QPointF( _globalPosX + mm_x*dpiX/25.4*SCREENFACTOR, _globalPosY + mm_y*dpiY/25.4*SCREENFACTOR );
	}
}

QSizeF NCReport::toSize( qreal mm_w, qreal mm_h )
{
	if ( reportOutput == Printer || reportOutput == Pdf )
		return QSizeF( mm_w*dpiX/25.4, mm_h*dpiY/25.4 );
	else {
		return QSizeF( mm_w*dpiX/25.4*SCREENFACTOR, mm_h*dpiY/25.4*SCREENFACTOR );
	}
}


int NCReport::toPixelX( const double& mm )
{
	if ( reportOutput == Printer || reportOutput == Pdf )
		return qRound(mm*dpiX/25.4);
	else {
		return qRound(mm*dpiX/25.4*SCREENFACTOR);
	}
}

int NCReport::toPixelY( const double& mm )
{
	if ( reportOutput == Printer || reportOutput == Pdf )
		return qRound(mm*dpiY/25.4);
	else {
		//double d = (mm*dpiY/25.4*SCREENFACTOR);
		return qRound(mm*dpiY/25.4*SCREENFACTOR);
	}
}

int NCReport::_roundToInt( const double& d )
{
	int val = (int)d;
	if ( d-0.499 < val )
		return val;
	else
		return val+1;
}

double NCReport::toMillimeterX( int pixel )
{
	if ( reportOutput == Printer )
		return pixel*25.4/dpiX;
	else
		return pixel*25.4/dpiX/SCREENFACTOR;
}

double NCReport::toMillimeterY( int pixel )
{
	if ( reportOutput == Printer )
		return pixel*25.4/dpiY;
	else
		return pixel*25.4/dpiY/SCREENFACTOR;
}

void NCReport::reportLog( const QString& log )
{
	if ( !logging )
		return;

	QFile f( NCREPORT_LOGFILE );
	if ( !f.open( QIODevice::ReadWrite | QIODevice::Append ) )
		return;
	QTextStream t( &f );
	t << log << "\n";
	f.close();	
}

NCPreview * NCReport::previewWidget( )
{
	return previewForm;
}

void NCReport::setPreviewAsMain( bool set )
{
	previewismainform = set;
}

void NCReport::setFileEncoding( const QString & e )
{
	encoding = e;
}

void NCReport::setDeleteReportAfterPreview( bool set )
{
	deleteReportAfterPreview = set;
}

void NCReport::setPreviewIsMaximized( bool set )
{
	previewismaximized = set;
}

void NCReport::setReportDef( const QString& rdef )
{
	_xml = rdef;
}

void NCReport::translate_position( int x, int y, bool relative )
{
	if ( relative ) {
		_globalPosX += x;
		_globalPosY += y;
		startY += toMillimeterY( y );
	} else {
		_globalPosX = x;
		_globalPosY = y;
		startY = toMillimeterY( y );
	}
#ifdef REPORT_DEBUG_ON
	qDebug("translate_position( %i, %i )", x, y);
	qDebug("Global positions: _globalPosX=%i _globalPosY=%i", _globalPosX, _globalPosY);
#endif
	reportLog( QString("translate( %1, %2 )").arg(x).arg(y) );
}


void NCReport::registerLookupClass( NCReportLookup* lc )
{
	lookupClasses[lc->name()]=lc;
}

int NCReport::fixRichTextBreakPos( QTextDocument*, int )
{
	return 0;
/*	bool posInText = true;
	int _ypos =0;
	while ( true ) {
		posInText = rt.inText( QPoint( rt.width()/2, ypos + _ypos) );
		if ( !posInText )
			break;
		_ypos--;
	}
	
	return _ypos;*/
}


bool NCReport::textFromResource( QString & txt, report_Label * obj, bool isField )
{
	if ( obj->loadFromFile ) {
		QString filename = ( obj->displayValue.isEmpty() ? obj->text : obj->displayValue );
		bool ok = loadResourceFromFile( txt, filename );
		if (ok) {
			if ( isField )
				obj->displayValue = txt;
			else
				obj->text = txt;
		} else
			qDebug("Cannot load text file from %s!", qPrintable(filename) );
		return ok;
	} else {
		txt = ( isField ? obj->displayValue : obj->text);
	}
			
	return true;
}

QDomDocument NCReport::xmlOutput() const
{
	return outputDomDocument;
}
