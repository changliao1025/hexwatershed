
/**
 * @file flowline.cpp
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief
 * @version 0.1
 * @date 2019-06-11Created by Chang Liao on 4/26/18.
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "parameter.h"

namespace hexwatershed
{
  std::map<std::string, eMesh_type> mMesh_type = {
      {"hexagon", eM_hexagon},
      {"square", eM_square},
      {"latlon", eM_latlon},
      {"mpas", eM_mpas},
      {"dggrid", eM_dggrid},
      {"tin", eM_tin}};

  parameter::parameter()
  {
    iFlag_global = 0;
    iFlag_vtk = 0;
    iFlag_animation = 1;
    iFlag_hillslope = 1;
    iFlag_multiple_outlet = 0;
    iFlag_elevation_profile = 0;
    iFlag_stream_grid_option = 3;
    iFlag_accumulation_threshold_ratio = 0;
    dAccumulation_threshold_ratio = 0.001;
    dAccumulation_threshold_value = 2.5E7;
    
    dBreach_threshold = 5.0; // unit in meter
    nOutlet = 1;
  }

  parameter::~parameter()
  {
  }

  eMesh_type parameter::define_mesh_type(const std::string &sMesh_type)
  {
    if (mMesh_type.find(sMesh_type) != mMesh_type.end())
    {
      this->pMesh_type = mMesh_type[sMesh_type];
      return mMesh_type[sMesh_type];
    }
    else
    {
      this->pMesh_type = static_cast<eMesh_type>(-1);
      return static_cast<eMesh_type>(-1); // Indicates not found
    }
  }

} // namespace hexwatershed
