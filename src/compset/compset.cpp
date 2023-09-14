
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
    int iFlag_debug = cParameter.iFlag_debug;
    std::string sFilename;
    std::string sTime ;
    
    //step 1
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

    //step 2
    compset_calculate_flow_direction();
    sTime = get_current_time();     
    sLog = "Finished flow direction at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::flush(std::cout);
    std::cout << sLog << std::endl;

    //step 3
    compset_calculate_flow_accumulation();
    sTime = get_current_time();  
    sLog = "Finished flow accumulation at " + sTime;
    ofs_log << sLog << std::endl;
    ofs_log.flush();
    std::cout << sLog << std::endl;

    if (iFlag_global == 1)
    {
      // global simulation
    }
    else
    {
      if (iFlag_multiple_outlet == 1)
      {
        // multiple outlet case, do we need these information?        
        compset_define_stream_grid();
        sTime = get_current_time();  
        sLog = "Finished defining stream grid  at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        iFlag_debug = 0;
        compset_define_watershed_boundary();
        sTime = get_current_time();  
        sLog = "Finished defining watershed boundary";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        // start from here, we can actually run all the algorithm using the watershed object
        compset_define_stream_confluence();
        sTime = get_current_time();  
        sLog = "Finished defining confluence";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_define_stream_segment();
        sLog = "Finished defining stream segment";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_build_stream_topology();
        sLog = "Finished defining stream topology";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_define_stream_order();
        sLog = "Finished defining stream order";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_define_subbasin();
        sLog = "Finished defining subbasin";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        compset_calculate_watershed_characteristics();
        sLog = "Finished watershed characteristics";
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;
        std::flush(std::cout);
        // now all the watersheds are processed, we can transfer back to main object
        compset_transfer_watershed_to_domain();
        compset_update_cell_elevation();
        compset_update_vertex_elevation();
      }
      else
      {
        //step 4
        compset_define_stream_grid(); 
        sTime = get_current_time(); 
        sLog = "Finished defining stream grid at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        iFlag_debug = 0;
        //step 5
        compset_define_watershed_boundary();
        sTime = get_current_time();  
        sLog = "Finished defining watershed boundary at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 6
        //start from here, we can actually run all the algorithm using the watershed object
        compset_define_stream_confluence();
        sTime = get_current_time();  
        sLog = "Finished defining confluence at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 7
        compset_define_stream_segment();
        sTime = get_current_time();  
        sLog = "Finished defining stream segment at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 8
        compset_build_stream_topology();
        sTime = get_current_time();  
        sLog = "Finished defining stream topology at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 9
        compset_define_stream_order();
        sTime = get_current_time();  
        sLog = "Finished defining stream order at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 10
        compset_define_subbasin();
        sTime = get_current_time();  
        sLog = "Finished defining subbasin at " + sTime;
        ofs_log << sLog << std::endl;
        ofs_log.flush();
        std::cout << sLog << std::endl;

        //step 11
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
      }
    }

    std::flush(std::cout);

    return error_code;
  }

  int compset::compset_cleanup_model()
  {
    int error_code = 1;
    return error_code;
  }

} // namespace hexwatershed
