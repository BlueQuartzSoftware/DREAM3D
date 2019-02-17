# Building a DREAM3D Conda Package #


## Prerequisites ##

+ Install Anaconda3 onto your system. For the examples below we installed it into `c:/Applications/Anaconda3`
+ Using either Ananconda Navigator or the terminal create a _DREAM3D_ environment. Be sure to install conda-build and numpy at the minimum.
+ **The Python version must match the one used in the CMake build**


## Conda Package Build Instructions ##

Using CMake set the following Variables:
  
    ANACONDA_DIR=c:/Applications/Anaconda3
    ANACONDA_ENVIRONMENT_NAME=DREAM3D
    SIMPL_ENABLE_PYTHON=ON
    SIMPL_ENABLE_ANACONDA_PACKAGE=ON
    SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL=ON
    
These variables can be set through the DREAM3D_SDK.cmake file, through CMake-GUI.exe (by adding them manually), or by adding them manually as command line arguments to the cmake configuration invocation (using the -DXXXXX=YYY style).

6. Configure and generate the build files using CMake
7. Build the project

  Alternatively, you can build the Conda package manually by using “conda build conda-recipe” in the folder where conda-recipe is left

## Conda Package Install Instructions ##

1. Open a terminal / command prompt and navigate to the $BUILD_DIR/Anaconda
2. Run the package.bat (Windows) or package.sh (macOS/Linux) script file which will build the package and optionally install the package in your local Python Environment.

## Testing ##
3. Run python from the command line from the **anaconda environment** that was setup earlier
4. Type “from dream3d import simpl” into interpreter prompt and hit return/enter
  * There should be nothing printed to the screen
5. Type “dca = simpl.DataContainerArray.New()” and hit return/enter
  * There should be nothing printed to the screen
6. Type “dca” and hit return/enter
  * There should be something along the lines of “<dream3d.dream3d.simpl.DataContainerArray object at 0x00000208C665D308>”
7. If there are no issues, DREAM3D has been successfully built and installed to the local Python environment

* Alternatively, you can switch SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL to ON and the dream3d Conda package will be installed to the Python Conda environment given in CMake

## Upload to Anaconda ##

* No changes to CMake are required
* Only need to edit the .condarc file (usually in the home / root directory of the user)
Possible contents of file:
```
channels:
  - <USER ACCOUNT CHANNEL>
  - defaults
ssl_verify: true
anaconda_upload: true
conda-build:
  anaconda_token: < TOKEN FROM ANACONDA CLOUD SITE >
```
* Where USER ACCOUNT CHANNEL is the channel with the same name as the user for Anaconda Cloud and
 the token is generated for the user to copy and past to anaconda_token
* The anaconda_upload option needs to be “true” for the upload to occur and a valid token is required
