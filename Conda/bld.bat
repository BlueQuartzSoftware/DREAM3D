if not exist "%PREFIX%\Menu" mkdir "%PREFIX%\Menu"
copy "%SRC_DIR%\DREAM3D\Conda\menu-windows.json" "%PREFIX%\Menu"
copy "%SRC_DIR%\DREAM3D\Resources\OpenSourceEdition\icons\ico\DREAM3D.ico" "%PREFIX%\Menu"

copy "%SRC_DIR%\DREAM3D\Conda\run_dream3d_win.py" "%PREFIX%\Scripts"

mkdir build
cd build

cmake -S "%SRC_DIR%/DREAM3D" -B . -G "Ninja" ^
  -D CMAKE_BUILD_TYPE:STRING=Release ^
  -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
  -D CMAKE_SYSTEM_PREFIX_PATH:PATH="%LIBRARY_PREFIX%" ^
  -D QWT_INSTALL:PATH="%LIBRARY_PREFIX%" ^
  -D DREAM3D_EXTRA_PLUGINS:STRING="DREAM3DReview;ITKImageProcessing;SimulationIO;UCSBUtilities" ^
  -D SIMPL_WRAP_PYTHON:BOOL=ON ^
  -D ITKImageProcessing_ENABLE_ITK_MONTAGE:BOOL=OFF ^
  -D ITKImageProcessing_ENABLE_ITK_TOTAL_VARIATION:BOOL=OFF ^
  -D ITKImageProcessing_LeanAndMean:BOOL=OFF ^
  -D SIMPL_USE_MULTITHREADED_ALGOS:BOOL=ON ^
  -D SIMPL_EMBED_PYTHON:BOOL=ON ^
  -D DREAM3D_DATA_DIR:PATH="%SRC_DIR%/DREAM3D_Data" ^
  -D DREAM3D_DISABLE_DEPENDENCY_COPY_INSTALL_RULES:BOOL=ON ^
  -D SIMPL_Group_REST:BOOL=OFF ^
  -D BUILD_TESTING:BOOL=OFF ^
  -D SIMPL_BUILD_TESTING:BOOL=OFF ^
  -D EbsdLib_ENABLE_TESTING:BOOL=OFF ^
  -D H5Support_BUILD_TESTING:BOOL=OFF ^
  -D DREAM3D_ANACONDA:BOOL=ON ^
  -D PYTHON_EXECUTABLE:FILEPATH="%PYTHON%" ^
  -D MKDOCS_PYTHON_EXECUTABLE:FILEPATH="%PYTHON%" ^
  -D SIMPLView_BUILD_DOCUMENTATION:BOOL=ON
if errorlevel 1 exit 1

cmake --build . --target all
if errorlevel 1 exit 1

cmake --build . --target install
if errorlevel 1 exit 1

cd python_package
%PYTHON% setup.py install
if errorlevel 1 exit 1
