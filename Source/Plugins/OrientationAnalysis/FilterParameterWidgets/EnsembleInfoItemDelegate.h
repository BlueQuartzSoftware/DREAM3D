/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#ifndef _ensembleinfoitemdelegate_h_
#define _ensembleinfoitemdelegate_h_

#include <iostream>

#include <QtCore/QModelIndex>
#include <QtCore/QStringList>
#include <QtWidgets/QStyledItemDelegate>

#include "SIMPLib/Common/Constants.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "EnsembleInfoTableModel.h"

class QPainter;
class QAbstractItemModel;

class EnsembleInfoItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit EnsembleInfoItemDelegate(int numPhases, QObject* parent = 0);

  virtual ~EnsembleInfoItemDelegate();

  void setNumberOfPhases(int n);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  void setCrystalStructureList(QStringList structures);

  void setPhaseTypeList(QStringList phases);

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  void setEditorData(QWidget* editor, const QModelIndex& index) const;

  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

public slots:
  void crystalStructureChangedData(int i);
  void phaseTypeChangedData(int i);
  void phaseNameChangedData(const QString& text);

private:
  QStringList m_CrystalStructureList;
  QStringList m_PhaseTypeList;
  int m_NumberOfPhases;

  EnsembleInfoItemDelegate(const EnsembleInfoItemDelegate&); // Copy Constructor Not Implemented
  void operator=(const EnsembleInfoItemDelegate&);           // Operator '=' Not Implemented
};

#endif /*  */
