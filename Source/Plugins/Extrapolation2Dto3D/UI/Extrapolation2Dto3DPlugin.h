/* Insert your Open Source license Choice, Distribution Statement or
* any other statement that dictates the type of distribution this
* plugin code can have.
*/


#ifndef Extrapolation2Dto3DPlugin_H_
#define Extrapolation2Dto3DPlugin_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include "QtSupport/DREAM3DPluginInterface.h"

class Extrapolation2Dto3DWidget;
class AIMPluginFrame;

DREAM3D_PLUGIN_CONSTANTS(Extrapolation2Dto3D, Extrapolation2Dto3D)
/**
 * @class Extrapolation2Dto3DPlugin Extrapolation2Dto3DPlugin.h UIPlugins/Extrapolation2Dto3DPlugin.h
 * @brief
 * @author 
 * @date 
 * @version 1.0
 */
class Extrapolation2Dto3DPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    Extrapolation2Dto3DPlugin();
    virtual ~Extrapolation2Dto3DPlugin();

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
    Extrapolation2Dto3DWidget* m_InputWidget;

  private:
    Extrapolation2Dto3DPlugin(const Extrapolation2Dto3DPlugin&); // Copy Constructor Not Implemented
    void operator=(const Extrapolation2Dto3DPlugin&); // Operator '=' Not Implemented
};

#endif /* Extrapolation2Dto3DPlugin_H_ */
