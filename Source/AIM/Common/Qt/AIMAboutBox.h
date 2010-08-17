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

#ifndef R3DABOUTBOX_H_
#define R3DABOUTBOX_H_

#include <QtGui/QDialog>

//-- UIC generated Header
#include <ui_AIMAboutBox.h>


/**
 * @class AIMAboutBox AIMAboutBox.h R3D/Common/Qt/AIMAboutBox.h
 * @brief This will display a simple dialog box that acts like an "About Box" for
 * showing some basic information about the application being used.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 14, 2009
 * @version 1.0
 */
class AIMAboutBox: public QDialog, private Ui::AIMAboutBox
{
  Q_OBJECT
    ;

  public:
    AIMAboutBox(QWidget *parent = 0);
    virtual ~AIMAboutBox();

    void setApplicationName( QString appName);

  private:
    AIMAboutBox(const AIMAboutBox&); // Copy Constructor Not Implemented
    void operator=(const AIMAboutBox&); // Operator '=' Not Implemented


};

#endif /* R3DABOUTBOX_H_ */
