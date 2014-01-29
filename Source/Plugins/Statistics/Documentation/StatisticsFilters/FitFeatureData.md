Fit Feature Data {#fitfeaturedata}
==========

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This filter will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can chose both which attributes to fit and with what distribution to fit them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Feature Array To Fit | String | Feature to be fit to a distribution. |
| Distribution Type | Choice | Choose from a lognormal, power and beta distribution. |
| Remove Biased Features | Boolean | TRUE if biased features are to be omitted from the binning counts. |

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not. | | Find Biased Features (Generic) |
| Feature | SelectedFeature | Selected feature to be binned. | | |

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | Statistics |  |  |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

