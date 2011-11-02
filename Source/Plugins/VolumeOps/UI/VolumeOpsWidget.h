

#ifndef VolumeOpsWIDGET_H_
#define VolumeOpsWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_VolumeOpsWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "QVolumeOps.h"

class QListWidgetItem;


/**
 * @class VolumeOpsWidget VolumeOpsWidget.h AIM/GUI/VolumeOpsWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class VolumeOpsWidget : public DREAM3DPluginFrame, private Ui::VolumeOpsWidget
{
  Q_OBJECT

  public:
    VolumeOpsWidget(QWidget *parent = 0);
    virtual ~VolumeOpsWidget();

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
    void on_m_H5Btn_clicked();
    void on_m_OutputDirBtn_clicked();
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    void on_m_H5InputFile_textChanged(const QString &text);

    void on_m_OutputDir_textChanged(const QString &text);
    void on_m_OutputFilePrefix_textChanged(const QString &text);

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
    QVolumeOps*            m_VolumeOps;
    QThread*                  m_WorkerThread;
    QString                   m_OpenDialogLastDirectory;

    VolumeOpsWidget(const VolumeOpsWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeOpsWidget&); // Operator '=' Not Implemented
};

#endif /* VolumeOpsWIDGET_H_ */
