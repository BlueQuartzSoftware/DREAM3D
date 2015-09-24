/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FilterParameterWidget_H_
#define _FilterParameterWidget_H_



#include <QtCore/QObject>
#include <QtCore/QPointer>


#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>

#include "QtSupportLib/FaderWidget.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"


class AbstractFilter;
class FilterParameter;
class QPropertyAnimation;
class QGraphicsOpacityEffect;
class QTimer;


class DREAM3DWidgetsLib_EXPORT FilterParameterWidget : public QFrame
{
    Q_OBJECT
  public:
    FilterParameterWidget(QWidget* parent = 0);
    FilterParameterWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    virtual ~FilterParameterWidget();

    SIMPL_VIRTUAL_INSTANCE_PROPERTY(AbstractFilter*, Filter)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(FilterParameter*, FilterParameter)
    void fadeInWidget(QWidget* widget);

    QPointer<FaderWidget> getFaderWidget() const;
    void setFaderWidget(QPointer<FaderWidget> faderWidget);

    virtual void setupGui();

  public slots:

    void setLinkedConditionalState(int state);
    void setLinkedComboBoxState(int groupId);

    void fadeWidget(QWidget* widget, bool in);
    void animationFinished();

  protected slots:
    void showBorder();
    void hideBorder();

  private:
    QPointer<FaderWidget>         m_FaderWidget;

    float startValue;
    float endValue;
    bool fadeIn;
    QTimer*                       m_Timer;
    QPropertyAnimation* animation;
    QGraphicsOpacityEffect* effect;


    FilterParameterWidget(const FilterParameterWidget&); // Copy Constructor Not Implemented
    void operator=(const FilterParameterWidget&); // Operator '=' Not Implemented
};



#endif /* _FilterParameterWidget_H_ */

