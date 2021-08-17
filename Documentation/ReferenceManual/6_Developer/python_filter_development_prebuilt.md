# Writing a DREAM.3D Filter using Python Language #

If you are just wanting to use DREAM3D from a python script or write new filters using python language then this is the best solution for you.

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

In order for the DREAM.3D UI Application to pick up your filters you will need to tell DREAM3D where to look for your filters. This is done by setting the **SIMPL_PYTHONPATH** environment variable. In this example we have created a new folder called **PyD3D** and then we are going to set the **SIMPL_PYTHONPATH** environment variable to point to this folder. Note that the **SIMPL_PYTHONPATH** can have _multiple_ folders listed. Each folder should be delimited by a ';' (Windows) or a ':' (Unix/Linux/macOS)

```lang-console
(d3d_embed) C:\Users\johnsmith>mkdir PyD3D
(d3d_embed) C:\Users\johnsmith>set SIMPL_PYTHONPATH=C:\Users\johnsmith\PyD3D
```

## Writing DREAM3D filters in Python ##

Inside `SIMPL/Source/SIMPLib/Python`, there are two examples of filters written in Python, `initialize_data.py` and `ExampleFilter.py`.

Filters must derive from the abstract base class `dream3d.Filter` and implement its methods similiary to C++ filters. Then inside the Python file, a global variable `filters` must exist which should be a list of types where each type is a filter.

To use Python filters in DREAM3D or PipelineRunner, create an environment variable `SIMPL_PYTHONPATH` by exporting in from the *anaconda prompt* like this:

```lang-console
(d3d_embed) C:\Users\johnsmith> set SIMPL_PYTHONPATH=C:\Users\johnsmith\DREAM3D-Dev\SIMPL\Source\SIMPLib\Python
```

If you have multiple directories that you want **DREAM3D** to search for python filters set the `SIMPL_PYTHONPATH` to a list of directories. For Windows use a `;` to separate the directories. For Linux/Unix/macOS use the `:` to separate each directory. If there are **spaces** anywhere in the path you **MUST** use double quotes around the path.

If using the DREAM3D GUI, Python filters can be reloaded at runtime. DREAM3D will clear all existing Python filters and attempt to reload them from directories in `SIMPL_PYTHONPATH`. Currently loaded pipelines with Python filters will be preserved if the relevant filters still exist in those directories.

If you want to use a Python filter with `simpl.FilterPipeline`, you must first wrap in a `simpl.PythonFilter`.


## Major Parts of a DREAM.3D Filter ##

### Filter Parameters ###

This function defines the various inputs that your filter will need to run. These include external file/folder locations, various scalar or vector values, paths to specific data arrays and any other information that your filter will need to run. DREAM.3D has many types of *FilterParameters* to accomplish this task. Please take a look through the source codes to find a filter parameter that works for your needs.

```{.python}
def setup_parameters(self) -> List[FilterParameter]:
```

### Preflight ###

DREAM.3D has the idea of a *preflight* function whose responsibilty is to sanity check all of the inputs to a filter and report errors or warnings if those inputs do not meet your specifications. For instance, if a filter requires an input file, the **data_check()** function would be the place to ensure that the file does exist. If if an integer falls within a specified bounds.

```{.python}
 def data_check(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
```

### Execute ###

The **execute()** function is where the actual filter algorithm will be implemented. You are free to have other functions as needed but this is the main entry point to the filter's algorithm. The function signature is:

```{.python}
  def _execute_impl(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
```

### Making Filters Unique ###

Each filter is assigned a UUID in code that uniquely identifies the filter to DREAM.3D. It is **extremely important** to generate a UUID and **NOT** simply copy/paste from another filter. A very quick way to generate a random UUID from python is to run the following code either in an interactive terminal or through a python file:

```lang-console
(pyd3d) C:\Users\johnsmith>python
>>> import uuid
>>> uuid.uuid4()
UUID('f8035905-9882-4423-a2ab-65bc37a77c7e')
```

Take the output from those commands and implement the "uuid()" function in your filter.

```{.python}
  @staticmethod
  def uuid() -> str:
    return '{f8035905-9882-4423-a2ab-65bc37a77c7e}'
```

### Misc Other Functions ###

A few of the other required functions should be filled in for your specific filter. These include:
+ 'name'
+ 'group_name'
+ 'sub_group_name'
+ 'human_label' where the 'human_label' is the most important as that is what will show up to the user in the DREAM.3D user interface.

## Creating a Python Filter (An Example) ##

Use the file SIMPL/Source/SIMPLib/Python/Example.py as the base file. Copy the contents into a new python file.

### Change the Class Name ###

Do a search and replace on `ExampleFilter` and change it to the name of your filter.

### Change the UUID ###

```lang-console
(pyd3d) C:\Users\johnsmith>python
>>> import uuid
>>> uuid.uuid4()
UUID('f8035905-9882-4423-a2ab-65bc37a77c7e')
```

copy the result and replace the UUID value in your source.

### Names and Descriptions ##

Update the methods `name`, `group_name`, `sub_group_name`, `human_label`, `version` and `compiled_library_name` to return the proper strings for your filter.

### Adding Parameters ###

This section is best done as an example. Let's say that we want our filter to take in a floating point value called `Temperature` from the user. There are several pieces of code that we need to write.

+ Create a variable in the `__init__` section, for instance:

```{.python}
self.temperature_param:float = 0.0
```

+ Create a getter and setting method for the variable

```{.python}
def _set_temperature_param(self, value: float) -> None:
    self.temperature_param = value`
def _get_temperature_param(self) -> float:
    return self.temperature_param
```

+ Decide on the type of SIMPL Filter Parameter that you will use to get the value from the user interface into the filter. For this example case we are going to use the `FloatFilterParameter`. There are many other classes of *FilterParameters*. (Insert list somewhere)

```{.python}
from dream3d.simpl import FloatFilterParameter
```

Lastly create an entry in the list of FilterParameters for the filter connecting the `self.temperature_param` to the DREAM.3D application. If your filter will have more than one *filter parameter* then you will create a comma delimited list in this section.

```{.python}
  def setup_parameters(self) -> List[FilterParameter]:
    return [
        FloatFilterParameter('Integer', 'temperature_param', self.temperature_param, FilterParameter.Category.Parameter, 
                              self._set_temperature_param, self._get_temperature_param, -1)
    ]
```


