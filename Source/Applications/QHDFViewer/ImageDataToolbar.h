///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef IMAGEDATATOOLBAR_H_
#define IMAGEDATATOOLBAR_H_

#include <QtCore/QObject>

class QMainWindow;


class ImageDataToolbar : public QObject
{

Q_OBJECT;

  public:
    ImageDataToolbar(QMainWindow *parent = 0);
    virtual ~ImageDataToolbar();


  protected:
    void initToolbar(QMainWindow* parent);


  private:
    ImageDataToolbar(const ImageDataToolbar&);    // Copy Constructor Not Implemented
      void operator=(const ImageDataToolbar&);  // Operator '=' Not Implemented
};

#endif /* IMAGEDATATOOLBAR_H_ */
