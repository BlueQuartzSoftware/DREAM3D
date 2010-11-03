# --------------------------------------------------------------------
# create Resource files for the various license files that are used and 
# also create a header file that lists all the License Files
set(LICENSE_FILES ${PROJECT_SOURCE_DIR}/License/Boost.license
                  ${PROJECT_SOURCE_DIR}/License/ITK.license
                  ${PROJECT_SOURCE_DIR}/License/MXA.license
                  ${PROJECT_SOURCE_DIR}/License/Qt.license
                  ${PROJECT_SOURCE_DIR}/License/tiff.license )
set(QRC_LICENSE_FILES "")
file(WRITE ${PROJECT_BINARY_DIR}/License/LicenseFiles.h "#ifndef _LICENSE_FILES_H_\n")
file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h "#define _LICENSE_FILES_H_\n")
file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h "namespace ${PROJECT_PREFIX} {\n")
file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h "  QStringList LicenseList = (QStringList()  ")
foreach(lf ${LICENSE_FILES})
    get_filename_component(cmp_text_file_name ${lf} NAME)
    get_filename_component(lf_fn ${lf} NAME_WE)
    # Copy the text file into the Build Directory
    configure_file("${lf}" ${PROJECT_BINARY_DIR}/License/${cmp_text_file_name}   COPYONLY )
        
    # create the Qt Resource File
    configure_file(${PROJECT_CMAKE_DIR}/ConfiguredFiles/QtResourceFile.qrc.in 
                   ${PROJECT_BINARY_DIR}/License/${lf_fn}.qrc)
                   
    set(QRC_LICENSE_FILES ${QRC_LICENSE_FILES} ${PROJECT_BINARY_DIR}/License/${lf_fn}.qrc)
    file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h " << \":/${cmp_text_file_name}\"")
    
endforeach(lf ${LICENSE_FILES})
file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h ");\n")
file(APPEND ${PROJECT_BINARY_DIR}/License/LicenseFiles.h "}\n#endif /* _LICENSE_FILES_H_ */ \n")

