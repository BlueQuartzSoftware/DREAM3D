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

#include "QtGui/QApplication"
#include "QtGui/QWidget"
#include "QtGui/QGraphicsView"
#include "QtGui/QGraphicsScene"
#include "QtGui/QGraphicsProxyWidget"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DFilters.h"

#include "FilterWidgets/QFilterWidgetManager.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  QApplication app(argc, argv);

  QWidget* top = new QWidget();
  top->setObjectName(QString::fromUtf8("topWidget"));

  QVBoxLayout* verticalLayout_3 = new QVBoxLayout(top);
  verticalLayout_3->setContentsMargins(0, 0, 0, 20);
  verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

  QScrollArea* scrollArea = new QScrollArea(top);
  scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
//  scrollArea->setFrameShadow(QFrame::Plain);
  QWidget* scrollAreaWidgetContents = new QWidget();
  scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
  //scrollAreaWidgetContents->setGeometry(QRect(0, 0, 753, 584));
  QVBoxLayout* verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  verticalLayout->setSpacing(20);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

  // Get the QFilterWidget Mangager Instance
  QFilterWidgetManager::RegisterKnownQFilterWidgets();
  QFilterWidgetManager::Pointer fm = QFilterWidgetManager::Instance();

  // Get all the Widget Factories and loop over each one we know about and instantiate a new one
  QFilterWidgetManager::Collection factories = fm->getFactories();
  for (QFilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory )
  {
    IFilterWidgetFactory::Pointer f = factory.value();

    QFilterWidget* filterWidget = f->createWidget();
    if (filterWidget != NULL)
    {
      verticalLayout->addWidget(filterWidget);
    }
  }

  // Add some flexible space
  QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout->addSpacerItem(verticalSpacer);

  // Set the contents of the scroll area
  scrollArea->setWidget(scrollAreaWidgetContents);
  QPushButton* button = new QPushButton("Start");

  verticalLayout_3->addWidget(button);
  verticalLayout_3->addWidget(scrollArea);

  top->show();
  return app.exec();

}


