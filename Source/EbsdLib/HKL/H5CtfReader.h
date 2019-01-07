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

#pragma once

#include <hdf5.h>

#include <QtCore/QString>
#include <QtCore/QSet>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/HKL/CtfPhase.h"

/**
 * @class H5CtfReader H5CtfReader EbsdLib/HKL/H5CtfReader.h
 * @brief
 *
 * @date Aug 2, 2011
 * @version 1.0
 *
 * HKL EBSD Scans are stored in the HDF5 file into a heirarchy of folders where
 * each folder stores an individual .ctf file. The folder has a simple index as
 * its name, for example if the index of the .ctf file was 11 then the HDF folder
 * that contains that .ctf file's data has an HDF Group name of "11". Within this
 * folder there are 2 more folders: <b>Data</b> and <b>Header</b>.\n
 *   The <b>Data</b> folder contains all the columner data from the .ang file where
 * each column is its own HDF data set. The <b>Header</b> folder contains the complete
 * header data from the .ctf file as either data sets or as subfolders. The dataset
 * with the name <b>OriginalFile</b> contains the unchanged complete header from
 * the .ctf file in a single string HDF data set. Each of the header keys has been
 * also saved to its own HDF data set which can be seen with any HDF5 data viewer
 * application. For the Phase data inside the header a new HDF group is created
 * called <b>Phases</b> which contains each of the phase data. Again each phase
 * has an HDF5 group where its data is collected with a simple numerical index
 * as its name. Within each Phase's HDF Group folder are the various data sets
 * that make up the Phase data.
 *
 * After reading the HDF5 file one can use the various methods to extract the phases
 * or other header information or all the columnar data.
 */
class EbsdLib_EXPORT H5CtfReader : public CtfReader
{
  public:
    EBSD_SHARED_POINTERS(H5CtfReader)
    EBSD_STATIC_NEW_MACRO(H5CtfReader)
    EBSD_TYPE_MACRO_SUPER(H5CtfReader, CtfReader)
    ~H5CtfReader() override;

    /**
     * @brief The HDF5 path to find the EBSD data
     */
    EBSD_INSTANCE_STRING_PROPERTY(HDF5Path)

    EBSD_POINTER_PROPERTY(Phase, Phase, int)
    EBSD_POINTER_PROPERTY(X, X, float)
    EBSD_POINTER_PROPERTY(Y, Y, float)
    EBSD_POINTER_PROPERTY(Z, Z, float)
    EBSD_POINTER_PROPERTY(BandCount, Bands, int)
    EBSD_POINTER_PROPERTY(Error, Error, int)
    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(MeanAngularDeviation, MAD, float)
    EBSD_POINTER_PROPERTY(BandContrast, BC, int)
    EBSD_POINTER_PROPERTY(BandSlope, BS, int)

    /* These will be in a 3D ctf file */
    EBSD_POINTER_PROPERTY(GrainIndex, GrainIndex, int)
    EBSD_POINTER_PROPERTY(GrainRandomColourR, GrainRandomColourR, int)
    EBSD_POINTER_PROPERTY(GrainRandomColourG, GrainRandomColourG, int)
    EBSD_POINTER_PROPERTY(GrainRandomColourB, GrainRandomColourB, int)

    /**
     * @brief Reads the file
     * @return error condition
     */
    int readFile() override;

    /**
     * @brief Reads the header section of the file
     * @param Valid HDF5 Group ID
     * @return error condition
     */
    int readHeader(hid_t parId);

    /**
    * @brief Reads ONLY the header portion of the HKL .ctf file
    * @return 1 on success
    */
    int readHeaderOnly() override;

    /**
     * @brief Returns a vector of AngPhase objects corresponding to the phases
     * present in the file
     */
    QVector<CtfPhase::Pointer> getPhases() { return m_Phases; }

    /**
     * @brief Sets the names of the arrays to read out of the file
     * @param names
     */
    void setArraysToRead(const QSet<QString>& names);

    /**
     * @brief Over rides the setArraysToReads to tell the reader to load ALL the data from the HDF5 file. If the
     * ArrayNames to read is empty and this is true then all arrays will be read.
     * @param b
     */
    void readAllArrays(bool b);

  protected:
    H5CtfReader();

    /**
     * @brief Reads the data section of the file
     * @param Valid HDF5 Group ID
     * @return error condition
     */
    int readData(hid_t parId);

  private:
    QVector<CtfPhase::Pointer> m_Phases;
    QSet<QString> m_ArrayNames;
    bool                  m_ReadAllArrays;

  public:
    H5CtfReader(const H5CtfReader&) = delete;    // Copy Constructor Not Implemented
    H5CtfReader(H5CtfReader&&) = delete;         // Move Constructor Not Implemented
    H5CtfReader& operator=(const H5CtfReader&) = delete; // Copy Assignment Not Implemented
    H5CtfReader& operator=(H5CtfReader&&) = delete;      // Move Assignment Not Implemented
};


