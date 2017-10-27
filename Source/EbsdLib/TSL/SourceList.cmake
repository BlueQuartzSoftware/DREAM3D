# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#-- Get the TSL Sources
set(TSL_SRCS
    ${EbsdLib_SOURCE_DIR}/TSL/AngReader.cpp
    ${EbsdLib_SOURCE_DIR}/TSL/AngPhase.cpp
    ${EbsdLib_SOURCE_DIR}/TSL/AngFields.cpp
    )

set(TSL_HDRS
    ${EbsdLib_SOURCE_DIR}/TSL/AngConstants.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngReader.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngPhase.h
    ${EbsdLib_SOURCE_DIR}/TSL/AngFields.h
)

if(EbsdLib_ENABLE_HDF5)
    set(TSL_SRCS ${TSL_SRCS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.cpp
        ${EbsdLib_SOURCE_DIR}/TSL/H5OIMReader.cpp
    )
    set(TSL_HDRS ${TSL_HDRS}
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngImporter.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngReader.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5AngVolumeReader.h
        ${EbsdLib_SOURCE_DIR}/TSL/H5OIMReader.h
    )
endif()

cmp_IDE_SOURCE_PROPERTIES( "TSL" "${TSL_HDRS}" "${TSL_SRCS}" ${PROJECT_INSTALL_HEADERS})

if( ${EbsdLib_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${TSL_HDRS}
            DESTINATION include/EbsdLib/TSL
            COMPONENT Headers   )
endif()

