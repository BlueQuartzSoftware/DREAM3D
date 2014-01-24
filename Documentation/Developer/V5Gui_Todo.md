# Version 5 GUI Punch Card #


## Menus ##

+ all menus need to be reimplemented
+ Get list of menus from Version 4
+ Implement "Dock" Menu to toggle the dock widgets

## PipelineView ##

+ Drag and Drop not quite correct. The Vertical Spacer is getting taken into account. We always want to be BEFORE the vertical spacer item

## General Layout ##

+ properly figure out the state of each Dock Widget and save the state to the prefs file
+ properly init the dock widget buttons at the bottom of the GUI based on if they are showing or not

## Filter List Dock Widget ##

+ fix the search box to actually search

## Favorites/Documents ##

+ Add folder hierarchy capabilities

## Filter Parameter Widgets ##

+ Implement the remaining Filter Parameter Widgets
+ Start implementing the Advanced Widgets to allow the user to set the DataContainer, AttributeMatrix, AttributeMatrix array name.
+ Advanced inputs need to be hooked up in the SetupParameters() function in each filter
+ Probably have to add more filter parameter classes or add a property to the FilterParameter to mark it as advanced.
+ All the Advanced need to be written to and read from the pipeline file

## Plugins ##

+ Finish moving the filter groups to their own plugin structure.

## Misc Stuff ##

+ implement a global Error/Issue IObserver so ANY class can emit an error message and it will get picked up somewhere. 
+ Actually be able to execute the pipeline
+ find and fix this error in QFilterParametersReader.cpp. I think we are returning early due to error and we need to close the array before returning

	QSettings::endGroup: Expected endArray() instead
	QSettings::endGroup: Expected endArray() instead