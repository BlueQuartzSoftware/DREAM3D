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

#include "FilterListDockWidget.h"

#include <QtCore/QBitArray>
#include <QtCore/QFileInfo>
#include <QtCore/QMapIterator>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMainWindow>
#include <QtGui/QPainter>

#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"

#include "DREAM3DWidgetsLib/moc_FilterListDockWidget.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::FilterListDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_ContextMenu(new QMenu(this)),
  m_Mapper(NULL),
  m_SearchAnyWords(false),
  m_SearchExactPhrase(false),
  m_SearchAllWords(true)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::~FilterListDockWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::setupGui()
{
  updateFilterList(true);

  setupSearchField();

  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFFFF;\
              }");
  filterList->setStyleSheet(css);

  filterList->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(filterList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuForWidget(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::setupSearchField()
{
  filterSearch->setAttribute(Qt::WA_MacShowFocusRect, false);
  QMenu* lineEditMenu = new QMenu(filterSearch);
  filterSearch->setButtonMenu(SearchLineEdit::Left, lineEditMenu);
  filterSearch->setButtonVisible(SearchLineEdit::Left, true);
  filterSearch->setPlaceholderText("Search for filter");
  QPixmap pixmap(24, 24);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(QLatin1String(":/search_menu.png"));
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  filterSearch->setButtonPixmap(SearchLineEdit::Left, pixmap);
  connect(filterSearch, SIGNAL(textChanged(QString)), this, SLOT(searchFilters(QString)));

  {
    m_ActionAllWords = new QAction(filterSearch);
    m_ActionAllWords->setObjectName(QString::fromUtf8("actionAllWords"));
    m_ActionAllWords->setText(QApplication::translate("DREAM3D_UI", "All Words", 0));
    m_ActionAllWords->setCheckable(true);
    m_ActionAllWords->setChecked(m_SearchAllWords);
    filterSearch->addAction(m_ActionAllWords);
    connect(m_ActionAllWords, SIGNAL(toggled(bool)),
            this, SLOT(searchFieldsChanged(bool)));
    lineEditMenu->addAction(m_ActionAllWords);
  }

  {
    m_ActionAnyWords = new QAction(filterSearch);
    m_ActionAnyWords->setObjectName(QString::fromUtf8("actionWordForWord"));
    m_ActionAnyWords->setText(QApplication::translate("DREAM3D_UI", "Any Words", 0));
    m_ActionAnyWords->setCheckable(true);
    m_ActionAnyWords->setChecked(m_SearchAnyWords);
    filterSearch->addAction(m_ActionAnyWords);
    connect(m_ActionAnyWords, SIGNAL(toggled(bool)),
            this, SLOT(searchFieldsChanged(bool)));
    lineEditMenu->addAction(m_ActionAnyWords);
  }

  {
    m_ActionExactPhrase = new QAction(filterSearch);
    m_ActionExactPhrase->setObjectName(QString::fromUtf8("actionExactPhrase"));
    m_ActionExactPhrase->setText(QApplication::translate("DREAM3D_UI", "Exact Phrase", 0));
    m_ActionExactPhrase->setCheckable(true);
    m_ActionExactPhrase->setChecked(m_SearchExactPhrase);
    filterSearch->addAction(m_ActionExactPhrase);
    connect(m_ActionExactPhrase, SIGNAL(toggled(bool)),
            this, SLOT(searchFieldsChanged(bool)));
    lineEditMenu->addAction(m_ActionExactPhrase);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::showContextMenuForWidget(const QPoint& pos)
{
  // Clear the existing context menu
  m_ContextMenu->clear();

  QListWidgetItem* item = filterList->itemAt(pos);
  if(item)
  {
    m_Mapper = new QSignalMapper(this);
    QAction* actionLaunchHelp = new QAction(m_ContextMenu);
    actionLaunchHelp->setObjectName(QString::fromUtf8("actionLaunchHelp"));
    actionLaunchHelp->setText(QApplication::translate("DREAM3D_UI", "Filter Help", 0));
    connect(actionLaunchHelp, SIGNAL(triggered()),
            m_Mapper, SLOT(map()));

    QString itemName = item->text();
    m_Mapper->setMapping(actionLaunchHelp, itemName);
    connect(m_Mapper, SIGNAL(mapped(QString)),
            this, SLOT(launchHelpForItem(QString)));

    m_ContextMenu->addAction(actionLaunchHelp);
    m_ContextMenu->exec(QCursor::pos());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::launchHelpForItem(QString humanLabel)
{
  FilterManager* fm = FilterManager::Instance();
  if (NULL == fm)
  {
    return;
  }
  IFilterFactory::Pointer factory = fm->getFactoryForFilterHumanName(humanLabel);
  if (NULL == factory.get())
  {
    return;
  }
  AbstractFilter::Pointer filter = factory->create();
  if (NULL == filter.get())
  {
    return;
  }
  QString className = filter->getNameOfClass();
  // Launch the dialog
  DREAM3DUserManualDialog::LaunchHelpDialog(className);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::updateFilterList(bool sortItems)
{
  // Clear the list first
  filterList->clear();

  // Get the FilterManager and loop over all the factories
  FilterManager* fm = FilterManager::Instance();
  m_LoadedFilters = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(m_LoadedFilters);

  QString countText = QObject::tr("Filter Count: %1").arg(m_LoadedFilters.size());
  filterCountLabel->setText(countText);

  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    if (NULL == factory.get() )
    {
      continue;
    }

    AbstractFilter::Pointer filter = factory->create();
    if(NULL == filter.get())
    {
      continue;
    }

    addItemToList(filter);
  }
  if (sortItems)
  {
    filterList->sortItems(Qt::AscendingOrder);
  }
  filterSearch->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::addItemToList(AbstractFilter::Pointer filter)
{

  QString humanName = filter->getHumanLabel();
  QString iconName(":/Groups/");
  iconName.append( filter->getGroupName() );
  iconName.append("_Icon.png");

  // Validate the icon is in the resource system
  QFileInfo iconInfo(iconName);
  if (iconInfo.exists() == false)
  {
    iconName = ":/Groups/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
  }

  QIcon icon(iconName);
  // Create the QListWidgetItem and add it to the filterList
  QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
  // Set an "internal" QString that is the name of the filter. We need this value
  // when the item is clicked in order to retreive the Filter Widget from the
  // filter widget manager.
  filterItem->setData( Qt::UserRole, filter->getNameOfClass());
  // Allow a basic mouse hover tool tip that gives some summary information on the filter.
  filterItem->setToolTip(filter->generateHtmlSummary());

#if 0
  // This chunk of code would load up the html help file for the filter into the QToolTip
  // This is a problem because everytime this function ran it would involve file IO and
  // that is very bad. And the currently Doxygen generated html files do not look very
  // good in the QToolTip, and there is no Scrolling in the QToolTip window. We really
  // need a more custom solution for this
  QUrl url = DREAM3DHelpUrlGenerator::generateHTMLUrl( filter->getNameOfClass().toLower() );
  QString filePath = url.toLocalFile();
  QFileInfo fi(filePath);
  QFile source(filePath);
  source.open(QFile::ReadOnly);
  QString html = source.readAll();
  source.close();
  filterItem->setToolTip(html);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> FilterListDockWidget::serializeString(QString string, char token)
{
  std::string stringString = string.toStdString();
  QList<QString> list;
  int currentIndex = 0;
  int spaceIndex = 0;
  QString strPart = "";

  while (spaceIndex >= 0 && string.isEmpty() == false)
  {
    spaceIndex = string.indexOf(token);
    strPart = string.left(spaceIndex);
    strPart = strPart.simplified();
    if (strPart != "")
    {
      list.push_back(strPart);
    }
    string = string.remove(currentIndex, spaceIndex + 1);
    stringString = string.toStdString();
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterListDockWidget::deserializeString(QList<QString> list, char token)
{
  QString str = "";
  for (int i = 0; i < list.size(); i++)
  {
    str.append(list[i]);
    str.append(" ");
  }
  str.chop(1);

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::matchFilter(QMapIterator<QString, IFilterFactory::Pointer> iter, QString fullWord, int& filterCount)
{
  QList<QString> wordList = serializeString(fullWord, ' ');
  fullWord = deserializeString(wordList, ' ');
  QMap<AbstractFilter::Pointer, int> wordCountMap;
  QMultiMap<int, AbstractFilter::Pointer> relevanceMap;

  while (iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    if (NULL == factory.get())
    {
      continue;
    }

    AbstractFilter::Pointer filter = factory->create();
    if (NULL == filter.get())
    {
      continue;
    }

    QString filterHumanLabel = filter->getHumanLabel();
    QBitArray bitArray(wordList.size(), false);

    int consecutiveWordsCount = 0, maxConsecutiveWordsCount = 0, consecutiveWordsStartingIndex = 0;
    for (int i = 0; i < wordList.size(); i++)
    {
      QString keyword = wordList[i];

      if (filterHumanLabel.contains(keyword, Qt::CaseInsensitive) == true && filterList->findItems(filterHumanLabel, Qt::MatchExactly).size() <= 0)
      {
        bitArray.setBit(i, true);

        QList<QString> phraseList;
        for (int j = consecutiveWordsStartingIndex; j <= i; j++)
        {
          phraseList.append(wordList[j]);
        }
        QString phrase = deserializeString(phraseList, ' ');

        if (filterHumanLabel.contains(phrase, Qt::CaseInsensitive) && consecutiveWordsCount < phraseList.size())
        {
          consecutiveWordsCount++;
        }
        else
        {
          if (consecutiveWordsCount > maxConsecutiveWordsCount)
          {
            maxConsecutiveWordsCount = consecutiveWordsCount;
          }
          consecutiveWordsCount = 1;
          consecutiveWordsStartingIndex = i;
        }
      }
    }

    if (consecutiveWordsCount > maxConsecutiveWordsCount)
    {
      maxConsecutiveWordsCount = consecutiveWordsCount;
    }

    if (wordCountMap.contains(filter) == false && bitArray.count(true) > 0)
    {
      wordCountMap.insert(filter, bitArray.count(true));
      relevanceMap.insert(maxConsecutiveWordsCount, filter);
    }
  }

  // Match according to "Exact Phrase"
  if (m_ActionExactPhrase->isChecked())
  {
    QList<AbstractFilter::Pointer> filterList = relevanceMap.values(wordList.size());
    for (QList<AbstractFilter::Pointer>::iterator iter = filterList.begin(); iter != filterList.end(); ++iter)
    {
      // Do not display results that have the exact phrase in the middle or end of the search phrase
      if ((*iter)->getHumanLabel().startsWith(fullWord))
      {
        addItemToList(*iter);
        filterCount++;
      }
    }
  }
  // Match according to "All Words"
  else if (m_ActionAllWords->isChecked())
  {
    QList<AbstractFilter::Pointer> filterList = wordCountMap.keys(wordList.size());
    QMapIterator<int, AbstractFilter::Pointer> iter(relevanceMap);
    iter.toBack();
    while (iter.hasPrevious())
    {
      iter.previous();
      AbstractFilter::Pointer filter = iter.value();

      if (filterList.contains(filter))
      {
        addItemToList(filter);
        filterCount++;
      }
    }
  }
  // Match according to "Any Words"
  else if (m_ActionAnyWords->isChecked())
  {
    //QList<AbstractFilter::Pointer> filterList = wordCountMap.keys();
    QMapIterator<int, AbstractFilter::Pointer> iter(relevanceMap);
    iter.toBack();
    while (iter.hasPrevious())
    {
      iter.previous();
      AbstractFilter::Pointer filter = iter.value();

      addItemToList(filter);
      filterCount++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::searchFilters(QString text)
{
  // Set scroll bar back to the top
  filterList->scrollToTop();

  if( text.isEmpty() )
  {
    // Put back the entire list of Filters
    updateFilterList(true);

    // Set the text for the total number of filters
    QString countText = QObject::tr("Filter Count: %1").arg(m_LoadedFilters.size());
    filterCountLabel->setText(countText);
    return;
  }

  filterList->clear();

  // The user is typing something in the search box so lets search the filter class name and human label
  // int listWidgetSize = m_LoadedFilters.size();
  int filterCount = 0;
  QMapIterator<QString, IFilterFactory::Pointer> iter(m_LoadedFilters);

  matchFilter(iter, text, filterCount);

  QString countText = QObject::tr("Filter Count: %1").arg(filterCount);
  filterCountLabel->setText(countText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  emit filterItemDoubleClicked(item->data(Qt::UserRole).toString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::keyPressEvent(QKeyEvent* event)
{
  if (searchInProgress() == true)
  {
    if (NULL == filterList || filterList->count() <= 0)
    {
      return;
    }

    QList<QListWidgetItem*> selectedList = filterList->selectedItems();

    if (event->key() == Qt::Key_Down)
    {
      if (selectedList.size() == 0)
      {
        filterList->setItemSelected(filterList->item(0), true);
        filterList->setFocus();
      }
    }
    else if (event->key() == Qt::Key_Return)
    {
      if (selectedList.size() == 1)
      {
        QListWidgetItem* selectedItem = selectedList[0];
        on_filterList_itemDoubleClicked(selectedItem);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::searchFieldsChanged(bool isChecked)
{
  QAction* senderAction = qobject_cast<QAction*>(sender());

  if (isChecked == true)
  {
    m_ActionExactPhrase->blockSignals(true);
    m_ActionAllWords->blockSignals(true);
    m_ActionAnyWords->blockSignals(true);

    if (senderAction == m_ActionAnyWords)
    {
      m_ActionExactPhrase->setChecked(false);
      m_ActionAllWords->setChecked(false);
    }
    else if (senderAction == m_ActionExactPhrase)
    {
      m_ActionAnyWords->setChecked(false);
      m_ActionAllWords->setChecked(false);
    }
    else if (senderAction == m_ActionAllWords)
    {
      m_ActionExactPhrase->setChecked(false);
      m_ActionAnyWords->setChecked(false);
    }

    m_ActionExactPhrase->blockSignals(false);
    m_ActionAllWords->blockSignals(false);
    m_ActionAnyWords->blockSignals(false);

    searchFilters(filterSearch->text());
  }
  else
  {
    senderAction->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* FilterListDockWidget::getActiveSearchAction()
{
  if (m_ActionExactPhrase->isChecked())
  {
    return m_ActionExactPhrase;
  }
  else if (m_ActionAnyWords->isChecked())
  {
    return m_ActionAnyWords;
  }
  else
  {
    return m_ActionAllWords;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::setActiveSearchAction(QAction* action)
{
  m_ActionExactPhrase->blockSignals(true);
  m_ActionAllWords->blockSignals(true);
  m_ActionAnyWords->blockSignals(true);

  m_ActionExactPhrase->setChecked(false);
  m_ActionAllWords->setChecked(false);
  m_ActionAnyWords->setChecked(false);
  action->setChecked(true);

  m_ActionExactPhrase->blockSignals(false);
  m_ActionAllWords->blockSignals(false);
  m_ActionAnyWords->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QAction*> FilterListDockWidget::getSearchActionList()
{
  QList<QAction*> list;
  list.append(m_ActionAllWords);
  list.append(m_ActionAnyWords);
  list.append(m_ActionExactPhrase);
  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, AbstractFilter::Pointer> FilterListDockWidget::getHumanNameMap(QList<AbstractFilter::Pointer> list)
{
  QMap<QString, AbstractFilter::Pointer> map;
  for (int i = 0; i < list.size(); i++)
  {
    map.insert(list[i]->getHumanLabel(), list[i]);
  }

  return map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::writeSettings(DREAM3DSettings& prefs)
{
  prefs.setValue(objectName(), isHidden());
  prefs.setValue("ActiveSearchAction", getActiveSearchAction()->objectName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::readSettings(QMainWindow* main, DREAM3DSettings& prefs)
{
  main->restoreDockWidget(this);

  QString objectName = prefs.value("ActiveSearchAction", "").toString();
  QList<QAction*> list = getSearchActionList();

  bool didCheck = false;
  for (int i = 0; i < list.size(); i++)
  {
    if (list[i]->objectName() == objectName)
    {
      list[i]->setChecked(true);
      didCheck = true;
    }
    else
    {
      list[i]->setChecked(false);
    }
  }

  if (didCheck == false && list.size() > 0)
  {
    // Set "All Words" as checked by default
    list[0]->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterListDockWidget::searchInProgress()
{
  if (filterSearch->text().isEmpty())
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListWidget* FilterListDockWidget::getFilterListWidget()
{
  return filterList;
}


