/**
 * @file domain.cpp
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief the realization of the domain class
 * @version 0.1
 * @date 2019-08-02
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "domain.h"

namespace hexwatershed
{

  /**
   * read data from the model configuration file
   * @return
   */
  int domain::domain_read()
  {
    int error_code = 1;
    domain_read_configuration_file();
    domain_retrieve_user_input();
    domain_read_input_data();

    cCompset.sFilename_mesh_info = this->sFilename_mesh_info;
    cCompset.compset_read_model();
    sTime = get_current_time();
    sLog = "Finished reading data at " + sTime;
    std::cout << sLog << std::endl;
    std::flush(std::cout);
    return error_code;
  }

  /**
   * read the user provided configuration file
   * @return
   */
  int domain::domain_read_configuration_file()
  {
    int error_code = 1;
    std::size_t iVector_size;
    std::string sLine;  // used to store each sLine
    std::string sKey;   // used to store the sKey
    std::string sValue; // used to store the sValue

    std::vector<std::string> vTokens;

    std::string sFilename_configuration = cCompset.cParameter.sFilename_configuration;
    // 50==================================================
    // the existence of the configuration file is checked already
    // 50==================================================

    std::ifstream ifs(sFilename_configuration.c_str());
    if (!ifs.is_open())
    {
      std::cerr << "Could not open file for reading!\n";
      return error_code;
    }

    IStreamWrapper isw(ifs);
    pConfigDoc.ParseStream(isw);

    return error_code;
  }

  /**
   * extract the dictionary from user provided configuration file
   * @return
   */
  int domain::domain_retrieve_user_input()
  {
    int error_code = 1;
    float dValue;
    eMesh_type pMesh_type;
    std::string sMesh_type;
    std::string sKey = "sMesh_type";
    std::list<basin>::iterator iIterator;
    // std::string sWorkspace_output_hexwatershed;
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      sMesh_type = pConfigDoc[sKey.c_str()].GetString();
    }
    // use the enum to define mesh type

    pMesh_type = cCompset.cParameter.define_mesh_type(sMesh_type);

    cCompset.cParameter.sMesh_type = sMesh_type;

    cCompset.cParameter.pMesh_type = pMesh_type;

    sKey = "iFlag_resample_method";

    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_resample_method = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "iFlag_global";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_global = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "iFlag_elevation_profile";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_elevation_profile = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "iFlag_multiple_outlet";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_multiple_outlet = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "iFlag_flowline";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_flowline = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "iFlag_stream_grid_option";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_stream_grid_option = pConfigDoc[sKey.c_str()].GetInt();
    }
    sKey = "iFlag_hillslope";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iFlag_hillslope = pConfigDoc[sKey.c_str()].GetInt();
    }

    sKey = "dAccumulation_threshold";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      dValue = pConfigDoc[sKey.c_str()].GetFloat();

      if (dValue >= 1.0)
      {
        // this is provided as the actual accumulation value;
        cCompset.cParameter.iFlag_accumulation_threshold_ratio = 0;
        cCompset.cParameter.dAccumulation_threshold_value = dValue;
      }
      else
      {
        // this is provided as a ratio
        cCompset.cParameter.iFlag_accumulation_threshold_ratio = 1;
        cCompset.cParameter.dAccumulation_threshold_ratio = dValue;
        if (cCompset.cParameter.dAccumulation_threshold_ratio < 0.0)
        {
          cCompset.cParameter.dAccumulation_threshold_ratio = 0.001;
        }
      }
    }

    if (cCompset.cParameter.iFlag_flowline == 1)
    {
      sKey = "nOutlet";
      if (pConfigDoc.HasMember(sKey.c_str()))
      {
        cCompset.cParameter.nOutlet = pConfigDoc[sKey.c_str()].GetInt();
      }

      sKey = "iFlag_stream_burning_topology";
      if (pConfigDoc.HasMember(sKey.c_str()))
      {
        cCompset.cParameter.iFlag_stream_burning_topology = pConfigDoc[sKey.c_str()].GetInt();
      }

      sKey = "dBreach_threshold"; //this could also be a local parameter for each basin
      if (pConfigDoc.HasMember(sKey.c_str()))
      {
        cCompset.cParameter.dBreach_threshold = pConfigDoc[sKey.c_str()].GetFloat();
      }

      sKey = "sFilename_basins";

      if (pConfigDoc.HasMember(sKey.c_str()))
      {
        sFilename_basins = pConfigDoc[sKey.c_str()].GetString();
        // read basin info
        domain_read_basin_json(sFilename_basins);

        for (iIterator = cBasin.aBasin.begin(); iIterator != cBasin.aBasin.end(); ++iIterator)
        {
          // quality control of basin parameters
          iIterator->iFlag_flowline = cCompset.cParameter.iFlag_flowline; //this must be 1 in this case
          if (iIterator->iFlag_accumulation_threshold_provided == 0)
          {
            iIterator->dAccumulation_threshold_ratio = cCompset.cParameter.iFlag_accumulation_threshold_ratio;
            iIterator->dAccumulation_threshold_value = cCompset.cParameter.dAccumulation_threshold_value;
          }
          cCompset.aBasin.push_back((*iIterator));
        }
      }
    }

    sKey = "dMissing_value_dem";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.dMissing_value_dem = pConfigDoc[sKey.c_str()].GetFloat();
    }

    sKey = "iCase_index";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      cCompset.cParameter.iCase_index = pConfigDoc[sKey.c_str()].GetInt();
    }

    cCompset.cParameter.sMissing_value_default = trim(convert_float_to_string(dMissing_value_default));

    sKey = "sDate";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      sDate = pConfigDoc[sKey.c_str()].GetString();
    }
    else
    {
      sDate = sDate_default;
    }

    sKey = "sWorkspace_input";
    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      sWorkspace_input = pConfigDoc[sKey.c_str()].GetString();
    }

    sKey = "sWorkspace_output";

    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      sWorkspace_output = pConfigDoc[sKey.c_str()].GetString();
    }

    sKey = "sWorkspace_output_hexwatershed";

    if (pConfigDoc.HasMember(sKey.c_str()))
    {
      sWorkspace_output_hexwatershed = pConfigDoc[sKey.c_str()].GetString();
    }

    // output=================================

    if (sWorkspace_output.length() > 3)
    {
      // this will be the output workspace
      if (path_test(sWorkspace_output) == 0)
      {
        make_directory(sWorkspace_output);
      }
      sWorkspace_output_pyflowline = sWorkspace_output + slash + "pyflowline";
    }
    else
    {
      if (path_test(sWorkspace_output_hexwatershed) == 0)
      {
        make_directory(sWorkspace_output_hexwatershed);
      }
      sWorkspace_output_pyflowline = sWorkspace_output + slash + "pyflowline";
    }

    sFilename_mesh_info = sWorkspace_output_pyflowline + slash + sMesh_type + "_mesh_info.json";

    cCompset.sWorkspace_output_hexwatershed = sWorkspace_output_hexwatershed;

    // vtk
    cCompset.sFilename_domain_vtk = sWorkspace_output_hexwatershed + slash + "hexwatershed.vtk";

    // json

    // others
    cCompset.sFilename_domain_json = sWorkspace_output_hexwatershed + slash + "hexwatershed.json";

    cCompset.sFilename_domain_animation_json = sWorkspace_output_hexwatershed + slash + "animation.json";

    sFilename_log = sWorkspace_output_hexwatershed + slash + "starlog" + sExtension_text;
    ofs_log.open(sFilename_log.c_str(), ios::out);

    sLog = "=Level 1: Simulation log will be save to log file: " +
           sFilename_log;
    ofs_log << sLog << std::endl;

    // copy the configuration file to the output workspace
    std::ifstream src(cCompset.cParameter.sFilename_configuration.c_str(), std::ios::binary);
    std::string sFilename_dummy = sWorkspace_output_hexwatershed + slash + "configuration.in";
    std::ofstream dst(sFilename_dummy.c_str(), std::ios::binary);
    dst << src.rdbuf();

    return error_code;
  }

  /**
   * @brief read input data
   *
   * @return int
   */
  int domain::domain_read_input_data()
  {
    int error_code = 1;
    // eMesh_type pMesh_type = cCompset.cParameter.pMesh_type;
    // int iFlag_flowline = cCompset.cParameter.iFlag_flowline;
    // int iFlag_stream_burning_topology = cCompset.cParameter.iFlag_stream_burning_topology;
    std::vector<hexagon>::iterator iIterator;
    domain_read_elevation_json(sFilename_mesh_info);
    for (auto &it : cMesh.aCell)
    {
      cCompset.aCell.push_back(it);
    }
    return error_code;
  }

  int domain::domain_read_elevation_json(const std::string &sFilename_elevation_in)
  {
    int error_code = 1;
    cMesh.DeserializeFromFile(sFilename_elevation_in);
    return error_code;
  }
  int domain::domain_read_basin_json(const std::string &sFilename_basin_in)
  {
    int error_code = 1;
    cBasin.DeserializeFromFile(sFilename_basin_in);
    return error_code;
  }

}
