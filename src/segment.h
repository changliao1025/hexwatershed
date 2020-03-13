/**
 * @file segment.h
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief Header file of the stream segment class
 * @version 0.1
 * @date 2019-08-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#include <vector>

#include "hexagon.h"

using namespace std;
namespace hexwatershed
{
    class segment
    {
    public:
        segment();

        ~segment();

        int nReach;
        int iSegment;
        //int iSegment_upstream; do not use because one stream can have 2 upstream segment
        int iSegment_downstream;
        int iSegment_order;
        int iFlag_headwater;
        int iFlag_has_upstream;
        int iFlag_has_downstream;
        int nSegment_upstream;
        std::vector<int> vSegment_upstream;

        hexagon cReach_start;
        hexagon cReach_end;
        std::vector<hexagon> vReach_segment;

//sort 
        bool operator < (const segment& cSegment) const
    {
        return (iSegment < cSegment.iSegment);
    }


    };
}

