

PROJECT( SimpleITKExplicit VERSION 0.9.0)


#-----------------------------------------------------------
# Check and set required flags for compilation
# This must be done before any other try compiles based tests are
# done.
include(${PROJECT_CODE_DIR}/SimpleITKExplicit/CMake/sitkCheckRequiredFlags.cmake)


# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SimpleITK_REQUIRED_C_FLAGS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SimpleITK_REQUIRED_CXX_FLAGS}")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SimpleITK_REQUIRED_LINK_FLAGS}")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SimpleITK_REQUIRED_LINK_FLAGS}")
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SimpleITK_REQUIRED_LINK_FLAGS}")

# message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
# message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
# message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
# message(STATUS "CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}")

include(${PROJECT_CODE_DIR}/SimpleITKExplicit/CMake/sitkCheckCXX11.cmake)

## YOU MUST INCLUDE THIS FILE TO USE ITK !!!
## Then do not forget to add "${ITK_LIBRARIES}" to the target_link_libraries below
include( ${DREAM3D_SUPPORT_DIR}/ITKSupport/IncludeITK.cmake)
include_directories(${ITK_INCLUDE_DIRS})

#----------------------------------------------------------
# Place all options to go into sitkConfigure.h here
set(SITK_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

option(SITK_EXPLICIT_INSTANTIATION "Enable an ITK static library of explicitly instantiated templates." ON)
if ( MSVC AND SITK_BUILD_SHARED_LIBS )
  set( SITK_SimpleITKExplit_STATIC 1 )
endif()

set( SITK_INT64_PIXELIDS_DEFAULT ON )
if( MSVC )
  # See http://www.cmake.org/cmake/help/v2.8.10/cmake.html#variable:MSVC_VERSION
  # and https://en.wikipedia.org/wiki/Microsoft_Visual_Studio#Version_history
  #   1500 = VS  9.0 (Visual Studio 2008)
  #   1600 = VS 10.0 (Visual Studio 2010)
  #   1700 = VS 11.0 (Visual Studio 2012)
  if(MSVC_VERSION VERSION_LESS 1600)
    # with this option on the linker runs out of memory
    set( SITK_INT64_PIXELIDS_DEFAULT OFF )
  endif()
endif()
option( SITK_INT64_PIXELIDS "Instantiate 64-bit integer pixels, including unsigned, vector and label maps." ${SITK_INT64_PIXELIDS_DEFAULT} )
# Setting this option will shorten compile times by reducing the
# amount of pixel types and generated code.
# NB: this option should _NOT_ be used for releases!
option ( SITK_EXPRESS_INSTANTIATEDPIXELS "Instantiate very few pixel types ( for use for development only )" OFF )
mark_as_advanced ( SITK_EXPRESS_INSTANTIATEDPIXELS )



function(sitk_install_exported_target tgt)
  # Remove the build tree's SimpleITKTargets file if this is the first call:
  get_property(first_time GLOBAL PROPERTY SimpleITK_FIRST_EXPORTED_TARGET)
  if(NOT first_time)
    file(REMOVE ${CMAKE_BINARY_DIR}/SimpleITKTargets.cmake)
    set_property(GLOBAL PROPERTY SimpleITK_FIRST_EXPORTED_TARGET 1)
  endif()

  set_property(TARGET ${tgt} PROPERTY VERSION 1)
  set_property(TARGET ${tgt} PROPERTY SOVERSION 1)
  set_property(TARGET ${tgt} PROPERTY
    OUTPUT_NAME ${tgt}-${SimpleITK_VERSION_MAJOR}.${SimpleITK_VERSION_MINOR})

  export(TARGETS ${tgt}
    APPEND FILE "${CMAKE_BINARY_DIR}/SimpleITKTargets.cmake"
    )

  install(TARGETS ${tgt}
    EXPORT SimpleITKTargets
    RUNTIME DESTINATION ${SimpleITK_INSTALL_RUNTIME_DIR}
    LIBRARY DESTINATION ${SimpleITK_INSTALL_LIBRARY_DIR}
    ARCHIVE DESTINATION ${SimpleITK_INSTALL_ARCHIVE_DIR}
    )
endfunction()

set(SimpleITK_INSTALL_RUNTIME_DIR "Bin")
set(SimpleITK_INSTALL_LIBRARY_DIR "Bin")
set(SimpleITK_INSTALL_ARCHIVE_DIR "Bin")



include_directories( ${SimpleITKExplicit_SOURCE_DIR}/Explicit/include)
include_directories( ${SimpleITKExplicit_BINARY_DIR}/Common/include)

configure_file( "${SimpleITKExplicit_SOURCE_DIR}/Common/src/sitkConfigure.h.in"
                "${SimpleITKExplicit_BINARY_DIR}/Common/include/sitkConfigure.h" )

include( Explicit/src/CMakeLists.txt)
