project(AStyle)
cmake_minimum_required(VERSION 2.8.9)
message(STATUS "Building AStyle Code Formatting Program....")

set(AStyle_SRCS
  ${AStyle_SOURCE_DIR}/src/ASBeautifier.cpp
  ${AStyle_SOURCE_DIR}/src/ASEnhancer.cpp
  ${AStyle_SOURCE_DIR}/src/ASFormatter.cpp
  ${AStyle_SOURCE_DIR}/src/ASLocalizer.cpp
  ${AStyle_SOURCE_DIR}/src/ASResource.cpp
  ${AStyle_SOURCE_DIR}/src/astyle_main.cpp
)

set(AStyle_HDRS
  ${AStyle_SOURCE_DIR}/src/astyle.h
  ${AStyle_SOURCE_DIR}/src/astyle_main.h
  ${AStyle_SOURCE_DIR}/src/ASLocalizer.h
)

add_executable(astyle ${AStyle_SRCS} ${AStyle_HDRS} )

ADD_CUSTOM_TARGET(FormatCode
                  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/astyle --options=${DREAM3D_SUPPORT_DIR}/Scripts/AStyleOptions.txt ${PROJECT_CODE_DIR}/*.cpp ${PROJECT_CODE_DIR}/*.h ${PROJECT_CODE_DIR}/*.hpp
                  )
