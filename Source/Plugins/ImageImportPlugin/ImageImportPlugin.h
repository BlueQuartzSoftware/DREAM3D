/*
 * Your License or Copyright Information can go here
 */

#ifndef _ImageImportPlugin_H_
#define _ImageImportPlugin_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "QtSupport/DREAM3DPluginInterface.h"


/**
 * @class ImageImportPlugin ImageImportPlugin.h SurfaceMeshing/ImageImportPlugin.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 10, 2012
 * @version 1.0
 */
class ImageImportPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT;
    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    ImageImportPlugin();
    virtual ~ImageImportPlugin();
    /**
     * @brief Returns the name of the plugin
     */
    virtual QString getPluginName();

    /**
     * @brief Register all the filters with the FilterWidgetFactory
     */
    virtual void registerFilterWidgets();

    /**
     * @brief registerFilters
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
    ImageImportPlugin(const ImageImportPlugin&); // Copy Constructor Not Implemented
    void operator=(const ImageImportPlugin&); // Operator '=' Not Implemented
};

#endif /* _ImageImportPlugin_H_ */
