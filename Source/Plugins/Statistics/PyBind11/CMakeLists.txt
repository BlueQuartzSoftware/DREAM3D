


add_custom_target(${PLUGIN_NAME}CreatePythonBindings ALL
      COMMAND GeneratePythonBindings "${${PLUGIN_NAME}_SOURCE_DIR}" "-" "${PLUGIN_NAME}"
      DEPENDS ${COPY_LIBRARY_TARGETS} GeneratePythonBindings
      COMMENT "Creating Python Bindings for ${PLUGIN_NAME}"
    )
    
# --------------------------------------------------------------------------
# Find the Pybind11 installation
set(PYBIND11_PYTHON_VERSION "3.6")
find_package(PyBind11)

messagE(STATUS "*********** SIMPLProj_SOURCE_DIR: ${SIMPLProj_SOURCE_DIR}")
# --------------------------------------------------------------------------
# Now compile the Python module
pybind11_add_module(${PLUGIN_NAME}Py ${${PLUGIN_NAME}_SOURCE_DIR}/PyBind11/${PLUGIN_NAME}Py.cxx)
set_target_properties(${PLUGIN_NAME}Py PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PLUGIN_NAME}Py PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  "${SIMPLProj_SOURCE_DIR}/PyBind11"
  )
target_link_libraries(${plug_target_name}Py LINK_PUBLIC ${PLUGIN_NAME} LINK_PRIVATE pybind11::module)
target_compile_features(${PLUGIN_NAME}Py PRIVATE cxx_local_type_template_args)
set_target_properties(${PLUGIN_NAME}Py PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}"
                                         LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python
)
add_dependencies(${PLUGIN_NAME}Py ${PLUGIN_NAME}CreatePythonBindings)                                         

                                         
install (TARGETS ${PLUGIN_NAME}Py 
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)


