/* Insert your Open Source license Choice, Distribution Statement or
* any other statement that dictates the type of distribution this
* plugin code can have.
*/


#ifndef FileConversionPlugin_H_
#define FileConversionPlugin_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include "QtSupport/DREAM3DPluginInterface.h"

class FileConversionWidget;
class AIMPluginFrame;

DREAM3D_PLUGIN_CONSTANTS(FileConversion, FileConversion)
/**
 * @class FileConversionPlugin FileConversionPlugin.h UIPlugins/FileConversionPlugin.h
 * @brief
 * @author 
 * @date 
 * @version 1.0
 */
class FileConversionPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    FileConversionPlugin();
    virtual ~FileConversionPlugin();

    QString getPluginName();

    virtual QWidget* getInputWidget(QWidget* parent);
    virtual DREAM3DPluginFrame* getPluginFrame(QWidget* parent);


    virtual void writeSettings(QSettings &prefs);
    virtual void readSettings(QSettings &prefs);

    virtual QIcon icon();
    virtual QUrl htmlHelpIndexFile();

  public slots:
    virtual void displayHelp();

    signals:
      void showHelp(QUrl);
    
  protected:
    FileConversionWidget* m_InputWidget;

  private:
    FileConversionPlugin(const FileConversionPlugin&); // Copy Constructor Not Implemented
    void operator=(const FileConversionPlugin&); // Operator '=' Not Implemented
};

#endif /* FileConversionPlugin_H_ */
