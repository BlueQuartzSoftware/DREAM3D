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

set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableData.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AxisAngleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/BooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArrayCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataBundleSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerArrayProxyFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerCreationFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerReaderFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DoubleFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicChoiceFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FileListInfoFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FourthOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersConstants.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntVec3FilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedBooleanFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedChoicesFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiDataArraySelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputFileFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputPathFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PreflightUpdatedValueFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QFilterParametersReader.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QFilterParametersWriter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SecondOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SeparatorFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeTypeSelectionFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThirdOrderPolynomialFilterParameter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/UnknownFilterParameter.h
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AttributeMatrixSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AxisAngleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/BooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArrayCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataBundleSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerArrayProxyFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerCreationFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerReaderFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DataContainerSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DoubleFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicChoiceFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableData.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DynamicTableFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FileListInfoFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FloatVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FourthOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/H5FilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/InputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IntVec3FilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/JsonFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedBooleanFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/LinkedChoicesFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/MultiDataArraySelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputFileFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/OutputPathFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PreflightUpdatedValueFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QFilterParametersReader.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QFilterParametersWriter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SecondOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SeparatorFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeTypeSelectionFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/StringFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThirdOrderPolynomialFilterParameter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/UnknownFilterParameter.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/SIMPLib/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")


set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_${SUBDIR_NAME}_HDRS}
  ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_${SUBDIR_NAME}_SRCS}
  ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}
)

# -- Add the binary directory for this subdirectory to the include path which is where the moc files are generated
include_directories( ${SIMPLib_BINARY_DIR}/${SUBDIR_NAME})


if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_${SUBDIR_NAME}_HDRS}
                   ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}
            DESTINATION include/SIMPLib/${SUBDIR_NAME}
            COMPONENT Headers   )
endif()
