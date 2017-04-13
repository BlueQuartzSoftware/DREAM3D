Native DREAM.3D File Format {#nativedream3d}
===========

DREAM.3D uses [HDF5](http://www.hdfgroup.org) as its native file format, which means that _any_ software package capable of reading an HDF5 file can read a DREAM.3D file. HDF5 stores data in a hierarchical format with complete descriptors for the data stored in the file. HDF5 is open source and a number of tools exist that allow a user to view and manipulate HDF5 data files. One such tool from The HDF Group is the free [HDFView](http://www.hdfgroup.org/hdf-java-html/hdfview/index.html#download_hdfview), which is a Java based program that can view and edit HDF5 data files. 

The version number for the .dream3d file format is incremented independently of the version number for the DREAM.3D applicaiton. The current .dream3d file format version is 7.0. The general layout of a version 7.0 .dream3d file is as follows:

+ DataContainerBundles (_Group_)
	+ MetaAttributeMatrix (_Group_)
		+ MetaData (_Dataset_)
 	+ MetaAttributeMatrix2 (_Group_)
 	+ .
 	+ .
 	+ MetaAttributeMatrixN (_Group_)
+ DataContainers (_Group_)
	+ DataContainer1 (_Group_)
		+ \_SIMPL\_GEOMETRY (_Group_)
			+ DIMENSIONS (_Dataset_)
			+ ORIGIN (_Dataset_)
			+ RESOLUTION (_Dataset_)
		+ AttributeMatrix1 (_Group_)
			+ AttributeArray1 (_Dataset_)
			+ AttributeArray2 (_Dataset_)
			+ .
			+ .
			+ AttributeArrayN (_Dataset_)
		+ AttributeMatrix2 (_Group_)
		+ .
		+ .
		+ AttributeMatrixN (_Group_)
	+ DataContainer2 (_Group_)
		+ \_SIMPL\_GEOMETRY (_Group_)
			+ SharedVertexList (_Dataset_)
			+ SharedTriList (_Dataset_)
		+ AttributeMatrix1 (_Group_)
			+ AttributeArray1 (_Dataset_)
			+ AttributeArray2 (_Dataset_)
			+ .
			+ .
			+ AttributeArrayN (_Dataset_)
		+ AttributeMatrix2 (_Group_)
		+ .
		+ .
		+ AttributeMatrixN (_Group_)
	+ .
	+ .
	+ DataContainerN (_Group_)
+ Pipline (_Group_)
	+ Pipeline (_Dataset_)

## File Format Details ##

+ _DataContainerBundles_: A _Group_ that stores a information about collections of **Data Containers** called _bundles_. A _bundle_ is a group of **Data Containers** that has been tagged as belonging to one another for a particular reason. For example, a **Data Container Bundle** may exist for a simulation through time, where each **Data Container** represents a simulation time step. There is only ever one _DataContainerBundle_ _Group_ at the top level of a .dream3d file.
	+ _MetaAttributeMatrix_: A _Group_ that stores a set of _metainformation_ about why a particular _bundle_ exists. A _MetaAttributeMatrix_ will exist for each _bundle_ in the data structure. 
		+ _MetaData_: A _Dataset_ that stores the raw _metainformation_ for the grouping rules behind a particular _bundle_. There may be more than one _MetaData_ array for each _MetaAttributeMatrix_.
+ _DataContainers_: A series of _Groups_ that correspond to the **Data Containers** of the data structure. These _Groups_ hold the majority of the raw data from DREAM.3D, and include storage for the **Geometry** information. There is only ever one _DataContainers_ _Group_ at the top level of a .dream3d file.
	+ _DataContainer_: A _Group_ that holds a set of **Attribute Matrices** and a unique **Geometry**. There may be many _DataContainer_ _Groups_ within the _DataContainers_ top level _Group_.
		+ *\_SIMPL\_GEOMETRY*: A _Group_ that stores the **Geometry** information for a **Data Container**. Only one *\_SIMPL\_GEOMETRY* _Group_ will exists in a _DataContainer_.
			+ *GeometryInfo*: A collection of _Datasets_ that define the **Geometry** for the parent **Data Container**. For the case of an **Image Geometry**, these will only be the _DIMENSIONS_, _ORIGIN_, and _RESOLUTION_, each of which are 3-vectors. For other **Geometries**, there will always be a _SharedVertexList_ that lists the vertex coordinates, and a _SharedElementList_ that lists the primary unit element as indexing into the _SharedVertexList_. There may be additional arrays at this level as well, such as a _SharedEdgeList_ or an _ElementNeighbors_ list.
		+ _AttributeMatrix_: A _Group_ that stores **Attribute Arrays**. There may be many _AttributeMatrix_ _Groups_ in each _DataContainer_ _Group_. 
			+ _AttributeArray_: A _Dataset_ that holds raw data. These _Datasets_ hold the actual array data of the data structure.
+ _Pipeline_: A _Group_ that stores JSON information about the **Pipeline** that was run to produce the data structure present in the _DataContainerBundles_ and _DataContainers_ _Groups_. The information stored in this _Group_ allows for the data provenance to be maintained. There is only ever one _Pipeline_ _Group_ at the top level of a .dream3d file.
	+ _FilterId_: A _Group_ that holds the _FilterParameters_ for a given **Filter** in the **Pipeline**. This _Group_ will be named a number that corresponds to the _index_ of the **Filter** in the **Pipeline**.
		+ _FilterParameter_: A _Dataset_ that holds the information about the setting of a _FilterParameter_ for its parent **Filter**. 


Any _Group_ or _Dataset_ may also have HDF5 _attributes_ stored on it as _metadata_. For example, the _component dimensions_ are stored as an attribute on an _AttributeArray_ _Dataset_. These attributes can be useful for determining additional information about the data structure. The attributes can be viewed in [HDFView](http://www.hdfgroup.org/hdf-java-html/hdfview/index.html#download_hdfview), which allows for users to determine how to read them using other programs, if necessary.  


