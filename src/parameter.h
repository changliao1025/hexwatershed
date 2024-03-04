//
// Created by liao313 on 4/8/2021.
//

#pragma once
#include <map>
#include <string>
#include <vector>
using namespace std;

namespace hexwatershed
{

  enum eMesh_type
  {
    eM_hexagon = 1,
    eM_square = 2,
    eM_latlon = 3,
    eM_mpas = 4,
    eM_dggrid = 5,
    eM_tin = 6,
  };

  extern std::map<std::string, eMesh_type> mMesh_type;

  enum eVariable
  {
    eV_elevation,
    eV_flow_direction,
    eV_flow_accumulation,
    eV_stream_grid,
    eV_confluence,
    eV_watershed,
    eV_subbasin,
    eV_segment,
    eV_slope_between,
    eV_slope_within,
    eV_stream_order,
    eV_wetness_index,
  };

  class parameter
  {

  public:
    parameter();

    ~parameter();
    int iCase_index;

    int iFlag_configuration_file;
    int iFlag_debug;
    int iFlag_elevation_profile;
    int iFlag_animation;
    int iFlag_vtk;

    eMesh_type pMesh_type;

    int iFlag_global;
    int iFlag_multiple_outlet; // multiple outlet in one simulation
    int iFlag_resample_method;
    int iFlag_flowline;                // has stream burning or not
    int iFlag_stream_burning_topology; // whethet to use topology for stream burning
    int iFlag_stream_grid_option;
    int iFlag_slope_provided;
    int iFlag_merge_reach;
    long nOutlet;

    // parameters
    
    float dBreach_threshold; // the threshold parameter for stream burning breaching algorithm
    float dMissing_value_dem;
    //float dAccumulation_threshold; //real accumuation
    float dAccumulation_threshold_ratio; //the ratio of accumulation compared with max accumulation
    std::string sMesh_type;
    std::string sFilename_configuration;
    std::string sMissing_value_default;
    eMesh_type define_mesh_type(const std::string &sMesh_type);
  };
}
