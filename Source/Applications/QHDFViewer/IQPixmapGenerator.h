///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef IQPIXMAPGENERATOR_H_
#define IQPIXMAPGENERATOR_H_


//-- Qt includes
#include <QtCore/QObject>
#include <QtGui/QPixmap>

/**
 * @brief
 * @class IQPixmapGenerator IQPixmapGenerator.h Shared/IQPixmapGenerator.h
 * @author
 * @date
 * @version $Revision: 1.3 $
 */
class IQPixmapGenerator : public QObject
{
  Q_OBJECT;

public:
  IQPixmapGenerator(QObject* parent = 0) {};
  virtual ~IQPixmapGenerator() {};

  virtual QPixmap generatePixmap() = 0;

};


#endif /*IQPIXMAPGENERATOR_H_*/
