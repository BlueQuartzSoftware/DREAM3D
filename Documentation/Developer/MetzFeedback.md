# Metz Workshop June 13,14 2013 Feedback #

## Jerome Ferarri ##

### Ergonomically ###

I am against space wasting and I feel that on the right part of the GUI, less space could be left between functions without loosing in clarity.

### Mesh from Binary Images ###

If I want to make a mesh from a stack of binary images, I need to import a "signed 32 bit image" otherwise "M3C surface meshing" or "Quick surface mesh" complain. Furthermore the XDMF format doesn't accept 16 bit. This forces the user to **1**-be aware of that **2**-being able to convert images. It would make things easier for the user if Dream3D could manage this kind of problem by converting format when needed.

### Image Import Plugin ###

The module "Import images (3D stack)" doesn't allow an easy import of a single image. Maybe a new dedicated module should be created or a check box should toggle on and off the numbering so that any file name without number can be given.

### License Information ###

I was unable to find where it is stated that your code is under BSD license. It should probably appears clearly in the license web page and in the code.

### Misc Information ###
Just as a reminder, the command line that transforms a tiff image into a signed 32bit raw is :
  gmic input_file_name.tif -type int -output input_file_name.raw
to go from an unsigned 16bit raw to a signed 32bit :
  gmic -type ushort input_file_name.raw -type int -output input_file_name.raw
This should work under MacOS : http://gmic.sourceforge.net/

### HDF5 .med Format Support ###

You expressed some interest about the softwares we use and notable about the MED format. Here are some links. I will send you a mesh inside a med file next week.
http://www.salome-platform.org/
http://www.code-aster.org/V2/spip.php?rubrique1
http://www.salome-platform.org/about/med


## Lionel Germain ##

1. You have to test what happened if dream3d is on a remote drive and if the directories are only indicated \Data\etc ...
2. Maybe rename the "minim size filter" into "minimum size filter and fill bad data" ??
3. Possibility to export the alignement list
4. Export slice XY/ZY/XZ to a 2D CTF or ANG?
5. The meshing is nice to smooth, if we could get the meshing back to "voxels" could be intersting

## Ben Britton ##

**Data history** 