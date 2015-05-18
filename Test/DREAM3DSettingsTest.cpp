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
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QByteArray>

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DWidgetsLib/Widgets/FilterLibraryTreeWidget.h"

#include "QtSupportLib/DREAM3DSettings.h"

#include "DREAM3DTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DREAM3DSettingsTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettingsTest()
{
  FilterLibraryTreeWidget* tree = new FilterLibraryTreeWidget();
  {
    DREAM3DSettings dream3dPrefs(UnitTest::DREAM3DSettingsTest::TestFile);

    /* ------------------ Tree test ------------------ */

    QTreeWidgetItem* folder1 = new QTreeWidgetItem(tree->invisibleRootItem(), FilterLibraryTreeWidget::Node_Item_Type);
    folder1->setText(0, "Folder1");
    folder1->setData(0, Qt::UserRole, "Folder1");
    folder1->setText(1, "FolderPath1");
    folder1->setData(1, Qt::UserRole, "FolderPath1");
    folder1->setIcon(0, QIcon(":/folder_blue.png"));
    folder1->setFlags(folder1->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    tree->addTopLevelItem(folder1);

    QTreeWidgetItem* item1 = new QTreeWidgetItem(tree->invisibleRootItem(), FilterLibraryTreeWidget::Node_Item_Type);
    item1->setText(0, "Item1");
    item1->setData(0, Qt::UserRole, "Item1");
    item1->setText(1, "ItemPath1");
    item1->setData(1, Qt::UserRole, "ItemPath1");
    item1->setIcon(0, QIcon(":/text.png"));
    item1->setFlags(item1->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
    tree->addTopLevelItem(item1);

    QTreeWidgetItem* folder2a = new QTreeWidgetItem(folder1, FilterLibraryTreeWidget::Node_Item_Type);
    folder2a->setText(0, "Folder2a");
    folder2a->setData(0, Qt::UserRole, "Folder2a");
    folder2a->setText(1, "FolderPath2a");
    folder2a->setData(1, Qt::UserRole, "FolderPath2a");
    folder2a->setIcon(0, QIcon(":/folder_blue.png"));
    folder2a->setFlags(folder2a->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    QTreeWidgetItem* folder2b = new QTreeWidgetItem(folder1, FilterLibraryTreeWidget::Node_Item_Type);
    folder2b->setText(0, "Folder2b");
    folder2b->setData(0, Qt::UserRole, "Folder2b");
    folder2b->setText(1, "FolderPath2b");
    folder2b->setData(1, Qt::UserRole, "FolderPath2b");
    folder2b->setIcon(0, QIcon(":/folder_blue.png"));
    folder2b->setFlags(folder2b->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    QTreeWidgetItem* item2 = new QTreeWidgetItem(folder1, FilterLibraryTreeWidget::Node_Item_Type);
    item2->setText(0, "Item2");
    item2->setData(0, Qt::UserRole, "Item2");
    item2->setText(1, "ItemPath2");
    item2->setData(1, Qt::UserRole, "ItemPath2");
    item2->setIcon(0, QIcon(":/text.png"));
    item2->setFlags(item2->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

    tree->sortItems(0, Qt::AscendingOrder);

    dream3dPrefs.beginGroup("Joey's Test Section");
    QJsonObject treeObj = tree->toJsonObject();
    dream3dPrefs.setValue("Joey's Tree Widget", treeObj);
    dream3dPrefs.endGroup();
  }

  DREAM3DSettings dream3dPrefs(UnitTest::DREAM3DSettingsTest::TestFile);

  // Read it back in
  dream3dPrefs.beginGroup("Joey's Test Section");
  QJsonObject obj = dream3dPrefs.value("Joey's Tree Widget", QJsonObject());
  FilterLibraryTreeWidget* newTree = FilterLibraryTreeWidget::FromJsonObject(obj);
  dream3dPrefs.endGroup();

  QTreeWidgetItemIterator iter(tree);
  QTreeWidgetItemIterator newIter(newTree);

  while (*iter)
  {
    QTreeWidgetItem* item = *iter;
    QTreeWidgetItem* newItem = *newIter;

    QString name = item->text(0);
    QString newName = newItem->text(0);

    DREAM3D_REQUIRE_EQUAL(item->text(0), newItem->text(0))
    DREAM3D_REQUIRE_EQUAL(item->text(1), newItem->text(1))
    DREAM3D_REQUIRE_EQUAL(item->data(0, Qt::UserRole).toString(), newItem->data(0, Qt::UserRole).toString())
    DREAM3D_REQUIRE_EQUAL(item->data(1, Qt::UserRole).toString(), newItem->data(1, Qt::UserRole).toString())
    DREAM3D_REQUIRE_EQUAL(item->type(), newItem->type())

    ++iter;
    ++newIter;
  }
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QApplication app(argc, argv);
  QApplication::setOrganizationName("BlueQuartz Software");
  QApplication::setOrganizationDomain("bluequartz.net");
  QApplication::setApplicationName("DREAM3DSettingsTest");

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST(RemoveTestFiles())

  DREAM3D_REGISTER_TEST(DREAM3DSettingsTest())

  DREAM3D_REGISTER_TEST(RemoveTestFiles())

  PRINT_TEST_SUMMARY();

  return err;
}

