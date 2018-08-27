/***************************************************************************
 *   Copyright (C) 2005-2007 by Norbert Szabo                              *
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
#ifndef NCREPORT_H
#define NCREPORT_H

#include "ncsqlquery.h"

#include <QObject>
#include <QPrinter>
#include <QColor>
#include <QList>
#include <QHash>
#include <QFont>
#include <QFile>
#include <QTextStream>

#include <QtXml>

//Added by qt3to4:
//#include <Q3CString>

//#include "reportparser.h"

//#define NCAPP_USE_KDEPRINT


/*
#if defined(NCAPP_USE_KDEPRINT)
	#define QPrinter KPrinter
#endif

#if defined(NCAPP_USE_KDEPRINT)
	#define QPrinter KPrinter
#endif
*/

class NCSqlQuery;
class NCPreview;
class QSqlDatabase;
class NCReportLookup;
class QTextDocument;
class NCIconFactoryBase;
//class QAbstractTextDocumentLayout;

/*!
Report's elemental item base class.
*/

class report_ElementObject : public QObject
{
public:
	report_ElementObject( QObject* parent =0, const char* nm ="element" );
	~report_ElementObject();
	QString etype;
	QString name;
	QString printWhen;
	bool printDone;
};

/*!
Report's geometric object base class such as line, circle, rectangle...
*/
class report_GeometricObject : public report_ElementObject
{

public:
	report_GeometricObject( QObject* parent =0, const char* nm ="geometric" ) : report_ElementObject( parent, nm ) {

		etype="";
		name="";
		posX=0; posY=0; width=0; height=0;
		lineWidth =0;
		lineStyle = Qt::SolidLine;
		lineColor = QColor(0,0,0);
		fillStyle = transparent;
		fillColor = QColor(255,255,255);
	}
	~report_GeometricObject() {}

	double posX, posY, width, height;
	int lineWidth;


	QColor lineColor;
	Qt::PenStyle lineStyle;
	enum fillStyles { transparent=0, filled };


	fillStyles fillStyle;
	QColor fillColor;

};

/*!
Report's container object base class
*/

class report_ContainerObject : public QObject
{
public:
	report_ContainerObject( QObject* parent =0, const char* nm ="container" ) : QObject( parent ) {
		//objects.setAutoDelete( false );
		setObjectName(nm);
    	height=0;
		drawTryAgain = false;
		//objectIterator(objects);
		//objects.clear();
	}
	~report_ContainerObject() {}

	double height;
	bool drawTryAgain;	// draw succeeded?
	QList<report_ElementObject*> objects;

	//QListIterator<report_ElementObject> objectIterator;
};

/*
  ************************** Derived container objects
*/

/*!
Report's query object
*/

class report_Query  : public QObject
{
public:
	report_Query( QObject* parent =0, const char* nm ="query" ) : QObject( parent ) {
		setObjectName(nm);
		queryString ="";
		master = true;
		//alias ="";
	}
	~report_Query() {}

	QString queryString;
	QString alias;
	bool master;
};

/*!
Report's query's collection object
*/

class report_Queries : public QObject
{
public:
	report_Queries( QObject *parent =0, const char* nm ="queries" ) : QObject( parent ) {
		//queries.setAutoDelete( TRUE);
		setObjectName(nm);
		queryCount =0;
	}
	~report_Queries() {}

	QList<report_Query*> queries;

	int queryCount;
};

/*!
Report's page header object
*/
class report_pageHeader : public report_ContainerObject
{
public:
	report_pageHeader( QObject* parent =0, const char* nm ="pageheader" ) : report_ContainerObject( parent, nm ) {}
	~report_pageHeader() {}
};

/*!
Report's page footer object
*/
class report_pageFooter : public report_ContainerObject
{
public:
	report_pageFooter( QObject* parent =0, const char* nm ="pagefooter" ) : report_ContainerObject( parent, nm ) {}
	~report_pageFooter() {}

	QString resetVariables;		//list of variables which have to be reset at the end of group. (delimited with "," comma)

};


/*!
Report's page groupheader object
*/

class report_groupHeader : public report_ContainerObject
{
public:
	report_groupHeader( QObject* parent =0, const char* nm ="groupheader" ) : report_ContainerObject( parent, nm ) {}
	~report_groupHeader() {}

};

/*!
Report's page groupfooter object
*/
class report_groupFooter : public report_ContainerObject
{
public:
	report_groupFooter( QObject* parent =0, const char* nm ="groupfooter" ) : report_ContainerObject( parent, nm ) {
		Queries = new report_Queries( this );
	
	}
	~report_groupFooter() {}

	report_Queries *Queries;
};

/*!
Report's page datail object
*/
class report_Detail : public report_ContainerObject
{
public:
	report_Detail( QObject* parent =0, const char* nm ="detail" ) : report_ContainerObject( parent, nm ) {
		expandable = false;
	}
	~report_Detail() {}

	bool expandable;

};

/*!
Report's group object
*/

class report_Group : public QObject
{
public:
	report_Group( QObject *parent =0, const char* nm ="group" ) : QObject( parent ) {
		setObjectName(nm);
		name="";
		groupExpression="";
		startOnNewPage = false;
		startOnNewPageWL = 0;
		resetPageno = false;
		reprintHeader = false;
		lastValue = "_******";
		state = closed;

		groupHeader = new report_groupHeader( this );
		groupFooter = new report_groupFooter( this );
	}

	~report_Group() {
		//delete groupHeader;
		//delete groupFooter;
		
		//groupHeader->objects.clear();

		//groupFooter->objects.clear();
	}
	
	enum groupState { closed=0, onProcess };
	groupState state;
	bool needDrawGroupHeader, needDrawGroupFooter, groupChanged;
	QString name;
	QString groupExpression;
	QString currentValue, lastValue;
	QString resetVariables;		//list of variables which have to be reset at the end of group. (delimited with "," comma)

	bool startOnNewPage;
	int startOnNewPageWL;
	bool resetPageno;

	bool reprintHeader;
	report_groupHeader *groupHeader;
	report_groupFooter *groupFooter;

};

/*!
Report's group collection object
*/

class report_Groups : public QObject
{
public:
	report_Groups( QObject *parent =0, const char* nm ="groups" ) : QObject( parent ) {
		//groups.setAutoDelete( TRUE);
		setObjectName(nm);
		groupCount =0;



	}
	~report_Groups() {}

	QList<report_Group*> groups;
	int groupCount;

};


//************************ Simple element objects: query, group


//************************ Derived element objects
/*!
Report's text label base object
*/
class report_Label : public report_ElementObject
{
public:
	report_Label( QObject *parent =0, const char* nm ="label" ) : report_ElementObject( parent, nm ) {
		etype = "label";
		name="";
		posX=0; posY=0; height=0; width=0;
		fontName=QString::null; fontSize=0; fontWeight = QFont::Normal;

		fontItalic=false; fontUnderline=false; fontStrikeOut=false;
		alignmentH=Qt::AlignLeft; alignmentV=Qt::AlignTop;
		wordbreak=false;
		forecolor = QColor(0,0,0);
		backcolor = QColor(255,255,255);
		text = "";
		rotation=0;
		dynamicHeight=false;
		isRichText=false;
		printedSnip =0;
		loadFromFile=false;
		richText = 0;
		//docLayout = 0;
	}
	~report_Label() {}

	double posX, posY, height, width;
	QString fontName;
	int fontSize;
	int fontWeight;
	bool fontItalic, fontStrikeOut, fontUnderline;
	//Qt::AlignmentFlags alignmentH;
	//Qt::AlignmentFlags alignmentV;
	int alignmentH;
	int alignmentV;
	bool wordbreak;
	QColor forecolor;
	QColor backcolor;
	QString text;
	QString displayValue;
	int rotation;
	bool dynamicHeight;
	bool isRichText;
	//QRect printable;
	int printedSnip;
	bool loadFromFile;
	QTextDocument *richText;
	//QAbstractTextDocumentLayout *docLayout;
};

/*!
Report's field object. Based on label
*/

class report_Field : public report_Label
{

public:
	report_Field( QObject *parent =0, const char* nm ="field" ) : report_Label( parent, nm ) {
		etype = "field";
		//name=
		type = Text;
		toUpper=false, toLower=false;
		numDecimals=-1;
		numSeparation=false;
		numSeparator=' ';
		numDigitPoint='.';
		numBlankIfZero=false;

		numFormat = "";
		dateFormat = "YYYYMMDD";
		numValue = 0;
		refreshType = atDetail;
	}
	~report_Field() {}

	enum fieldTypes { variable=0, sysvar, sqlcolumn, parameter, lookup, textsection, invalid };
	enum baseTypes { Text=0, Numeric, Date };
	enum refreshTypes { atDetail=0, atGroupFooter, always };

	fieldTypes fieldType;
	baseTypes type;
	refreshTypes refreshType;
	bool toUpper, toLower;
	int numDecimals;
	bool numSeparation;
	char numSeparator;
	char numDigitPoint;
	bool numBlankIfZero;
	QString dateFormat;
	QString embedString;
    QString callFunction;
    QString lookupClass;

	QString numFormat;
	double numValue;
	QString stringValue;
};

/*!
Report's variable object
*/
class report_Variable : public report_Field
{

public:
	report_Variable( QObject *parent =0, const char* nm ="variable" ) : report_Field( parent, nm ) {
		
		setObjectName( nm );
		etype = "variable";
		//name=
		funcType = nothing;
	}
	~report_Variable() {}

	enum funcTypes { nothing=0, count, sum, average };
	enum resetTypes { eofreport=0, eofpage, group };

	funcTypes funcType;
	QString initValue;
	resetTypes resetAt;

	QString resetGroupName;
	QString varExp;		// = Variable expression
};

/*!
Report's parameter object
*/
class report_Parameter : public report_Field
{
public:
	report_Parameter( QObject *parent =0, const char* nm ="parameter" ) : report_Field( parent, nm ) {
		etype = "parameter";
		//name=
	}
};
/*
class report_Variables : public QObject
{
public:
	report_Variables( QObject *parent =0, const char* nm ="variables" ) : QObject( parent, nm ) {
		variables.setAutoDelete( true );
	}
	~report_Variables() {}

	QDict<report_Variable> variables;
};
*/

//class report_Fields
//{
//public:
//	report_Fields() {
//	}
//
//	QList<report_Field> fields;
//};

/*!
Report's line object, based on geometric class
*/

class report_Line : public report_GeometricObject
{

public:
	report_Line( QObject *parent =0, const char* nm ="field" ) : report_GeometricObject( parent, nm ) {
		etype = "line";
		//name=

		fromX=0; fromY=0; toX=0; toY=0;
	}
	~report_Line() {}

	double fromX, fromY, toX, toY;

};

/*!
Report's rectangle object, based on geometric class
*/
class report_Rectangle : public report_GeometricObject
{

public:
	report_Rectangle( QObject *parent =0, const char* nm ="rectangle" ) : report_GeometricObject( parent, nm ) {
		etype = "rectangle";

		//name=
		roundValue = 0;
	}
	~report_Rectangle() {}



	int roundValue;
};

/*!
Report's ellipse object, based on geometric class
*/
class report_Ellipse : public report_GeometricObject
{

public:
	report_Ellipse( QObject *parent =0, const char* nm ="ellipse" ) : report_GeometricObject( parent, nm ) {
		etype = "ellipse";
		//name=
	}
	~report_Ellipse() {}
};

/*!
Report's pixmap/image object, based on geometric class
*/
class report_Pixmap : public report_GeometricObject
{
public:
	report_Pixmap( QObject *parent =0, const char* nm ="pixmap" ) : report_GeometricObject( parent, nm ) {
		etype = "pixmap";
		//name=
		resource = "";
		width = -1;
		height = -1;


	}
	~report_Pixmap() {}
	

	QString resource;
};

// ********************************* report_DataDef Data definition collector object
/*!
Report's data definiton container class
*/
class report_DataDef : public QObject
{
public:
	report_DataDef( QObject *parent =0, const char* nm ="definition" ) : QObject( parent )
	{
		setObjectName( nm );
		Queries = new report_Queries( this );
		pageHeader = new report_pageHeader( this );
		pageFooter = new report_pageFooter( this );
		Groups = new report_Groups( this );
		Detail = new report_Detail( this );

	}
	~report_DataDef() {}


	report_Queries *Queries;
	report_pageHeader *pageHeader;
	report_pageFooter *pageFooter;
	report_Groups *Groups;
	report_Detail *Detail;

	//report_Variables Variables;
	QHash<QString,report_Variable*> Variables;
	//Q3Dict<report_Variable> Variables;
	//report_Fields Fields;
	QList<report_Field*> Flds;
};


/*!
Master report engine class. Calls XML parser class to parse report definition, 
executes sql queries and renders report result in preview form or print it.
*/

#ifdef Q_WS_WIN
	#ifdef _EXPORTING_NCREPORT
		#define CLASS_NCREPORT_DECLSPEC    __declspec(dllexport)
	#else
		#define CLASS_NCREPORT_DECLSPEC    __declspec(dllimport)
	#endif
//	#define CLASS_NCREPORT_DECLSPEC
#else
	#define CLASS_NCREPORT_DECLSPEC
#endif
class CLASS_NCREPORT_DECLSPEC NCReport : public QObject
{
	Q_OBJECT
	friend class ReportParser;
public:
	//NCReport(const QString & fileName);
	NCReport(const QString & fileName =QString::null, QObject *parent=0, const char* name=0 );
	~NCReport();

	enum parsemode { fromFile=0, fromDatabase, fromString };
	enum Orientation { Portrait=0, Landscape };
	enum DataSource { Database=0, Text };
	enum Output { Printer=0, Preview, Pdf, XML, TXT };
	
	void setParseMode( parsemode );
	void setReportFile( const QString& );	// sets report def. XML file.
	void setReportDef( const QString& );	// sets report def. XML string.
	void setReportID( int );	// set pk value in report table -if we use report def. file from Database
	void setReportID( const QString& );	// set string id value in report table -if we use report def. file from Database
	void setShowPrintDialog( bool );
	void setCopies( int num );
	void setForceCopies( bool );
	void setPageSize( const QString& ps );
	void setTrimmers( int tfont_print, int tfont_prevw, int tline_print, int tline_prevw );
	void setDeleteReportAfterPreview( bool );
	//void setQueries( QStringList & );
	void addQuery( const QString&, const QString&, bool =false );
	void addParameter( const QString&, const QString&, const char* =0 );
	bool wasError();
	QString ErrorMsg();
	void setPreviewAsMain( bool );
	NCPreview* previewWidget();
	void setFileEncoding( const QString& );
	void setPreviewIsMaximized( bool set );
	/*!Print output format*/
	void setOrientation( NCReport::Orientation );
	void setDatabase( QSqlDatabase* );
	void setReportQuery( const QString& );
	void setIconFactory( NCIconFactoryBase* iconFactory );
	void registerLookupClass( NCReportLookup* );
	void setOutput( NCReport::Output );
	void setOutputFile( const QString& );

	QDomDocument xmlOutput() const;

signals:
	void queryIsRunning( int );
	void onProcess( int );
	void processTotal( int );
	void pageDone( int );
public slots:
	bool runReport();
	void runReportToPrinter();
	void runReportToPreview();
	void runReportToPrinterFromPreview();
	void runReportToPDF();
	//QList<report_Query> Queries;
	//QList<report_Group> Groups;
protected:
	bool openTextSource();
	bool closeTextSource();

private:
	QString _xml;	// xml data def;
	DataSource dataSource;
	char textDataDelimiter;
	QString resourceTextFile;
	QString currentTextRow, lastTextRow, nextTextRow;
	report_DataDef *dataDef;
	QFile resourceFile;
	QTextStream resourceStream;

	QString name;
	QString sqlHostname, sqlDatabase, sqlUser, sqlPassword;	//data

	bool queryByParameter;
	bool queryListChanged;
	//QStringList qryList;
	report_Queries *pQueries;	// for parameter queries.
	double pageWidth, pageHeight;
	int pageWidth_screen, pageHeight_screen;
	QString pageSize;
	int columnCount;
	double columnWidth;
	double columnSpacing;

	double topMargin;

	double bottomMargin;
	double leftMargin;
	double rightMargin;
	QString defaultFontName;
	int defaultFontSize;
	bool showPrintDialog;
	bool alwaysShowPrintDialog;
	bool isforcecopies;
	// system variables
	int _pageno;
	int _numcopies;
	int _numforcecopies;
	int _currentforcecopy;
	//NCPictureList *piclist;
	QPrinter *pr;
	bool _init;	// init printing
	bool paintBegin;

	QColor backcolor;
	QPrinter::PageSize printerPageSize;
	QPrinter::PrinterMode printerMode;
	Orientation pageOrientation;
	//////////////////
	// DYNAMIC HEIGH 
	//////////////////
	int _globalPosX, _globalPosY;
	report_ContainerObject *currentSection;
	int dynamicSectionIncrement;
	QList<report_Label*> overPageObjects;
	//QHash<report_Label*,report_Label*> overPageObjects;
	
	QList<QPicture*> piclist;
	//QList<QPicture> picPages;
	//QPicture *pic;
	//NCPreview *previewForm;

	//tmode reportMode;
	Output reportOutput;
	QString outputFileName;	 
	QSqlDatabase* database;
	QString reportQuery;
	NCIconFactoryBase* iconFactory;
	//bool reportParsed;
	// process flags:
	QString errorMsg;
	bool flagNewPage, flagBeginGroup, flagEndGroup,
		flagFirstRecord, flagBeginReport, flagEndReport, flagError, flagGroupChanged,
		flagNPFGroupFooter, flagNPFGroupHeader, flagNPFDetail;

	int dpiFactor, dpiX, dpiY;
	QString reportFileName;
	bool reportFileParsed;
	parsemode pmode;
	//bool parseFromFile;
	int reportID;
	QString reportStringID;
	QPainter *pa;
	double _pageWidth, _pageHeight;	//netto size without margins
	double pageFooterY, startY;

	//NCSqlQuery *masterSql;
	QHash<QString,NCSqlQuery*> DataSession; //Q3Dict<NCSqlQuery> DataSession;
	QHash<QString,report_Parameter*> Parameters; //Q3Dict<report_Parameter> Parameters;
	QHash<QString,NCReportLookup*> lookupClasses;

	int recno, reccount, columncount;
	QString masterAlias;
	int trimFont_prn, trimFont_pvw, trimLine_prn, trimLine_pvw;
	bool logging;
	NCPreview *previewForm;
	bool previewismainform;
	QString encoding;
	bool deleteReportAfterPreview;
	QWidget *printerSetupParent;
	bool previewismaximized;
	bool prevRecordPending;
	//QMap<QString, sqlField> currentRecord;

	QDomDocument outputDomDocument;
	QDomElement currentElement;

	//bool execQueries( QDict<NCSql> & );
	void loadConfig();
	bool openXMLFile();
	bool parseXMLResource();
	bool loadResourceFromFile( QString& txt, const QString& filename );
	bool printerSetup( QPrinter * );
	void reportProcess();
	void copyQueryListToDataDef();
	bool execQueries( QListIterator<report_Query*>&, const char ='P' );	// iterator is parameter, because more lists are used for store queries
	bool runQueryToFile();	// execute master query only to file

	void initNewPage();
	void resetObjContainers();
	void drawReport();
	void nextRecord();
	void prevRecord();
	void drawObject( report_ElementObject *obj, bool checkPrintDone = false );

	void handleGroupHeader();
	void handleGroupFooter();
	bool drawPageHeader();
	bool drawPageFooter();
	bool drawGroupHeader( report_Group* );
	bool drawGroupFooter( report_Group* );
	bool drawDetailWithGroup();
	bool drawDetail();


	void _drawLabel( report_Label *obj, bool isField = false );
	void _drawLine( report_Line *obj );
	void _drawRectangle( report_Rectangle *obj );
	void _drawEllipse( report_Ellipse *obj );
	void _drawPixmap( report_Pixmap *obj );
	void _drawRichText( report_Label *obj, bool isField );

	void _setPenAndStyle( report_GeometricObject *obj );
	void _updateFieldValues( bool =false, report_Field::refreshTypes =report_Field::always );
	void _updateSystemFieldValues();
	void _updateGroupExp();
	void _updateVariableExp();
	void _resetVariables( const QString & );
	void _resetAllVariable();
	void _evalFieldExp( report_Field* );
	void _evalFunction( report_Field* );
	void _evalGroupExp( report_Group* );
	void _evalVariableExp( report_Variable* );
	void _evalParameters( QString &, const char ='P' );
	//QString _evalQuery( const QString &, const char ='P' );
	void _setFieldNumDisplayValue( report_Field*, double =0 );
	bool expressionParser( const QString & );
	bool _evalOperand( const QString & , QString &, double &, char &);
	QString systemVariable( const QString & );
	QVariant getSqlColumnValue( const QString &, DataSource ds );
	QString getSqlColumnStringValue( const QString &, DataSource ds );
	QString getParameterValue( const QString & );
	QString embed( const QString& es, const QString& valuelist );
	/*! Logical translate position in page, instead of QPaint::translate()*/
	void translate_position( int x, int y, bool relative = true );
	int _x( const int& val );
	int _y( const int& val );
	int toPixelX( const double& mm );
	int toPixelY( const double& mm );
	double toMillimeterX( int pixel );
	double toMillimeterY( int pixel );
	void reportLog( const QString& log );
	void drawOverPageObjects();
	int fixRichTextBreakPos( QTextDocument* rt, int ypos );
	bool textFromResource( QString&, report_Label *obj, bool isField );

	QPointF toPoint( qreal mm_x, qreal mm_y );
	QSizeF toSize( qreal mm_w, qreal mm_h );
	int _roundToInt( const double& d );
};


class NCReportLookup
{
public:
	//NCReport(const QString & fileName);
	NCReportLookup() {}
	NCReportLookup(const QString & name) { funcName = name; }
	virtual ~NCReportLookup() {}

	/*! Virtual class for lookup data by value */
	virtual QString lookupResult( const QVariant& value, const QSqlRecord& ) =0;
	void setName(const QString & name) { funcName = name; }
	QString name() { return funcName; }
private:
	QString funcName;
};

#endif //NCREPORT_H

