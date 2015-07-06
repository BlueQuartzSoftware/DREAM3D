HDF5 Voxel File (.h5voxel) {#h5voxelvolume}
=========

## Introduction
  The Voxel data from the Reconstruction Module is stored in an HDF5 file for use in the MicrostructureStatistics Module.

## HDF5 File Layout


    /-|
      | "VoxelData" HDF Group
         |- "DIMENSIONS" HDF Dataset
         |- "ORIGIN" HDF Dataset
         |- "SPACING" HDF Dataset
         |- "SCALARS" HDF Group
             |- "Euler Angles" HDF Dataset
             |- "GrainID" HDF Dataset
             |- "PhaseID" HDF Dataset
         |- "FIELD_DATA" HDF Group
             |-"CrystalStructure" HDF Dataset
             |-"PhaseType" HDF Dataset


##  Detailed Description

- The **VoxelData** group has 2 attributes which are


| Attribute Name | Attribute Value |
|--------------------|---------------------|
| NUM_POINTS | 64 Bit Int: Total Number of points (nPoints) |
| VTK_DATA_OBJECT | String with Value "VTK_STRUCTURED_POINTS" |


- Datasets in the **VoxelData** group include:


| Dataset Name | Description |
|--------------|-------------|
| DIMENSIONS | HDF Dataset 64 Bit Int x 3 Values: X, Y, Z Voxel Dims |
| ORIGIN | HDF Dataset 32 Bit Float x 3 Values: X, Y, Z Origin Values |
|SPACING | HDF Dataset 32 Bit Float x 3 Values: X, Y, Z Resolution Values |


- Groups in the **VoxelData** group include:


| Group Name | Attribute Name:Value |
|--------------|-------------|
| FIELD_DATA | "NAME":"FeatureData" |
| SCALARS | none |

- Datasets in the **SCALARS** Group include:

| Dataset Name | Description |
|--------------|-------------|
|Euler Angles | HDF Dataset. Rank=2; Dims=[nPoints, 3], 32 Bit Float |
| GrainID |HDF Dataset. Rank=1; Dims=[nPoints], 32 Bit Int |
|PhaseID | HDF Dataset. Rank=1; Dims=[nPoints], 32 Bit Int |

- Datasets in the **FIELD_DATA** Group include:


| Dataset Name | Description |
|--------------|-------------|
| CrystalStructure | HDF Dataset. 32 Bit Int. List of Phase ID to Crystal  structure mapping. The Zeroth index is always a value of "999". The real list starts with index 1. |
| PhaseType | HDF Dataset. 32 Bit Int. List of Phase ID to Phase Type mapping. The Zeroth index is always a value of "999". The real list starts with index 1.|


##  Enumeration Values for the Crystal Structure

    enum CrystalStructure {
        Hexagonal = 0,
        Cubic = 1,
        OrthoRhombic = 2,
        AxisOrthoRhombic = 3,
        UnknownCrystalStructure = 999
    };

##  Enumeration Values for the Phase Type

    enum PhaseType {
        PrimaryPhase = 0,
        PrecipitatePhase = 1,
        TransformationPhase = 2,
        UnknownPhaseType = 999
    };


 
