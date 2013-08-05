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

#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/SolidMeshDataContainer.h"

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
     * @brief populateArrayNames
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void populateArrayNames(VoxelDataContainer::Pointer vdc,
                                    SurfaceMeshDataContainer::Pointer smdc,
                                    SolidMeshDataContainer::Pointer sdc);
    /**
     * @brief This method examines the selections in each of the array lists and if an array is selected
     * it is removed from the data container.
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void removeSelectionsFromDataContainers(VoxelDataContainer::Pointer vdc,
                                    SurfaceMeshDataContainer::Pointer smdc,
                                    SolidMeshDataContainer::Pointer sdc);

    /**
     * @brief This method examines the selections in each of the array lists and if an array is <b>NOT</b> selected
     * it is removed from the data container.
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void removeNonSelectionsFromDataContainers(VoxelDataContainer::Pointer vdc,
                                    SurfaceMeshDataContainer::Pointer smdc,
                                    SolidMeshDataContainer::Pointer sdc);

    /**
     * @brief
     */
    template<typename Filter>
    void getArraySelections(Filter* filter)
    {
      filter->setVoxelSelectedArrayNames( getSelectedArrays(voxelCellArrayList),
                                          getSelectedArrays(voxelFieldArrayList),
                                          getSelectedArrays(voxelEnsembleArrayList));
      filter->setSurfaceMeshSelectedArrayNames( getSelectedArrays(surfaceMeshVertexArrayList),
                                                getSelectedArrays(surfaceMeshFaceArrayList),
                                                getSelectedArrays(surfaceMeshEdgeArrayList),
                                                getSelectedArrays(surfaceMeshFieldArrayList),
                                                getSelectedArrays(surfaceMeshEnsembleArrayList));
      filter->setSolidMeshSelectedArrayNames( getSelectedArrays(solidMeshVertexArrayList),
                                              getSelectedArrays(solidMeshFaceArrayList),
                                              getSelectedArrays(solidMeshEdgeArrayList));
    }

    template<typename Filter>
    void setArraySelections(Filter* filter)
    {
      setSelectedArrays( filter->getSelectedVoxelCellArrays(), voxelCellArrayList );
      setSelectedArrays( filter->getSelectedVoxelFieldArrays(), voxelFieldArrayList );
      setSelectedArrays( filter->getSelectedVoxelEnsembleArrays(), voxelEnsembleArrayList );
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
    virtual void setVoxelDataEnabled(bool b);

    /**
     * @brief setSurfaceMeshEnabled Enable/Disable the Surface Mesh Tab
     * @param b
     */
    virtual void setSurfaceMeshEnabled(bool b);

    /**
     * @brief setSolidMeshEnabled Enable/Disable the solid mesh tab
     * @param b
     */
    virtual void setSolidMeshEnabled(bool b);

    /**
     * @brief setVoxelDataEnabled Show the Voxel Tab
     * @param b
     */
    virtual void removeVoxelData();

    /**
     * @brief setSurfaceMeshEnabled Show the Surface Mesh Tab
     * @param b
     */
    virtual void removeSurfaceMeshData();

    /**
     * @brief setSolidMeshEnabled Show the solid mesh tab
     * @param b
     */
    virtual void removeSolidMeshData();

  signals:
    void arrayListsChanged();

  protected slots:
    void arrayListUpdated(QListWidgetItem* item);

//    void on_vertexArraysCB_stateChanged(int state);
//    void on_faceArraysCB_stateChanged(int state);
//    void on_edgeArraysCB_stateChanged(int state);

    void on_voxelCellCB_stateChanged(int state);
    void on_voxelFieldCB_stateChanged(int state);
    void on_voxelEnsembleCB_stateChanged(int state);


    void on_smVertexArraysCB_stateChanged(int state);
    void on_smFaceArraysCB_stateChanged(int state);
    void on_smEdgeArraysCB_stateChanged(int state);
    void on_smFieldArraysCB_stateChanged(int state);
    void on_smEnsembleArraysCB_stateChanged(int state);

  protected:

    void toggleListSelections(QListWidget* w, int state);

    virtual void populateVoxelArrayNames(VoxelDataContainer::Pointer vdc);
    virtual void populateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer smdc);
    virtual void populateSolidMeshArrayNames(SolidMeshDataContainer::Pointer sdc);

    virtual void populateArrayList(QListWidget* listWidget, QStringList &arrayNames, QCheckBox *cb = NULL);
    virtual void populateArrayList(QListWidget* listWidget,
                                    std::list<std::string> &arrayNames, QCheckBox *cb = NULL);

    /**
     * @brief Returns the list of selected Array Names
     */
    virtual std::set<std::string> getSelectedArrays(QListWidget* listWidget);
    virtual void setSelectedArrays(std::set<std::string> set, QListWidget* listWidget);
    virtual std::set<std::string> getNonSelectedArrays(QListWidget* listWidget);
    virtual void clearArraySelectionLists();

    void writeSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget);
    void readSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget);
    void setSelections(QListWidget* listWidget, QStringList &selections);

  private:
    ArraySelectionWidget(const ArraySelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const ArraySelectionWidget&); // Operator '=' Not Implemented

};




#endif /* _ArraySelectionWidget_H_ */
