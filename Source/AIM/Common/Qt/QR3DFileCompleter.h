///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#ifndef R3DFILECOMPLETER_H_
#define R3DFILECOMPLETER_H_

#include <QtCore/QObject>
#include <QtGui/QCompleter>

class QR3DFileCompleter : public QCompleter
{
  Q_OBJECT
public:
  QR3DFileCompleter(QObject* o, bool dirs);
  virtual QString pathFromIndex(const QModelIndex& idx) const;
};

#endif /* R3DFILECOMPLETER_H_ */
