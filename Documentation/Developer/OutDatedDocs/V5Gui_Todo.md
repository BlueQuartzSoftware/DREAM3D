# Version 5 GUI Punch Card #


## Menus ##

+ all menus need to be reimplemented
+ Get list of menus from Version 4
+ Implement "Dock" Menu to toggle the dock widgets
	+ DONE

## PipelineView ##


+ Loading a pipeline can take a while. Put up a progress dialog box
+ Font sizes are messed up on each system (Windows & OS X)

## General Layout ##

+ properly figure out the state of each Dock Widget and save the state to the prefs file
	+ DONE
+ properly init the dock widget buttons at the bottom of the GUI based on if they are showing or not
	+ DONE

## Filter List Dock Widget ##

+ fix the search box to actually search

## Favorites/Documents ##

+ Add folder hierarchy capabilities
	+ DONE

## Filter Parameter Widgets ##
+ Changing a parameter does not trigger a preflight.
+ Implement the remaining Filter Parameter Widgets
+ Start implementing the Advanced Widgets to allow the user to set the DataContainer, AttributeMatrix, AttributeMatrix array name.
+ Advanced inputs need to be hooked up in the SetupParameters() function in each filter
+ Probably have to add more filter parameter classes or add a property to the FilterParameter to mark it as advanced.
+ All the Advanced parameters need to be written to and read from the pipeline file
+ Some filters do not have the needed Q_PROPERTY macro so each filter needs to be inspected. This can actually be put into a "test" program because we can get the FilterParameters and we can get the QObject Properties and there should be a 1-to-1 relationship. This would help double check when we add filters to make sure all the needed code is implemented.
	+ DONE

## Plugins ##

+ Finish moving the filter groups to their own plugin structure.

## Misc Stuff ##

+ implement a global Error/Issue IObserver so ANY class can emit an error message and it will get picked up somewhere. 
+ Actually be able to execute the pipeline
