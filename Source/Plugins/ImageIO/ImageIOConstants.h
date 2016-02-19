#ifndef _imageioconstants_h_
#define _imageioconstants_h_

#include <QtCore/QString>

namespace ImageIOConstants
{
  const QString ImageIOPluginFile("ImageIOPlugin");
  const QString ImageIOPluginDisplayName("ImageIO");
  const QString ImageIOBaseName("ImageIO");
}

namespace FilterParameterWidgetType
{
  const QString ImportImagesWidget("ImportImagesWidget");
  const QString ImportVectorImageStackWidget("ImportVectorImageStackWidget");
}

namespace BlueQuartz
{
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2016 BlueQuartz Software, LLC");
}

#endif
