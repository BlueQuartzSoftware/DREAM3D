/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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



#ifndef MicREADER_H_
#define MicREADER_H_

#include <string>
#include <map>


#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdReader.h"
#include "MicConstants.h"
#include "MicHeaderEntry.h"
#include "MicPhase.h"



/**
* @class MicReader MicReader.h EbsdLib/HEDM/MicReader.h
* @brief This class is a self contained HEDM OIM .Mic file reader and will read a
* single .Mic file and store all the data in column centric pointers.
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 1, 2010
* @version 1.0
*/
class EbsdLib_EXPORT MicReader : public EbsdReader
{
  public:
    MicReader();
    virtual ~MicReader();

    EBSD_INSTANCE_PROPERTY(std::vector<MicPhase::Pointer>, PhaseVector)

    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(XPos, X, float)
    EBSD_POINTER_PROPERTY(YPos, Y, float)
    EBSD_POINTER_PROPERTY(Confidence, Conf, float)
    EBSD_POINTER_PROPERTY(Phase, Phase, int)
    EBSD_POINTER_PROPERTY(Level, Level, int)
    EBSD_POINTER_PROPERTY(Up, Up, int)

    /**
     * @brief Returns the pointer to the data for a given field
     * @param fieldName The name of the field to return the pointer to.
     */
    void* getPointerByName(const std::string &fieldName);

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param fieldName The name of the field.
     */
    Ebsd::NumType getPointerType(const std::string &fieldName);

    /**
    * @brief Reads the complete HEDM .Mic file.
    * @return 1 on success
    */
    virtual int readFile();

    /**
    * @brief Reads ONLY the header portion of the HEDM .Mic file
    * @return 1 on success
    */
    virtual int readHeaderOnly();

	/** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    virtual void initPointers(size_t numElements);

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    virtual void deletePointers();

    virtual int getXDimension();
    virtual int getYDimension();
    virtual void setXDimension(int xD);
    virtual void setYDimension(int yD);
    virtual float getXStep();
    virtual float getYStep();

    virtual void transformData();

protected:


private:
    MicPhase::Pointer   m_CurrentPhase;

	int xDim;
	int yDim;
	float xRes;
	float yRes;

    int readData(std::ifstream &in, char* buf, size_t bufSize);

    /** @brief Parses the value from a single line of the header section of the HEDM .Mic file
    * @param line The line to parse
    */
    void parseHeaderLine(char* buf, size_t length);
	
	/** @brief Parses the data from a line of data from the HEDM .Mic file
    * @param line The line of data to parse
    */
    void parseDataLine(const std::string &line, size_t i);

    MicReader(const MicReader&);    // Copy Constructor Not Implemented
    void operator=(const MicReader&);  // Operator '=' Not Implemented

};

#endif /* MicREADER_H_ */
