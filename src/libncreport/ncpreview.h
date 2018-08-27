/***************************************************************************
 *   Copyright (C) 2005 by Norbert Szabo                                   *
 *   Szolo str. 45. Ersekcsanad 6347                                       *
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
#ifndef NCPREVIEW_H
#define NCPREVIEW_H


//#include <qobject.h>
//#include <qpointer.h>

#include <QPointer>
#include <QWidget>
#include <QList>
#include <QMainWindow>
#include <QScrollArea>

#include <QBrush>
#include <QPen>

//#include <QPaintEvent>
//#include <QMoveEvent>
//#include <QKeyEvent>
//#include <QShowEvent>
//#include <QResizeEvent>
//#include <QPicture>


//class QToolButton;
class QPainter;
class QAction;
class QComboBox;
class QScrollArea;
class QPicture;
class NCWidgetPosHandler;
class NCReport;
class NCPreview;
class NCIconFactoryBase;
//class QResizeEvent;

/*!
Page widget in order to render report's page
*/
class NCPreviewPage : public QWidget
{
    Q_OBJECT
public:
    NCPreviewPage( QWidget * parent=0, const char * name=0, Qt::WFlags f=0 );
    ~NCPreviewPage();

	//void setZoomLevel(int z);
	void setPageSize(int width, int height);
	void setPicture( QPicture *picture);
	void setAntialiased( bool );
	void zoom( int z );
	int currentZoomLevel();
	void setContainer( QWidget* );
	void setContainerMargin(int);
public slots:
    void updatePage( int );
    //void printPage();

protected:
    //void drawPage( QPainter * );
    void paintEvent( QPaintEvent *  );
	void testPaint( QPainter &painter );
	
	void paintPage( QPainter &painter );
	void paintTest( QPainter &painter );
	
	int mmToPixel( const double& mm );
private:
	void drawContents(QPainter * p);
	//int mmToPixel(double mm, Q3PaintDeviceMetrics* );
	int mmToPixel(double mm, QPaintDevice *pd );
	int realSize(int size);

	int baseWidth, baseHeight;
	int zoomLevel;
	int dpi;
	int cMargin;
	bool antialiased;
	QPicture *picture;
	QWidget *containerWidget;

};

/*!
Container object for reference preview pages
*/

class NCPictureList : public QObject
{
public:
    NCPictureList( QWidget *parent=0, const char *name=0 );
    ~NCPictureList();

    void append();
    QPicture* current();
    int count();
    QList<QPicture*>& list();
private:
	QList<QPicture*> pList;

};

class NCPreviewScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	NCPreviewScrollArea( NCPreview* parent=0 );
	~NCPreviewScrollArea();
	
	//void setPage( NCPreviewPage* );
protected:
	NCPreview *pvForm;
	//NCPreviewPage *page;
	void resizeEvent( QResizeEvent * event );
	void keyPressEvent ( QKeyEvent * e );	
};

/*!
Preview manager main form. Handles paging, zooming and so on.
*/

class NCPreview : public QMainWindow
{
	Q_OBJECT
    friend class NCReport;
	friend class NCPreviewScrollArea;
public:
	//NCPreview( int height, int width, QList<QPicture> & picList, QWidget * parent = 0, const char * name = 0, WFlags f = WType_TopLevel );
	//NCPreview( int height, int width, QWidget * parent = 0, const char * name = 0, WFlags f = WType_TopLevel );
	//NCPreview( int height, int width, NCPictureList *piclist, QWidget * parent = 0, const char * name = 0, Qt::WFlags f =0 );
	NCPreview( int height, int width, QList<QPicture*>& piclist, QWidget * parent = 0, const char * name = 0, Qt::WFlags f =0, NCIconFactoryBase* factory = 0);
    ~NCPreview();

	void setReport( NCReport* );
	void setDeleteReportOnClose( bool );
	void setIconFactory( NCIconFactoryBase* );
public slots:
	void slotZoom( const QString & s);
	void zoomPlus();
	void zoomMinus();
	void slotPrint();
	void slotNext();
	void slotPrev();
	void slotFirst();
	void slotLast();
	void slotGoTo();
	void writeSettings();
	void readSettings();
	//void slotParseXML();

private:
	void initTools();
	void refreshPage();
	QList<NCPreviewPage*> pageList;
	//NCPictureList *plist;
	QList<QPicture*> picList;
    int pageCount, pageNo;
	NCWidgetPosHandler *posHandler;
	bool formInitDone;
	
	NCPreviewPage *currentPage;
	int zoomLevel;
	NCPreviewScrollArea *sa;
	QComboBox *cbZoom;
	int pvMargin;
	int basePageHeight, basePageWidth;
	QPointer<NCReport> report;
	bool deleteReportOnClose;
	QString mImagesPath;
	NCIconFactoryBase* iconFactory;

	void zoom(int z);
	void drawTest();
	//NCReport *report;

protected:
	//QToolButton *btnPrint;
	QToolBar *toolbar;
	QMenu *mnFile, *mnNav;
	QAction *actionPrint, *actionFrst, *actionNext, *actionPrev, *actionLast;
	QAction *actionGo, *actionExit, *actionZP, *actionZM;

	//virtual void keyPressEvent ( QKeyEvent * e );
	virtual void showEvent( QShowEvent * );
	virtual void closeEvent( QCloseEvent * );
	//virtual void moveEvent( QMoveEvent * );
	//virtual void resizeEvent( QResizeEvent * );
};

#endif // NCPREVIEW_H
