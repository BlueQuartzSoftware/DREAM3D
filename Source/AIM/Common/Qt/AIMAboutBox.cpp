/* <distribution-statement>
 * *****************************************************************************
 * Distribution authorized to DOD components and DOD contractors only;         *
 * Administrative or Operational Use;                                          *
 * February 2009.                                                              *
 * Other requests for this document shall be referred to                       *
 * U.S. Air Force Research Laboratory                                          *
 * 2230 Tenth St., Ste. 1                                                      *
 * Wright-Patterson AFB, OH 45433-7817                                         *
 *                                                                             *
 * Destroy by contents or reconstruction of the document.                      *
 *                                                                             *
 * This code was written under the U.S. Air Force contrct F33615-03-C-5220     *
 * *****************************************************************************
 * </distribution-statement> */
#include "AIMAboutBox.h"
#include <AIM/Common/AIMVersion.h>

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMAboutBox::AIMAboutBox(QWidget *parent) :
  QDialog(parent)
{
  this->setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMAboutBox::~AIMAboutBox()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMAboutBox::setApplicationName(QString applicationName)
{
  QString title("About ");
  title.append(applicationName);
  setWindowTitle(title);

  appName->setText(applicationName);
  QString version("Version ");
  version.append(AIMRepresentation_VERSION);
  appVersion->setText( version );
}


