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
set(SUBDIR_NAME FilterParameters)

set(${PLUGIN_NAME}_Parameters_Moc_HDRS
  
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
QT5_WRAP_CPP( ${PLUGIN_NAME}_Parameters_Generated_MOC_SRCS ${${PLUGIN_NAME}_Parameters_Moc_HDRS})
set_source_files_properties( ${${PLUGIN_NAME}_Parameters_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${${PLUGIN_NAME}_Parameters_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(${PLUGIN_NAME}_Parameters_HDRS
  ${EMMPM_SOURCE_DIR}/${SUBDIR_NAME}/EMMPMFilterParameter.h
)

set(${PLUGIN_NAME}_Parameters_SRCS
  ${EMMPM_SOURCE_DIR}/${SUBDIR_NAME}/EMMPMFilterParameter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${${PLUGIN_NAME}_Parameters_HDRS};${${PLUGIN_NAME}_Parameters_Moc_HDRS}" "${${PLUGIN_NAME}_Parameters_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${${PLUGIN_NAME}_Parameters_Generated_MOC_SRCS}" "0")


set(${PLUGIN_NAME}_Parameters_HDRS
  ${${PLUGIN_NAME}_Parameters_HDRS}
  ${${PLUGIN_NAME}_Parameters_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(${PLUGIN_NAME}_Parameters_SRCS
  ${${PLUGIN_NAME}_Parameters_SRCS}
  ${${PLUGIN_NAME}_Parameters_Generated_MOC_SRCS}
)


if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${${PLUGIN_NAME}_Parameters_HDRS}
                   ${${PLUGIN_NAME}_Parameters_Moc_HDRS}
            DESTINATION include/EMMPM/${SUBDIR_NAME}
            COMPONENT Headers   )
endif()
