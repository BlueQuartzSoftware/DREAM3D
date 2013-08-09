# DREAM3D  Change Plan #


## Phase 1 ##

+ Replace MXA with QtCore Functionality
+ Update CMakeLists.txt files to remove compiling of MXA
+ Update CMakeLists.txt files to link against Qt and make Qt mandatory
+ Update all source codes using functionality from MXA

## Phase 2 ##
+ Disable all filters example "ExamplePlugin"
+ Update DREAM3D Macros to use Qt structures such as QString, QVector, QMap
+ Update DataContainers to use Qt structures (QMap<QString, IDataArray::Pointer>)
+ Update Observable to inherit from QObject
+ Update all filters to add QObject to the class declaration

## Phase 3 ##

+ Implement all ParameterWidgets
+ Implement AbstractFilterParameter  Reader & Writer subclass that wraps QSettings
+ Remove FilterWidgetCodeGen

## Phase 4 ##
+ Update PluginMaker with new templates for the generated files
+ Update plugins
+ Test Like Crazy
+ Fix unknown engineering issues

