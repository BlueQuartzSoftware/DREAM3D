Fit Distribution to Feature Data {#fitfeaturedata}
=============

## Group (Subgroup) ##
Statistics (Ensemble)

## Description ##
This **Filter** will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can chose both which attributes to fit and with what distribution to fit them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.  The user also has the option to remove *biased* **Features** from the fitting process.

*Note:* The array selected must be a scalar array

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Distribution Type | Enumeration | Specifies distribution type to fit to the data (options include lognormal, power and beta distributions) |
| Remove Biased Features | Boolean | Whether biased features are to be omitted from the binning counts |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | FitParameters | Any | (1) | Selected feature array to be fit |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Feature Attribute Array** | BiasedFeatures | bool | (1) | Flag of 1 if **Feature** is biased or of 0 if it is not. Only required if *Remove Biased Features* is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | Statistics | float | (2) or (3) | Parameters of the fit distribution | 

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


