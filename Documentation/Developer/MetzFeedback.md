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

**Data history** – it would be good to have an ‘export’ for the dream3d pipeline. Could you also inherit the pipelines from prior data sets (i.e. when you do a read step, copy the pipeline from the prior steps)? It would be good to also have a ‘generate pipeline favourite from dream3d file’?
 
+ It would be good to have a few pointers on where to find key tutorials/features from paraview.
+ (I like that you have attributed a personality to Dream3D e.g. ‘Dream3D would like to pick names for you’.)
+ For export CSV – I think that it would be good to have a flexible output of different fields (maybe two filters for this – the default CSV export and a custom CSV output).
+ Removing the .ini for a favourite pipeline when Dream3D can result in no action when adding files.
+ For the output file, the ‘\’ in the file path is replaced with ‘/’.
+ Is it possible to maintain a ‘cleaned points’ file (i.e. that keeps track of points which have changed due to clean up operations)?
+ Can we have an ‘export colour keys’ (e.g. for the IPF coloring) as images from the pipeline?
+ The grainID step leaves a ‘skirt’ on the bottom of one of the slices. 
+ For the twin separation – I **think** that separating the misorientation axis/angle values is perhaps slightly ‘unfair’ on the data uncertainty. Formally there should be 3 degrees of freedom that describe the misorientation between the perfect twin orientation and a measured one. Maybe this means that the misorientation ‘error’ (or tolerance) should be described as something in terms of the (unit) quaternion error (sorry not sure what this actually is!)?
 + [From a module teaching standpoint – it could be handy to find some software which gives you a load of ‘ready buttons’ which connect to a network server to indicate when people are ready for the next step/have problems/still working – I suspect that something is already out there]
+ Can extract the pileline history from the dream3d file?
+ Euclidian distance – is this a distance to the edge of a voxel, or can we also get a distance to a reconstructed face? (It would be good to have the option for both.) Would it be possible to name these “Euclidian_GB”, “Euclidian_TJ” etc. (Not sure if there is a ‘style guide’ on how you determine variables. Formally I think that Manhatten distance is a different metric to Euclidian (e.g. Pythagorean) distance. [Not a great reference I know! http://en.wikipedia.org/wiki/Euclidean_distance]. Can we also have some Euclidian metrics for twin parents structures?
+ It would be great to have a ‘flat land’ tutorial - also including some translation/algorithm switches for a 2D dataset.
+ .ang files – there’s a slight change between OIM6 vs OIM5 Analysis (see attached for a mini two phase data set from Si/SiN). The ‘other ang’ files are not quite ang files – they are .txt files that look like .ang files (but they don’t have PC etc listed, and have some other things like GrainID, and can be cleaned up in 2d).
+ Favorites – can we have an ‘update favourite’ (maybe this is the append button, but not clear)?
+ Distributions – grain sizes etc. can we exclude grains on edge of the data set (I think this is covered by one of the filters!)?
+ KAM in 3d – we know that the grid is slightly misshapen due to the translations required to align the slices. I think this *could* give some problems when you want to consider KAM.
+ For the IPF colours – can you give the option to have a few IPF colour outputs (e.g. so that we can generate 001, 010 and 100 IPFs from the same dataset and switch between them within the same data set in paraview) – I think this will need a new textID for each IPF colouring.
 

 
**Noting the list that Tony put on the board**

+ Gradients in grain size.
+ Data translation – extra inputs.
+ Combine measured EBSD (+3D slices, e.g. from DAXM) w/ synthetic 3D.
+ Distinguish 1p from 3p deltaG. (Care with misorientation relationships).
+ Intensity based image.
+ Volume meshing (e.g. Abaqus) – link info to each mesh point.
+ Data fusion.
+ Name of GrainID (which is a field).
+ Oblique slicing to export .ang/.ctf.
+ Partitioning by misorientation type.
+ Exporting data – using R?
+ How to slice in paraview + export.
+ 2D data, guess 3D stats, generate 3D.
+ Merge some sequences of filters (cleaner GUI). [Can we have a ‘meta filter’ built from a favourite pipe-line – i.e. not adding 3 steps within the parent pipe.]
+ Offer ‘advanced’ vs ‘easy’ settings.
