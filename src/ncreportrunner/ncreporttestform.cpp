/***************************************************************************
 *   Copyright (C) 2003-2006 by Norbert Szabo                              *
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
//#include "../libncreport/ncsqlquery.h"
#include "../libncreport/globals.h"

#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTableWidget>
#include <QLayout>
#include <QToolTip>
#include <QWhatsThis>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QProgressDialog>
#include <QApplication>
#include <QStringList>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QCloseEvent>
#include <QFileDialog>

/*
 *  Constructs a NCReportTestForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
NCReportTestForm::NCReportTestForm( QWidget* parent, Qt::WindowFlags fl )
    : QWidget( parent, fl )
{
	testformLayout = new QHBoxLayout; 
	testformLayout->setMargin( 6 );
	testformLayout->setSpacing( 3 );
	pdia = 0;

    layBig = new QVBoxLayout;
	layBig->setSpacing( 3 );

    layConn = new QHBoxLayout; 
	layConn->setSpacing( 3 );

    bgConn = new QGroupBox( this );
    //bgConn->setAlignment(0, Qt::Vertical );
    //bgConn->layout()->setSpacing( 6 );
    //bgConn->layout()->setMargin( 6 );
    bgConnLayout = new QGridLayout;
	bgConnLayout->setSpacing( 3 );
    //bgConnLayout->setAlignment( Qt::AlignTop );

    lblHost = new QLabel( bgConn );

    bgConnLayout->addWidget( lblHost, 1, 0 );

    comboDriver = new QComboBox( bgConn );

    bgConnLayout->addWidget( comboDriver, 0, 1 );

    leDB = new QLineEdit( bgConn );

    bgConnLayout->addWidget( leDB, 2, 1 );

    lblDriver = new QLabel( bgConn);

    bgConnLayout->addWidget( lblDriver, 0, 0 );

    leHost = new QLineEdit( bgConn );

    bgConnLayout->addWidget( leHost, 1, 1 );

    lblDatabase = new QLabel( bgConn );

    bgConnLayout->addWidget( lblDatabase, 2, 0 );

    lePassw = new QLineEdit( bgConn );
	lePassw->setEchoMode( QLineEdit::Password );
    bgConnLayout->addWidget( lePassw, 4, 1 );

    leUser = new QLineEdit( bgConn );

    bgConnLayout->addWidget( leUser, 3, 1 );

    lblUser = new QLabel( bgConn );

    bgConnLayout->addWidget( lblUser, 3, 0 );

    lblPassw = new QLabel( bgConn );

    bgConnLayout->addWidget( lblPassw, 4, 0 );
	
	bgConn->setLayout( bgConnLayout );
    layConn->addWidget( bgConn );
	layBig->addLayout( layConn );

	layRadios = new QVBoxLayout;
	layRadios->setSpacing( 3 );
	layConn->addLayout( layRadios );

	// --------- OUTPUT -------------------
	bgOut = new QGroupBox( this );
    //bgOut->setColumnLayout(0, Qt::Vertical );
    //bgOut->layout()->setSpacing( 6 );
    //bgOut->layout()->setMargin( 6 );
    bgOutLayout = new QVBoxLayout;
	bgOutLayout->setSpacing( 3 );
//bgOutLayout->setAlignment( Qt::AlignTop );

    radioPreview = new QRadioButton( bgOut );
    radioPreview->setChecked( TRUE );
    bgOutLayout->addWidget( radioPreview );

    radioPrint = new QRadioButton( bgOut );
    bgOutLayout->addWidget( radioPrint );

    radioPdf = new QRadioButton( bgOut );
    bgOutLayout->addWidget( radioPdf );

    radioXml = new QRadioButton( bgOut );
    bgOutLayout->addWidget( radioXml );

    radioTxt = new QRadioButton( bgOut );
    bgOutLayout->addWidget( radioTxt );

	bgOut->setLayout( bgOutLayout );
	layRadios->addWidget( bgOut );

	// --------- ORIENTATION -------------------
	/*
	bgOrient = new QGroupBox( this );
	bgOrientLayout = new QVBoxLayout;
	bgOrientLayout->setSpacing( 3 );

	radioPortrait = new QRadioButton( bgOrient );
	radioPortrait->setChecked( TRUE );
	bgOrientLayout->addWidget( radioPortrait );

	radioLandscape = new QRadioButton( bgOrient );
	bgOrientLayout->addWidget( radioLandscape );
	
	bgOrient->setLayout( bgOrientLayout );
	layRadios->addWidget( bgOrient );
	*/
	
	// --------- FILE -------------------
    bgFile = new QGroupBox( this );
    bgFileLayout = new QHBoxLayout;
	bgFileLayout->setMargin( 3 );
	bgFileLayout->setSpacing( 3 );

    leFile = new QLineEdit( bgFile );
    bgFileLayout->addWidget( leFile );

    btnSelFile = new QPushButton( bgFile );
    btnSelFile->setMaximumSize( QSize( 23, 32767 ) );
    bgFileLayout->addWidget( btnSelFile );

	bgFile->setLayout( bgFileLayout );

    layBig->addWidget( bgFile );

    bgPar = new QGroupBox( this );
    bgParLayout = new QVBoxLayout();

    layParEdit = new QHBoxLayout; 
	layParEdit->setSpacing( 3 );

    lblParname = new QLabel( bgPar );
    layParEdit->addWidget( lblParname );

    leParname = new QLineEdit( bgPar );
    layParEdit->addWidget( leParname );

    lblParVal = new QLabel( bgPar );
    layParEdit->addWidget( lblParVal );

    leParvalue = new QLineEdit( bgPar );
    layParEdit->addWidget( leParvalue );
    bgParLayout->addLayout( layParEdit );

    layParList = new QHBoxLayout;
	layParList->setSpacing( 3 );

	///////////////////////
	// LISTVIEW
	///////////////////////
    listPar = new QTableWidget( bgPar );
	listPar->setColumnCount(2);
	QStringList l;
	l << tr( "Name" ) << tr( "Value" );
	listPar->setHorizontalHeaderLabels( l );
    layParList->addWidget( listPar );

    layParBtn = new QVBoxLayout;

    btnAdd = new QPushButton( bgPar );
    layParBtn->addWidget( btnAdd );

    btnRemove = new QPushButton( bgPar);
    layParBtn->addWidget( btnRemove );
    spcParBtn = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layParBtn->addItem( spcParBtn );
    layParList->addLayout( layParBtn );
    bgParLayout->addLayout( layParList );
    
	
	bgPar->setLayout( bgParLayout );

	layBig->addWidget( bgPar );
    
	testformLayout->addLayout( layBig );

    layBtn = new QVBoxLayout; 
	layBtn->setSpacing( 3 );
	layBtn->setMargin( 5 );

    btnRun = new QPushButton( this );
    QFont btnRun_font(  btnRun->font() );
    btnRun_font.setBold( TRUE );
    btnRun->setFont( btnRun_font ); 
    layBtn->addWidget( btnRun );

    btnCancel = new QPushButton( this );
    layBtn->addWidget( btnCancel );
    spcBtn = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layBtn->addItem( spcBtn );
    testformLayout->addLayout( layBtn );
	
	setLayout( testformLayout );

    languageChange();
    
    // signals and slots connections
    connect( btnRun, SIGNAL( clicked() ), this, SLOT( run() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( btnAdd, SIGNAL( clicked() ), this, SLOT( addParam() ) );
    connect( btnRemove, SIGNAL( clicked() ), this, SLOT( removeParam() ) );
	connect( btnSelFile, SIGNAL( clicked() ), this, SLOT( selectFile() ) );

    // tab order
    setTabOrder( comboDriver, leHost );
    setTabOrder( leHost, leDB );
    setTabOrder( leDB, leUser );
    setTabOrder( leUser, lePassw );
    setTabOrder( lePassw, leFile );
    setTabOrder( leFile, btnSelFile );
    setTabOrder( btnSelFile, leParname );
    setTabOrder( leParname, leParvalue );
    setTabOrder( leParvalue, btnAdd );
    setTabOrder( btnAdd, btnRemove );
    setTabOrder( btnRemove, listPar );
    setTabOrder( listPar, radioPreview );
    setTabOrder( radioPreview, btnRun );
    setTabOrder( btnRun, btnCancel );
	
	//resize( QSize(579, 419).expandedTo(minimumSizeHint()) );
	resize( QSize(550, 400) );
	loadDefaults();
}

/*
 *  Destroys the object and frees any allocated resources
 */
NCReportTestForm::~NCReportTestForm()
{
    // no need to delete child widgets, Qt does it all for us
	//saveDefaults();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void NCReportTestForm::languageChange()
{
    setWindowTitle( tr( "Tester window" )+" ("+NCAPP_NAME+" "+NCAPP_VERSION+")" );
    bgConn->setTitle( tr( "Database connection" ) );
    lblHost->setText( tr( "Host name:" ) );
    comboDriver->clear();
    comboDriver->addItem( tr( "QMYSQL3" ) );
    comboDriver->addItem( tr( "QODBC" ) );
    comboDriver->addItem( tr( "QSQLITE" ) );
    lblDriver->setText( tr( "Database driver:" ) );
    lblDatabase->setText( tr( "Default database:" ) );
    lblUser->setText( tr( "Username:" ) );
    lblPassw->setText( tr( "Password:" ) );
    bgOut->setTitle( tr( "Output" ) );
	radioPreview->setText( tr( "Print preview" ) );
    radioPrint->setText( tr( "Printer" ) );
    radioPdf->setText( tr( "PDF file" ) );
    radioXml->setText( tr( "XML file" ) );
    radioTxt->setText( tr( "Text file (data only)" ) );
	//bgOrient->setTitle( tr( "Orientation" ) );
	//radioPortrait->setText( tr( "Portrait" ) );
	//radioLandscape->setText( tr( "Landscape" ) );
	bgFile->setTitle( tr( "Report file" ) );
    btnSelFile->setText( tr( "..." ) );
    bgPar->setTitle( tr( "Parameters" ) );
    lblParname->setText( tr( "Parameter name:" ) );
    lblParVal->setText( tr( "Value:" ) );
    listPar->clear();
    
    btnAdd->setText( tr( "Add" ) );
    btnRemove->setText( tr( "Remove" ) );
    btnRun->setText( tr( "Run report" ) );
    btnCancel->setText( tr( "Exit" ) );
}

void NCReportTestForm::selectFile()
{
	QFileInfo fi( leFile->text() );
	QString s = QFileDialog::getOpenFileName( this, tr("Select report file"), fi.absoluteFilePath(), "Reports (*.xml *.txt)" );
	if ( !s.isEmpty() )
		leFile->setText( s );
}

void NCReportTestForm::addParam()
{
	addParam( leParname->text(), leParvalue->text() );
	leParname->clear();
	leParvalue->clear();
}


void NCReportTestForm::addParam( const QString& name, const QString& value )
{
	int rows = listPar->rowCount();
	listPar->setRowCount( rows+1 );
	QTableWidgetItem *newItem = new QTableWidgetItem(name);
    listPar->setItem(rows, 0, newItem);
	newItem = new QTableWidgetItem(value);
    listPar->setItem(rows, 1, newItem);
	listPar->setRowHeight( rows, 21 );
}

void NCReportTestForm::removeParam()
{
	int curr = listPar->currentRow();
	QSettings settings("NCProg", "NCReport");
	//settings.setPath( "NCReport", "TestForm" );
	settings.remove( "ncreporttest/parameters/"+listPar->item(curr,0)->text() );
	listPar->removeRow( curr );	

}

void NCReportTestForm::saveDefaults( )
{
	QSettings settings("NCProg", "NCReport");
	//settings.setPath( "NCReport", "TestForm" );
	
	settings.setValue( "ncreporttest/driver", comboDriver->currentIndex() );
	settings.setValue( "ncreporttest/host", leHost->text() );
	settings.setValue( "ncreporttest/DB", leDB->text() );
	settings.setValue( "ncreporttest/user", leUser->text() );
	settings.setValue( "ncreporttest/passw", lePassw->text() );
	
	settings.setValue( "ncreporttest/reportfile", leFile->text() );
	// paramters
	for (int i=0; i<listPar->rowCount(); ++i )
		settings.setValue( "ncreporttest/parameters/"+listPar->item(i,0)->text(), listPar->item(i,1)->text() );
}

void NCReportTestForm::loadDefaults( )
{
	QSettings settings("NCProg", "NCReport");

	//settings.setPath( "NCReport", "TestForm" );

	comboDriver->setCurrentIndex(settings.value( "ncreporttest/driver", "0" ).toInt());
	leHost->setText(settings.value( "ncreporttest/host", "localhost" ).toString());
	leDB->setText(settings.value( "ncreporttest/db", "test" ).toString());
	leUser->setText(settings.value( "ncreporttest/user", "root" ).toString());
	lePassw->setText(settings.value( "ncreporttest/passw", "" ).toString());
	
	leFile->setText(settings.value( "ncreporttest/reportfile", "" ).toString());
	
	// parameters

	//1
	//int size = settings.beginReadArray("ncreporttest/parameters");
	//for (int i = 0; i < size; ++i) {
	//	settings.setArrayIndex(i);
	//	addParam( settings.value("name").toString(), settings.value("value").toString() );
	//}
	//settings.endArray();

	//2
	//QStringList list = settings.entryList( "ncreporttest/parameters/" );
	//QStringList::Iterator it = list.begin();
	//
	//while( it != list.end() ) {
	//	addParam( (*it), settings.value( "ncreporttest/parameters/"+ *it ).toString() );
	//	//addParam( (*it).section(';',0,0), (*it).section(';',0,1) );
	//	++it;
	//}	

	//3
	settings.beginGroup("ncreporttest/parameters");
	QStringList keys = settings.childKeys();
	
	QStringList::const_iterator it;
	for (it = keys.constBegin(); it != keys.constEnd(); ++it) {
		QString name = *it;
		QString value = settings.value(*it).toString();
		addParam( name, value );
	}

	if ( listPar->rowCount() == 0 ) {
		addParam( "documentPK", "10367" );
		addParam( "prodFilt", "%" );
	}
	//QStringList::Iterator it = groups.begin();
	//while( it != groups.end() ) {
	//	addParam( (*it), settings.value( "ncreporttest/parameters/"+ *it ).toString() );
	//	++it;
	//}	

	settings.endGroup();

}


void NCReportTestForm::closeEvent( QCloseEvent* ce )
{
	saveDefaults();
	ce->accept();
}

void NCReportTestForm::pageLoaded( int p )
{
	pdia->setLabelText( QString("Printing page %1 ...").arg(p) );
}

void NCReportTestForm::run()
{
	// SQL/DATABASE CONNECTION
	QSqlDatabase defaultDB = QSqlDatabase::addDatabase(comboDriver->currentText());
	if ( !defaultDB.isValid() ) {
        //fprintf( stderr, "Error: Could not load database driver. \n" );
		QMessageBox::warning( 0, "NCReport error", QObject::tr("Could not load database driver.") );
		return;
	}

	//defaultDB->setHostName( "localhost" );
	defaultDB.setHostName( leHost->text() );
	defaultDB.setDatabaseName( leDB->text() );
	defaultDB.setUserName( leUser->text() );
	defaultDB.setPassword( lePassw->text() );

	if ( !defaultDB.open() ) {
		//fprintf( stderr, "Error: Cannot open database. %s\n", qPrintable( defaultDB.lastError().databaseText() ) );
		QMessageBox::warning( 0, "NCReport error", QObject::tr("Cannot open database: ")+defaultDB.lastError().databaseText() );
		return;
	}		
	// CREATE lookup class example
	MyLookup *lc = new MyLookup;
	lc->setName("test");
	

	// CREATE REPORT OBJECT		
    NCReport * report = new NCReport( QString::null, 0 );
	connect( report, SIGNAL(pageDone( int )), this, SLOT(pageLoaded( int )) );
	
	report->setParseMode( NCReport::fromFile );
	report->setFileEncoding( "iso8859-1");
	//report->setOrientation( radioPortrait->isChecked() ? NCReport::Portrait : NCReport::Landscape );
	report->setReportFile( leFile->text() );
	report->registerLookupClass( lc );

	//ADD PARAMETERS
	for (int i=0; i<listPar->rowCount(); ++i ) {
        report->addParameter( listPar->item(i,1)->text(), listPar->item(i,0)->text() );	// Adds a parameter. Parameter name = "prodFilt", value="%"
    }
	
	bool isPreview = radioPreview->isChecked();


	if ( radioPreview->isChecked() ) {
		//report->setPreviewAsMain( true );	// preview is the main form
		report->setDeleteReportAfterPreview( true );	// delete report object after close preview
		report->setPreviewIsMaximized( false );
		//report->runReportToPreview();
		report->setOutput( NCReport::Preview );

	} else if ( radioPrint->isChecked() ) {
		report->setShowPrintDialog( true );
		report->setCopies( 1 );
		//report->setForceCopies( false );
		//report->runReportToPrinter();
		report->setOutput( NCReport::Printer );

	} else if ( radioPdf->isChecked() ) {

		QString fileName = QFileDialog::getSaveFileName(this, tr("Save PDF File"),
                            "report.pdf", tr("Pdf files (*.pdf)"));
		if ( !fileName.isEmpty() ) {
			report->setShowPrintDialog( true );
			report->setCopies( 1 );
			report->setOutputFile( fileName );
			report->setOutput( NCReport::Pdf );
		}
	} else if ( radioXml->isChecked() ) {

		QString fileName = QFileDialog::getSaveFileName(this, tr("Save XML File"),
                            "result.xml", tr("Xml files (*.xml)"));
		if ( !fileName.isEmpty() ) {
			report->setOutputFile( fileName );
			report->setOutput( NCReport::XML );
		}
	} else if ( radioTxt->isChecked() ) {

		QString fileName = QFileDialog::getSaveFileName(this, tr("Save TXT File"),
                            "result.txt", tr("Text files (*.txt)"));
		if ( !fileName.isEmpty() ) {
			report->setOutputFile( fileName );
			report->setOutput( NCReport::TXT );
		}
	}

	if (!pdia)
		pdia = new QProgressDialog( this );
	//pdia->setAutoClose(true);
	pdia->setLabelText( "Initializing report..." );
	pdia->show();

	report->runReport();

	bool error = report->wasError();
	QString err = report->ErrorMsg();

	if ( !isPreview )	//  delete report object if report has done directly to printer
		delete report;

	if ( error )
		QMessageBox::information( 0, "Riport error", err );
	
	pdia->done(0);
	pdia->setLabelText( "" );
}


////////////////////////////
// LOOKUP EXAMPLE
////////////////////////////
QString MyLookup::lookupResult( const QVariant & value, const QSqlRecord& rec )
{
/*	QString data;
	for ( int i=0; i>rec.count(); i++ )
		data += " "+rec.value(i).toString();
	
	qDebug( "Data: %s", data.latin1() );
	return data;*/
	
	//return sql->stringValue("UnitPrice")+" "+value.toString()+" - XX";
	return rec.field("UnitPrice").value().toString()+" "+value.toString()+" - XX";
}
