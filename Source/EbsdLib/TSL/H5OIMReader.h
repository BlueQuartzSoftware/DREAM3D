/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _h5oimreader_h_
#define _h5oimreader_h_

#include <hdf5.h>

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QSet>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

#include "AngReader.h"
#include "AngPhase.h"

/**
 * @class H5OIMReader H5OIMReader.h EbsdLib/TSL/H5OIMReader.h
 * @brief
 *
 * @date Mar 24, 2011
 * @version 1.0
 *
 * All the documentation for this needs to be updated to reflect information from
 * EDAX on the correct layout of their HDF5 file.
 *
 * @li Phase
 * @li MaterialName
 * @li Formula
 * @li Info
 * @li Symmetry
 * @li LatticeConstants
 * @li NumberHKLFamilies
 *
 * The HKLFamilies form another sub hdf group within the Phase group. This folder
 * contains datasets where each data set holds a single HKLFamily from the Phase
 * data. The dataset is an HDF5 Compound Data Type where you have the following
 * layout
 * @code
 * typedef struct
{
    int h;
    int k;
    int l;
    int s1;
    float diffractionIntensity;
    int s2;
} HKLFamily_t;
 * @endcode
 *
 * After reading the HDF5 file one can use the various methods to extract the phases
 * or other header information or all the columnar data.
 */
class EbsdLib_EXPORT H5OIMReader : public AngReader
{
  public:
    EBSD_SHARED_POINTERS(H5OIMReader)
    EBSD_STATIC_NEW_MACRO(H5OIMReader)
    EBSD_TYPE_MACRO(H5OIMReader)
    virtual ~H5OIMReader();

    /**
     * @brief The HDF5 path to find the EBSD data
     */
    EBSD_INSTANCE_STRING_PROPERTY(HDF5Path)
    EBSD_INSTANCE_PROPERTY(bool, ReadPatternData)
    EBSD_INSTANCE_PROPERTY(uint8_t*, PatternData)
    EBSD_INSTANCE_2DVECTOR_PROPERTY(int, PatternDims)

    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumColumns, Ebsd::Ang::nColumns)
    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumRows, Ebsd::Ang::nRows)
    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, XStep, Ebsd::Ang::StepX)
    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, YStep, Ebsd::Ang::StepY)
    EbsdHeader_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, Grid, Ebsd::Ang::GridType)
    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, PatternWidth, Ebsd::Ang::PatternWidth)
    EbsdHeader_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, PatternHeight, Ebsd::Ang::PatternHeight)


    /**
     * @brief Reads the file
     * @return error condition
     */
    virtual int readFile();

    /**
     * @brief readScanNames
     * @return
     */
    virtual int readScanNames(QStringList& names);

    /**
     * @brief Reads the header section of the file
     * @param Valid HDF5 Group ID
     * @return error condition
     */
    int readHeader(hid_t parId);

    /**
    * @brief Reads ONLY the header portion of the TSL .ang file
    * @return 1 on success
    */
    virtual int readHeaderOnly();

    /**
     * @brief Returns a vector of AngPhase objects corresponding to the phases
     * present in the file
     */
    // QVector<AngPhase::Pointer> getPhases() { return m_Phases; }

    /**
     * @brief Sets the names of the arrays to read out of the file
     * @param names
     */
    virtual void setArraysToRead(QSet<QString> names);

    /**
     * @brief Over rides the setArraysToReads to tell the reader to load ALL the data from the HDF5 file. If the
     * ArrayNames to read is empty and this is true then all arrays will be read.
     * @param b
     */
    virtual void readAllArrays(bool b);

    virtual int getXDimension();
    virtual void setXDimension(int xdim);
    virtual int getYDimension();
    virtual void setYDimension(int ydim);

  protected:
    H5OIMReader();

    /**
     * @brief Reads the data associated with HKL Families for a given phase.
     * @param hklGid Valid HDF5 Group ID where the HKL Family data is located.
     * @param phase The AngPhase to parse the HKL Family data
     */
    int readHKLFamilies(hid_t hklGid, AngPhase::Pointer phase);

    /**
     * @brief Reads the data section of the file
     * @param Valid HDF5 Group ID
     * @return error condition
     */
    int readData(hid_t parId);

  private:
    // QVector<AngPhase::Pointer> m_Phases;
    QSet<QString>         m_ArrayNames;
    bool                  m_ReadAllArrays;

    H5OIMReader(const H5OIMReader&); // Copy Constructor Not Implemented
    void operator=(const H5OIMReader&); // Operator '=' Not Implemented
};

#endif /* _H5OIMReader_H_ */

