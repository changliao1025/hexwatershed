###########
Application
###########


********
Overview
********

The recommended approach to run a HexWatershed simulation is through the Python package interface.

**************
Data structure
**************

HexWatershed uses the JavaScript Object Notation (JSON) file format for model configuration and data exchange.

The input data includes:

.. * A ESRI shapefile that defines the domain boundary

* A ESRI shapefile that defines the original river network

* A raster Geotiff file that contains the digital elevation model (DEM) data


Note that depending on the configuration, not all the input files are needed, or additional input files are needed.

****************
Data preparation
****************

Because the core algorithms within HexWatershed assume that all the data are on the Geographic Coordinate System (GCS), most input and output data use GCS. 

However, since most DEM data use the Projected Coordinate System (PCS), a reprojection is sometimes required. 

Besides, depending on the simulation configuration, different data are needed. Below are some instructions for different scenarios.

To support all the major computer systems, we use the QGIS to operate on most spatial datasets.

Single watershed
################

.. Boundary
.. ########
.. The domain boundary file can be defined using a vector-based watershed boundary file. 

River network
-------------

The river network file can be defined using a vector-based river flowline file.

Because the real-worlkd river network is often complex, some simplication is recommended. For example, the river network should only include major flowlines.

The shapefile should use the GCS system. If the vector you have is in a different PCS, you can re-project it to the GCS.

Boundary with buffer
--------------------

This boundary with buffer zone is mainly used to extract the DEM.
* obtain a vector watershed boundary (PCS system), if the boudnary is in a GCS system, you should convert it to PCS simialr to the flowline.
* create a buffer zone watershed boundary (PCS system), the buffer increase distance should be linked to your resolution of intestes. For example, if the highest mesh resolution you will use is arounf 5km, then the buffer zone distance should be set to 5km.

DEM
---

The DEM file can be extracted from a large DEM which contains the study domain.

To do so, the recommended steps are:

* prepare a large DEM with includes the study domain (PCS system)

* overlay the DEM (PCS system) and river network, and edit the boundary (PCS system) near the outlet so that there is less extended DEM near the outlet (GCS or PCS system)
* extract the large DEM using the edited boundary (PCS system)


Continuous domain with multiple watersheds
##########################################



River network
-------------

The river network file can be defined using a vector-based river flowline file.

Because the real-worlkd river network is often complex, some simplication is recommended. For example, the river network should only include major flowlines.



DEM
---

The DEM file can be extracted from a large DEM which contains the study domain.


Global (Disontinuous domain with multiple watersheds)
#####################################################

River network
-------------

Global scale hydrology dataset such as hydroshed may be used.
A subset of global river network can also be used.

DEM
---

It is recommended to assign/inject elevation within the mesh similart to MPAS.


.. list-table:: Input usage by domain
   :widths: 25 25 25 25
   :header-rows: 1

   * - Domain
     - Single watershed
     - Multiple continuous watershed
     - Multiple discontinuous watershed (global)
   * - Flowline
     - Yes if iFlag_flowline == 1
     - Yes if iFlag_flowline == 1
     - Yes if iFlag_flowline == 1
   * - Raster DEM
     - Yes if mesh type is not MPAS
     - Yes if mesh type is not MPAS
     - Yes if mesh type is not MPAS
   * - Boudnary
     - Yes, it will be used to generate mesh 
     - Yes, it will be used to generate mesh 
     - No, so far global mesh is pre-defined



*******************
Model configuration
*******************

HexWatershed (as well as its submodule **PyFlowline**) uses two **JSON** files (main and basin) as the configuration files.

Main configuration
##################

The main configuration JSON file contains the domain scale parameters.
Each domain may contain one or more basins.

.. list-table:: Main configuration JSON file
   :widths: 25 25 25 25
   :header-rows: 1

   * - Keyword
     - Data type
     - Value
     - Description
   * - sFilename_model_configuration
     - string
     - 
     - The full path to the main configuration file
   * - sModel
     - string
     - "hexwatershed"
     - The model name
   * - sRegion
     - string
     - 
     - The study region
   * - sWorkspace_bin
     - string
     - 
     - The directory of the complied HexWatershed binary
   * - sWorkspace_input
     - string
     - 
     - The directory of the input
   * - sWorkspace_output
     - string
     - 
     - The directory of the output
   * - sJob
     - string
     - 
     - The job name for HPC   
   * - iFlag_create_mesh
     - int
     - 0 or 1
     - Flag to generate mesh 
   * - iFlag_save_mesh
     - int
     - 0 or 1
     - Flag to save mesh 
   * - iFlag_simplification
     - int
     - 0 or 1
     - Flag for flowline simplication 
   * - iFlag_intersect
     - int
     - 0 or 1
     - Flag to flowline mesh intersection
   * - iFlag_resample_method
     - int
     - 1 or 2
     - Method for DEM resampling 
   * - iFlag_flowline
     - int
     - 0 or 1
     - Flag for flowline 
   * - iFlag_global
     - int
     - 0 or 1
     - Flag for global simulation 
   * - iFlag_multiple_outlet
     - int
     - 0 or 1
     - Flag for multiple basin simulation   
   * - iFlag_use_mesh_dem
     - int
     - 0 or 1
     - Flag to use DEM within the mesh
   * - iFlag_elevation_profile
     - int
     - 0 or 1
     - Flag to turn on elevation profile 
   * - iFlag_rotation
     - int
     - 0 or 1
     - Flag for mesh generation using rotation 
   * - iFlag_stream_burning_topology
     - int
     - 0 or 1
     - Flag to turn on stream burning topology
   * - iFlag_save_elevation
     - int
     - 0 or 1
     - Flag to save elevation 
   * - iCase_index
     - int
     - 
     - ID of case
   * - iMesh_type
     - int
     - 1 to 5
     - Mesh type 
   * - dMissing_value_dem
     - float
     - 
     - The missing value in the DEM
   * - dBreach_threshold
     - float
     - 
     - The threshold parameter for the hybrid breaching filling algorithm
   * - dAccumulation_threshold
     - float
     - 
     - The accumulation parameter to define stream cell
   * - dLongitude_left
     - float
     - 0 or 1
     - The domain left boundary
   * - dLongitude_right
     - float
     - 
     - The domain right boundary
   * - dLatitude_bot
     - float
     - 
     - The domain bottom boundary
   * - dLatitude_top
     - float
     - 
     - The domain top boundary
   * - dResolution_degree
     - float
     - 
     - Mesh resolution in degree
   * - dResolution_meter
     - float
     - 
     - Mesh resolution in meter
   * - sDate
     - string
     - 
     - The date of the simulation
   * - sMesh_type
     - string
     - hexagon
     - The mesh type
   * - sFilename_hexwatershed
     - string
     - 
     - The filename of the binary 
   * - sFilename_spatial_reference
     - string
     - 
     - The spatial reference of the river network 
   * - sFilename_dem
     - string
     - 
     - The filename of the DEM
   * - sFilename_mesh_netcdf
     - float
     - 
     - The filename of the MPAS netcdf file
   * - sFilename_basins
     - string
     - 
     - The full path of the basin configuration file 

Basin configuration
###################

The basin configuration file contains one or more block of JSON basin object. Each block contains the configuration to a unique basin. Different basin may have different parameters.


.. list-table:: Basin configuration JSON file
   :widths: 25 25 25 25
   :header-rows: 1

   * - Keyword
     - Data type
     - Value
     - Description
   * - dLatitude_outlet_degree
     - float
     - 
     - The outlet latitude
   * - dLongitude_outlet_degree
     - float
     - 0 or 1
     - The outlet longitude 
   * - dAccumulation_threshold
     - float
     - 
     - The accumulation parameter to define the stream cell
   * - dThreshold_small_river
     - float
     - 
     - The threshold parameter to remove small river 
   * - iFlag_dam
     - int
     - 0
     - Reserved for dam burning
   * - iFlag_disconnected
     - int
     - 0
     - Reserved for disconnected flowline
   * - lBasinID
     - long
     - 
     - The basin ID
   * - sFilename_dam
     - string
     - 
     - Reserved for dam burning
   * - sFilename_flowline_filter
     - string
     - 
     - The filename of the stream vector 
   * - sFilename_flowline_raw
     - string
     - 
     - The filename of the raw stream vector 
   * - sFilename_flowline_topo
     - string
     - 
     - Reserved for dam burning

****************
Model simulation
****************

The easiest way to setup a simulation is to use an existing template. You can also generate an emtpy template using the provided APIs.

Then you can edit the template by replacing with the actual input filenames and paths.

Last, you can run the model through the Python APIs.


******************
Simulation results
******************

After the simulation is finished, you should obtain a list of fils within the output directory. Depending on the configuration, not all files will be outputed.

* depression filled DEM

* flow direction

* flow accumulation

* stream segment

* stream order

* subbasin boundary

* watershed boundary

These files are saved using the GeoJSON file format.

.. list-table:: Domain-scale output option
   :widths: 20 20 20 20 20
   :header-rows: 1

   * - Vector type
     - Variable
     - Global
     - Multiple outlets
     - Single outlet
   * - Point
     - Dam
     - No
     - No
     - No
   * - Polyline
     - Flow direction
     - Yes
     - Yes
     - No
   * - Polyline
     - Stream segment
     - Yes
     - Yes
     - No
   * - Polygon
     - Elevation
     - Yes
     - Yes
     - No
   * - Polygon
     - Slope
     - Yes
     - Yes
     - No
   * - Polygon
     - Drainage area
     - Yes
     - Yes
     - No
   * - Polygon
     - Travel distance
     - Yes
     - Yes
     - No


.. list-table:: Watershed-scale output option
   :widths: 20 20 20 20 20
   :header-rows: 1

   * - Vector type
     - Variable
     - Global
     - Multiple outlets
     - Single outlet
   * - Point
     - Dam
     - No
     - No
     - No
   * - Polyline
     - Flow direction
     - No
     - Yes
     - Yes
   * - Polyline
     - Stream segment
     - No
     - Yes
     - Yes
   * - Polygon
     - Elevation
     - Yes
     - Yes
     - Yes
   * - Polygon
     - Slope
     - Yes
     - Yes
     - Yes
   * - Polygon
     - Drainage area
     - Yes
     - Yes
     - Yes
   * - Polygon
     - Travel distance
     - No
     - Yes
     - Yes

You can use any GIS tools (ArcGIS, ENVI, and QGIS, etc.) to visualize the results.



********
Tutorial
********

A full tutorial is provide at `<https://github.com/changliao1025/hexwatershed_tutorial>`_
