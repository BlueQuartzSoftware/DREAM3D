/* Insert your Open Source license Choice, Distribution Statement or
* any other statement that dictates the type of distribution this
* plugin code can have.
*/


#ifndef VolumeOpsPlugin_H_
#define VolumeOpsPlugin_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include "QtSupport/DREAM3DPluginInterface.h"

class VolumeOpsWidget;
class DREAM3DPluginFrame;

DREAM3D_PLUGIN_CONSTANTS(VolumeOps, VolumeOps)
/**
 * @class VolumeOpsPlugin VolumeOpsPlugin.h UIPlugins/VolumeOpsPlugin.h
 * @brief
 * @author 
 * @date 
 * @version 1.0
 */
class VolumeOpsPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    VolumeOpsPlugin();
    virtual ~VolumeOpsPlugin();

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
    VolumeOpsWidget* m_InputWidget;

  private:
    VolumeOpsPlugin(const VolumeOpsPlugin&); // Copy Constructor Not Implemented
    void operator=(const VolumeOpsPlugin&); // Operator '=' Not Implemented
};

#endif /* VolumeOpsPlugin_H_ */
