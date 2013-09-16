/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ArraySelectionWidget_H_
#define _ArraySelectionWidget_H_

#include <QtCore/QSettings>
#include <QtGui/QTabWidget>

#include "ui_ArraySelectionWidget.h"

#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"
#include "DREAM3DLib/DataContainers/VertexDataContainer.h"

#include "PipelineBuilder/PipelineBuilderDLLExport.h"

/**
 * @class ArraySelectionWidget ArraySelectionWidget.h PipelineBuilder/UI/ArraySelectionWidget.h
 * @brief This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderLib_EXPORT ArraySelectionWidget : public QTabWidget, private Ui::ArraySelectionWidget
{

    Q_OBJECT
  public:
    ArraySelectionWidget(QWidget* parent = 0);
    virtual ~ArraySelectionWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    /**
     * @brief populateArrayNamesl
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void populateArrayNames(VolumeDataContainer::Pointer vldc,
                                    SurfaceDataContainer::Pointer sdc,
                                    EdgeDataContainer::Pointer edc,
                                    VertexDataContainer::Pointer vdc);
    /**
     * @brief This method examines the selections in each of the array lists and if an array is selected
     * it is removed from the data container.
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void removeSelectionsFromDataContainers(VolumeDataContainer::Pointer vldc,
                                    SurfaceDataContainer::Pointer sdc,
                                    EdgeDataContainer::Pointer edc,
                                    VertexDataContainer::Pointer vdc);

    /**
     * @brief This method examines the selections in each of the array lists and if an array is <b>NOT</b> selected
     * it is removed from the data container.
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void removeNonSelectionsFromDataContainers(VolumeDataContainer::Pointer vldc,
                                    SurfaceDataContainer::Pointer sdc,
                                    EdgeDataContainer::Pointer edc,
                                    VertexDataContainer::Pointer vdc);

    /**
     * @brief
     */
    template<typename Filter>
    void getArraySelections(Filter* filter)
    {
      filter->setVolumeSelectedArrayNames( getSelectedArrays(volumeVertexArrayList),
                                                getSelectedArrays(volumeFaceArrayList),
                                                getSelectedArrays(volumeEdgeArrayList),
                                                getSelectedArrays(volumeCellArrayList),
                                                getSelectedArrays(volumeCellFieldArrayList),
                                                getSelectedArrays(volumeCellEnsembleArrayList));
      filter->setSurfaceSelectedArrayNames( getSelectedArrays(surfaceVertexArrayList),
                                                getSelectedArrays(surfaceFaceArrayList),
                                                getSelectedArrays(surfaceEdgeArrayList),
                                                getSelectedArrays(surfaceFaceFieldArrayList),
                                                getSelectedArrays(surfaceFaceEnsembleArrayList));
      filter->setEdgeSelectedArrayNames( getSelectedArrays(edgeVertexArrayList),
                                                getSelectedArrays(edgeEdgeArrayList),
                                                getSelectedArrays(edgeEdgeFieldArrayList),
                                                getSelectedArrays(edgeEdgeEnsembleArrayList));
      filter->setVertexSelectedArrayNames( getSelectedArrays(vertexVertexArrayList),
                                                getSelectedArrays(vertexVertexFieldArrayList),
                                                getSelectedArrays(vertexVertexEnsembleArrayList));
    }

    template<typename Filter>
    void setArraySelections(Filter* filter)
    {
      setSelectedArrays( filter->getSelectedVolumeVertexArrays(), volumeVertexArrayList );
      setSelectedArrays( filter->getSelectedVolumeEdgeArrays(), volumeEdgeArrayList );
      setSelectedArrays( filter->getSelectedVolumeFaceArrays(), volumeFaceArrayList );
      setSelectedArrays( filter->getSelectedVolumeCellArrays(), volumeCellArrayList );
      setSelectedArrays( filter->getSelectedVolumeCellFieldArrays(), volumeCellFieldArrayList );
      setSelectedArrays( filter->getSelectedVolumeCellEnsembleArrays(), volumeCellEnsembleArrayList );
      setSelectedArrays( filter->getSelectedSurfaceVertexArrays(), surfaceVertexArrayList );
      setSelectedArrays( filter->getSelectedSurfaceFaceArrays(), surfaceFaceArrayList );
      setSelectedArrays( filter->getSelectedSurfaceEdgeArrays(), surfaceEdgeArrayList );
      setSelectedArrays( filter->getSelectedSurfaceFaceFieldArrays(), surfaceFaceFieldArrayList );
      setSelectedArrays( filter->getSelectedSurfaceFaceEnsembleArrays(), surfaceFaceEnsembleArrayList );
      setSelectedArrays( filter->getSelectedEdgeVertexArrays(), edgeVertexArrayList );
      setSelectedArrays( filter->getSelectedEdgeEdgeArrays(), edgeEdgeArrayList );
      setSelectedArrays( filter->getSelectedEdgeEdgeFieldArrays(), edgeEdgeFieldArrayList );
      setSelectedArrays( filter->getSelectedEdgeEdgeEnsembleArrays(), edgeEdgeEnsembleArrayList );
      setSelectedArrays( filter->getSelectedVertexVertexArrays(), vertexVertexArrayList );
      setSelectedArrays( filter->getSelectedVertexVertexFieldArrays(), vertexVertexFieldArrayList );
      setSelectedArrays( filter->getSelectedVertexVertexEnsembleArrays(), vertexVertexEnsembleArrayList );
    }

    /**
     * @brief writeOptions
     * @param prefs
     */
    virtual void readOptions(QSettings &prefs, QString name);

    /**
     * @brief writeOptions
     * @param prefs
     */
    virtual void writeOptions(QSettings &prefs, QString name);

    /**
     * @brief setVoxelDataEnabled Enable/Disable the Voxel Tab
     * @param b
     */
    virtual void setVolumeDataEnabled(bool b);

    /**
     * @brief setSurfaceMeshEnabled Enable/Disable the Surface Mesh Tab
     * @param b
     */
    virtual void setSurfaceEnabled(bool b);

    /**
     * @brief setEdgeMeshEnabled Enable/Disable the Surface Mesh Tab
     * @param b
     */
    virtual void setEdgeEnabled(bool b);

    /**
     * @brief setVertexMeshEnabled Enable/Disable the solid mesh tab
     * @param b
     */
    virtual void setVertexEnabled(bool b);

    /**
     * @brief setVoxelDataEnabled Show the Voxel Tab
     * @param b
     */
    virtual void removeVolumeData();

    /**
     * @brief setSurfaceMeshEnabled Show the Surface Mesh Tab
     * @param b
     */
    virtual void removeSurfaceData();

    /**
     * @brief setEdgeMeshEnabled Show the Surface Mesh Tab
     * @param b
     */
    virtual void removeEdgeData();

    /**
     * @brief setSolidMeshEnabled Show the solid mesh tab
     * @param b
     */
    virtual void removeVertexData();

  signals:
    void arrayListsChanged();

  protected slots:
    void arrayListUpdated(QListWidgetItem* item);

    void on_volumeVertexCB_stateChanged(int state);
    void on_volumeEdgeCB_stateChanged(int state);
    void on_volumeFaceCB_stateChanged(int state);
    void on_volumeCellCB_stateChanged(int state);
    void on_volumeCellFieldCB_stateChanged(int state);
    void on_volumeCellEnsembleCB_stateChanged(int state);

    void on_surfaceVertexArraysCB_stateChanged(int state);
    void on_surfaceFaceArraysCB_stateChanged(int state);
    void on_surfaceEdgeArraysCB_stateChanged(int state);
    void on_surfaceFaceFieldArraysCB_stateChanged(int state);
    void on_surfaceFaceEnsembleArraysCB_stateChanged(int state);

    void on_edgeVertexCB_stateChanged(int state);
    void on_edgeEdgeCB_stateChanged(int state);
    void on_edgeEdgeFieldCB_stateChanged(int state);
    void on_edgeEdgeEnsembleCB_stateChanged(int state);

    void on_vertexVertexCB_stateChanged(int state);
    void on_vertexVertexFieldCB_stateChanged(int state);
    void on_vertexVertexEnsembleCB_stateChanged(int state);

  protected:

    void toggleListSelections(QListWidget* w, int state);

    virtual void populateVolumeArrayNames(VolumeDataContainer::Pointer vldc);
    virtual void populateSurfaceArrayNames(SurfaceDataContainer::Pointer sdc);
    virtual void populateEdgeArrayNames(EdgeDataContainer::Pointer edc);
    virtual void populateVertexArrayNames(VertexDataContainer::Pointer vdc);

    virtual void populateArrayList(QListWidget* listWidget, QStringList &arrayNames, QCheckBox *cb = NULL);
    virtual void populateArrayList(QListWidget* listWidget,
                                    QList<QString> &arrayNames, QCheckBox *cb = NULL);

    /**
     * @brief Returns the list of selected Array Names
     */
    virtual QSet<QString> getSelectedArrays(QListWidget* listWidget);
    virtual void setSelectedArrays(QSet<QString> names, QListWidget* listWidget);
    virtual QSet<QString> getNonSelectedArrays(QListWidget* listWidget);
    virtual void clearArraySelectionLists();

    void writeSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget);
    void readSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget);
    void setSelections(QListWidget* listWidget, QStringList &selections);

  private:
    ArraySelectionWidget(const ArraySelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const ArraySelectionWidget&); // Operator '=' Not Implemented

};




#endif /* _ArraySelectionWidget_H_ */
