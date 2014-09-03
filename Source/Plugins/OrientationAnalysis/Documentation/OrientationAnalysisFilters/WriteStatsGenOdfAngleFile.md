Write StatsGenerator ODF Angle File{#writestatsgenodfanglefile}
=====

## Group (Subgroup) ##

IOFilters (Output)


## Description ##

This filter is used in a workflow where the user would like to generate a synthetic microstructure with an ODF that matches (as closely as possible) an existing experimental data set or other data set that is being mimiced. The basic workflow is the following:

0. Import or Read in Euler Angle data
1. Optionally threshold the data so each cell is marked as allowable or not-allowable.
2. Insert the "Write StatsGenerator IDF Angle File" filter in the pipeline.
3. Run the pipeline to produce the file.
4. Launch StatsGenerator
5. Generate the data
6. Click on the ODF Tab
7. Click on the *Bulk Load* sub tab
8. Select the file that was just written
9. Load the data and inspect the ODF that was generated.

## Notes on Implementation ##

+ A separate file is written for each phase.
+ Spaces are used as the delimiters between values
+ Default values of 1.0 are used for both the weight and sigma
+ Because DREAM3D typically works with Radians, the Euler Angles are converted to Degrees by default. This is a user configurable option



## Parameters ##
| Name             | Type |
|------------------|------|
| Output File | File Path |  


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|--------|----------------------------|-----------------|---------------|
| Int  | Cell Phases                   | The phase data for each cell                 |    |  
| Float | Cell Euler Angles      | The Euler Angles for each cell        |    |  
| Bool | Convert to Degrees    | Should the filter convert the output to Degrees      |    |  
| Bool  | Apply to Good Voxels | Should only cells marked as **good** be used.     |    |  

## Created Arrays ##

NONE

## Authors ##

**Copyright** 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
