# Reference Frame Notes #

When dealing with **orientation** data such as Euler angles, Quaternions and Rodrigues vectors the user will often need to ensure that _both_ the **sample** reference frame and the **crystal** reference frame are in alignment. DREAM.3D makes the following assumptions about reference frames:

+ Passive Rotations are used
+ Rotations indicate the rotation of the **sample** reference frame to the **crystal** reference frame

DREAM.3D can represent orientations in seven different forms:

| Name | Number of Components |
|------|----------------------|
| Eulers | 3 |
| Rodrigues | 4 |
| Orientation Matrix | 3x3 |
| Quaternion | 4 (< x, y, z > w) Note the order of the data. Vector-Scalar |
| Axis-Angle | 4 (< ax0, ax1, ax2 >, w)
| Cubochoric | 3 |
| Homochoric | 3 |

## Master Table of Conversions ##

| From/To |  e   |  o   |  a   |  r   |  q   |  h   |  c   |
|  -      |  -   |  -   |  -   |  -   |  -   |  -   |  -   |
|  e      |  -   |  X   |  X   |  X   |  X   |  a   | ah   |
|  o      |  X   |  --  |  X   |  e   |  X   |  a   | ah   |
|  a      |  o   |  X   | --   |  X   |  X   |  X   |  h   |
|  r      |  o   |  a   |  X   | --   |  a   |  X   |  h   |
|  q      |  X   |  X   |  X   |  X   | --   |  X   |  h   |
|  h      |  ao  |  a   |  X   |  a   |  a   | --   |  X   |
|  c      | hao  |  ha  |  h   |  ha  | ha   |  X   | --   |



## Filters to Assist the User ##

When importing EBSD data through the various readers (.ang, .ctf files) there are a pair of filters that can assist the user in performing these transformations. The following section is a non-exhaustive list of filters that will help the user perform various manipulations on the orientation data.

**NOTE:** This is very important to perform as any data analysis that depends on the orientation data will be potentially incorrect if the correct transformations are not performed.

| Filter | Discussion |
|--------|------------|
| Rotate Euler Reference Frame | This will rotate the Crystal Reference frame. Input Orienation Data is possibly changed |
| Rotate Sample Reference Frame | This will rotate the actual data into new locations in an Image Geometry array. |
| Rodrigues Convertor | DREAM.3D expects Rodrigues vectors to be a 4 component where the 4th component is the length of the vector. This filter will convert 3 component Rodrigues vectors to 4 component vectors |
| Generate Quaternion Conjugate | This uses an input set of Quaternions and generates the conjugate quaternion. A copy of the input array is made and changes are made to that array |
| Generate Orientation Matrix Transpose | This will generate the transpose of a 3x3 matrix. A copy of the input array is made and changes are made to that array |

## Examples of what can go wrong ##

In this section we show some images of data imported from a LabDCT experiment. The LabDCT input data provided 3 Component Rodrigues vectors which were converted to 4 component. Note that while some of the grains match up most do not. This is because LabDCT assumes a **Crystal** to **Sample** transformation which is opposite of what DREAM.3D expects. The discrepency comes from a different interpretation of a _passive_ rotation. There are at least 3 possible ways that the input Rodrigues vectors can be modified to comply with DREAM.3D's assumptions.

| LabDCT Generated IPF 001 Colors | DREAM.3D Generated IPF 001 Colors |
|---------------------------------|-----------------------------------|
| ![Correctly imported orientation data](Images/AlCu-485_LabDCT.png)   | ![Incorrectly imported orientation data](Images/AlCu-485_WRONG.png) |
|   |   |

+ Calculate the negative of the Rodrigues vector
+ Calculate the conjugate of the Quaternion
+ Calcualte the transpose of the Orientation Matrix

![Correctly imported orienation data to conform to DREAM.3D's assumptions](Images/AlCu-485_Transformed.png)

