# Building a DREAM3D Conda Package #

## Conda Package Build Instructions ##

1.	Download and install Anaconda and/or Miniconda
2.	Create environment in conda
    1.	conda create --name myenv
    2.	Where myenv is the name of the environment
    3.	The Python version must match the one used in the CMake build
3.	In CMake-gui (or direct edit of CMake cache), set SIMPL_WRAP_PYTHON and SIMPL_PYTHON_BUILD_CONDA_PACKAGE to ON
4.	Then set the PYTHON_CONDA_DIRECTORY to the location of your Anaconda / Miniconda (the one where you created the environment “myenv”)
5.	Then set the PYTHON_CONDA_ENVIRONMENT_NAME to the environment name created in step 2
6.	Configure and generate the build files using CMake
7.	Build the project

- Alternatively, you can build the Conda package manually by using “conda build conda-recipe” in the folder where conda-recipe is left

## Conda Package Install Instructions ##

1.	In a terminal / command prompt, activate the environment you created
    * Typically, in the Scripts subdirectory of the Anaconda / Miniconda installation there is an activate.bat file
2.	After the environment is activated, run: “conda install --use-local dream3d”
    * Install the dream3d
    * Possible issue: conda is not available as a command
    * Return to the Scripts directory and run “conda install conda-build” and install necessary dependencies. This should add conda to this environment’s path
3.	Run python from the command line
4.	Type “from dream3d import simpl” into interpreter prompt and hit return/enter
    * There should be nothing printed to the screen
5.	Type “dca = simpl.DataContainerArray.New()” and hit return/enter
    * There should be nothing printed to the screen
6.	Type “dca” and hit return/enter
    * There should be something along the lines of “<dream3d.dream3d.simpl.DataContainerArray object at 0x00000208C665D308>”
7.	If there are no issues, DREAM3D has been successfully built and installed to the local Python environment

* Alternatively, you can switch SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY to ON and the dream3d Conda package will be installed to the Python Conda environment given in CMake

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
