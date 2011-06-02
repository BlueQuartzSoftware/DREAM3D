

SET (DREAM3D_Common_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMRandomNG.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Face.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ISegment.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Neighbor.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Node.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OIMColoring.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Patch.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Segment.h
  ${DREAM3DLib_SOURCE_DIR}/Common/STLWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKFileReader.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKWriterMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.h
)

SET (DREAM3D_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMRandomNG.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Face.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ISegment.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Neighbor.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Node.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Patch.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Segment.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/STLWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKFileReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3D_Common_HDRS}" "${DREAM3D_Common_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()

