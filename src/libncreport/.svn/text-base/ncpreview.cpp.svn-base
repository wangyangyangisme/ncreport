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
#include "ncpreview.h"
#include "ncreport.h"
#include "nciconfactorybase.h"
#include "nciconfactory.h"
#include "globals.h"

#include <QToolBar>
#include <QScrollBar>
#include <QToolButton>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QInputDialog>
#include <QComboBox>
#include <QPicture>
#include <QAction>
#include <QPainterPath>
#include <QSettings>
#include <QMenuBar>
#include <QStatusBar>
#include <QIcon>
#include <QPaintEvent>
#include <QMoveEvent>
#include <QKeyEvent>
#include <QFrame>
#include <QShowEvent>
#include <QResizeEvent>

#define TRIMFACTOR	1.15

NCPreview::NCPreview( int height, int width, QList<QPicture*>& piclist, QWidget * parent, const char * name, Qt::WFlags f, NCIconFactoryBase* factory )
	: QMainWindow( parent, f )
{

	picList     = piclist;	//save QList<QPicture> reference to picList
	iconFactory = 0;
	setObjectName( name );
	setIconFactory( factory );
	formInitDone = FALSE;
	//picList
	//plist->list().first();
	pageCount = picList.count();
	//picList.setAutoDelete(TRUE);
	deleteReportOnClose = false;
	
	
	//picList = QList( picList);
	//report = rpt;	
	pvMargin = 20;
	//basePageWidth = 2100;
	//basePageHeight = 2960;
	basePageWidth = (int)(width*TRIMFACTOR);
	basePageHeight = (int)(height*TRIMFACTOR);
	readSettings();

	initTools();
	
	sa = new NCPreviewScrollArea( this );
	
	QWidget *frameWidget = new QWidget( sa );
	//frameWidget->resize( basePageWidth + pvMargin*4, basePageHeight + pvMargin*4 );
	//frameWidget->setBackgroundRole(QPalette::Dark);

	currentPage = new NCPreviewPage( frameWidget );
	currentPage->setContainer( frameWidget );
	currentPage->setContainerMargin( pvMargin );
	//currentPage->resize( basePageWidth, basePageHeight );
	currentPage->setPageSize( basePageWidth, basePageHeight );
	currentPage->move( pvMargin, pvMargin );
	sa->setWidget( frameWidget );

	//currentPage->setPicture( picList.current() );
	
	pageNo = 1;
	refreshPage();
	currentPage->zoom( 100 );
	
	//svw->setWidget( currentPage, pvMargin, pvMargin);
	//svw->show();
	/*
	currentPage->setFrameShadow( QFrame::Plain );
	currentPage->setFrameStyle( QFrame::Box );
	currentPage->setFrameShape( QFrame::Box );
	currentPage->setLineWidth(1);

	*/

	//pageList.append( currentPage );

	setCentralWidget( sa );
	setWindowTitle( tr("Report print preview") );
	setWindowIcon( QIcon( mImagesPath + "/app.png" ) );

}

NCPreview::~NCPreview()
{
	while (!picList.isEmpty())

		delete picList.takeFirst();	
		
	//delete plist;
	if ( deleteReportOnClose && report ) {
		delete report;
		report =0;
	}
}



void NCPreview::setIconFactory(NCIconFactoryBase* factory)
{
	iconFactory = factory;

	// eventually fall back to defaulta
	if ( !iconFactory ) {
		iconFactory = new NCIconFactory();
	}
}



/*
void NCPreview::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(cutAct);
	menu.addAction(copyAct);
	menu.addAction(pasteAct);
	menu.exec(event->globalPos());
}
*/
void NCPreview::closeEvent( QCloseEvent *ev )
{
	writeSettings();
	//qDebug( "Closing and writing settings" );
	ev->accept();
}


void NCPreview::initTools()
{
	/////////////////
	// ACTIONS
	/////////////////
	actionPrint = new QAction( this );
	actionPrint->setText( tr("Print...") );
	actionPrint->setIcon( iconFactory->findIcon("print") );
	actionPrint->setToolTip( tr("Print") );
	connect( actionPrint, SIGNAL( triggered() ), this, SLOT( slotPrint() ) );
	actionPrint->setEnabled( false );	// alapetelmezetten kikapcsolva

	actionZP = new QAction( this );
	actionZP->setText( tr("Zoom") );
	actionZP->setIcon( iconFactory->findIcon("zoom_plus")  );
	actionZP->setToolTip( tr("Zoom page") );
	actionZP->setShortcut( Qt::Key_Plus );
	connect( actionZP, SIGNAL( triggered() ), this, SLOT( zoomPlus() ) );

	actionZM = new QAction( this );
	actionZM->setText( tr("Unzoom") );
	actionZM->setIcon( iconFactory->findIcon("zoom_minus") );
	actionZM->setToolTip( tr("Unzoom page") );
	actionZM->setShortcut( Qt::Key_Minus );
	connect( actionZM, SIGNAL( triggered() ), this, SLOT( zoomMinus() ) );

	actionFrst = new QAction( this );
	actionFrst->setText( tr("First page") );
	actionFrst->setIcon( iconFactory->findIcon("firstpage") );
	actionFrst->setToolTip( tr("Go to first page") );
	actionFrst->setShortcut( Qt::CTRL+Qt::Key_Home );
	connect( actionFrst, SIGNAL( triggered() ), this, SLOT( slotFirst() ) );

	actionPrev = new QAction( this );
	actionPrev->setText( tr("Previous page") );
	actionPrev->setIcon( iconFactory->findIcon("prevpage") );
	actionPrev->setToolTip( tr("Go to previous page") );
	actionPrev->setShortcut( Qt::CTRL+Qt::Key_Left );
	connect( actionPrev, SIGNAL( triggered() ), this, SLOT( slotPrev() ) );

	actionNext = new QAction( this );
	actionNext->setText( tr("Next page") );
	actionNext->setIcon( iconFactory->findIcon("nextpage") );
	actionNext->setToolTip( tr("Go to next page") );
	actionNext->setShortcut( Qt::CTRL+Qt::Key_Right );
	connect( actionNext, SIGNAL( triggered() ), this, SLOT( slotNext() ) );

	actionLast = new QAction( this );
	actionLast->setText( tr("Last page") );
	actionLast->setIcon( iconFactory->findIcon("lastpage") );
	actionLast->setToolTip( tr("Go to final page") );
	actionLast->setShortcut( Qt::CTRL+Qt::Key_End );
	connect( actionLast, SIGNAL( triggered() ), this, SLOT( slotLast() ) );

	actionGo = new QAction( this );
	actionGo->setText( tr("Go to page...") );
	actionGo->setIcon( iconFactory->findIcon("gotopage") );
	actionGo->setToolTip( tr("Go to specified page") );
	actionGo->setShortcut( Qt::CTRL+Qt::Key_G );
	connect( actionGo, SIGNAL( triggered() ), this, SLOT( slotGoTo() ) );

	actionExit = new QAction( this );
	actionExit->setText( tr("Exit") );
	actionExit->setIcon( iconFactory->findIcon("exit") );
	actionExit->setToolTip( tr("Exit preview") );
	actionExit->setShortcut( Qt::Key_Escape );
	connect( actionExit, SIGNAL( triggered() ), this, SLOT( close() ) );

	/////////////////
	// MENUS
	/////////////////
	//QAction *actionPrint, *actionFrst, *actionNext, *actionPrev, *actionLast;
	//QAction *actionGo, *actionExit, *actionZP, *actionZM;

	mnFile = menuBar()->addMenu(tr("&File"));
	mnFile->addAction(actionPrint);
	mnFile->addAction(actionZP);
	mnFile->addAction(actionZM);
	mnFile->addSeparator();
	mnFile->addAction(actionExit);
	
	mnNav = menuBar()->addMenu(tr("&Navigate"));
	mnNav->addAction(actionFrst);
	mnNav->addAction(actionPrev);
	mnNav->addAction(actionNext);
	mnNav->addAction(actionLast);
	mnNav->addSeparator();
	mnNav->addAction(actionGo);	
	
	/////////////////
	// TOOLBAR
	/////////////////
	toolbar = addToolBar( tr("Tools") );
	//********************************************* combo
	cbZoom =  new QComboBox( toolbar );
	cbZoom->addItem( tr( " 10 %" ));
	cbZoom->addItem( tr( " 20 %" ));
	cbZoom->addItem( tr( " 50 %" ));
	cbZoom->addItem( tr( " 75 %" ));
	cbZoom->addItem( tr( "100 %" ));
	cbZoom->addItem( tr( "120 %" ));
	cbZoom->addItem( tr( "150 %" ));
	cbZoom->addItem( tr( "200 %" ));
	cbZoom->addItem( tr( "300 %" ));
	cbZoom->addItem( tr( "400 %" ));
	cbZoom->addItem( tr( "500 %" ));
	cbZoom->setFocusPolicy( Qt::NoFocus );
	cbZoom->setCurrentIndex( 4 );
	connect( cbZoom, SIGNAL(activated(const QString &) ), SLOT( slotZoom(const QString &) ) );
	
	toolbar->addAction(actionPrint);
	toolbar->addSeparator();
	toolbar->addAction(actionZP);
	toolbar->addAction(actionZM);
	toolbar->addSeparator();
	toolbar->addWidget(cbZoom);	
	toolbar->addSeparator();
	toolbar->addAction(actionFrst);
	toolbar->addAction(actionPrev);
	toolbar->addAction(actionNext);
	toolbar->addAction(actionLast);

	toolbar->setIconSize(QSize(16,16));
	statusBar();
	//statusBar()->showMessage(tr("Ready"));	
}

void NCPreview::setReport( NCReport* r )
{
	report = r;
	actionPrint->setEnabled( true );
}
void NCPreview::slotZoom(const QString & s)
{
	bool ok;
	int zm = s.left(3).toInt(&ok, 10);
	if (ok) {
		currentPage->zoom( zm);
		//QMessageBox::information( 0, "Xxxx", s);
		sa->setFocus();
	}
}

void NCPreview::showEvent( QShowEvent *e )
{
	QMainWindow::showEvent( e );
	formInitDone = TRUE;
	//refreshPage();
	//zoom( 100 );
}

void NCPreview::drawTest()
{
}


void NCPreview::slotPrint()
{
	//currentPage->printPage();
	if ( !report )
		return;

	report->runReportToPrinterFromPreview();
}

void NCPreview::slotNext()
{
	if ( pageNo < pageCount ) {
		pageNo++;
	 	refreshPage();
	}
}
void NCPreview::slotPrev()
{
	if ( pageNo > 1 ) {
		pageNo--;
	 	refreshPage();
	}
}
void NCPreview::slotFirst()
{
	if ( pageNo != 1 ) {
		pageNo = 1;
		refreshPage();
	}
}
void NCPreview::slotLast()
{
	if ( pageNo < pageCount ) {
		pageNo = pageCount;
		refreshPage();
	}
}

void NCPreview::slotGoTo()
{
	bool ok = FALSE;
	int res = QInputDialog::getInteger( this, tr( "Go to page" ), tr( "Page number:" ), 1, 1, pageCount, 1, &ok );
	if ( ok ) {
		//picList.at( res-1 );
		pageNo = res;
		refreshPage();
	}
	else
		;// user pressed cancel
}

void NCPreview::refreshPage()
{
	currentPage->setPicture( picList.at(pageNo-1) );
	currentPage->update();
	//svw->updateContents( 0, 0, svw->visibleWidth(), svw->visibleHeight() );
	//currentPage->repaint( svw->contentsX(), svw->contentsY() - pvMargin , svw->visibleWidth(), svw->visibleHeight() );
	
	statusBar()->showMessage( tr("Page: %1 / %2").arg(pageNo).arg(pageCount) );

	actionLast->setEnabled( true );
	actionNext->setEnabled( true );
	actionFrst->setEnabled( true );
	actionPrev->setEnabled( true );

	if (pageNo == pageCount) {
		actionLast->setEnabled( false );
		actionNext->setEnabled( false );
	}
	if (pageNo ==1) {
		actionFrst->setEnabled( false );
		actionPrev->setEnabled( false );
	} 

}

void NCPreview::zoomPlus()
{
	currentPage->zoom( currentPage->currentZoomLevel() +10 );
}
void NCPreview::zoomMinus()
{
	currentPage->zoom( currentPage->currentZoomLevel() -10 );
}

/*
void NCPreview::zoom(int z)
{
	if (z<10) z=10;
	if (z>500) z=500;

	int nw = (int)(basePageWidth * z/100);
	int nh = (int)(basePageHeight * z/100);
	//svw->resizeContents( nw + 2*pvMargin ,nh + 2*pvMargin);
	
	currentPage->setZoomLevel( z );
	currentPage->resize( nw, nh );
	zoomLevel = z;
}
*/


void NCPreview::setDeleteReportOnClose( bool set )
{
	deleteReportOnClose = set;
}

void NCPreview::writeSettings()
{
	QSettings settings("NCProg", "NCReport");
	
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
}

void NCPreview::readSettings()
{
	QSettings settings("NCProg", "NCReport");
	
	settings.beginGroup("MainWindow");
	resize( settings.value( "size", QSize(400, 400) ).toSize() );
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	
	mImagesPath = settings.value( "iconbasepath" ).toString();
	if ( mImagesPath.isEmpty() )
		mImagesPath = "images";
		
	settings.endGroup();
}


/****************************************************
* Preview frame

******************************************************/
NCPreviewPage::NCPreviewPage( QWidget * parent, const char * name, Qt::WFlags f ):
	QWidget( parent, f )
{
	setObjectName( name );
	setPageSize( 595, 839 );
	setBackgroundRole( QPalette::Base );
	setAutoFillBackground(true);
	antialiased = false;
	zoomLevel =1;
	containerWidget =0;
	cMargin =0;
}

NCPreviewPage::~NCPreviewPage()
{}

void NCPreviewPage::setContainer( QWidget *w )
{
	containerWidget = w;
}
void NCPreviewPage::setContainerMargin( int m )
{
	cMargin = m;
}

void NCPreviewPage::paintEvent( QPaintEvent *e  )
{
	QPainter painter;
	painter.begin( this );              // paint in myWidget
	// border
	painter.setPen(Qt::black);
	painter.drawRect( 0,0, width()-1, height()-1 );
	painter.setClipRect(e->rect());
	if ( antialiased )
		painter.setRenderHint(QPainter::Antialiasing);
	
	painter.scale( (double)(zoomLevel/100.0), zoomLevel/100.0);
	paintPage( painter );
	//paintTest( painter );
	
	painter.end();
}

void NCPreviewPage::paintPage( QPainter &painter )
{
	//QPicture picture;
	//bool loadOK = picture.load("temp/p1.tmp");
	//if ( !ok );           // load picture
		//picture.load("drawing.pic");           // load picture
	
	//QPicture _picture;
	//_picture.load("drawing.pic");           // load picture
	painter.drawPicture(0, 0, *picture);    // draw the picture at (0,0)
}

void NCPreviewPage::paintTest( QPainter &painter )
{
	//painter.scale( width()/720.0, width()/720.0);
	
	//painter.translate(50.0, 50.0);
	//painter.setWindow(QRect(0, 0, width(), height()));		
	//painter.translate( 80, 80);
	//painter.scale( width(), height() );
	//painter.setViewport(QRect(0, 0, width(), height()));
	
	QPicture _picture;
	_picture.load("drawing.pic");           // load picture
	painter.drawPicture(0, 0, _picture);    // draw the picture at (0,0)
	
	painter.drawPixmap( 100,100, QPixmap("companylogo.png"));	
	//////////////////
	// tesztÌÌÛÛÌÌ
	//////////////////
	
	painter.setPen(Qt::blue);
	painter.setFont(QFont("Arial", 24));
	painter.drawText( 0,24, "Qt - test ’⁄…¡€ˆ¸Ûı˙È·˚");
	painter.setFont(QFont("Arial", 10));
	painter.setPen(Qt::black);
	painter.drawText( 0,44, "This is a test for a simple text message - test ’⁄…¡€ˆ¸Ûı˙È·˚");
	painter.drawEllipse( 0,50, 50,50);
	painter.drawEllipse( 0,100, 50,50);
	painter.drawEllipse( 0,150, 50,50);
	painter.setPen(Qt::red);
	painter.drawEllipse( 50,50, 50,50);
	painter.drawEllipse( 50,100, 50,50);
	painter.drawEllipse( 50,150, 50,50);
	painter.setPen(Qt::green);
	painter.drawEllipse( 100,50, 50,50);
	painter.drawEllipse( 100,100, 50,50);
	painter.drawEllipse( 100,150, 50,50);
	painter.setFont(QFont("Adobe Helvetica", 9));
	painter.setPen(Qt::black);
	painter.drawText( 0,200,mmToPixel(100.0),mmToPixel(10.0), Qt::TextWordWrap, 
				"This is another test for a simple text message - test ’⁄…¡€ˆ¸Ûı˙È·˚\n"
				"Uwe he iqwueh ishdk asdk hqwgd kjguqwge uzguzggz ÈÈa");
	painter.drawRect( mmToPixel(10.0),250,mmToPixel(190.0),mmToPixel(10.0) );
}

int NCPreviewPage::mmToPixel( const double& mm )
{
	double d = mm*72/25.4;
	int val = (int)d;
	if ( d-0.499 < val ) {}
	else
		val+=1;
	return val;
}

void NCPreviewPage::setPicture( QPicture *pic) { picture = pic; }
void NCPreviewPage::setAntialiased( bool set ) { antialiased = set; }

void NCPreviewPage::updatePage(int) {}
void NCPreviewPage::drawContents( QPainter* )
{
	// it was a test only
}

void NCPreviewPage::setPageSize(int width, int height)
{
	baseWidth = width;
	baseHeight = height;
}

/*
void NCPreviewPage::setZoomLevel(int z)
{
	zoomLevel = z;
}
*/

int NCPreviewPage::currentZoomLevel() { return zoomLevel; }


void NCPreviewPage::zoom(int z)
{
	if (z<10) z=10;
	if (z>500) z=500;
	int nw = (int)(baseWidth * z/100);
	int nh = (int)(baseHeight * z/100);
	//svw->resizeContents( nw + 2*pvMargin ,nh + 2*pvMargin);
	zoomLevel = z;
	if ( containerWidget )
		containerWidget->resize( qRound( nw*TRIMFACTOR )+cMargin*2, qRound( nh*TRIMFACTOR )+cMargin*2 );
	resize( qRound( nw*TRIMFACTOR ), qRound( nh*TRIMFACTOR ) );
}

// 
//int NCPreviewPage::mmToPixel(double mm, Q3PaintDeviceMetrics *pd )
int NCPreviewPage::mmToPixel(double mm,  QPaintDevice *pd )
{
	//double result = mm*(double)dpi/25.4;
	//int retval= (int)result;

	double retval = pd->width() * mm / pd->widthMM();
	//cout << mm << " : " << dpi << " : " << result << " : " << retval;
	return (int)retval;
}

int NCPreviewPage::realSize(int size)
{
	return size;
}

/****************************************************
* Piclist
******************************************************/
NCPictureList::NCPictureList( QWidget *parent, const char *name ) : QObject( parent )
{
	setObjectName(name);
}

NCPictureList::~NCPictureList()
{
#ifdef REPORT_DEBUG_ON
	qDebug("Picture List object destroyed..." );
#endif
	// delete QPicture -s
	while (!pList.isEmpty())
        delete pList.takeFirst();	
}

void NCPictureList::append()
{
	pList.append( new QPicture() );
}

QPicture* NCPictureList::current() { return pList.last(); }
int NCPictureList::count() { return pList.count(); }
QList<QPicture*>& NCPictureList::list() { return pList; }

///////////////////////
// ScrollArea
//////////////////////
NCPreviewScrollArea::NCPreviewScrollArea( NCPreview * parent ) : QScrollArea( parent )
{
	setBackgroundRole(QPalette::Dark);
	//setAlignment( Qt::AlignCenter );
	setWidgetResizable( false );
	pvForm = parent;
	//setViewportMargins ( 20, 20, 20, 20 );
}

NCPreviewScrollArea::~ NCPreviewScrollArea( ) {}

//void NCPreviewScrollArea::setPage( NCPreviewPage* p )
//{
//	page = p;
//}

void NCPreviewScrollArea::resizeEvent( QResizeEvent *e )
{
	if ( !widget() )	
		return;

	QScrollArea::resizeEvent(e);
	//widget()->move( (viewport()->width()-widget()->width() )/2, 20 );
}

void NCPreviewScrollArea::keyPressEvent( QKeyEvent * e )
{
	int key = e->key();
	//const int scrollstep =20;

#ifdef REPORT_DEBUG_ON
	qDebug("NCPreviewScrollArea::keyPressEvent ... e->key()=%i", key );
	qDebug("verticalScrollBar()->value()=%i verticalScrollBar()->minimum()=%i verticalScrollBar()->maximum()=%i", 
		   verticalScrollBar()->value(), verticalScrollBar()->minimum(), verticalScrollBar()->maximum());
#endif
	
	if ( key == Qt::Key_PageUp ) {
		if ( verticalScrollBar()->value() == verticalScrollBar()->minimum() && pvForm->pageNo > 1  )
			pvForm->slotPrev();
		QScrollArea::keyPressEvent( e );
	}
	else if ( key == Qt::Key_PageDown ) {
		if ( verticalScrollBar()->value() == verticalScrollBar()->maximum() && pvForm->pageNo < pvForm->pageCount ) {
			pvForm->slotNext();
			ensureVisible( 0, 0 );
		} else
			QScrollArea::keyPressEvent( e );
	}
	else if ( key == Qt::Key_Home ) {
		ensureVisible( 0, 0 );
	}
	else if ( key == Qt::Key_End ) {
		ensureVisible( 0, widget()->height() );
	}
	else
		QScrollArea::keyPressEvent( e );
	
}
