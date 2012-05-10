
#ifndef FileConversionWIDGET_H_
#define FileConversionWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_FileConversionWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "QFileConversion.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

class QListWidgetItem;

/**
 * @class FileConversionWidget FileConversionWidget.h AIM/GUI/FileConversionWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class FileConversionWidget : public DREAM3DPluginFrame, private Ui::FileConversionWidget
{
  Q_OBJECT

  public:
    FileConversionWidget(QWidget *parent = 0);
    virtual ~FileConversionWidget();

    DREAM3D_INSTANCE_PROPERTY(QString, OpenDialogLastDirectory)

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

    void checkFile(QLineEdit* lineEdit, QLabel* label);

  protected slots:
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    void on_m_InputFilePath_textChanged(const QString text);
    void on_m_OutputFilePath_textChanged(const QString text);

    void on_m_SelectInputBtn_clicked();

    void on_m_SaveOutputBtn_clicked();




  private slots:
    // slots for our worker thread to communicate
    virtual void addErrorMessage(QString message);
    virtual void addWarningMessage(QString message);
    virtual void addProgressMessage(QString message);

    /* Surface Mesh Thread communicates throught these methods */
    virtual void pipelineComplete();
    virtual void pipelineProgress(int value);

  private:
    QList<QWidget*> m_WidgetList;
    QFileConversion* m_FileConversion;
    QThread* m_WorkerThread;


    FileConversionWidget(const FileConversionWidget&); // Copy Constructor Not Implemented
    void operator=(const FileConversionWidget&); // Operator '=' Not Implemented
};

#endif /* FileConversionWIDGET_H_ */
