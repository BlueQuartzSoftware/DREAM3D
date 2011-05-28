///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "QFSDropLineEdit.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtGui/QWidget>
#include <QtGui/QDragEnterEvent>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFSDropLineEdit::QFSDropLineEdit(QWidget *parent)
: QLineEdit(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
	// accept just text/uri-list mime format
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDropLineEdit::dropEvent(QDropEvent *event)
{
	QList<QUrl> urlList;
	QString fName;
	QFileInfo info;

	if (event->mimeData()->hasUrls())
	{
		urlList = event->mimeData()->urls(); // returns list of QUrls
		// if just text was dropped, urlList is empty (size == 0)

		if ( urlList.size() > 0) // if at least one QUrl is present in list
		{
			fName = urlList[0].toLocalFile(); // convert first QUrl to local path
			info.setFile( fName ); // information about file
			 setText( fName ); // if is file, setText
		}
	}

	event->acceptProposedAction();
}


