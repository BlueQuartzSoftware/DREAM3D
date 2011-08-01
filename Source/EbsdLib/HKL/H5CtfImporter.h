/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef H5CTFIMPORTER_H_
#define H5CTFIMPORTER_H_

#include "hdf5.h"

#include <vector>
#include <string>

#include "EbsdLib/EbsdLibTypes.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "CtfReader.h"

/**
 * @class H5CtfImporter H5CtfImporter.h AIM/ANG/H5CtfImporter.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date March 23, 2011
 * @version 1.2
 *
 */
class EbsdLib_EXPORT H5CtfImporter
{

  public:
    EBSD_SHARED_POINTERS(H5CtfImporter)
    EBSD_TYPE_MACRO(H5CtfImporter)
    EBSD_STATIC_NEW_MACRO(H5CtfImporter)


    virtual ~H5CtfImporter();

    EBSD_INSTANCE_STRING_PROPERTY(ErrorMessage);
    EBSD_INSTANCE_PROPERTY(bool, ErrorCondition);


    /**
     * @brief Cancel the operation
     */
    EBSD_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the Reconstruction normalized to a value between 0 and 100
     */
    void progressMessage(const std::string &message, int progress);


    int importFile(hid_t fileId, int index, const std::string &angFile);

    int writePhaseData(CtfReader &reader, hid_t gid);

  protected:
    H5CtfImporter();


  private:
    H5CtfImporter(const H5CtfImporter&); // Copy Constructor Not Implemented
    void operator=(const H5CtfImporter&); // Operator '=' Not Implemented
};

#endif /* H5CTFIMPORTER_H_ */
