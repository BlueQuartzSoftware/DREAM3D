############################################
#
# Check the existence of the libraries.
#
############################################
# This macro was taken directly from the FindQt4.cmake file that is included
# with the CMake distribution. This is NOT my work. All work was done by the
# original authors of the FindQt4.cmake file. Only minor modifications were
# made to remove references to Qt and make this file more generally applicable
#########################################################################

MACRO (cmp_ADJUST_LIB_VARS basename)
  #  message(STATUS "${basename}_INCLUDE_DIR: ${${basename}_INCLUDE_DIR}")
    
  if(${basename}_INCLUDE_DIR)

  # if only the release version was found, set the debug variable also to the release version
  if(${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)
    SET(${basename}_LIBRARY_DEBUG ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARY       ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARIES     ${${basename}_LIBRARY_RELEASE})
  endif(${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)

  # if only the debug version was found, set the release variable also to the debug version
  if(${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
    SET(${basename}_LIBRARY_RELEASE ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARY         ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARIES       ${${basename}_LIBRARY_DEBUG})
  endif(${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
  if(${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)
    # if the generator supports configuration types then set
    # optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
    if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(${basename}_LIBRARY       optimized ${${basename}_LIBRARY_RELEASE} debug ${${basename}_LIBRARY_DEBUG})
    ELSE(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      # if there are no configuration types and CMAKE_BUILD_TYPE has no value
      # then just use the release libraries
      SET(${basename}_LIBRARY       ${${basename}_LIBRARY_RELEASE} )
    ENDif(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    SET(${basename}_LIBRARIES       optimized ${${basename}_LIBRARY_RELEASE} debug ${${basename}_LIBRARY_DEBUG})
  endif(${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)

  SET(${basename}_LIBRARY ${${basename}_LIBRARY} CACHE FILEPATH "The ${basename} library")

  if(${basename}_LIBRARY)
    SET(${basename}_FOUND 1)
  endif(${basename}_LIBRARY)

endif(${basename}_INCLUDE_DIR )

  # Make variables changeble to the advanced user
  MARK_AS_ADVANCED(${basename}_LIBRARY ${basename}_LIBRARY_RELEASE ${basename}_LIBRARY_DEBUG ${basename}_INCLUDE_DIR )
ENDMACRO (cmp_ADJUST_LIB_VARS)
  
