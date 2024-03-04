//
// Created by Liao, Chang on 2020-09-07.
//

#pragma once
#include <iostream>
#include <unordered_map>
#include "hexagon.h"
#include "data.h"
// #include "hillslope.h"
#include "./json/JSONBase.h"
#include "./json/mesh.h"
#include "./json/cell.h"
#include "./json/basin.h"
#include "./json/multibasin.h"
using namespace std;
namespace hexwatershed
{
    class subbasin
    {
    public:
        subbasin();

        ~subbasin();
        std::vector<hexagon> vCell;         // all the cells
        std::vector<hexagon> vCell_segment; // all the cells on the channel (upstream to downstream)
        // std::vector <hillslope> vHillslope; //all the hillslopes, max 3
        std::unordered_map<long, long> mCellIdToIndex;
        int iFlag_headwater; // 1: headwater, 0: not headwater
        int iFlag_outlet;    // 1: outlet, 0: not outlet
        int nHillslope;
        int iFlag_hillslope;

        long lSubbasin; // each subbasin should have the same index with its segment
        long lSubbasinIndex;
        long nCell;
        long nCell_hillslope_left;
        long nCell_hillslope_right;
        long nCell_hillslope_headwater;
        long lCellID_headwater; // the id of the headwater cell
        long lCellID_outlet;    // the id of the subbasin outlet
        long lHillslope_left; //id for hillslope, the first cell of the hillslope
        long lHillslope_right;
        long lHillslope_headwater;
        double dArea;
        float dSlope;
        float dSlope_mean;
        float dSlope_max;
        float dSlope_min;
        float dArea_2_stream_ratio; // the drainage density: https://en.wikipedia.org/wiki/Drainage_density
        float dLength_2_area_ratio; // the drainage density: https://en.wikipedia.org/wiki/Drainage_density
        float dDistance_to_subbasin_outlet;
        float dDrainage_density;

        float dLength_hillslope_left;
        float dLength_hillslope_right;
        float dLength_hillslope_headwater;

        float dWidth_hillslope_left;
        float dWidth_hillslope_right;
        float dWidth_hillslope_headwater;
        
        double dArea_hillslope_left;
        double dArea_hillslope_right;
        double dArea_hillslope_headwater;
 
        float dSlope_hillslope_left_mean;  //mean slope of cell
        float dSlope_hillslope_right_mean;
        float dSlope_hillslope_headwater_mean;

        float dSlope_hillslope_left;  //slope of the hillslope
        float dSlope_hillslope_right;
        float dSlope_hillslope_headwater;
        std::array<float, 11> aElevation_profile_left;
        std::array<float, 11> aElevation_profile_right;
        std::array<float, 11> aElevation_profile_headwater;
   

        hexagon cCell_headwater; // the first cell of its segment
        hexagon cCell_outlet;    // the outlet of this subbasin, this cell is actually within the subbasin because it is shared by multiple subbasin
        hexagon cCell_outlet_downslope;

        // function
        int subbasin_define_hillslope();
        int subbasin_calculate_characteristics(float dLength_stream_conceptual);
        int subbasin_calculate_hillslope_characteristics();
        int subbasin_calculate_total_area();
        int subbasin_calculate_slope();
        int subbasin_calculate_drainage_density(float dLength_stream_conceptual);
        int subbasin_calculate_travel_distance();
        long subbasin_find_index_by_cellid(long lCellID_in);
    };
} // namespace hexwatershed
