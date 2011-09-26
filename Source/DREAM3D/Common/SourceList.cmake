

SET (DREAM3D_Common_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AIMArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DSetGetMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DxGrainIdWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OIMColoring.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhWriter.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.h
)

SET (DREAM3D_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DxGrainIdWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Grain.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3D_Common_HDRS}" "${DREAM3D_Common_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3D_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()


SET (DREAM3DLIB_ORIENT_OPS_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.h
)
SET (DREAM3DLIB_ORIENT_OPS_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/CubicOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/HexagonalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/OrientationOps/OrthoRhombicOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common/OrientationOps" "${DREAM3DLIB_ORIENT_OPS_HDRS}" "${DREAM3DLIB_ORIENT_OPS_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLIB_ORIENT_OPS_HDRS}
            DESTINATION include/DREAM3D/Common/OrientationOps
            COMPONENT Headers   )
endif()

SET (DREAM3DLIB_SHAPE_OPS_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/ShapeOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/CubeOctohedronOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/CylinderOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/EllipsoidOps.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/SuperEllipsoidOps.h
)
SET (DREAM3DLIB_SHAPE_OPS_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/ShapeOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/CubeOctohedronOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/CylinderOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/EllipsoidOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeOps/SuperEllipsoidOps.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common/ShapeOps" "${DREAM3DLIB_SHAPE_OPS_HDRS}" "${DREAM3DLIB_SHAPE_OPS_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLIB_SHAPE_OPS_HDRS}
            DESTINATION include/DREAM3D/Common/ShapeOps
            COMPONENT Headers   )
endif()

SET (DREAM3DLIB_VTKUtils_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileReader.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileWriters.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKWriterMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKRectilinearGridFileReader.h
)
SET (DREAM3DLIB_VTKUtils_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKFileReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VTKUtils/VTKRectilinearGridFileReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common/VTKUtils" "${DREAM3DLIB_VTKUtils_HDRS}" "${DREAM3DLIB_VTKUtils_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLIB_VTKUtils_HDRS}
            DESTINATION include/DREAM3D/Common/VTKUtils
            COMPONENT Headers   )
endif()

set (DREAM3D_Common_HDRS
   ${DREAM3D_Common_HDRS}
   ${DREAM3DLIB_ORIENT_OPS_HDRS}
   ${DREAM3DLIB_SHAPE_OPS_HDRS}
   ${DREAM3DLIB_VTKUtils_HDRS}
   )
set (DREAM3D_Common_SRCS
   ${DREAM3D_Common_SRCS}
   ${DREAM3DLIB_ORIENT_OPS_SRCS}
   ${DREAM3DLIB_SHAPE_OPS_SRCS}
   ${DREAM3DLIB_VTKUtils_SRCS}
)

