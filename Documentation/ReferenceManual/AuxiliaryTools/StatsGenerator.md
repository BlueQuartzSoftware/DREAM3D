Stats Generator {#statsgenerator}
========

## Introduction ##
In order to allow the user to quickly generate a set of statistics that can be used to create a synthetic microstructure, the “StatsGenerator” program was created. Using this program, the user can generate the necessary statistics that describe a microstructure and save those statistics to a DREAM3D file that can then be used in a DREAM3D pipeline to generate a synthetic microstructure.
The main features are:
+ Describe the grain size distribution via a preset Gaussian distribution
+ Select from several preset microstructure types
+ Define the Omega 3 Distributions
+ Define the Shape distributions
+ Define the Neighbor distributions
+ Define a Crystallographic Texture through ODF parameters
+ Define the MDF through various parameters
+ Define the Axis ODF for the grains
+ Add as many phases as the user needs
+ Define each phase according to several presets
    - Primary
    - Precipitate
    - Transformation
    - Matrix
    - Boundary

## Size Distribution Tab ##

![StatsGenerator Main Window](Images/image001.png)

@image latex Images/image001.png "StatsGenerator Main Window" width=6in 


  + **Mu** This is the average value of the lognormal grain size distribution.
  + **Sigma** This is the standard deviation of the lognormal grain size distribution.
  + **Sigma Cut Off Value** This allows the user to truncate the distribution to remove very large grains.
  + **Bin Step Size**  This is the size of bin to use in segregating the grains into size classes for correlating other statistics to grain size. Note that the Bins to be Created value is displayed in the bottom left corner.
  + **Preset Statistic Models** This allows the user to select a _morphological-type_ of microstructure to populate the default data.
    - Default: This populates the statistic tabs with generic random data that may not create a _buildable_ microstructure. This option should only be used if the user is going to enter the values on all the  statistics tabs themselves.
    - Equiaxed: this populates the statistic tabs with data that is  designed to generate a random equiaxed microstructure.
    - Rolled: this populates the statistic tabs with data that is designed to generate a rolled microstructure with elongated grains with user defined aspect ratios.
        - Aspect Ratio 1: this is the aspect ratio between the dimensions of the grains in the rolling direction and transverse direction, respectively.
        - Aspect Ratio 2: this is the aspect ratio between the dimensions of the grains in the rolling direction and normal direction, respectively. This value must be larger than Aspect Ratio 1 or the default values will be wrong.
  + **Phase Properties**
  This is the area where the user can enter information about the phase for
  which statistics are currently being generated. The Plus Button allows the
  user to add a phase, the Minus Button allows the user to remove a phase and
  the Wheel Button allows the user to edit the currently selected phase.
  
![Phase Properties](Images/image002.png)

@image latex Images/image002.png "Phase Properties" width=6in 

+ Select Crystal Structure: This allows the user to specify the crystal structure of the phase. Currently, the two options are Cubic and  Hexagonal.
+ Fraction: This is the volume fraction of the phase. The Calculated Phase Fraction is updated as more phases are added, by scaling the current total of all the phases fractions to 1 (in case the user's total is not equal to 1 when finished).
+ Select Phase Type: This specifies the type of the phase. Currently, the two types of phases are Primary and Precipitate. Note that there must be at least one Primary phase before any Precipitate phase can be created or the Synthetic Builder will fail.
+ Fraction of Precipitate on Boundary: If the phase type is set to  Precipitate, then the user must specify the number fraction of the precipitates that are located on grain boundaries of the primary phase. This value will be scaled to 1 if the user's value is larger than 1. The value is keyed to -1 for Primary phases.
+ **Create Data** This locks in the values the user has entered and populates the other tabs with default values. The user can move through the other tabs and change any default values if desired.


## Omega3 Tab ##

![Omega 3 Gui](Images/image003.png)

@image latex Images/image003.png "Omega 3 Gui" width=6in 

+ Bin: This column is calculated from the size distribution and cannot be changed.
+ Alpha: This is the alpha parameter of a Beta distribution. Omega 3 is normalized and can only be between 0 and 1 to insure that the Beta distribution is a good fit.
+ Beta: This is the beta parameter of the Beta distribution.
+ Color: This allows the user to change colors of the curves for
  image creation or easier identification during stats generation.
  

## Shape Distribution Tab ##

![Shape Distribution Function GUI](Images/image004.png)

@image latex Images/image004.png "Shape Distribution Function GUI" width=6in 
  
+ Bin: This column is calculated from the size distribution and cannot be changed.
+  Alpha: This is the alpha parameter of a Beta distribution. B/A, C/A and C/B are normalized and can only be between 0 and 1 to insure that the Beta distribution is a good fit.
+ Beta: This is the beta parameter of the Beta distribution.
+ Color: This allows the user to change colors of the curves for image creation or easier identification during stats generation.
  


## Neighbor Distribution Tab ##

![Neighbor Distribution Function GUI](Images/image005.png)

@image latex Images/image005.png "Neighbor Distribution Function GUI" width=6in 

+ Bin: This column is calculated from the size distribution and cannot be changed.
+ Alpha: This is the alpha parameter of a Power Law distribution.
    + This is the exponent of a Power Law Distribution
+ Beta: This is the beta parameter of a Power Law distribution.
+ Color: This allows the user to change colors of the curves for image creation or easier identification during stats generation.
  


## ODF Tab ##

![ODF Gui](Images/image006.png)

@image latex Images/image006.png "ODF Gui" width=6in 

**Weights and Spreads Sub-Tab**

- Euler 1-3: These are the Euler angles that define an orientation that the user would like to increase in weight.
- Weight: This is the weight in MRD (multiples of random distribution).
- Sigma: This is the spread to use in blurring out the orientation chosen. The value corresponds to the number of bins in Rodrigues (orientation) space that it takes for the MRD value entered in the Weight column to reduce to 0.0 (decreasing quadratically from the bin of the entered orientation).
- Calculate ODF: This builds the ODF and then creates pole figures (PFs) for the user to inspect.
      
    
**Pole Figure (PF) Sub-Tabs**

- There are three PFs formed for each of the crystal structures that can be chosen (though they are of different directions for the different crystal structures).
      
    
**MDF Sub Tab** This sub-tab will display the baseline MDF for the generated ODF.  The  implemented algorithm proceeds by randomly sampling pairs of orientations from  the ODF and calculating the misorientation (axis-angle only). Only the angle is plotted in the misorientation distribution plot.  The user can also add axis-angle pairs to increase in weight.

![MDF Gui](Images/image007.png)

@image latex Images/image007.png "MDF Gui" width=6in 
  
- Angle: This is the angle of the misorientation to increase in weight.
- Axis: This is the axis of the misorientation to increase in weight.  If the crystal structure being used for the phase is Hexagonal, then this axis is in the 3-index, orthogonal convention, not the true (hkil) convention.
- Weight: This is the weight in units of MRD (multiples of random) of the entered misorientation.
    
    
 


## Axis ODF Tab ##

![Axis ODF Gui](Images/image008.png)
  
@image latex Images/image008.png "Axis ODF Gui" width=6in 

**Weights and Spreads Sub-Tab**
      
- Euler 1-3: These are the Euler angles that define an orientation that the user would like to increase in weight.
- Weight: This is the weight in MRD (multiples of random) to be assigned to the orientation listed.
- Sigma: This is the spread to use in blurring out the orientation chosen. The value corresponds to the number of bins in Rodrigues (orientation) space it takes for the MRD value entered in the Weight column to reduce to 0.0 (decreasing quadratically from the bin of the entered orientation).
- Calculate ODF: This builds the ODF and then creates pole figures (PFs) for the user to inspect.
      
    
**Pole Figure (PF) Sub-Tabs**
      
There are three pole figures formed, which correspond to the location of the 3 principal axes of the grains to be generated (i.e a > b > c).
