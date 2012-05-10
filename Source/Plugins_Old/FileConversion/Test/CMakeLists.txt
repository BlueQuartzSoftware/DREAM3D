





# --------

add_executable(VTKFileWritersTest 
            ${FileConversionPlugin_SOURCE_DIR}/Test/VTKFileWritersTest.cpp 
            ${FileConversionPlugin_SOURCE_DIR}/VtkGrainIdReader.cpp)
target_link_libraries(VTKFileWritersTest DREAM3DLib)
add_test(VTKFileWritersTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/VTKFileWritersTest)

