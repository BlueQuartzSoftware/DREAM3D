#ifndef _QFilterParameterWidget_H_
#define _QFilterParameterWidget_H_

#include <QtGui/QWidget>

#include "FilterWidgetsLib/FilterWidgetsLib.h"


class FilterWidgetsLib_EXPORT QFilterParameterWidget : public QWidget
{
    Q_OBJECT

    public:
      QFilterParameterWidget(QWidget* parent = NULL);
      virtual ~QFilterParameterWidget();

//    DREAM3D_INSTANCE_STRING_PROPERTY(HumanLabel)
//    DREAM3D_INSTANCE_STRING_PROPERTY(PropertyName)
//    DREAM3D_INSTANCE_STRING_PROPERTY(WidgetType)
//    DREAM3D_INSTANCE_STRING_PROPERTY(ValueType)
//    DREAM3D_INSTANCE_STRING_PROPERTY(CastableValueType)
//    DREAM3D_INSTANCE_STRING_PROPERTY(FileExtension)
//    DREAM3D_INSTANCE_STRING_PROPERTY(FileType)
//    DREAM3D_INSTANCE_STRING_PROPERTY(Units)


  private:
      QFilterParameterWidget(const QFilterParameterWidget&); // Copy Constructor Not Implemented
      void operator=(const QFilterParameterWidget&); // Operator '=' Not Implemented
};

#endif /* _QFilterParameterWidget_H_ */
