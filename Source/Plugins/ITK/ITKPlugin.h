/*
 * Your License or Copyright Information can go here
 */

#ifndef _ITK_H_
#define _ITK_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "QtSupport/DREAM3DPluginInterface.h"


/**
 * @class ITKPlugin ITKPlugin.h SurfaceMeshing/ITKPlugin.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 10, 2012
 * @version 1.0
 */
class ITKPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    ITKPlugin();
    virtual ~ITKPlugin();
    /**
     * @brief Returns the name of the plugin
     */
    virtual QString getPluginName();

    /**
     * @brief Register all the filters with the FilterWidgetFactory
     */
    virtual void registerFilterWidgets();

    /**
    * @brief Registers all the filters that this plugin provides
    */
    virtual void registerFilters(FilterManager* fm);


    /**
     * @brief Writes the settings in the input gui to the Application's preference file
     * @param prefs A valid QSettings pointer.
     */
    virtual void writeSettings(QSettings &prefs);

    /**
     * @brief Reads the settings from the Application's preference file and sets
     * the input GUI widgets accordingly.
     * @param prefs
     */
    virtual void readSettings(QSettings &prefs);

  private:
    ITKPlugin(const ITKPlugin&); // Copy Constructor Not Implemented
    void operator=(const ITKPlugin&); // Operator '=' Not Implemented
};

#endif /* _ITK_H_ */
