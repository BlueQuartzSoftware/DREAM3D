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


function(ADD_DOXYGEN_TARGETS)
    set(options )
    set(oneValueArgs  PDF_FILENAME PROJECT_NAME PROJECT_BINARY_DIR OUTPUT_DIR OUTPUT_SUB_DIR
                      TITLE AUTHOR GENERATE_HTML GENERATE_LATEX GENERATE_RTF TEMPLATE_DIR )
    set(multiValueArgs SOURCE_FILES EPS_FILES IMAGE_DIR)
    cmake_parse_arguments(DOX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    if("${DOX_OUTPUT_DIR}" STREQUAL "")
      message(FATAL_ERROR "DOX_OUTPUT_DIR Variable MUST be set and it is empty")
    endif()
  if(0)
		message(STATUS "------------------------------------------")
		message(STATUS "Adding Doxygen Targets for ${DOX_PROJECT_NAME}")
		message(STATUS "DOX_PROJECT_NAME: ${DOX_PROJECT_NAME}")
		message(STATUS "DOX_GENERATE_HTML: ${DOX_GENERATE_HTML}")
		message(STATUS "DOX_GENERATE_LATEX: ${DOX_GENERATE_LATEX}")
		message(STATUS "DOX_GENERATE_RTF: ${DOX_GENERATE_RTF}")
		message(STATUS "DOX_PROJECT_NAME: ${DOX_PROJECT_NAME}")
		message(STATUS "DOX_PROJECT_BINARY_DIR: ${DOX_PROJECT_BINARY_DIR}")
		message(STATUS "DOX_TITLE:${LATEX_TITLE}")
		message(STATUS "DOX_AUTHOR:${LATEX_AUTHOR}")
		message(STATUS "DOX_TEMPLATE_FILE: ${DOX_TEMPLATE_FILE}")
		message(STATUS "DOX_OUTPUT_DIR: ${DOX_OUTPUT_DIR}")
	endif()
    set(GENERATE_HTML ${GENERATE_HTML})
    set(GENERATE_LATEX ${GENERATE_LATEX})
    set(GENERATE_RTF  ${GENERATE_RTF})
    set(OUTPUT_SUB_DIR ${OUTPUT_SUB_DIR})

    STRING(REPLACE ";" " \\\n" DOXYGEN_INPUT_FILES "${DOX_SOURCE_FILES}"  )
    STRING(REPLACE ";" " \\\n" DOXYGEN_IMAGE_DIR "${DOX_IMAGE_DIR}"  )

    set(DOXYFILE ${DOX_PROJECT_NAME}_Doxyfile)
    if(DOX_GENERATE_LATEX STREQUAL "YES")
        set(LATEX_HEADER_FILE ${DOX_PROJECT_BINARY_DIR}/BlueQuartz.tex )
        set(LATEX_STYLE_FILE  ${DOX_PROJECT_BINARY_DIR}/BlueQuartz.sty )
    else()
      set(LATEX_HEADER_FILE "" )
      set(LATEX_STYLE_FILE  "" )
    endif()

    option(DOXYGEN_QUIET_OUTPUT "Do not output anything from Doxygen Run" ON)
    if(DOXYGEN_QUIET_OUTPUT)
      set(DOXYGEN_QUIET "YES")
    else()
      set(DOXYGEN_QUIET "NO")
    endif()

    configure_file(${DOX_TEMPLATE_DIR}/Doxyfile.in
                   ${DOX_PROJECT_BINARY_DIR}/${DOXYFILE} @ONLY IMMEDIATE)

    add_custom_target(${DOX_PROJECT_NAME}
              COMMAND ${DOXYGEN_EXECUTABLE} ${DOX_PROJECT_BINARY_DIR}/${DOXYFILE}
              SOURCES ${DOX_PROJECT_BINARY_DIR}/${DOXYFILE} )
    SET_TARGET_PROPERTIES(${DOX_PROJECT_NAME} PROPERTIES FOLDER DocumentationTargets)


#-- If we are generating PDF output then we need to make a few passes on the PDF to make sure things
# like the table of contents and references are all up to date.
    if(DOX_GENERATE_LATEX STREQUAL "YES")

        configure_file(${DOX_TEMPLATE_DIR}/BlueQuartz.tex.in
                       ${LATEX_HEADER_FILE}  @ONLY IMMEDIATE)

        configure_file(${DOX_TEMPLATE_DIR}/BlueQuartz.sty.in
                       ${LATEX_STYLE_FILE}  @ONLY IMMEDIATE)

        #-- Create the proper Directory Structure
        file(MAKE_DIRECTORY ${DOX_PROJECT_BINARY_DIR}/latex )

        #-- Copy the LaTex Style File into the Documentation/Latex subdirectory
        add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LATEX_STYLE_FILE}
                    ${DOX_PROJECT_BINARY_DIR}/latex/doxygen.sty
                    COMMENT "Copying BlueQuartz LaTex Style file")

        #-- Copy extra LaTex Style files that are needed to process the LaTex
        file(COPY ${DOX_TEMPLATE_DIR}/latex/ DESTINATION ${DOX_PROJECT_BINARY_DIR}/latex/ )


        #-- Set some additional Directories to remove when cleaning the project (Not sure if this actually works)
        set( ADDITIONAL_MAKE_CLEAN_FILES ${ADDITIONAL_MAKE_CLEAN_FILES} ${DOX_PROJECT_BINARY_DIR}/latex )

        #-- Setup some additional LaTex processor arguments
        SET(LATEX_COMPILER_FLAGS "-interaction=nonstopmode"
          CACHE STRING "Flags passed to latex.")
        SET(PDFLATEX_COMPILER_FLAGS ${LATEX_COMPILER_FLAGS}
          CACHE STRING "Flags passed to pdflatex.")

        set(LATEX_OUTPUT_PATH ${DOX_PROJECT_BINARY_DIR}/latex/output)

        #-- Create more directory structures
        file(MAKE_DIRECTORY ${LATEX_OUTPUT_PATH})

        set(LATEX_MAIN_INPUT ${DOX_PROJECT_BINARY_DIR}/latex/refman)


        add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
                COMMAND  ${PDFLATEX_COMPILER} ${PDFLATEX_COMPILER_FLAGS}
                   -output-directory=${LATEX_OUTPUT_PATH}
                   --jobname=${DOX_PDF_FILENAME}
                   ${LATEX_MAIN_INPUT}.tex
                WORKING_DIRECTORY ${DOX_PROJECT_BINARY_DIR}/latex/
                COMMENT "======> ${DOX_PROJECT_NAME} Generating PDF - First Pass")

        add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
              COMMAND ${MAKEINDEX_COMPILER}
              ${MAKEINDEX_COMPILER_FLAGS}
              output/${DOX_PDF_FILENAME}.idx
              WORKING_DIRECTORY ${DOX_PROJECT_BINARY_DIR}/latex/
              COMMENT "======> ${DOX_PROJECT_NAME}  Generating Index")

        add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
                          COMMAND  ${PDFLATEX_COMPILER}
                          ${PDFLATEX_COMPILER_FLAGS}
              -output-directory=${LATEX_OUTPUT_PATH}
              --jobname=${DOX_PDF_FILENAME}
              ${LATEX_MAIN_INPUT}.tex
              WORKING_DIRECTORY ${DOX_PROJECT_BINARY_DIR}/latex/
              COMMENT "======> ${DOX_PROJECT_NAME}  Generating PDF - Second Pass")

   if(0)
        add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
              COMMAND  ${PDFLATEX_COMPILER}
               ${PDFLATEX_COMPILER_FLAGS}
               -output-directory=${LATEX_OUTPUT_PATH}
               ${LATEX_MAIN_INPUT}.tex
              WORKING_DIRECTORY ${DOX_PROJECT_BINARY_DIR}/latex/
              COMMENT "======> ${DOX_PROJECT_NAME}  Generating PDF - Third Pass")
    endif()

        if(APPLE)
          if(AUTO_OPEN_PDF)
              add_custom_command(TARGET ${DOX_PROJECT_NAME}  POST_BUILD
                COMMAND /usr/bin/open ${LATEX_OUTPUT_PATH}/${DOX_PDF_FILENAME}.pdf
                COMMENT "Opening PDF file with default Viewer")
          endif()
        endif()

    endif() #-- Close the if(GENERATE_LATEX) conditional

endfunction()
