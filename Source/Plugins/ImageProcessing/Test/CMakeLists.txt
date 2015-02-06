project(ImageProcessingTest)



ADD_EXECUTABLE(ITKUtilitiesTest ITKUtilitiesTest.cpp)
TARGET_LINK_LIBRARIES(ITKUtilitiesTest Qt5::Core
										DREAM3DLib
										Qt5::Gui Qt5::Widgets
										QtSupport
										DREAM3DWidgetsLib
										${ITK_LIBRARIES}
)
SET_TARGET_PROPERTIES(ITKUtilitiesTest PROPERTIES FOLDER ${PLUGIN_NAME}Plugin/Test)

