/* ============================================================================
 * Copyright (c) 2021-2021 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

namespace Sampling
{

/* clang-format off */
/**
 * @brief RenumberFeatures
 * @param filter
 * @param newDataContainerPath
 * @param cellAttributeMatrixPath
 * @param cellFeatureAttributeMatrixPath
 * @param featureIdsArrayPath
 * @param saveAsNewDataContainer
 */
static void RenumberFeatures(AbstractFilter* filter, 
              const DataArrayPath& newDataContainerPath, 
              const DataArrayPath& cellAttributeMatrixPath, 
              const DataArrayPath& cellFeatureAttributeMatrixPath,
              const DataArrayPath& featureIdsArrayPath,
              bool saveAsNewDataContainer)
/* clang-format on */
{
  DataContainerArray::Pointer dca = filter->getDataContainerArray();

  // Get the resampled data container, by default it would have been inserted into the Data Container Array
  DataContainer::Pointer destCellDataContainer = dca->getDataContainer(cellAttributeMatrixPath);
  // This just sanity checks to make sure there were existing features before the cropping
  AttributeMatrix::Pointer destCellFeatureAttrMat = destCellDataContainer->getAttributeMatrix(cellFeatureAttributeMatrixPath.getAttributeMatrixName());

  // Unless the user selected to create a new one
  if(saveAsNewDataContainer)
  {
    destCellDataContainer = dca->getDataContainer(newDataContainerPath);
    destCellFeatureAttrMat = destCellDataContainer->getAttributeMatrix(cellFeatureAttributeMatrixPath.getAttributeMatrixName());
  }

  if(nullptr == destCellFeatureAttrMat)
  {
    filter->setErrorCondition(-610, QString("The Cell Feature Attribute Matrix '%1' was not found.").arg(cellFeatureAttributeMatrixPath.serialize("/")));
    return;
  }

  size_t totalPoints = destCellDataContainer->getGeometryAs<ImageGeom>()->getNumberOfElements();

  size_t totalFeatures = destCellFeatureAttrMat->getNumberOfTuples();
  QVector<bool> activeObjects(totalFeatures, false);
  if(0 == totalFeatures)
  {
    filter->setErrorCondition(-600, "The number of Features is 0 and should be greater than 0");
    return;
  }

  // std::vector<size_t> cDims(1, 1);
  DataArrayPath dap = featureIdsArrayPath;

  dap.setDataContainerName(destCellDataContainer->getName());
  AttributeMatrix::Pointer destCellAttrMat = destCellDataContainer->getAttributeMatrix(cellAttributeMatrixPath.getAttributeMatrixName());
  Int32ArrayType::Pointer destFeatureIdsPtr = destCellAttrMat->getAttributeArrayAs<Int32ArrayType>(dap.getDataArrayName());
  if(nullptr == destFeatureIdsPtr)
  {
    QString ss = QObject::tr("The FeatureIds array with name '%1' was not found in the destination DataContainer. The expected path was '%2'").arg(dap.getDataArrayName()).arg(dap.serialize("/"));
    filter->setErrorCondition(-601, ss);
    return;
  }

  Int32ArrayType& featureIds = *(destFeatureIdsPtr);
  // Find the unique set of feature ids
  for(size_t i = 0; i < totalPoints; ++i)
  {
    if(filter->getCancel())
    {
      break;
    }

    int32_t currentFeatureId = featureIds[i];
    if(currentFeatureId < 0)
    {
      QString ss = QString("FeatureIds values MUST be >= ZERO. Negative FeatureId found at index %1 into the resampled feature ids array").arg(i);
      filter->setErrorCondition(-605, ss);
    }
    if(static_cast<size_t>(currentFeatureId) < totalFeatures)
    {
      activeObjects[currentFeatureId] = true;
    }
    else
    {
      QString ss = QObject::tr("The total number of Features from %1 is %2, but a value of %3 was found in DataArray %4.")
                       .arg(destCellFeatureAttrMat->getName())
                       .arg(totalFeatures)
                       .arg(currentFeatureId)
                       .arg(featureIdsArrayPath.serialize("/"));
      qDebug() << ss;
      filter->setErrorCondition(-602, ss);
      return;
    }
  }
  destCellFeatureAttrMat->removeInactiveObjects(activeObjects, destFeatureIdsPtr.get());
}

} // namespace Sampling
