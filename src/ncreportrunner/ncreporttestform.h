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
#ifndef NCREPORTTESTFORM_H
#define NCREPORTTESTFORM_H

#include <QVariant>
#include <QWidget>

#include "../libncreport/ncreport.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QComboBox;
class QLineEdit;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QProgressDialog;

class NCReportTestForm : public QWidget
{
Q_OBJECT
public:
    NCReportTestForm( QWidget* parent = 0,  Qt::WindowFlags fl = 0 );
    ~NCReportTestForm();

    QGroupBox* bgConn;
    QLabel* lblHost;
    QComboBox* comboDriver;
    QLineEdit* leDB;
    QLabel* lblDriver;
    QLineEdit* leHost;
    QLabel* lblDatabase;
    QLineEdit* lePassw;
    QLineEdit* leUser;
    QLabel* lblUser;
    QLabel* lblPassw;
    QGroupBox* bgOut;
    QRadioButton* radioPreview;
    QRadioButton* radioPrint;
    QRadioButton* radioPdf;
    QRadioButton* radioXml;
    QRadioButton* radioTxt;
	//QRadioButton* radioPortrait;
	//QRadioButton* radioLandscape;
	QGroupBox* bgFile;
	//QGroupBox* bgOrient;
	QLineEdit* leFile;
    QPushButton* btnSelFile;
    QGroupBox* bgPar;
    QLabel* lblParname;
    QLineEdit* leParname;
    QLabel* lblParVal;
    QLineEdit* leParvalue;
    QTableWidget* listPar;
    QPushButton* btnAdd;
    QPushButton* btnRemove;
    QPushButton* btnRun;
    QPushButton* btnCancel;
	QButtonGroup *groupOut, *groupOrient;

public slots:
    virtual void run();
    virtual void selectFile();
	virtual void saveDefaults();
	virtual void loadDefaults();
	virtual void pageLoaded(int);
	

protected:
    QHBoxLayout* testformLayout;
    QVBoxLayout* layBig;
    QHBoxLayout* layConn;
	QVBoxLayout* layRadios;
	QGridLayout* bgConnLayout;
    QVBoxLayout* bgOutLayout;
	QVBoxLayout* bgOrientLayout;
	QHBoxLayout* bgFileLayout;
    QVBoxLayout* bgParLayout;
    QHBoxLayout* layParEdit;
    QHBoxLayout* layParList;
    QVBoxLayout* layParBtn;
    QSpacerItem* spcParBtn;
    QVBoxLayout* layBtn;
    QSpacerItem* spcBtn;
	QProgressDialog *pdia;

	void addParam(const QString& name, const QString& value);
	virtual void closeEvent( QCloseEvent* );
	
protected slots:
    virtual void languageChange();

    virtual void addParam();
    virtual void removeParam();


};

class MyLookup : public NCReportLookup
{
public:
	QString lookupResult( const QVariant& value, const QSqlRecord& );

};

#endif // TESTFORM_H
