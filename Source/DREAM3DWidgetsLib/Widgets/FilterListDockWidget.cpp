/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FilterListDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QTreeWidgetItem>
#include <QtGui/QPainter>

#include "QtSupport/DREAM3DHelpUrlGenerator.h"


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DWidgetsLib/moc_FilterListDockWidget.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::FilterListDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_SearchWordForWord(false),
  m_SearchExactPhrase(true)
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
  //updateSearchIcons();

  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFFFF;\
              }");
  filterList->setStyleSheet(css);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::setupSearchField()
{
  filterSearch->setAttribute(Qt::WA_MacShowFocusRect, false);
/*

  3 Menu Items
  * Exact Phrase
  * Any Words
  * All Words

  Exact Phrase: Words in the search box must appear in order but anywhere in the human label

  Any Words: Any of the words in any order appear in the human label

  All Words: ALL of the Words appear in the human label, but in any order.

  */



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
  connect(filterSearch, SIGNAL(textChanged(QString)), this, SLOT(searchFieldsChanged()));

  {
    m_ActionExactPhrase = new QAction(filterSearch);
    m_ActionExactPhrase->setObjectName(QString::fromUtf8("actionExactPhrase"));
    m_ActionExactPhrase->setText(QApplication::translate("DREAM3D_UI", "Exact Phrase", 0));
    m_ActionExactPhrase->setCheckable(true);
    m_ActionExactPhrase->setChecked(m_SearchExactPhrase);
    filterSearch->addAction(m_ActionExactPhrase);
    connect(m_ActionExactPhrase, SIGNAL(triggered()),
      this, SLOT(searchFieldsChanged()));
    lineEditMenu->addAction(m_ActionExactPhrase);
  }
  {
    m_ActionWordForWord = new QAction(filterSearch);
    m_ActionWordForWord->setObjectName(QString::fromUtf8("actionWordForWord"));
    m_ActionWordForWord->setText(QApplication::translate("DREAM3D_UI", "Contains At Least One Word", 0));
    m_ActionWordForWord->setCheckable(true);
    m_ActionWordForWord->setChecked(m_SearchWordForWord);
    filterSearch->addAction(m_ActionWordForWord);
    connect(m_ActionWordForWord, SIGNAL(triggered()),
      this, SLOT(searchFieldsChanged()));
    lineEditMenu->addAction(m_ActionWordForWord);
  }
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
    std::string strPartString = strPart.toStdString();
    list.push_back(strPart);
    string = string.remove(currentIndex, spaceIndex + 1);
    stringString = string.toStdString();
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::matchFilter(QMapIterator<QString, IFilterFactory::Pointer> iter, QString fullWord, int &filterCount)
{
  QList<QString> wordList = serializeString(fullWord, ' ');
  QList<AbstractFilter::Pointer> filterCache;

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

    bool match = false;
    QString filterHumanLabel = filter->getHumanLabel();

    for (QList<QString>::iterator wordIter = wordList.begin(); wordIter != wordList.end(); wordIter++)
    {
      QString keyword = *wordIter;

      if (filterHumanLabel.contains(keyword, Qt::CaseInsensitive) == true && filterList->findItems(filterHumanLabel, Qt::MatchExactly).size() <= 0 && filterHumanLabel.startsWith(fullWord, Qt::CaseInsensitive))
      {
        filterCount++;
        addItemToList(filter);
      }
      else if (filterHumanLabel.contains(keyword, Qt::CaseInsensitive) == true && filterList->findItems(filterHumanLabel, Qt::MatchExactly).size() <= 0 && filterCache.contains(filter) == false)
      {
        filterCount++;
        filterCache.push_back(filter);
      }
    }
  }

  // Add the remaining items to the list that do not match the full text, but match one or more words
  if (m_ActionWordForWord->isChecked())
  {
    for (QList<AbstractFilter::Pointer>::iterator iter = filterCache.begin(); iter != filterCache.end(); iter++)
    {
      addItemToList(*iter);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::searchFilters()
{
  // Set scroll bar back to the top
  filterList->scrollToTop();

  // Get the text from the search box
  QString text = filterSearch->text();

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
void FilterListDockWidget::searchFieldsChanged()
{
  //updateSearchIcons();
  if (sender() == m_ActionWordForWord)
  {
    m_ActionExactPhrase->blockSignals(true);
    m_ActionExactPhrase->setChecked(false);
    m_ActionExactPhrase->blockSignals(false);
  }
  else if (sender() == m_ActionExactPhrase)
  {
    m_ActionWordForWord->blockSignals(true);
    m_ActionWordForWord->setChecked(false);
    m_ActionWordForWord->blockSignals(false);
  }

  searchFilters();
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//void FilterListDockWidget::updateSearchIcons()
//{
//  m_SearchFilterHumanName = m_ActionSearchFilterHumanName->isChecked();
//  m_SearchFilterClassName = m_ActionSearchFilterClassName->isChecked();
//  m_SearchParameterHumanName = m_ActionSearchParameterName->isChecked();
//  m_SearchParameterPropertyName = m_ActionSearchParameterPropertyName->isChecked();
//  if (!m_SearchFilterHumanName && !m_SearchFilterClassName && !m_SearchParameterHumanName && !m_SearchParameterPropertyName)
//  {
//    QPixmap pixmap(17, 17);
//    pixmap.fill(Qt::transparent);
//    QPainter painter(&pixmap);
//    const QPixmap mag = QPixmap(QLatin1String(":/cube_molecule_view.png"));
//    painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
//    filterSearch->setButtonPixmap(SearchLineEdit::Left, pixmap);
//  }
//  else
//  {
//    QPixmap pixmap(17, 17);
//    pixmap.fill(Qt::transparent);
//    QPainter painter(&pixmap);
//    const QPixmap mag = QPixmap(QLatin1String(":/view_add.png"));
//    painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
//    filterSearch->setButtonPixmap(SearchLineEdit::Left, pixmap);
//  }
//}




