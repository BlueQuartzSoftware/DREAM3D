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


#include "IssuesDockWidget.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QMainWindow>

#include "SIMPLib/Common/DocRequestManager.h"

#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DWidgetsLib/moc_IssuesDockWidget.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesDockWidget::IssuesDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesDockWidget::~IssuesDockWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::setupGui()
{
  errorTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  errorTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  errorTableWidget->horizontalHeader()->resizeSection(1, 250);
  errorTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  errorTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  errorTableWidget->setVisible(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::on_errorTableWidget_itemClicked( QTableWidgetItem* item )
{
  qDebug() << item->text().toLatin1().data() << "\n";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::clearIssues()
{
  errorTableWidget->clearContents();
  errorTableWidget->setRowCount(0);
  m_CachedMessages.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::processPipelineMessage(const PipelineMessage& msg)
{
  m_CachedMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::displayCachedMessages()
{
  // Figure out how many error and warning messages that we have. We ignore the rest
  int count = 0;
  for(int i = 0; i < m_CachedMessages.size(); i++)
  {
    PipelineMessage msg = m_CachedMessages[i];
    switch(msg.getType())
    {
      case PipelineMessage::Error:
        count++;
        break;
      case PipelineMessage::Warning:
        count++;
        break;
      default:
        break;
    }
  }

  // Now create the correct number of table rows.
  errorTableWidget->setRowCount(count);
  int row = 0;
  // Add in the content for the cells of the table.
  for(int j = 0; j < m_CachedMessages.size(); j++)
  {
    PipelineMessage msg = m_CachedMessages[j];
    // Create error hyperlink
    QLabel* hyperlinkLabel = createHyperlinkLabel(msg);

    QColor msgColor;
    switch(msg.getType())
    {
      case PipelineMessage::Error:
        // m_hasErrors = true;
        msgColor.setRed(255);
        msgColor.setGreen(191);
        msgColor.setBlue(193);
        {
          QBrush msgBrush(msgColor);

          QString msgDesc = (msg.getText());
          int msgCode = msg.getCode();

          QString msgPrefix = (msg.getPrefix());
          QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(msgPrefix);
          filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
          QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msgDesc);
          QTableWidgetItem* codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
          codeWidgetItem->setTextAlignment(Qt::AlignCenter);

          filterNameWidgetItem->setBackground(msgBrush);
          descriptionWidgetItem->setBackground(msgBrush);
          codeWidgetItem->setBackground(msgBrush);

          if (hyperlinkLabel == NULL)
          {
            errorTableWidget->setItem(row, 0, filterNameWidgetItem);
          }
          else
          {
            errorTableWidget->setCellWidget(row, 0, hyperlinkLabel);
          }
          errorTableWidget->setItem(row, 1, descriptionWidgetItem);
          errorTableWidget->setItem(row, 2, codeWidgetItem);
        }
        row++;
        break;

      case PipelineMessage::Warning:
        //  m_hasWarnings = true;
        msgColor.setRed(251);
        msgColor.setGreen(254);
        msgColor.setBlue(137);

        {
          QBrush msgBrush(msgColor);

          QString msgName = (msg.getPrefix());
          QString msgDesc = (msg.getText());
          int msgCode = msg.getCode();



          QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(msgName);
          filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
          QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msgDesc);
          QTableWidgetItem* codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
          codeWidgetItem->setTextAlignment(Qt::AlignCenter);

          filterNameWidgetItem->setBackground(msgBrush);
          descriptionWidgetItem->setBackground(msgBrush);
          codeWidgetItem->setBackground(msgBrush);

          if (hyperlinkLabel == NULL)
          {
            errorTableWidget->setItem(row, 0, filterNameWidgetItem);
          }
          else
          {
            errorTableWidget->setCellWidget(row, 0, hyperlinkLabel);
          }
          errorTableWidget->setItem(row, 1, descriptionWidgetItem);
          errorTableWidget->setItem(row, 2, codeWidgetItem);
        }
        row++;
        break;

      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* IssuesDockWidget::createHyperlinkLabel(PipelineMessage msg)
{
  QString filterClassName = (msg.getFilterClassName() );
  QString filterHumanLabel = (msg.getFilterHumanLabel() );
  QString msgPrefix = (msg.getPrefix());

  if ( filterClassName.isEmpty() || filterHumanLabel.isEmpty() )
  {
    if(filterClassName.isEmpty() == false) { return new QLabel(filterClassName); }
    else if(filterHumanLabel.isEmpty() == false) { return new QLabel(filterHumanLabel); }

    return new QLabel("Unknown Filter Class");
  }

  QUrl filterURL = DREAM3DHelpUrlGenerator::generateHTMLUrl( filterClassName.toLower() );
  QString filterHTMLText("<a href=\"");
  filterHTMLText.append(filterURL.toString()).append("\">").append(filterHumanLabel).append("</a>");

  QLabel* hyperlinkLabel = new QLabel(filterHTMLText);
  hyperlinkLabel->setTextFormat(Qt::RichText);
  hyperlinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  hyperlinkLabel->setOpenExternalLinks(false);
  connect(hyperlinkLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(showFilterHelp(const QString&)));

  return hyperlinkLabel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::showFilterHelp(const QString &urlString)
{
  QUrl helpURL(urlString);

  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocUrl(helpURL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::writeSettings(DREAM3DSettings& prefs)
{
  prefs.setValue(objectName(), isHidden());

  QByteArray headerState = errorTableWidget->horizontalHeader()->saveState();
  prefs.setValue("Horizontal Header State", headerState);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IssuesDockWidget::readSettings(QMainWindow* main, DREAM3DSettings& prefs)
{
  main->restoreDockWidget(this);

  bool b = prefs.value(objectName(), false).toBool();
  setHidden(b);

  QByteArray headerState = prefs.value("Horizontal Header State", QByteArray()).toByteArray();
  errorTableWidget->horizontalHeader()->restoreState(headerState);
}
