

#ifndef Extrapolation2Dto3DWIDGET_H_
#define Extrapolation2Dto3DWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_Extrapolation2Dto3DWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "QExtrapolation2Dto3D.h"

class QListWidgetItem;


/**
 * @class Extrapolation2Dto3DWidget Extrapolation2Dto3DWidget.h AIM/GUI/Extrapolation2Dto3DWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class Extrapolation2Dto3DWidget : public DREAM3DPluginFrame, private Ui::Extrapolation2Dto3DWidget
{
  Q_OBJECT

  public:
    Extrapolation2Dto3DWidget(QWidget *parent = 0);
    virtual ~Extrapolation2Dto3DWidget();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings &prefs);

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings &prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();


    virtual void checkIOFiles();

  protected slots:
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    private slots:
      // slots for our worker thread to communicate
      virtual void addErrorMessage(QString message);
      virtual void addWarningMessage(QString message);
      virtual void addProgressMessage(QString message);

      /* Surface Mesh Thread communicates throught these methods */
      virtual void pipelineComplete();
      virtual void pipelineProgress(int value);

  private:
    QList<QWidget*>           m_WidgetList;
    QExtrapolation2Dto3D*            m_Extrapolation2Dto3D;
    QThread*                  m_WorkerThread;
    QString                   m_OpenDialogLastDirectory;

    Extrapolation2Dto3DWidget(const Extrapolation2Dto3DWidget&); // Copy Constructor Not Implemented
    void operator=(const Extrapolation2Dto3DWidget&); // Operator '=' Not Implemented
};

#endif /* Extrapolation2Dto3DWIDGET_H_ */
