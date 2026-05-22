
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

#include "./compset.h"

namespace hexwatershed
{

  compset::compset()
  {
  }

  compset::~compset()
  {
  }

  int compset::compset_read_model()
  {
    int error_code = 1;
    return error_code;
  }
  int compset::compset_setup_model()
  {
    int error_code = 1;
    return error_code;
  }

  int compset::compset_run_model()
  {
    int error_code = 1;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_export_individual_watershed = cParameter.iFlag_export_individual_watershed;
    int iFlag_debug = cParameter.iFlag_debug;
    std::string sFilename;
    // step 1
    error_code = compset_priority_flood_depression_filling();
    if (error_code != 1)
    {
      return error_code;
    }
    sTime = get_current_time();
    sLog = "Finished depression filling at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    std::flush(std::cout);

    // step 2
    compset_calculate_flow_direction();
    sTime = get_current_time();
    sLog = "Finished flow direction at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::flush(std::cout);
    std::cout << sLog << std::endl;

    // step 3
    compset_calculate_flow_accumulation();
    sTime = get_current_time();
    sLog = "Finished flow accumulation at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    iFlag_debug = 0;
    if (iFlag_global == 1)
    {
      // either dem-based or single watershed based.
      //  only one watershed is constrained
      if (iFlag_export_individual_watershed == 1)
      {
        compset_stats_flow_accumulation();
        compset_define_watershed_boundary();
        sTime = get_current_time();
        sLog = "Finished defining watershed boundary at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        // once watershed is defined, all the the algorithm should be run within the watershed object
        compset_run_watershed();
      }
      else
      {
        // normally we dont need run individual watershed because models like MOSART dont need this information
      }
    }
    else
    {
      if (iFlag_multiple_outlet == 1) // user defined multiple outlets? how about pure dem-based
      {
        if (iFlag_export_individual_watershed == 1)
        {
          // pure dem-based watershed, so we need to manually define outlet first
          compset_stats_flow_accumulation();
          compset_define_watershed_boundary();
          sTime = get_current_time();
          sLog = "Finished defining watershed boundary at " + sTime;
          ofs_log << sLog << std::endl;
          ofs_log.flush();
          std::cout << sLog << std::endl;
          // once watershed is defined, all the the algorithm should be run within the watershed object
          compset_run_watershed();
        }
      }
      else
      {
        // we have at least one watershed, regardless of dem-based or user-defined flowline, so we can directly run the watershed algorithm
        compset_stats_flow_accumulation();
        compset_define_watershed_boundary();
        sTime = get_current_time();
        sLog = "Finished defining watershed boundary at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_run_watershed();
      }
    }

    std::flush(std::cout);

    return error_code;
  }

  /*
   * this function is used to run the watershed algorithm, it will run all the watershed algorithm
   * including stream grid, confluence, segment, topology, order, subbasin and watershed characteristics
   * start from here, we run all the algorithm using the watershed object
   * @return
   */
  int compset::compset_run_watershed()
  {
    int error_code = 1; //
    // if we want to use different threshold for different watersheds, then we need to redefine the stream grid here?
    compset_define_stream_grid();
    sTime = get_current_time();
    sLog = "Finished defining stream grid at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_define_stream_confluence();
    sTime = get_current_time();
    sLog = "Finished defining confluence at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_define_stream_segment();
    sTime = get_current_time();
    sLog = "Finished defining stream segment at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_build_stream_topology();
    sTime = get_current_time();
    sLog = "Finished defining stream topology at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_define_stream_order();
    sTime = get_current_time();
    sLog = "Finished defining stream order at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_define_subbasin();
    sTime = get_current_time();
    sLog = "Finished defining subbasin at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    compset_calculate_watershed_characteristics();
    sTime = get_current_time();
    sLog = "Finished watershed characteristics at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;
    std::flush(std::cout);
    // now all the watersheds are processed, we can transfer back to main object
    compset_transfer_watershed_to_domain();
    compset_update_cell_elevation();
    compset_update_vertex_elevation();
    return error_code;
  }

  int compset::compset_cleanup_model()
  {
    int error_code = 1;
    return error_code;
  }

} // namespace hexwatershed
