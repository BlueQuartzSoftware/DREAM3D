# Adding Filter Parameter Widget #


## Create the Widget With QDesigner ##


## Update CMakeLists.txt File ##

Update the PipelineBuilderLib/CMakeLists.txt file with the .h, .cpp and .ui file locations

Update DREAM3DLib/Common/FilterParameter.h with new Enumeration for the "WidgetType"

Add the new Filter Parameter to the Sources/Plugins/ExamplePlugin/ExamplePluginFilters/GenericExample.h and .cpp files to have a filter that actually uses the Filter Pararmeter

Update FilterWidget Code Gen to generate all the necessary codes

Update QFilterWidget::setupUI to correctly place the parameter widget in the QFilterWidget