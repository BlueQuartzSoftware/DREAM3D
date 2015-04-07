# DREAM3D Design #

## Internal Libraries ##

*MXA*: This library contains some low level routines for dealing with Files, Endian, Date/Time and a few other misc items.

*EBSDLib*: This library implements all the code necessary to read from TSL, HKL and HEDM data sets which are predominantly crystal orientation related data.

*DREAM3DLib*: This library contains all the low level codes and algorithms the Make up DREAM3D. All the filters are included in this library

*H5Support*: This library contains some files that make working with HDF5 much easier than in low level "C" but without the HDF5 C++ interface.

*QtSupport*:  This library contains misc reusable components that require the Qt libraries to compile.

*PipelineBuilderLib*: This library contains the high level GUI components that make up the GUI portions of DREAM3D

*FilterWidgetsLib*: This library contains all the individual filter widget classes where for each filter in DREAM3DLib has a specific FilterWidget. Some of the Filter Widgets are generated at compile time by the FilterWidgetCodeGen program.

## Places to Start Looking ##

### DREAM3DLib/Common/AbstractFilter ###

This class is the top level superclass for every filter in DREAM3DLib. This class inherits from the "Observable" class which implements a "Publish/Subscribe" design pattern. The AbstractFilter class acts as a publisher of information.

### DREAM3DLib/Common/FilterPipeline ###

This class implements a container strategy where the class can hold any number of AbstractFilters. 

### DREAM3DLib/OrientationOps ###

This is the superclass for all the Laue symmetry classes that perform crystal symmetry specific functions such as generating an IPF Color based on Euler Angles, getting the number of Symmetry operators and generating pole figures.

### FilterWidgetsLib ###

This library has all the GUI widgets for each filter. Each widget is subclassed from *PipelineBuilderLib/QFilterWidget*. Most source files are generated at compile time via the *FilterWidgetCodeGen* program that uses information from each filter's "setupParameters()" function to generate all the code necessary to generate an appropriate GUI widget for each input type and also generate the code that is needed to transfer the user input values to an instance of the filter. The Widget class can itself instantiate and properly initialize a DREAM3D filter that it wraps. A Factory design pattern is used to read a pipeline file from a text file.

## Background ##

The idea is that a Pipeline is a list of filters that are executed in order. The programmer can manually create a pipeline themselves using the classes or a user can just use the GUI application to create and execute a pipeline.

## Plugins ##

DREAM3D can load plugins that are compiled for DREAM3D. There is an Application called "PluginMaker" that can generate all the necessary codes to get a programmer started with a plugin.

## Misc Reading ##


There are some other documentation files that cover writing a filter and all the necessary steps that are needed to do this.