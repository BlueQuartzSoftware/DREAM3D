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

SET (DREAM3DLib_Common_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArray.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DDLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DSetGetMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Field.h
  ${DREAM3DLib_SOURCE_DIR}/Common/EbsdColoring.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/NeighborList.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PreFlightArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.h
)

SET (DREAM3DLib_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Field.cpp
#  ${DREAM3DLib_SOURCE_DIR}/Common/NeighborList.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3DLib_Common_HDRS}" "${DREAM3DLib_Common_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()
