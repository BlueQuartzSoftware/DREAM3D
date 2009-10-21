project(AIMTest)


set (srcs
    ${AIMTest_SOURCE_DIR}/AngDirectoryPatternTest.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    )
add_executable(AngDirectoryPattern ${srcs})
target_link_libraries(AngDirectoryPattern MXA)

