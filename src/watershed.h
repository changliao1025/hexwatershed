/**
 * @file watershed.h
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief This class provide interface to watershed characteristics,
 * @version 0.1
 * @date 2019-08-02
 *  @citation Liao, C., Tesfa, T., Duan, Z., & Leung, L. R. (2020).
 * Watershed delineation on a hexagonal mesh grid. Environmental Modelling & Software, 104702.
 * https://www.sciencedirect.com/science/article/pii/S1364815219308278
 * @github page https://github.com/changliao1025/hexwatershed
 * @copyright Copyright (c) 2019
 *
 */
#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "conversion.h"
#include "hexagon.h"
#include "segment.h"
#include "subbasin.h"

#include "./json/JSONBase.h"
#include "./json/mesh.h"
#include "./json/cell.h"
#include "./json/basin.h"
#include "./json/multibasin.h"
using namespace std;

namespace hexwatershed
{
  // some improvements are needed in next development
  class watershed
  {
  public:
    watershed();

    ~watershed();

    long lWatershed; // id of watershed 
    long lSegment_current;
    double dArea;
    float dSlope;
    float dSlope_mean;
    float dSlope_max;
    float dSlope_min;
    long nCell;
    long nSegment;
    long nSubbasin;
    long nConfluence;

    long lCellID_outlet; // the mesh ID of the outlet

    float dArea_2_stream_ratio; // the drainage density: https://en.wikipedia.org/wiki/Drainage_density
    float dLength_2_area_ratio; // the drainage density: https://en.wikipedia.org/wiki/Drainage_density
    float dDrainage_density;
    float dLongest_length_stream;    // the length of longest stream segment
    float dLength_stream_conceptual; // total stream length

    std::string sWorkspace_output_watershed;
    std::string sFilename_watershed_characteristics;
    std::string sFilename_segment_characteristics;
    std::string sFilename_subbasin_characteristics;

    std::string sFilename_watershed_json;
    std::string sFilename_watershed_stream_edge_json;


    std::vector<hexagon> vCell;
    std::vector<segment> vSegment;
    std::vector<subbasin> vSubbasin;
    std::vector<hexagon> vConfluence;

    //add an unordered map for index searching
    std::unordered_map<long, long> mCellIdToIndex;
    std::unordered_map<long, long> mSegmentIdToIndex;
    std::unordered_map<long, long> mSubbasinIdToIndex;

    // function

    int watershed_define_stream_confluence();
    int watershed_define_stream_segment();
    int watershed_tag_confluence_upstream( long lCellID_confluence);

    int watershed_build_stream_topology();
    int watershed_define_stream_order();    
    int watershed_define_subbasin();
    int watershed_define_hillslope();
    int watershed_update_attribute();

    // the watershed characteristics for comparison
    int watershed_calculate_characteristics();
    int watershed_calculate_drainage_area();
    int watershed_calculate_total_stream_length();
    int watershed_calculate_longest_stream_length();
    int watershed_calculate_drainage_density();
    int watershed_calculate_average_slope();
    int watershed_calculate_topographic_wetness_index();
    int watershed_calculate_travel_distance();


    int watershed_export_characteristics();
    int watershed_export_segment_characteristics();
    int watershed_export_subbasin_characteristics();

    int watershed_export_json();
    int watershed_export_stream_edge_json();

    long watershed_find_index_by_cell_id(long lCellID);
    long watershed_find_index_by_segment_id(long lSegment);
    long watershed_find_index_by_subbasin_id(long lSubbasin);

  };
}
