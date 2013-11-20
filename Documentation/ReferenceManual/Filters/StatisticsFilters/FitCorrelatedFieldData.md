Name of Filter {#fitcorrelatedfielddata}
=====

## Group (Subgroup) ##
Filter Group (Filter Subgroup)


## Description ##
This filter will correlate one **Feature** with another **Feature** and bin the result.

## Parameters ##
| Name             | Type |
|------------------|------|
| Feature Array To Fit | String | Feature to be fit to a distribution. |
| Distribution Type | Choice | Choose from a lognormal, power and beta distribution. |
| Array To Correlate With | String | Feature to correlate with the Selected Feature Array To Fit. |
| Number Of Bins For Correlated Array | Integer | |
| Remove Biased Features | Boolean | TRUE if biased features are to be omitted from the binning counts. |

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not. | | Find Biased Features (Generic) |
| Feature | SelectedFeature | | | |
| Feature | Correlated Feature | | | |

## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Ensemble | Statistics |  |  |



## Authors ##

**Copyright** 2013 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
