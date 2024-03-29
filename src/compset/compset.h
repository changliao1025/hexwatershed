/**
 * @file edge.h
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief Header file of the case class
 * @version 0.1
 * @date 2019-08-02
 * @citation Liao, C., Tesfa, T., Duan, Z., & Leung, L. R. (2020).
 * Watershed delineation on a hexagonal mesh grid. Environmental Modelling & Software, 104702.
 * https://www.sciencedirect.com/science/article/pii/S1364815219308278
 * @github page https://github.com/changliao1025/hexwatershed
 * @copyright Copyright (c) 2019
 *
 */
#pragma once
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath> // abs, floor
#include <unordered_map>
#include <chrono>
#include <ctime> //for timing recording
#include "../system.h"
#include "../hexagon.h"
#include "../flowline.h"
#include "../conversion.h"
#include "../watershed.h"
#include "../parameter.h"


#include "../json/JSONBase.h"
#include "../json/mesh.h"
#include "../json/cell.h"
#include "../json/basin.h"
#include "../json/multibasin.h"

using namespace std;
using namespace rapidjson;
using namespace jsonmodel;


namespace hexwatershed
{


  class compset {
  public:
    compset ();

    ~compset ();    

    int nWatershed_total;
    int nSegment_total;            //the total number of stream segment
    int nConfluence_total;         //the total number of stream confluence
  
       
    std::string sWorkspace_input;
    std::string sWorkspace_output;

    std::string sWorkspace_output_pyflowline;
    std::string sWorkspace_output_hexwatershed;
    
    std::string sFilename_configuration;
    std::string sFilename_log;
    std::string sLog;
    std::string sFilename_mesh_info;    

    std::string sFilename_domain_json;

    std::string sFilename_domain_animation_json;

    //vtk support
    std::string sFilename_domain_vtk;
    std::string sFilename_domain_vtk_debug;

    //others
    std::string sDate_default;
    std::string sDate;
    std::string sTime;
  
    std::ofstream ofs_log; // used for IO starlog file

    //std::vector <hexagon> vCell;                    //all the cells based on shapefile, no longer used as pyflowline deals with the dem already
    std::vector <hexagon> vCell_active;             //all calls has elevation (not missing value)
    std::unordered_map<long, long> mCellIdToIndex; //a map for fast search
    std::unordered_map<long, long> mVertexIdToIndex;
    std::vector <hexagon> vCell_priority_flood;             //all calls has elevation (not missing value)
    //watershed cWatershed;
    std::vector <watershed> vWatershed;
    std::vector<float> vElevation; //vector to store the DEM raster data
    //std::vector <flowline> vFlowline; //no longer used
    // this may be merged with global id
    std::vector <vertex> vVertex_active; //for vtk support, it store all the vertex in 3D

    std::string sFilename_netcdf_output; //if model use netcdf, we can put all results into one single netcdf file,
    // except the txt based results.
    std::string sFilename_hexagon_netcdf;
    parameter cParameter;
    std::vector<cell> aCell;
    std::vector<basin> aBasin;
    std::vector<hexagon> vContinent_boundary;
    //std::vector<instance> vInstance;

    int compset_initialize_model ();
    int compset_setup_model ();
    int compset_read_model();
    int compset_run_model ();
    int compset_export_model ();
    int compset_cleanup_model ();

    //int compset_assign_stream_burning_cell(); //no longer needed
    int compset_priority_flood_depression_filling ();
    int compset_stream_burning_with_topology (long lCellID_center);
    int compset_stream_burning_without_topology (long lCellID_center);
    int compset_breaching_stream_elevation (long lCellID_center);
    int compset_calculate_flow_direction ();
    int compset_calculate_flow_accumulation ();
    int compset_define_stream_grid ();
    int compset_define_watershed_boundary ();
    int compset_define_stream_confluence ();
    int compset_define_stream_segment ();
    int compset_tag_confluence_upstream (long lWatershed, long lCellID_confluence);

    int compset_build_stream_topology();
    int compset_define_stream_order();
    int compset_define_subbasin ();

    int compset_calculate_watershed_characteristics ();

    int compset_export_watershed_json ();
    int compset_export_watershed_characteristics ();
  

    int compset_transfer_watershed_to_domain();

    int compset_export_watershed_vtk (std::string sFilename_in); //only for watershed?
    int compset_export_domain_json(std::string sFilename_in);
    int compset_export_watershed_animation_json(std::string sFilename_in);
    std::vector <hexagon> compset_obtain_boundary (std::vector <hexagon> vCell_in);
    inline long compset_find_index_by_cell_id(long lCellID);
    

    int compset_find_continent_boundary(long lCellID_in);
    int compset_find_land_ocean_interface_neighbors(long lCellID_in);

    int priority_flood_depression_filling(std::vector <hexagon> &vCell_in);
    int compset_update_cell_elevation();
    int compset_update_vertex_elevation();

    int compset_check_digital_elevation_model_depression (std::vector <hexagon> vCell_in);

    std::array<long, 3> compset_find_lowest_cell_in_priority_queue (const std::vector <hexagon> &vCell_in);

  };
} // namespace hexwatershed
