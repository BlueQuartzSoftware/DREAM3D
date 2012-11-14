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




#ifndef CTFPHASE_H_
#define CTFPHASE_H_

#include <string.h>

#include <sstream>
#include <vector>
#include <string>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"

#include "CtfConstants.h"

/**
 * @class CtfPhase CtfPhase.h EbsdLib/HKL/CtfPhase.h
 * @brief This class holds all the values for a "Phase" header block in an HKL .ctf file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT CtfPhase
{
  public:
    EBSD_SHARED_POINTERS(CtfPhase)
    EBSD_STATIC_NEW_MACRO(CtfPhase)
    EBSD_TYPE_MACRO(CtfPhase)
    virtual ~CtfPhase();

    EBSD_INSTANCE_PROPERTY(int, PhaseIndex)

    EBSD_INSTANCE_PROPERTY(std::vector<float>, LatticeDimensions) // 1x3 array
    EBSD_INSTANCE_PROPERTY(std::vector<float>, LatticeAngles) // 1x3 array
    EBSD_INSTANCE_STRING_PROPERTY(PhaseName)
    EBSD_INSTANCE_PROPERTY(Ebsd::Ctf::LaueGroupTable, LaueGroup) // <== Laue Group

    EBSD_INSTANCE_PROPERTY(int, SpaceGroup)
    EBSD_INSTANCE_STRING_PROPERTY(Internal1)
    EBSD_INSTANCE_STRING_PROPERTY(Internal2)
    EBSD_INSTANCE_STRING_PROPERTY(Comment)


    /**
     * @brief Parses a header line into a CtfPhase class
     */
    void parsePhase(const std::vector<std::string> &tokens);

    /**
     * @brief Prints some debugging info about this class
     */
    void printSelf(std::ostream &stream);

    /**
     * @brief Returns the type of crystal structure for this phase.
     */
    unsigned int determineCrystalStructure();

    std::string getMaterialName();

    /**
     * @brief Converts a string to a number
     */
    template<typename T>
    bool stringToNum(T &t, const std::string &s)
    {
      // Filter the line to convert European comma style decimals to US/UK style points
      std::vector<char> cLine(s.size()+1);
      ::memcpy( &(cLine.front()), s.c_str(), s.size() + 1);
      for (size_t c = 0; c < cLine.size(); ++c)
      {
        if (cLine[c] == ',') { cLine[c] = '.';}
      }
      std::istringstream iss(std::string( &(cLine.front()) ) );
      return !(iss >> t).fail();
    }

    /**
     * @brief Parses a header line into string "tokens"
     */
    template<typename T>
    std::vector<T> tokenize(const std::string &values, char delimiter)
    {
      std::vector<T> output;
      std::string::size_type start = 0;
      std::string::size_type pos = 0;
      while(pos != std::string::npos && pos != values.size() - 1)
      {
        pos = values.find(delimiter, start);
        T value = 0;
        stringToNum(value, values.substr(start, pos-start));
        output.push_back(value);
        if (pos != std::string::npos)
        {
          start = pos + 1;
        }
      }
      return output;
    }


  protected:
    CtfPhase();

   private:
    CtfPhase(const CtfPhase&); // Copy Constructor Not Implemented
     void operator=(const CtfPhase&); // Operator '=' Not Implemented
 };
#endif /* CTFPHASE_H_ */
