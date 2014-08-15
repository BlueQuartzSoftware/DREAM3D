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
#include <QtGui/QTreeWidgetItem>
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
  m_SearchFilterClassName(false),
  m_SearchFilterHumanName(true),
  m_SearchParameterPropertyName(false),
  m_SearchParameterHumanName(true),
  m_SearchSearchGroupName(true),
  m_SearchSubGroupName(true),
  m_SearchPluginName(true)
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
  updateSearchIcons();
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
  //filterSearch->setAutoHideButton(SearchLineEdit::Left, true);
  filterSearch->setPlaceholderText("Search for filter");
  connect(filterSearch, SIGNAL(textChanged(QString)), this, SLOT(searchFieldsChanged()));

  {
    m_ActionSearchFilterHumanName = new QAction(filterSearch);
    m_ActionSearchFilterHumanName->setObjectName(QString::fromUtf8("actionFilterHumanLabel"));
    m_ActionSearchFilterHumanName->setText(QApplication::translate("DREAM3D_UI", "Filter Human Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchFilterHumanName->setCheckable(true);
    m_ActionSearchFilterHumanName->setChecked(m_SearchFilterHumanName);
    filterSearch->addAction(m_ActionSearchFilterHumanName);
    connect(m_ActionSearchFilterHumanName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchFilterHumanName);
  }
  {
    m_ActionSearchFilterClassName = new QAction(filterSearch);
    m_ActionSearchFilterClassName->setObjectName(QString::fromUtf8("actionFilterClassName"));
    m_ActionSearchFilterClassName->setText(QApplication::translate("DREAM3D_UI", "Filter Class Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchFilterClassName->setCheckable(true);
    m_ActionSearchFilterClassName->setChecked(m_SearchFilterClassName);
    filterSearch->addAction(m_ActionSearchFilterClassName);
    connect(m_ActionSearchFilterClassName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchFilterClassName);
  }

  {
    m_ActionSearchParameterName = new QAction(filterSearch);
    m_ActionSearchParameterName->setObjectName(QString::fromUtf8("actionFilterParameterName"));
    m_ActionSearchParameterName->setText(QApplication::translate("DREAM3D_UI", "Parameter Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchParameterName->setCheckable(true);
    m_ActionSearchParameterName->setChecked(m_SearchParameterHumanName);
    filterSearch->addAction(m_ActionSearchParameterName);
    connect(m_ActionSearchParameterName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchParameterName);
  }
  {
    m_ActionSearchParameterPropertyName = new QAction(filterSearch);
    m_ActionSearchParameterPropertyName->setObjectName(QString::fromUtf8("actionFilterParameterPropertyName"));
    m_ActionSearchParameterPropertyName->setText(QApplication::translate("DREAM3D_UI", "Parameter Property Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchParameterPropertyName->setCheckable(true);
    m_ActionSearchParameterPropertyName->setChecked(m_SearchParameterPropertyName);
    filterSearch->addAction(m_ActionSearchParameterPropertyName);
    connect(m_ActionSearchParameterPropertyName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchParameterPropertyName);
  }

  {
    m_ActionSearchGroupName = new QAction(filterSearch);
    m_ActionSearchGroupName->setObjectName(QString::fromUtf8("actionSearchGroupName"));
    m_ActionSearchGroupName->setText(QApplication::translate("DREAM3D_UI", "Filter Group", 0, QApplication::UnicodeUTF8));
    m_ActionSearchGroupName->setCheckable(true);
    m_ActionSearchGroupName->setChecked(m_SearchSearchGroupName);
    filterSearch->addAction(m_ActionSearchGroupName);
    connect(m_ActionSearchGroupName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchGroupName);
  }

  {
    m_ActionSearchSubGroupName = new QAction(filterSearch);
    m_ActionSearchSubGroupName->setObjectName(QString::fromUtf8("actionSearchSubGroupName"));
    m_ActionSearchSubGroupName->setText(QApplication::translate("DREAM3D_UI", "Filter Subgroup", 0, QApplication::UnicodeUTF8));
    m_ActionSearchSubGroupName->setCheckable(true);
    m_ActionSearchSubGroupName->setChecked(m_SearchSubGroupName);
    filterSearch->addAction(m_ActionSearchSubGroupName);
    connect(m_ActionSearchSubGroupName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchSubGroupName);
  }

  {
    m_ActionSearchPluginName = new QAction(filterSearch);
    m_ActionSearchPluginName->setObjectName(QString::fromUtf8("actionSearchPluginName"));
    m_ActionSearchPluginName->setText(QApplication::translate("DREAM3D_UI", "Filter Plugin", 0, QApplication::UnicodeUTF8));
    m_ActionSearchPluginName->setCheckable(true);
    m_ActionSearchPluginName->setChecked(m_SearchPluginName);
    filterSearch->addAction(m_ActionSearchPluginName);
    connect(m_ActionSearchPluginName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
    lineEditMenu->addAction(m_ActionSearchPluginName);
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
  QString iconName(":/");
  iconName.append( filter->getGroupName() );
  iconName.append("_Icon.png");

  // Validate the icon is in the resource system
  QFileInfo iconInfo(iconName);
  if (iconInfo.exists() == false)
  {
    iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
  }

  QIcon icon(iconName);
  // Create the QListWidgetItem and add it to the filterList
  QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
  // Set an "internal" QString that is the name of the filter. We need this value
  // when the item is clicked in order to retreive the Filter Widget from the
  // filter widget manager.
  filterItem->setData( Qt::UserRole, filter->getNameOfClass());

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
void FilterListDockWidget::searchFilters()
{

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
  bool match = false;
  QMapIterator<QString, IFilterFactory::Pointer> iter(m_LoadedFilters);

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

    match = false;
    QString filterHumanLabel = filter->getHumanLabel();

    if (m_ActionSearchFilterHumanName->isChecked() && filterHumanLabel.contains(text, Qt::CaseInsensitive) == true)
    {
      //  qDebug() << "   " << filterHumanLabel;
      match = true;
    }

    QString filterClassName = filter->getNameOfClass();
    if (m_ActionSearchFilterClassName->isChecked() && filterClassName.contains(text, Qt::CaseInsensitive) == true)
    {
      //   qDebug() << "   " << filterClassName;
      match = true;
    }

    QString groupName = filter->getGroupName();
    QString subGroupName = filter->getSubGroupName();
    QString compileLibrary = filter->getCompiledLibraryName();
    if (m_ActionSearchGroupName->isChecked() && groupName.contains(text, Qt::CaseInsensitive) == true)
    {
      //   qDebug() << "       " << option->getPropertyName();
      match = true;
    }
    if (m_ActionSearchSubGroupName->isChecked() && subGroupName.contains(text, Qt::CaseInsensitive) == true)
    {
      //   qDebug() << "       " << option->getPropertyName();
      match = true;
    }
    if (m_ActionSearchPluginName->isChecked() && compileLibrary.contains(text, Qt::CaseInsensitive) == true)
    {
      //   qDebug() << "       " << option->getPropertyName();
      match = true;
    }
    // Get a list of all the filterParameters from the filter.
    QVector<FilterParameter::Pointer> filterParameters = filter->getFilterParameters();
    // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
    for (QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter )
    {
      FilterParameter* option = (*iter).get();
      if (m_ActionSearchParameterName->isChecked() && option->getHumanLabel().contains(text, Qt::CaseInsensitive) == true)
      {
        //  qDebug() << "       " << option->getHumanLabel();
        match = true;
      }
      if (m_ActionSearchParameterPropertyName->isChecked() && option->getPropertyName().contains(text, Qt::CaseInsensitive) == true)
      {
        //   qDebug() << "       " << option->getPropertyName();
        match = true;
      }

    }

    if(match)
    {
      filterCount++;
      addItemToList(filter);
    }
  }

  bool sortItems = true;
  if (sortItems)
  {
    filterList->sortItems(Qt::AscendingOrder);
  }

  QString countText = QObject::tr("Filter Count: %1").arg(filterCount);
  filterCountLabel->setText(countText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::searchFieldsChanged()
{
  updateSearchIcons();
  searchFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::updateSearchIcons()
{
  m_SearchFilterHumanName = m_ActionSearchFilterHumanName->isChecked();
  m_SearchFilterClassName = m_ActionSearchFilterClassName->isChecked();
  m_SearchParameterHumanName = m_ActionSearchParameterName->isChecked();
  m_SearchParameterPropertyName = m_ActionSearchParameterPropertyName->isChecked();
  if (!m_SearchFilterHumanName && !m_SearchFilterClassName && !m_SearchParameterHumanName && !m_SearchParameterPropertyName)
  {
    QPixmap pixmap(17, 17);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    const QPixmap mag = QPixmap(QLatin1String(":/view.png"));
    painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
    filterSearch->setButtonPixmap(SearchLineEdit::Left, pixmap);
  }
  else
  {
    QPixmap pixmap(17, 17);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    const QPixmap mag = QPixmap(QLatin1String(":/view_add.png"));
    painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
    //filterSearch->setButtonPixmap(SearchLineEdit::Left, IFindFilter::pixmapForFindFlags(effectiveFlags));
    filterSearch->setButtonPixmap(SearchLineEdit::Left, pixmap);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  emit filterItemDoubleClicked(item->data(Qt::UserRole).toString());
}
