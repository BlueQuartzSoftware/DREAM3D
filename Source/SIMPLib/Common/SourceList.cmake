#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_Common_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/Common/AbstractFilter.h
  ${SIMPLib_SOURCE_DIR}/Common/ComparisonInputs.h
  ${SIMPLib_SOURCE_DIR}/Common/FilterPipeline.h
  ${SIMPLib_SOURCE_DIR}/Common/Observer.h
  ${SIMPLib_SOURCE_DIR}/Common/Observable.h
)
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
#QT5_WRAP_CPP( SIMPLib_Common_Generated_MOC_SRCS ${SIMPLib_Common_Moc_HDRS})


set(SIMPLib_Common_HDRS
  ${SIMPLib_Common_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
  ${SIMPLib_SOURCE_DIR}/Common/AppVersion.h
  ${SIMPLib_SOURCE_DIR}/Common/Constants.h
  ${SIMPLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.h
  ${SIMPLib_SOURCE_DIR}/Common/DocRequestManager.h
  ${SIMPLib_SOURCE_DIR}/Common/FilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/Common/FilterManager.h
  ${SIMPLib_SOURCE_DIR}/Common/IDataArrayFilter.h
  ${SIMPLib_SOURCE_DIR}/Common/IFilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/Common/IObserver.h
  ${SIMPLib_SOURCE_DIR}/Common/PhaseType.h
  ${SIMPLib_SOURCE_DIR}/Common/PipelineMessage.h
  ${SIMPLib_SOURCE_DIR}/Common/ScopedFileMonitor.hpp
  ${SIMPLib_SOURCE_DIR}/Common/ShapeType.h
  ${SIMPLib_SOURCE_DIR}/Common/SIMPLibDLLExport.h
  ${SIMPLib_SOURCE_DIR}/Common/SIMPLibSetGetMacros.h
  ${SIMPLib_SOURCE_DIR}/Common/TemplateHelpers.hpp
  ${SIMPLib_SOURCE_DIR}/Common/ThresholdFilterHelper.h
)


set(SIMPLib_Common_SRCS
  ${SIMPLib_SOURCE_DIR}/Common/AbstractFilter.cpp
  ${SIMPLib_SOURCE_DIR}/Common/AppVersion.cpp
  ${SIMPLib_SOURCE_DIR}/Common/DocRequestManager.cpp
  ${SIMPLib_SOURCE_DIR}/Common/ComparisonInputs.cpp
  ${SIMPLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.cpp
  ${SIMPLib_SOURCE_DIR}/Common/FilterManager.cpp
  ${SIMPLib_SOURCE_DIR}/Common/FilterPipeline.cpp
  ${SIMPLib_SOURCE_DIR}/Common/IDataArrayFilter.cpp
  ${SIMPLib_SOURCE_DIR}/Common/IObserver.cpp
  ${SIMPLib_SOURCE_DIR}/Common/Observable.cpp
  ${SIMPLib_SOURCE_DIR}/Common/Observer.cpp
  ${SIMPLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${SIMPLib_SOURCE_DIR}/Common/ShapeType.cpp
  ${SIMPLib_SOURCE_DIR}/Common/ThresholdFilterHelper.cpp
)



cmp_IDE_SOURCE_PROPERTIES( "SIMPLibLib/Common" "${SIMPLib_Common_HDRS};${SIMPLib_Common_Moc_HDRS}" "${SIMPLib_Common_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/SIMPLibLib/Common" "" "${SIMPLib_Common_Generated_MOC_SRCS}" "0")

set(SIMPLib_Common_SRCS
  ${SIMPLib_Common_SRCS}
  ${SIMPLib_Common_Generated_MOC_SRCS}  # Add the generated source files here so they get compiled.
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Common_HDRS}
                   ${SIMPLib_Common_Moc_HDRS}
            DESTINATION include/SIMPLib/Common
            COMPONENT Headers   )
endif()

if(MSVC)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/Common/FilterManager.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/Common/FilterManager.h PROPERTIES COMPILE_FLAGS /bigobj)
endif()
