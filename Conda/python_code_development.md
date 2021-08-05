# Using Python to Script DREAM.3D #

## Requirements ##

The prebuilt python is currently in testing and only supports Windows environments.
+ Windows Python Environment
+ Python 3.8/3.9
+ Ability to install from a custom URL source.

## Installation ##

```lang-console
(base) C:\Users\johnsmith> conda config --add channels conda-forge
(base) C:\Users\johnsmith> conda config --set channel_priority strict
(base) C:\Users\johnsmith> conda create -n d3d_embed python=3.8
(base) C:\Users\johnsmith> conda activate d3d_embed
(d3d_embed) C:\Users\johnsmith> 
(d3d_embed) C:\Users\johnsmith>conda install -c http://dream3d.bluequartz.net/binaries/conda dream3d-conda
```


## Using the DREAM.3D Python Bindings ##

The potential scripter will want to be familiar with the DREAM.3D data structure by reviewing [online docs for the DataStructure.](http://www.dream3d.io/1_UsingDREAM3D/%5BA%5D_DataStructure/). Once the developer has an understanding of the data structure we can proceed to write our first script using the DREAM3D Python bindings. All of the DREAM3D Python bindings are located in the `dream3d` python module. For basic operations you would want to do the following:

```{.python}
from dream3d import simpl
from dream3d import simplpy
```

to import the basic functionality. The DREAM.3D python bindings are broken into several modules that each have a specific bit of functionality.

+ simpl is the lowest C++ style of python bindings
+ simplpy is the lowest level module that has a *pythonic* interface.
+ Each plugin has an associated module that encapsulate both a low level interface and higher level *pythonic* interface. 

There are many use-cases where the DREAM.3D python bindings could be used. we are going to concentrate on two of those use-cases, specifically:

+ 1: Using the python bindings to access DREAM.3D functionality and integrate that functionality into another set of codes
+ 2: Using the python bindings to programmatically build a *pipeline* and execute that pipeline

### Accessing DREAM.3D Data Structures and Functionality ###

There are a few examples of the first use case. In one instance a user would like to leverage some of the DREAM.3D functionality within a wider workflow. An example of this would be to access DREAM.3D's EBSD file parsing ability and then feed that data into a histogram generation code to display a histogram. This code is documented in the `DREAM3D/Conda/Python_Examples` folder.