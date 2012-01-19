/* Insert your Open Source license Choice, Distribution Statement or
* any other statement that dictates the type of distribution this
* plugin code can have.
*/


#ifndef PipelineBuilderPlugin_H_
#define PipelineBuilderPlugin_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include "QtSupport/DREAM3DPluginInterface.h"

class PipelineBuilderWidget;
class DREAM3DPluginFrame;

DREAM3D_PLUGIN_CONSTANTS(PipelineBuilder, PipelineBuilder)
/**
 * @class PipelineBuilderPlugin PipelineBuilderPlugin.h UIPlugins/PipelineBuilderPlugin.h
 * @brief
 * @author 
 * @date 
 * @version 1.0
 */
class PipelineBuilderPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    PipelineBuilderPlugin();
    virtual ~PipelineBuilderPlugin();

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
    PipelineBuilderWidget* m_InputWidget;

  private:
    PipelineBuilderPlugin(const PipelineBuilderPlugin&); // Copy Constructor Not Implemented
    void operator=(const PipelineBuilderPlugin&); // Operator '=' Not Implemented
};

#endif /* PipelineBuilderPlugin_H_ */
