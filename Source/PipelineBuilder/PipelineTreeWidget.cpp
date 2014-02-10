/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Intern, University of Dayton)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PipelineTreeWidget.h"

#include "PipelineBuilderWidget.h"

PipelineTreeWidget::PipelineTreeWidget(QFrame* parent) :
QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this,
    SIGNAL(customContextMenuRequested(const QPoint&)),
    SLOT(onCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeWidget::addActionList(QList<QAction*> actionList)
{
  for (int i=0; i<actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeWidget::onCustomContextMenuRequested(const QPoint& pos)
{
  QTreeWidgetItem* item = itemAt(pos);

  if (item)
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    showContextMenu( item, viewport()->mapToGlobal(pos) );
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeWidget::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
  // Clear menu's previous actions
  m_Menu.clear();

  int itemType = item->type();
  switch (itemType)
  {
  case Favorite_Item_Type:
    if ( !m_FavoritesActions.isEmpty() )
      addActionList(m_FavoritesActions);
    break;

  case Prebuilt_Item_Type:
    if ( !m_PrebuiltActions.isEmpty() )
      addActionList(m_PrebuiltActions);
    break;

  case Default_Item_Type:
    if ( !m_DefaultActions.isEmpty() )
      addActionList(m_DefaultActions);
    break;

  case Favorite_Category_Item_Type:
    if ( !m_FavoriteCategoryActions.isEmpty() )
      addActionList(m_FavoriteCategoryActions);
    break;

  case Prebuilt_Category_Item_Type:
    if ( !m_PrebuiltCategoryActions.isEmpty() )
      addActionList(m_PrebuiltCategoryActions);
    break;
  }

  m_Menu.exec(globalPos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeWidget::setActionList(ItemType type, QList<QAction*> list)
{
  switch (type)
  {
  case Favorite_Item_Type:
    m_FavoritesActions = list;
    break;

  case Prebuilt_Item_Type:
    m_PrebuiltActions = list;
    break;

  case Default_Item_Type:
    m_DefaultActions = list;
    break;

  case Favorite_Category_Item_Type:
    m_FavoriteCategoryActions = list;
    break;

  case Prebuilt_Category_Item_Type:
    m_PrebuiltCategoryActions = list;
    break;
  }
}
