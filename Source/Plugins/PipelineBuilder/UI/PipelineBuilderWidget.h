

#ifndef PipelineBuilderWIDGET_H_
#define PipelineBuilderWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_PipelineBuilderWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "QPipelineBuilder.h"

class QListWidgetItem;


/**
 * @class PipelineBuilderWidget PipelineBuilderWidget.h AIM/GUI/PipelineBuilderWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderWidget : public DREAM3DPluginFrame, private Ui::PipelineBuilderWidget
{
  Q_OBJECT

  public:
    PipelineBuilderWidget(QWidget *parent = 0);
    virtual ~PipelineBuilderWidget();

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
    QPipelineBuilder*            m_PipelineBuilder;
    QThread*                  m_WorkerThread;
    QString                   m_OpenDialogLastDirectory;

    PipelineBuilderWidget(const PipelineBuilderWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineBuilderWidget&); // Operator '=' Not Implemented
};

#endif /* PipelineBuilderWIDGET_H_ */
