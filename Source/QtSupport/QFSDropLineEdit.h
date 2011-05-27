///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QtGui/QLineEdit>


/**
* @class QFSDropLineEdit QFSDropLineEdit.h EmMpm/Common/Qt/QFSDropLineEdit.h
* @brief
* @author Michael A. Jackson for BlueQuartz Software
* @date Feb 5, 2010
* @version 1.0
*/
class QFSDropLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	QFSDropLineEdit(QWidget *parent = NULL);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private:

};


#endif
