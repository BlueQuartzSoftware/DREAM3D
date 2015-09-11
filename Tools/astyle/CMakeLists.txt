project(AStyle)
cmake_minimum_required(VERSION 3.1.0)
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
SET_TARGET_PROPERTIES(astyle PROPERTIES FOLDER Tools)



# Find Git executable
Find_package(Git)
# Run 'git describe' to get our tag offset
if(GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} diff --name-only
    OUTPUT_VARIABLE GIT_CHANGED_FILES
    RESULT_VARIABLE did_run
    ERROR_VARIABLE git_error
    WORKING_DIRECTORY ${DREAM3DProj_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE)


  file(WRITE ${AStyle_BINARY_DIR}/GitChangedFiles.txt ${GIT_CHANGED_FILES})
  file(STRINGS ${AStyle_BINARY_DIR}/GitChangedFiles.txt GIT_CHANGED_FILES REGEX ".*\.cpp|.*\.h")
#  message(STATUS "Git Changed Files:\n${GIT_CHANGED_FILES}")

  ADD_CUSTOM_TARGET(FormatCode
    DEPENDS astyle
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/astyle --style=allman --indent=spaces=2 --indent-classes --indent-switches --indent-namespaces --indent-preprocessor --max-instatement-indent=60 --pad-oper --align-pointer=type --align-reference=type --add-one-line-brackets --convert-tabs --suffix=none --lineend=linux --options=none ${GIT_CHANGED_FILES}
    WORKING_DIRECTORY ${DREAM3DProj_SOURCE_DIR}
  )
  SET_TARGET_PROPERTIES(FormatCode PROPERTIES FOLDER Tools)

endif()


