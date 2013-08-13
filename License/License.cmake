
#message(STATUS "Gathering License Files")
#message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
#message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
# --------------------------------------------------------------------
# create Resource files for the various license files that are used and
# also create a header file that lists all the License Files
set(LICENSE_FILES
                   ${DREAM3DProj_SOURCE_DIR}/License.txt
                #   ${DREAM3DProj_SOURCE_DIR}/ReadMe.txt
                   ${DREAM3DProj_SOURCE_DIR}/License/Boost.license
                   ${DREAM3DProj_SOURCE_DIR}/License/MXA.license
                   ${DREAM3DProj_SOURCE_DIR}/License/Qt.license
                   ${DREAM3DProj_SOURCE_DIR}/License/Qwt.license
                   ${DREAM3DProj_SOURCE_DIR}/License/HDF5.license
           )
set(QRC_LICENSE_FILES "")
set(LICENSE_HEADER_FILE  ${PROJECT_BINARY_DIR}/DREAM3D/License/${PROJECT_NAME}LicenseFiles.h_tmp)
file(WRITE ${LICENSE_HEADER_FILE} "#ifndef _LICENSE_FILES_H_\n")
file(APPEND ${LICENSE_HEADER_FILE} "#define _LICENSE_FILES_H_\n")
file(APPEND ${LICENSE_HEADER_FILE} "namespace ${PROJECT_PREFIX} {\n")
file(APPEND ${LICENSE_HEADER_FILE} "  QStringList LicenseList = (QStringList()  ")
foreach(lf ${LICENSE_FILES})
    get_filename_component(cmp_text_file_name ${lf} NAME_WE)
    set(cmp_text_file_name "${cmp_text_file_name}.license")
    get_filename_component(lf_fn ${lf} NAME_WE)
    # Copy the text file into the Build Directory
#    message(STATUS "lf: ${lf}")
#    message(STATUS "cmp_text_file_name: ${cmp_text_file_name}")
#    message(STATUS "lf_fn: ${lf_fn}")
    configure_file("${lf}" ${PROJECT_BINARY_DIR}/DREAM3D/License/${lf_fn}.license   COPYONLY )

    # create the Qt Resource File
    set(CMP_RESOURCE_FILE_NAME ${lf_fn}.license)
    configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/QtResourceFile.qrc.in
                   ${PROJECT_BINARY_DIR}/DREAM3D/License/${lf_fn}.qrc)

    set(QRC_LICENSE_FILES ${QRC_LICENSE_FILES} ${PROJECT_BINARY_DIR}/DREAM3D/License/${lf_fn}.qrc)
    file(APPEND ${LICENSE_HEADER_FILE} " << \":/${lf_fn}.license\"")

endforeach(lf ${LICENSE_FILES})
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "")

file(APPEND ${LICENSE_HEADER_FILE}  ");\n")
file(APPEND ${LICENSE_HEADER_FILE}  "}\n#endif /* _LICENSE_FILES_H_ */ \n")


cmpReplaceFileIfDifferent(OLD_FILE_PATH ${PROJECT_BINARY_DIR}/DREAM3D/License/${PROJECT_NAME}LicenseFiles.h
                          NEW_FILE_PATH ${LICENSE_HEADER_FILE} )


