####################
Simulation Scenarios
####################

This document provides an overview of different simulation scenarios supported by HexWatershed, along with their typical configurations and use cases.

********
Overview
********

HexWatershed supports various simulation scenarios ranging from single watershed analysis to global-scale hydrological modeling. The choice of scenario depends on the study domain, available data, and research objectives.

*******************
Scenario Comparison
*******************

The following table summarizes the key characteristics and configuration requirements for different simulation scenarios:

.. list-table:: HexWatershed Simulation Scenarios
   :widths: 15 20 15 15 15 20
   :header-rows: 1

   * - Scenario
     - Description
     - Domain Type
     - Mesh Type
     - Key Flags
     - Typical Use Cases
   * - Single Watershed (Basic)
     - Single watershed with DEM-based delineation
     - Continuous
     - Hexagon, Square, Latitude-Longitude
     - iFlag_flowline=0, iFlag_global=0, iFlag_multiple_outlet=0
     - Small to medium watershed studies, local hydrology analysis
   * - Single Watershed (with Flowline)
     - Single watershed with stream network burning
     - Continuous
     - Hexagon, Square, Latitude-Longitude
     - iFlag_flowline=1, iFlag_global=0, iFlag_multiple_outlet=0, iFlag_stream_burning_topology=1
     - Watershed studies requiring accurate stream network representation
   * - Multiple Continuous Watersheds
     - Multiple connected watersheds in a region
     - Continuous
     - Hexagon, Square, Latitude-Longitude
     - iFlag_flowline=1, iFlag_global=0, iFlag_multiple_outlet=1
     - Regional hydrology, river basin management, multi-basin studies
   * - Global (DEM-based)
     - Global simulation using only DEM without flowline burning
     - Discontinuous
     - MPAS
     - iFlag_flowline=0, iFlag_global=1, iFlag_multiple_outlet=1, iFlag_use_mesh_dem=1
     - Pure DEM-based global hydrology, exploratory global studies, data-sparse regions
   * - Global Single Basin (with Flowline)
     - Global mesh with single basin/outlet and flowline burning
     - Discontinuous
     - MPAS
     - iFlag_flowline=1, iFlag_global=1, iFlag_multiple_outlet=0, iFlag_use_mesh_dem=1
     - Single large basin on global mesh, Amazon/Congo basin studies, testing global mesh
   * - Global Multi-Basin (with Flowline)
     - Global or large-scale with multiple disconnected basins and stream network
     - Discontinuous
     - MPAS
     - iFlag_flowline=1, iFlag_global=1, iFlag_multiple_outlet=1, iFlag_use_mesh_dem=1
     - Global hydrology modeling with known stream networks, continental-scale studies

***********************
Configuration Templates
***********************

Single Watershed (Basic) Example
#################################

Configuration for a single watershed with DEM-based delineation (no flowline):

.. code-block:: json

   {
     "iFlag_flowline": 0,
     "iFlag_global": 0,
     "iFlag_multiple_outlet": 0,
     "iFlag_create_mesh": 1,
     "iFlag_use_mesh_dem": 0,
     "iMesh_type": 1,
     "sMesh_type": "hexagon",
     "dResolution_meter": 5000
   }

Single Watershed (with Flowline) Example
#########################################

Configuration for a single watershed with stream network burning:

.. code-block:: json

   {
     "iFlag_flowline": 1,
     "iFlag_global": 0,
     "iFlag_multiple_outlet": 0,
     "iFlag_create_mesh": 1,
     "iFlag_use_mesh_dem": 0,
     "iFlag_stream_burning_topology": 1,
     "iMesh_type": 1,
     "sMesh_type": "hexagon",
     "dResolution_meter": 5000
   }

Multiple Watersheds Example
###########################

Configuration for multiple continuous watersheds:

.. code-block:: json

   {
     "iFlag_flowline": 1,
     "iFlag_global": 0,
     "iFlag_multiple_outlet": 1,
     "iFlag_create_mesh": 1,
     "iFlag_use_mesh_dem": 0,
     "iMesh_type": 1,
     "sMesh_type": "hexagon",
     "dResolution_meter": 10000
   }

Global (DEM-based) Simulation Example
######################################

Configuration for pure DEM-based global simulation without flowline (simplest global approach):

.. code-block:: json

   {
     "iFlag_flowline": 0,
     "iFlag_global": 1,
     "iFlag_multiple_outlet": 1,
     "iFlag_create_mesh": 0,
     "iFlag_use_mesh_dem": 1,
     "iMesh_type": 5,
     "sMesh_type": "mpas",
     "sFilename_mesh_netcdf": "/path/to/mpas_mesh.nc"
   }

Global Single Basin with Flowline Example
##########################################

Configuration for global mesh with single basin and flowline burning:

.. code-block:: json

   {
     "iFlag_flowline": 1,
     "iFlag_global": 1,
     "iFlag_multiple_outlet": 0,
     "iFlag_create_mesh": 0,
     "iFlag_use_mesh_dem": 1,
     "iMesh_type": 5,
     "sMesh_type": "mpas",
     "sFilename_mesh_netcdf": "/path/to/mpas_mesh.nc"
   }

Global Multi-Basin with Flowline Example
#########################################

Configuration for global-scale simulation with multiple basins and flowline burning:

.. code-block:: json

   {
     "iFlag_flowline": 1,
     "iFlag_global": 1,
     "iFlag_multiple_outlet": 1,
     "iFlag_create_mesh": 0,
     "iFlag_use_mesh_dem": 1,
     "iMesh_type": 5,
     "sMesh_type": "mpas",
     "sFilename_mesh_netcdf": "/path/to/mpas_mesh.nc"
   }

*********************
Input Data Requirements
*********************

The following table shows the input data requirements for each scenario:

.. list-table:: Input Data Requirements by Scenario
   :widths: 20 15 15 15 15 20
   :header-rows: 1

   * - Scenario
     - DEM Required
     - Flowline Required
     - Boundary Required
     - MPAS Mesh Required
     - Additional Notes
   * - Single Watershed (Basic)
     - Yes
     - No
     - Yes
     - No
     - DEM must cover entire watershed with buffer
   * - Single Watershed (with Flowline)
     - Yes
     - Yes
     - Yes
     - No
     - Flowline should be simplified and in GCS
   * - Multiple Continuous Watersheds
     - Yes
     - Yes
     - Yes
     - No
     - DEM must cover all watersheds; flowline should be continuous
   * - Global (DEM-based)
     - Optional
     - No
     - No
     - Yes
     - Pure DEM-based approach; elevation injected from mesh; no flowline needed
   * - Global Single Basin (with Flowline)
     - Optional
     - Yes
     - No
     - Yes
     - Single basin configuration; elevation from mesh; flowline for one basin only
   * - Global Multi-Basin (with Flowline)
     - Optional
     - Yes
     - No
     - Yes
     - Elevation can be injected from mesh; global flowline dataset recommended

***********************
Performance Considerations
***********************

.. list-table:: Computational Requirements
   :widths: 20 20 20 20 20
   :header-rows: 1

   * - Scenario
     - Typical Domain Size
     - Mesh Resolution
     - Memory Usage
     - Runtime
   * - Single Watershed (Basic)
     - 100-10,000 km²
     - 100m - 5km
     - Low (< 4GB)
     - Minutes to hours
   * - Single Watershed (with Flowline)
     - 100-10,000 km²
     - 100m - 5km
     - Low to Medium (2-8GB)
     - Minutes to hours
   * - Multiple Continuous Watersheds
     - 10,000-1,000,000 km²
     - 1km - 10km
     - Medium (4-16GB)
     - Hours to days
   * - Global (DEM-based)
     - Global (510 million km²)
     - 10km - 100km
     - Medium to High (8-32GB)
     - Days (faster than with flowline)
   * - Global Single Basin (with Flowline)
     - 1,000,000-10,000,000 km²
     - 10km - 100km
     - Medium to High (8-32GB)
     - Hours to days
   * - Global Multi-Basin (with Flowline)
     - Global (510 million km²)
     - 10km - 100km
     - High (16-64GB)
     - Days to weeks

********************
Selecting a Scenario
********************

When choosing a simulation scenario, consider the following factors:

1. **Study Domain**: Is your study area a single watershed, multiple watersheds, or global scale?

2. **Data Availability**: What input data do you have available (DEM, flowline, boundary, MPAS mesh)?

3. **Resolution Requirements**: What spatial resolution is needed for your analysis?

4. **Computational Resources**: What computing resources (memory, CPU, time) are available?

5. **Integration Needs**: Do you need to integrate with other models (e.g., E3SM, MPAS)?

6. **Output Requirements**: What outputs do you need (flow direction, stream network, subbasins)?

**************
Best Practices
**************

* Start with a smaller domain or coarser resolution to test your configuration
* Use flowline burning when accurate stream network representation is critical
* For global simulations, use pre-defined MPAS meshes to save computation time
* Apply appropriate buffer zones when extracting DEM data
* Simplify flowline networks to reduce computational complexity
* Validate results against observed data or existing hydrological products

********
See Also
********

* :doc:`../application` - General application documentation
* :doc:`../../installation/installation` - Installation instructions
* :doc:`../../api/api` - API reference

For complete working examples, see the `HexWatershed Tutorial <https://github.com/changliao1025/hexwatershed_tutorial>`_.
