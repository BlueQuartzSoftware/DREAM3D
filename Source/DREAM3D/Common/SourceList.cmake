

SET (DREAM3D_Common_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMRandomNG.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OIMColoring.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileReader.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileWriters.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKWriterMacros.h
)

SET (DREAM3D_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMRandomNG.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileWriters.cpp  
)


cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3D_Common_HDRS}" "${DREAM3D_Common_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()

