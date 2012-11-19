/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _PipelineArraySelectionWidget_H_
#define _PipelineArraySelectionWidget_H_

#include <string>
#include <set>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QEvent>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtGui/QDropEvent>


#include "ui_PipelineArraySelectionWidget.h"

#include "PipelineBuilder/PipelineBuilderDLLExport.h"

class QListWidgetItem;
class QTreeWidgetItem;
class QFilterWidget;
class QMenu;
class AbstractFilter;

typedef std::set<std::string> StringSet_t;


/**
 * @class PipelineArraySelectionWidget PipelineArraySelectionWidget.h PipelineBuilder/UI/PipelineArraySelectionWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderLib_EXPORT PipelineArraySelectionWidget : public QWidget, private Ui::PipelineArraySelectionWidget
{
  Q_OBJECT

  public:
    PipelineArraySelectionWidget(QWidget *parent = 0);
    virtual ~PipelineArraySelectionWidget();

      /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    virtual void setPossibleCellArrayNames(QStringList names);
    virtual void setPossibleFieldArrayNames(QStringList names);
    virtual void setPossibleEnsembleArrayNames(QStringList names);

    virtual void setCreatedCellArrayNames(QStringList names);
    virtual void setCreatedFieldArrayNames(QStringList names);
    virtual void setCreatedEnsembleArrayNames(QStringList names);

    virtual void setRequiredCellArrayNames(QStringList names);
    virtual void setRequiredFieldArrayNames(QStringList names);
    virtual void setRequiredEnsembleArrayNames(QStringList names);

    virtual void updatePipelineArrayNames(AbstractFilter* filter);
    virtual void updateCreatedTree(StringSet_t &names, QString groupName);
    virtual void updateRequiredTree(StringSet_t &names, QString groupName);

  private:
    QStringList m_PossibleCellArrayNames;
    QStringList m_PossibleFieldArrayNames;
    QStringList m_PossibleEnsembleArrayNames;

    PipelineArraySelectionWidget(const PipelineArraySelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineArraySelectionWidget&); // Operator '=' Not Implemented
};


#endif /* _PipelineArraySelectionWidget_H_ */
