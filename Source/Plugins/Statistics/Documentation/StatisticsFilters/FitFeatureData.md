Fit Feature Data {#fitfeaturedata}
=============

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This Filter will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can chose both which attributes to fit and with what distribution to fit them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.  The user also has the option to remove *biased* **Features** from the fitting process.

*Note:* The array selected must be a scalar array

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Distribution Type | Choice | Specifies distribution type to fit to the data (options include lognormal, power and beta distributions) |
| Remove Biased Features | Boolean | Specifies if biased features are to be omitted from the binning counts. |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | BiasedFeatures | Boolean | (1) | Flag of 1 if **Feature** is biased or of 0 if it is not (only required if *Remove Biased Features* is checked |
| Feature | FitParameters | Variable | (1) | Selected feature array to be fit. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | Statistics | Float | (2) or (3) | Parameters of the fit distribution | 

## Authors ##
**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

