: Greg Loughnane
: DREAM.3D Grain Generator & Microstructure Statistics Combination Batching Code
: August 11, 2011

: Description
REM THIS BATCH FILE RUNS THE COMMAND LINE VERSIONS OF GRAIN GENERATOR AND MICROSTRUCTURE STATISTICS FOR A GIVEN VOXEL EDGE LENGTH AND MICRON RESOLUTION

: Define the directory where the files are located
REM **** Ensure that a file named batch_count.txt file with a list of desired iteration counters (i.e. 01[enter]02[enter]03[enter]04[enter]...) exists in the directory you enter below ****
REM **** Ensure that a StatsGenerator input file (.h5 file) also exists in the directory entered ****
@echo off
echo.
set /P directory=Enter the file directory where files should go. This is also where the batch_count and StatsGenerator files should be: 
set /P stats_file=Enter the name of the file to use for grain generation that was created using StatsGenerator: 
set /P program_directory=Enter the location of the GrainGenerator and MicroStats executable program files: 

: Define error weights and shape type for grain generation
REM ERROR WEIGHTS FOR GRAIN GENERATIONS ARE 1.0 BY DEFAULT
REM SHAPE TYPE IS ELLIPSOID BY DEFAULT
set size_error="1"
set neigh_error="1"
set space_error="1"
set shape_type="1"

: Define bin size for statistical correlations
REM BIN SIZE FOR CORRELATION PARAMETERS IS 0.50 BY DEFAULT
set binsize="0.5"

: Set user input parameters
set /P edge=Enter the edge length of the cube: 
set /P res=Enter the resolution of the cube: 

set edges=%edge%,%edge%,%edge%
set reso=%res%,%res%,%res%

: **** Run Grain Generator
:: First change the directory to the appropriate one for opening command line program
cd %program_directory%
for /f "tokens=1" %%Y in (%directory%\batch_count.txt) do (GrainGenerator -i %directory%\%stats_file% --voxels %edges% --resolution %reso% --sizedisterror %size_error% --neighborhooderror %neigh_error% --spacefillerror %space_error% --outputdir %directory% --outputprefix %edge%_%%Y_ %* --no-surfacevoxels --no-phase --no-ipf --vtk_ascii --no-phfile)

:: ****Notes about the Grain Generator for-in-do statement****
:: Y is arbitrary
:: /f denotes Iterating and File Parsing: the executable is run and outputs to the sub-directory %directory% with iterated file names based on the file batch_count.txt 
:: in (%directory%\batch_count.txt) searches the working directory for a batch_count.txt file that NEEDS TO BE THERE with a list of iteration numbers
:: "tokens=1" takes the iteration numbers from the batch_count.txt file and uses them as prefixes for the file names
:: Additional options are available for writing output files - run graingenerator.exe in the command prompt for details

: **** Run Microstructure Statistics
:: First change the directory to the appropriate one for opening command line program
for /r %directory% %%X in (*.h5voxel) do (MicroStats -i %%X --outputdir %directory% --binstepsize %binsize% --outputprefix %%~nX %* --no-h5stats --no-vtk --no-surfacevoxels --no-phase --no-ipf --no-kernel-miso)
pause

:: ****Notes about the Microstructure Statistics for-in-do statement****
:: X is arbitrary
:: /r denotes Recursive: the executable is run recursively through the sub-directory %directory%
:: in (*.h5voxel) searches the directory for.h5voxel files and collects their file names including each entire file path
:: ~n takes the file name (without extension) and uses it as an output prefix for the statistics files
:: Additional options are available for writing output files - run microstats.exe in the command prompt for details