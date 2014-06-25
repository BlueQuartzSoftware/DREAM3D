project(ImageProcessingTest)



ADD_EXECUTABLE(ITKUtilitiesTest ITKUtilitiesTest.cpp)
TARGET_LINK_LIBRARIES(ITKUtilitiesTest ${QT_QTCORE_LIBRARY}
										DREAM3DLib
										${QT_QTGUI_LIBRARY}
										QtSupport
										DREAM3DWidgetsLib
										${ITK_LIBRARIES}
)
SET_TARGET_PROPERTIES(ITKUtilitiesTest PROPERTIES FOLDER ${PLUGIN_NAME}Targets)
