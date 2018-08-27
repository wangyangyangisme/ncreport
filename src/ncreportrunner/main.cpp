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
#include "ncreporttestform.h"

#include "../libncreport/ncreport.h"
#include "../libncreport/globals.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QStringList>
#include <QDateTime>
#include <QSqlError>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>


#define SHOW_ARGUMENTS


//////////////////////////////////////
// SAMPLE MODES
// 0 = demo mode
// 1 = command line mode
// 2 = test form mode
// 3 = special custom mode
//////////////////////////////////////

#define MODE_MAIN	2
// command line (non-demo) mode

//////////////////////////////////////
// DEMOS (by number)
// 0 = Price list
// 1 = Invoice
//////////////////////////////////////

#define MODE_DEMO_NUMBER		0	// demo number. Can change by command line parameter also

//////////////////////////////////////
// Print Preview? (true/false)
//////////////////////////////////////

#define MODE_PREVIEW		false	// if is true, the output is to preview


//////////////////////////////////////

#if (MODE_MAIN==0)

int main(int argc, char *argv[] )
{
	// CREATE QT APPLICATION
	int demo_number = MODE_DEMO_NUMBER;
	NCReport::Output output = NCReport::Preview;
	QString pdfFile("testpdf.pdf");
    if ( argc >= 2 )
		demo_number = QString( argv[1] ).toInt();
	if ( argc >= 3 ) {
		QString s = argv[2];
		if ( s == "-preview" ) output = NCReport::Preview;
		else if ( s == "-print" ) output = NCReport::Printer;
		else if ( s == "-pdf" ) output = NCReport::Pdf;
	}
	if ( argc >= 4 )
		pdfFile = argv[3];

    QApplication a( argc, argv );
    a.connect( &a, SIGNAL( lastWindowClosed()), &a, SLOT( quit()) );
	
	// Install translator
	QTranslator reportTranslator;
	reportTranslator.load("ncreport_" + QLocale::system().name());
	//reportTranslator.load("ncreport_de");
	a.installTranslator(&reportTranslator);	
	QTextCodec::setCodecForTr( QTextCodec::codecForName( "ISO8859-2" ) );
	
	// SQL/DATABASE CONNECTION
	QSqlDatabase defaultDB = QSqlDatabase::addDatabase("QMYSQL");
	if ( !defaultDB.isValid() ) {
        //fprintf( stderr, "Error: Could not load database driver. \n" );
		QMessageBox::warning( 0, "NCReport error", QObject::tr("Could not load database driver.") );
		return 1;
	}
								
	defaultDB.setHostName( "localhost" );
	defaultDB.setDatabaseName( "northwind" );
	defaultDB.setUserName( "northuser" );
	defaultDB.setPassword( "northwind" );

	if ( !defaultDB.open() ) {
		//fprintf( stderr, "Error: Cannot open database. %s\n", qPrintable( defaultDB.lastError().databaseText() ) );
		QMessageBox::warning( 0, "NCReport error", QObject::tr("Cannot open database: ")+defaultDB.lastError().databaseText() );
		return 1;
	}		

	// CREATE REPORT OBJECT		
    NCReport * report = new NCReport( QString::null, 0 );
	
	report->setParseMode( NCReport::fromFile );
	report->setFileEncoding( "iso8859-2");
	report->setShowPrintDialog( true );
	report->setPreviewAsMain( true );	// preview is the main form on preview mode
	report->setDeleteReportAfterPreview( true );	// delete report object after close preview
	report->setPreviewIsMaximized( false );
	report->setOutput( output );
	report->setOtputPdfFile( pdfFile );

	switch (demo_number) {
		case 0: {
			report->setReportFile( "samples/pricelist.xml" );
			report->addParameter( "%", "prodFilt" );	// Adds a parameter. Parameter name = "prodFilt", value="%"
			break;
		}
		case 1: {
			report->setReportFile( "samples/invoice.xml" );
			report->addParameter( "1", "companypk" );	// Adds a parameter. Parameter name = "companypk", value="1"
			report->addParameter( "10051", "documentPK" );	// Adds a parameter. Parameter name = "documentPK", value="10051"
			break;
		}
		case 2: {
			report->setReportFile( "samples/pricelist2.xml" );
			report->addParameter( "%", "prodFilt" );	// Adds a parameter. Parameter name = "prodFilt", value="%"
			break;
		}
		case 3: {
			report->setReportFile( "samples/p_text.xml" );
			report->addParameter( "%", "prodFilt" );	// Adds a parameter. Parameter name = "prodFilt", value="%"
			break;
		}
	}

	report->runReport();		

	bool error = report->wasError();
	QString err = report->ErrorMsg();

	if ( output != NCReport::Preview )	//  delete report object if report has done directly to printer
		delete report;

	if ( error ) {
		QMessageBox::information( 0, "Riport error", err );
		return -1;
	}
	if ( output == NCReport::Preview )
		return a.exec();

	return 0;
}

#elif (MODE_MAIN==1)

 static const char* _usage = "Usage: %s [OPTIONS]\n"
 " Options:\n"
 "  -?, -help                  Display this help and exit\n"
 "  -f, -report-file <file>    Name of the report definition (xml) file.\n"
 "                             If this is set, the report definition\n"
 "                             will be parsed from this file instead of a database.\n"
 "  -SD, -sql-driver <driver>  Qt sql driver name for database connection (default is QMYSQL)\n"
 "  -H, -host <hostname>       Database host name or IP address (default is localhost)\n"
 "  -U, -user <username>       Database user login name\n"
 "  -p, -password <password>   Database user login password\n"
 "  -D, -database <dbname>     Database name to use\n"
 "  -O, -output	<print | preview>\n"
 "                             Output type. Use \"print\" for send report to printer or \"preview\"\n"
 "                             to show report in a preview window. The default scenario is.\n"
 "                             the preview\n"
 "  -copies <1..50>            Number of copies in case output is printer\n"
 "  -force-copy                Use forced copy printing method. This is useful for documents\n"
 "                             in which have to know the number of current copy.\n"
 "  -rID, -report-db-id <id>   ID number of the report definition (xml) text in a database record.\n"
 "                             If this is set the report definition will be parsed from\n"
 "                             database/table/record instead of a file.\n"
 "  -par, -add-parameter <parameter value>,<parameter name>\n"
 "                             Adds a custom parameter to report. You must specify the value\n"
 "                             and name of the parameter separated by comma.\n"
 "                             The P{parametername} expression can be used in the report definition\n"
 "                             (Example: \"Robert\",firstname)\n"
 "";




int main(int argc, char *argv[] )
{
  if ( argc < 2 ) {
    fprintf( stdout, _usage, argv[0] );
    return 1;
  }

  int i;
  QString driver("QMYSQL");
  QString host("localhost");
  QString user("root");
  QString password;
  QString database("ncreport");
  enum OutputType { Print, Preview };
  OutputType output = Preview;
  bool showDialog = false;
  bool isforce = false;
  int copies =1;
// 	NCReport::parsemode pmode = NCReport::fromFile;
  QString repFile;
  int reportID = -1;

  // PARSE ARGUMENTS
  QString arg,value;

  //i = 1;
  //while ( i<argc ) {
  for (i = 1; i < argc; i++) {
    arg = argv[i];
    value = argv[i+1];
    if ( value.startsWith( '-' ) ) {
      value = QString();
    }
// #define REPORT_DEBUG_ON
#ifdef REPORT_DEBUG_ON
    fprintf( stderr, "Argument: %s\n", qPrintable(arg) );
#endif

    if ( arg == "-user" || arg =="-U")
      user = value;
    else if ( arg == "-host" || arg =="-H")
      host = value;
    else if ( arg == "-p" || arg == "-password" )
      password = value;
    else if ( arg == "-database" || arg =="-D")
      database = value;
    else if ( arg == "-sql-driver" || arg =="-SD")
      driver = value;
    else if ( arg == "-output" || arg =="-O")
      output =  (value == "print") ? Print : Preview;
    else if ( arg == "-copies" )
      copies = value.toInt();
    else if ( arg == "-force-copy" )
      isforce = true;
    else if ( arg == "-report-file" || arg == "-f" )
      repFile = value;
    else if ( arg == "-report-db-id" || arg == "-rID" )
      reportID = value.toInt();
    else if ( arg == "-?" || arg =="-help") {
      fprintf( stderr, _usage, argv[0] );
      return 0;
    }
  }

  // ARGUMENT VALIDATION
  if ( !repFile.isNull() && reportID > 0 ) {
    fprintf( stderr, "Invalid argument. Use -report-file <file> OR -report-db-id <ID> . You may not use both these parameters." );
    return 1;
  }

  // CREATE QT APPLICATION
  QApplication a( argc, argv );
  a.connect( &a, SIGNAL( lastWindowClosed()), &a, SLOT( quit()) );

  // SQL/DATABASE CONNECTION
  QSqlDatabase defaultDB = QSqlDatabase::addDatabase( driver );
  if ( !defaultDB.isValid() ) {
    fprintf( stderr, "Error: Could not load %s database driver. \n", qPrintable(driver) );
    //QMessageBox::warning( 0, "NCReport error", QObject::tr("Could not load %1 database driver.").arg( driver ) );
    return 1;
  }

  defaultDB.setDatabaseName( database );
  defaultDB.setUserName( user );
  defaultDB.setPassword( password );
  defaultDB.setHostName( host );

  if ( !defaultDB.open() ) {
    fprintf( stderr, "Error: Cannot open database. %s\n", qPrintable( defaultDB.lastError().databaseText()) );
    //QMessageBox::warning( 0, "NCReport error", QObject::tr("Cannot open database: ")+defaultDB.lastError().databaseText() );
    return 1;
  }
  /*
    QProgressDialog progress( "Copying files...", "Abort Copy", 1,
    0, "progress", TRUE );
    progress.show();
  */
  qDebug( "Start report process...\n" );
  QTime t;
  t.start();                          // start clock


  // CREATE REPORT OBJECT
  NCReport * report = new NCReport( QString::null, 0 );
  bool ok = true;

  report->setPreviewAsMain( true );
  report->setPreviewIsMaximized( false );

  report->setParseMode( reportID > 0 ? NCReport::fromDatabase : NCReport::fromFile );
  report->setReportID( reportID );	// use string id
  report->setReportFile( repFile );
  report->setFileEncoding( "iso8859-2");

  // ADD PARAMETERS To REPORT
  for (i = 1; i < argc; i++) {
    arg = argv[i];
    value = argv[i+1];		// "value","name"	(- separated by comma)
    QStringList list = value.split( ',' );	// split

    if ( arg == "-add-parameter" || arg == "-par" ) {
      report->addParameter( list[0], list[1] );
#ifdef REPORT_DEBUG_ON
      qDebug( "Add parameter to report: %s,%s", qPrintable(list[0]), qPrintable(list[1]) );
#endif
    }
  }
  /*
    report->addParameter( AppDB(), "AppDB" );
    report->addParameter( AppDB('T'), "AppDBT" );
    report->addParameter( QString("%1").arg( pk ), "analPK" );
    report->addParameter( QDate::currentDate().toString(), "date" );
  */

  if ( output == Preview )
    report->runReportToPreview();
  else {
    report->setShowPrintDialog( showDialog );
    report->setCopies( copies );
    report->setForceCopies( isforce );
    report->runReportToPrinter();
  }

  ok = !report->wasError();
  if ( !ok )
    fprintf( stderr, "Error in report: %s\n", qPrintable(report->ErrorMsg()) );

  delete report;

  qDebug( "Report process time: %d ms\n", t.elapsed() );


  if ( ok && output == Preview ) {
    //delete iw;
    return a.exec();
  }

  if (ok) return 0;
  else return -1;
}

#elif (MODE_MAIN==2)

int main(int argc, char *argv[] )
{
	QApplication app(argc, argv);
	NCReportTestForm window;
    window.show();
    return app.exec();
}

#elif (MODE_MAIN==3)

int main(int argc, char *argv[] )
{
	return 0;
}

#endif
