/**
 * @file segment.h
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief Header file of the stream segment class
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
#include <vector>

#include "hexagon.h"
#include "./json/JSONBase.h"
#include "./json/mesh.h"
#include "./json/cell.h"
#include "./json/basin.h"
#include "./json/multibasin.h"
using namespace std;
namespace hexwatershed
{
    class segment
    {
    public:
        segment();

        ~segment();

        long nReach;
        long lSegment;
        long lSegmentIndex;
        long lWatershed; //which watershed it belongs to

        long lSegment_downstream;

        int iSegment_order;

        int iFlag_headwater;
        int iFlag_has_upstream;
        int iFlag_has_downstream;
        int nSegment_upstream;

        float dLength; //the total length of the stream segment
        float dSlope_mean; //the average slope 
        float dElevation_drop; //elevation drop between headwater to outlet
        float dDistance_to_watershed_outlet;

        std::vector<long> vSegment_upstream;

        hexagon cReach_start;
        hexagon cReach_end;
        std::vector<hexagon> vReach_segment;

        //function
        //sort
        bool operator<(const segment &cSegment);

        int segment_calculate_stream_segment_characteristics();
        int segment_calculate_stream_segment_length();
        int segment_calculate_stream_segment_slope();
        int segment_calculate_travel_distance();
    };
} // namespace hexwatershed
