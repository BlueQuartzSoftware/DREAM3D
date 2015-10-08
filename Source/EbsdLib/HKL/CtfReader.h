/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _CTFREADER_H_
#define _CTFREADER_H_

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QtDebug>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdReader.h"
#include "CtfConstants.h"
#include "CtfHeaderEntry.h"
#include "CtfPhase.h"
#include "DataParser.hpp"


/**
* @class CtfReader CtfReader.h EbsdLib/HKL/CtfReader.h
* @brief This class is a self contained HKL .ctf file reader and will read a
* single .ctf file and store all the data in column centric pointers.
* @author Michael A. Jackson for BlueQuartz Software
* @date Aug 1, 2011
* @version 1.0
*/
class EbsdLib_EXPORT CtfReader : public EbsdReader
{
  public:
    CtfReader();
    virtual ~CtfReader();

    EBSD_TYPE_MACRO_SUPER(CtfReader, EbsdReader)

    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, QString, Channel, Ebsd::Ctf::ChannelTextFile)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, QString, Prj, Ebsd::Ctf::Prj)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, QString, Author, Ebsd::Ctf::Author)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, QString, JobMode, Ebsd::Ctf::JobMode)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, XCells, Ebsd::Ctf::XCells)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, YCells, Ebsd::Ctf::YCells)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, ZCells, Ebsd::Ctf::ZCells)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, XStep, Ebsd::Ctf::XStep)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, YStep, Ebsd::Ctf::YStep)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, ZStep, Ebsd::Ctf::ZStep)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE1, Ebsd::Ctf::AcqE1)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE2, Ebsd::Ctf::AcqE2)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE3, Ebsd::Ctf::AcqE3)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, QString, Euler, Ebsd::Ctf::Euler)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Mag, Ebsd::Ctf::Mag)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Coverage, Ebsd::Ctf::Coverage)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Device, Ebsd::Ctf::Device)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, KV, Ebsd::Ctf::KV)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, TiltAngle, Ebsd::Ctf::TiltAngle)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, TiltAxis, Ebsd::Ctf::TiltAxis)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, NumPhases, Ebsd::Ctf::NumPhases)

    EBSD_INSTANCE_PROPERTY(QVector<CtfPhase::Pointer>, PhaseVector)

    EBSD_POINTER_PROP(Phase, Phase, int)
    EBSD_POINTER_PROP(X, X, float)
    EBSD_POINTER_PROP(Y, Y, float)
    EBSD_POINTER_PROP(Z, Z, float)
    EBSD_POINTER_PROP(BandCount, Bands, int)
    EBSD_POINTER_PROP(Error, Error, int)
    EBSD_POINTER_PROP(Euler1, Euler1, float)
    EBSD_POINTER_PROP(Euler2, Euler2, float)
    EBSD_POINTER_PROP(Euler3, Euler3, float)
    EBSD_POINTER_PROP(MeanAngularDeviation, MAD, float)
    EBSD_POINTER_PROP(BandContrast, BC, int)
    EBSD_POINTER_PROP(BandSlope, BS, int)

    /* These will be in a 3D ctf file */
    EBSD_POINTER_PROP(GrainIndex, GrainIndex, int)
    EBSD_POINTER_PROP(GrainRandomColourR, GrainRandomColourR, int)
    EBSD_POINTER_PROP(GrainRandomColourG, GrainRandomColourG, int)
    EBSD_POINTER_PROP(GrainRandomColourB, GrainRandomColourB, int)

    /**
     * @brief Returns the pointer to the data for a given feature
     * @param featureName The name of the feature to return the pointer to.
     */
    void* getPointerByName(const QString& featureName);
    void setPointerByName(const QString& name, void* p);



    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param featureName The name of the feature.
     */
    Ebsd::NumType getPointerType(const QString& featureName);
    int getTypeSize(const QString& featureName);
    DataParser::Pointer getParser(const QString& featureName, void* ptr, size_t size);

    QList<QString> getColumnNames();

    /**
    * @brief Reads the complete HKL .ctf file.
    * @return 1 on success
    */
    virtual int readFile();

    /**
    * @brief Reads ONLY the header portion of the HKL .ctf file
    * @return 1 on success
    */
    virtual int readHeaderOnly();

    virtual void readOnlySliceIndex(int slice);

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    void initPointers(size_t numElements);

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    void deletePointers();


    virtual int getXDimension();
    virtual void setXDimension(int xdim);
    virtual int getYDimension();
    virtual void setYDimension(int ydim);

    virtual void printHeader(std::ostream& out);

  protected:

  private:
    int m_SingleSliceRead;
    QMap<QString, DataParser::Pointer> m_NamePointerMap;

    /**
     * @brief
     * @param reader
     * @param headerLines
     * @return
     */
    int getHeaderLines(QFile& reader, QList<QByteArray>& headerLines);

    /**
    * Checks that the line is the header of the columns for the data.
    *
    * @param columns
    *            line values
    * @return <code>true</code> if the line is the columns header line,
    *         <code>false</code> otherwise
    */
    bool isDataHeaderLine(QVector<QString>& columns);

    /**
    *
    */
    int parseHeaderLines(QList<QByteArray>& headerLines);

    /**
       * @brief
       * @param in The input file stream to read from
       */
    int readData(QFile& in);

    /**
    * @brief Reads a line of Data from the ASCII based file
    * @param line The current line of data
    * @param row Current Row of Data
    * @param i The current index into a flat array
    * @param xCells Number of X Data Points
    * @param yCells Number of Y Data Points
    * @param col The current Column of Data
    */
    int parseDataLine(QByteArray& line, size_t row, size_t col, size_t i, size_t xCells, size_t yCells );

    CtfReader(const CtfReader&); // Copy Constructor Not Implemented
    void operator=(const CtfReader&); // Operator '=' Not Implemented
};


#endif /* _CTFREADER_H_ */

