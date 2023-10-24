/**
 * @file watershed.cpp
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief
 * @version 0.1
 * @date 2019-08-02
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "watershed.h"

namespace hexwatershed
{
  watershed::watershed()
  {
    lWatershed = -1;
  }

  watershed::~watershed()
  {
  }

  int watershed::watershed_define_stream_confluence()
  {
    int error_code = 1;
    int iCount = 0;
    nSegment = 0;
    nConfluence = 0;
    long lCellID_downstream;
    long lCellIndex_downstream, lCellIndex_downstream1;
    std::vector<hexagon>::iterator iIterator_self;
    iCount = 0;
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      // we only consider cells within the watershed
      if ((*iIterator_self).iFlag_stream == 1 && (*iIterator_self).lWatershed == lWatershed)
      {
        lCellID_downstream = (*iIterator_self).lCellID_downslope_dominant;
        lCellIndex_downstream = watershed_find_index_by_cell_id(lCellID_downstream);        
        if (lCellIndex_downstream != -1)
        {
          (vCell[lCellIndex_downstream]).vUpstream.push_back((*iIterator_self).lCellID); // use id instead of index
        }
        else
        {
          // std::cout << (*iIterator_self).lCellIndex << ", outlet is: " << lCellIndex_downstream << std::endl;
        }
      }
    }
    // sum up the size the upstream
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      (*iIterator_self).nUpstream = ((*iIterator_self).vUpstream).size();
    }
    // calculate total segment
    vConfluence.clear();
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      if ((*iIterator_self).nUpstream > 1 && (*iIterator_self).lWatershed == lWatershed && (*iIterator_self).iFlag_stream == 1)
      {
        iCount = iCount + 1;
        (*iIterator_self).iFlag_confluence = 1;

        vConfluence.push_back((*iIterator_self));
      }
    }
    nConfluence = iCount;
    nSegment = 1;
    for (iIterator_self = vConfluence.begin(); iIterator_self != vConfluence.end(); iIterator_self++)
    {
      nSegment = nSegment + (*iIterator_self).vUpstream.size();
    }
    nSubbasin = nSegment;
    // sort cannot be used directly here

    return error_code;
  }
  int watershed::watershed_define_stream_segment()
  {
    int error_code = 1;
    long lCellIndex_outlet; // this is a local variable for each subbasin
    int iFlag_confluence;
    int iFlag_headwater;

    int iUpstream;

    long lCellIndex_current;
    long lCellID_current;
    long lCellID_upstream;

    lCellIndex_outlet = watershed_find_index_by_cell_id(lCellID_outlet);
    iFlag_confluence = vCell[lCellIndex_outlet].iFlag_confluence;
    lCellIndex_current = vCell[lCellIndex_outlet].lCellIndex_watershed;
    lCellID_current = vCell[lCellIndex_outlet].lCellID;
    vSegment.clear();
    segment cSegment;
    std::vector<hexagon> vReach_segment;
    vCell[lCellIndex_outlet].iFlag_last_reach = 1;

    if (nSegment == 1) // these is only one segment in this watershed
    {
      lSegment_current = nSegment;
      vCell[lCellIndex_outlet].lSegment = lSegment_current;
      vReach_segment.push_back(vCell[lCellIndex_outlet]);
      iFlag_headwater = 0;
      while (iFlag_headwater != 1)
      {
        iUpstream = vCell[lCellIndex_current].nUpstream;
        if (iUpstream == 1) //
        {
          lCellID_upstream = (vCell[lCellIndex_current]).vUpstream[0];
          lCellIndex_current = watershed_find_index_by_cell_id(lCellID_upstream);
          vCell[lCellIndex_current].lSegment = lSegment_current;
          vReach_segment.push_back(vCell[lCellIndex_current]);
        }
        else
        { // headwater
          vCell[lCellIndex_current].iFlag_first_reach = 1;
          lCellID_current = vCell[lCellIndex_current].lCellID;
          iFlag_headwater = 1;
        }
      }

      // this is the last reach of a segment
      std::reverse(vReach_segment.begin(), vReach_segment.end());
      cSegment.vReach_segment = vReach_segment;
      cSegment.nReach = vReach_segment.size();
      cSegment.cReach_start = vReach_segment.front();
      cSegment.cReach_end = vReach_segment.back();
      cSegment.lSegment = 1;
      cSegment.lSegmentIndex = cSegment.lSegment - 1; // - cSegment.lSegment;
      cSegment.nSegment_upstream = cSegment.cReach_start.nUpstream;
      cSegment.iFlag_has_upstream = 0;
      cSegment.lWatershed = lWatershed;
      cSegment.iFlag_has_downstream = 0; // it has no downstream
      cSegment.iFlag_headwater = 1;
      vSegment.push_back(cSegment);

      mSegmentIdToIndex[cSegment.lSegment] = cSegment.lSegmentIndex; //setup unordered map 
    }
    else
    {
      lSegment_current = nSegment;
      vCell[lCellIndex_outlet].lSegment = lSegment_current;
      vReach_segment.push_back(vCell[lCellIndex_outlet]);
      if (iFlag_confluence == 1) // the outlet is actually a confluence
      {
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = 1;
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.lSegment = nSegment;
        cSegment.lSegmentIndex = cSegment.lSegment - 1; //- cSegment.lSegment;
        cSegment.nSegment_upstream = cSegment.cReach_start.nUpstream;
        cSegment.iFlag_has_upstream = 1;
        cSegment.iFlag_has_downstream = 0;
        cSegment.lWatershed = lWatershed;
        vSegment.push_back(cSegment);
        mSegmentIdToIndex[cSegment.lSegment] = cSegment.lSegmentIndex; //setup unordered map 
      }
      else
      {
        while (iFlag_confluence != 1)
        {
          iUpstream = vCell[lCellIndex_current].nUpstream;
          if (iUpstream == 1) //
          {
            lCellID_upstream = (vCell[lCellIndex_current]).vUpstream[0];
            lCellIndex_current = watershed_find_index_by_cell_id(lCellID_upstream);
            vCell[lCellIndex_current].lSegment = lSegment_current;
            vReach_segment.push_back(vCell[lCellIndex_current]);
          }
          else
          { // headwater
            // vCell[lCellIndex_current].lSegment = iSegment_current;
            vCell[lCellIndex_current].iFlag_first_reach = 1;
            lCellID_current = vCell[lCellIndex_current].lCellID;
            // vReach_segment.push_back(vCell[lCellIndex_current]);
            iFlag_confluence = 1;
            // std::cout << lCellID_current << std::endl;
          }
        }

        // this is the last reach of a segment
        std::reverse(vReach_segment.begin(), vReach_segment.end());
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = vReach_segment.size();
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.lSegment = nSegment;
        cSegment.lSegmentIndex = cSegment.lSegment - 1; // - cSegment.lSegment;
        cSegment.nSegment_upstream = cSegment.cReach_start.nUpstream;
        cSegment.iFlag_has_upstream = 1;
        cSegment.lWatershed = lWatershed;
        cSegment.iFlag_has_downstream = 0;
        if (cSegment.cReach_start.iFlag_headwater == 1)
        {
          cSegment.iFlag_headwater = 1;
        }
        vSegment.push_back(cSegment);
        mSegmentIdToIndex[cSegment.lSegment] = cSegment.lSegmentIndex; //setup unordered map 
      }
      lSegment_current = lSegment_current - 1;
      watershed_tag_confluence_upstream(lCellID_current);
      // in fact the segment is ordered by default already, just reservely
      // std::sort(vSegment.begin(), vSegment.end());
      std::reverse(vSegment.begin(), vSegment.end());
    }
    return error_code;
  }
  int watershed::watershed_tag_confluence_upstream(long lCellID_confluence)
  {
    int error_code = 1;
    int nUpstream;
    int iFlag_first_reach;
    int iFlag_confluence;
    long lSegment_confluence;
    long lCellID_upstream;
    long lCellIndex_upstream;
    segment cSegment; // be careful with the scope
    std::vector<long> vUpstream;
    std::vector<long>::iterator iterator_upstream;
    std::vector<hexagon> vReach_segment;

    long lCellIndex_confluence = watershed_find_index_by_cell_id(lCellID_confluence);

    // if may not be necessary to use these flags in this algorithm because it will only search for the segment
    vUpstream = vCell[lCellIndex_confluence].vUpstream;
    lSegment_confluence = vCell[lCellIndex_confluence].lSegment;
    for (iterator_upstream = vUpstream.begin();
         iterator_upstream != vUpstream.end();
         iterator_upstream++)
    {
      lCellID_upstream = *iterator_upstream;
      lCellIndex_upstream = watershed_find_index_by_cell_id(lCellID_upstream);
      iFlag_first_reach = 0;
      // remember that it is possible a segment only has one reach
      iFlag_confluence = vCell[lCellIndex_upstream].iFlag_confluence;
      vCell[lCellIndex_upstream].iFlag_last_reach = 1;
      // use last reach to find next stream segment
      vCell[lCellIndex_upstream].lSegment_downstream = vCell[lCellIndex_confluence].lSegment;
      vReach_segment.clear();

      // if the immediate upstream is also confluence: 1-1, we can quickly setup then move on
      // to the confluence
      if (iFlag_confluence == 1)
      {
        // continuous confluence, in this case, we need to set only one reach and move on
        nUpstream = (vCell[lCellIndex_upstream]).nUpstream;
        // we need to set 4 importnat attributes
        vCell[lCellIndex_upstream].lSegment = lSegment_current;
        (vCell[lCellIndex_upstream]).iFlag_first_reach = 1;
        //(vCell[lCellIndex_upstream]).iFlag_last_reach = 1;
        (vCell[lCellIndex_upstream]).iFlag_headwater = 0;

        vReach_segment.push_back(vCell[lCellIndex_upstream]);
        // it has only one reach
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = vReach_segment.size();
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.lSegment = lSegment_current;
        cSegment.lSegmentIndex = cSegment.lSegment - 1; //- cSegment.lSegment;
        cSegment.iFlag_has_downstream = 1;
        cSegment.iFlag_has_upstream = 1;
        cSegment.iFlag_headwater = 0;
        cSegment.lWatershed = lWatershed;
        cSegment.lSegment_downstream = lSegment_confluence;
        // add the segment to the watershed object
        vSegment.push_back(cSegment);
        mSegmentIdToIndex[cSegment.lSegment] = cSegment.lSegmentIndex; //setup unordered map 

        // update segment index
        lSegment_current = lSegment_current - 1;
        watershed_tag_confluence_upstream(lCellID_upstream);
      }
      else
      {
        // if there is at least one reach that is not a confluence: 1-0-1 or 1-0-2
        while (iFlag_confluence != 1) // 1-0-1
        {
          // it has only one upstream
          nUpstream = (vCell[lCellIndex_upstream]).nUpstream;
          (vCell[lCellIndex_upstream]).lSegment = lSegment_current;
          if (nUpstream == 0)
          {
            // this is the headwater, 1-0-2
            iFlag_first_reach = 1;
            (vCell[lCellIndex_upstream]).iFlag_first_reach = 1;
            (vCell[lCellIndex_upstream]).iFlag_headwater = 1;
            vReach_segment.push_back(vCell[lCellIndex_upstream]);
            break;
          }
          else
          {
            // 1-0-0
            (vCell[lCellIndex_upstream]).iFlag_last_reach = 0;
            (vCell[lCellIndex_upstream]).iFlag_first_reach = 0;
            (vCell[lCellIndex_upstream]).iFlag_headwater = 0;
            vReach_segment.push_back(vCell[lCellIndex_upstream]);
            // we are on the stream segment and there is only one upstream
            // move to upstream
            lCellID_upstream = (vCell[lCellIndex_upstream]).vUpstream[0];
            lCellIndex_upstream = watershed_find_index_by_cell_id(lCellID_upstream);
            iFlag_confluence = vCell[lCellIndex_upstream].iFlag_confluence;
            // should not add now
          }
        }
        // either we reach the headwater or we find the next confluence
        if (iFlag_first_reach == 1)
        {
          // it is already been pushed back
          cSegment.iFlag_has_upstream = 0;
          cSegment.iFlag_headwater = 1;
        }
        else
        {
          // this is a new confluence, so we need to push it back
          (vCell[lCellIndex_upstream]).lSegment = lSegment_current;
          (vCell[lCellIndex_upstream]).iFlag_last_reach = 0;
          (vCell[lCellIndex_upstream]).iFlag_first_reach = 1;
          (vCell[lCellIndex_upstream]).iFlag_headwater = 0;
          vReach_segment.push_back(vCell[lCellIndex_upstream]);

          cSegment.iFlag_has_upstream = 1;
          cSegment.iFlag_headwater = 0;
        }

        std::reverse(vReach_segment.begin(), vReach_segment.end());
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = vReach_segment.size();
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.lSegment = lSegment_current;
        cSegment.lSegmentIndex = cSegment.lSegment - 1; // nSegment - cSegment.lSegment;
        cSegment.iFlag_has_downstream = 1;
        cSegment.lWatershed = lWatershed;
        cSegment.lSegment_downstream = lSegment_confluence;
        vSegment.push_back(cSegment);
        mSegmentIdToIndex[cSegment.lSegment] = cSegment.lSegmentIndex; //setup unordered map 
        lSegment_current = lSegment_current - 1;
        if (iFlag_first_reach != 1)
        {
          watershed_tag_confluence_upstream(lCellID_upstream);
        }
      }
    }

    return error_code;
  }

  /**
   * build the stream topology based on stream segment information
   * @return
   */
  int watershed::watershed_build_stream_topology()
  {
    int error_code = 1;
    long lSegment;
    // rebuild stream topology
    std::vector<segment>::iterator iIterator_segment_self;
    std::vector<segment>::iterator iIterator_segment;

    if (nSegment == 1)
    {
      vSegment[0].nSegment_upstream = 0;
      vSegment[0].vSegment_upstream.clear();
    }
    else
    {
      for (iIterator_segment_self = vSegment.begin();
           iIterator_segment_self != vSegment.end();
           iIterator_segment_self++)
      {
        lSegment = (*iIterator_segment_self).lSegment_downstream;
        for (iIterator_segment = vSegment.begin();
             iIterator_segment != vSegment.end();
             iIterator_segment++)
        {
          if (lSegment == (*iIterator_segment).lSegment)
          {
            (*iIterator_segment).vSegment_upstream.push_back((*iIterator_segment_self).lSegment);
          }
        }
      }
      for (iIterator_segment = vSegment.begin();
           iIterator_segment != vSegment.end();
           iIterator_segment++)
      {
        (*iIterator_segment).nSegment_upstream = (*iIterator_segment).vSegment_upstream.size();
      }
    }

    return error_code;
  }

  /**
   * build the stream order based on stream topology
   * @return
   */
  int watershed::watershed_define_stream_order()
  {
    int error_code = 1;
    long lSegment;
    int iUpstream;
    int iStream_order_max;
    int iFlag_all_upstream_done;
    std::vector<int> vStream_order;
    std::vector<segment>::iterator iIterator_segment;
    if (nSegment == 1)
    {
      vSegment[0].iSegment_order = 1;
    }
    else
    {

      for (iIterator_segment = vSegment.begin();
           iIterator_segment != vSegment.end();
           iIterator_segment++)
      {
        if ((*iIterator_segment).iFlag_headwater == 1)
        {
          (*iIterator_segment).iSegment_order = 1;
        }
      }

      while (vSegment.back().iSegment_order == -1)
      {
        for (iIterator_segment = vSegment.begin();
             iIterator_segment != vSegment.end();
             iIterator_segment++)
        {

          if ((*iIterator_segment).iSegment_order == -1)
          {
            iFlag_all_upstream_done = 1;
            vStream_order.clear();
            for (iUpstream = 0;
                 iUpstream < (*iIterator_segment).nSegment_upstream;
                 iUpstream++)
            {
              lSegment = (*iIterator_segment).vSegment_upstream[iUpstream];
              if (vSegment[lSegment - 1].iSegment_order == -1)
              {
                iFlag_all_upstream_done = 0;
              }
              else
              {
                vStream_order.push_back(vSegment[lSegment - 1].iSegment_order);
              }
            }

            if (iFlag_all_upstream_done == 1)
            {
              if (vStream_order[0] == vStream_order[1])
              {
                (*iIterator_segment).iSegment_order = vStream_order[0] + 1;
              }
              else
              {
                iStream_order_max = (*max_element(std::begin(vStream_order), std::end(vStream_order))); // c++11
                (*iIterator_segment).iSegment_order = iStream_order_max;
              }
            }
          }
          else
          {
            // finished
          }
        }
      }
    }
    return error_code;
  }
  
  // The new method for performance improvement
  int watershed::watershed_define_subbasin()
  {

    int error_code = 1;
    int iFound_outlet;
    int iFlag_checked;
    int iFlag_checked_downslope;
    long lSubbasin;
    long lCellIndex_self;
    long lCellIndex_current;
    long lCellID_outlet;
    long lCellIndex_outlet; // local outlet
    long lCellIndex_subbasin;
    long lCellID_downslope;
    long lCellIndex_downslope;
    long lCellIndex_accumulation;
    long lIndex_confluence;
    std::vector<float> vAccumulation;
    std::vector<float>::iterator iterator_accumulation;
    std::vector<hexagon>::iterator iIterator_self;
    std::vector<long>::iterator iIterator_path;
    std::vector<hexagon>::iterator iIterator_current;
    std::vector<long> vSearchPath;
    // assign watershed subbasin cell, maybe later?
    vSubbasin.clear();
    for (long lSubbasin = 1; lSubbasin <= nSubbasin; lSubbasin++)
    {
      subbasin cSubbasin;
      cSubbasin.lSubbasin = lSubbasin;
      cSubbasin.lSubbasinIndex = cSubbasin.lSubbasin - 1;
      vSubbasin.push_back(cSubbasin);
      mSubbasinIdToIndex[cSubbasin.lSubbasin] = cSubbasin.lSubbasin; //setup unordered map 
    }
    // the whole watershed first

    // reset flag, we set all segment cell as
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      if ((*iIterator_self).lSegment >= 1)
      {
        lSubbasin = (*iIterator_self).lSegment;
        (*iIterator_self).iFlag_checked = 1;
        (*iIterator_self).lSubbasin = lSubbasin;
        (*iIterator_self).lCellIndex_subbasin = vSubbasin[lSubbasin - 1].vCell.size();
        vSubbasin[lSubbasin - 1].vCell.push_back((*iIterator_self));
        vSubbasin[lSubbasin - 1].mCellIdToIndex[(*iIterator_self).lCellID] = (*iIterator_self).lCellIndex_subbasin;
      }
      else
      {
        (*iIterator_self).iFlag_checked = 0;
      }
    }

    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      lCellIndex_current = (*iIterator_self).lCellIndex_watershed;
      iFlag_checked = (*iIterator_self).iFlag_checked;
      if (iFlag_checked == 0)
      {
        vSearchPath.clear();
        iFlag_checked_downslope = 0;
        while (iFlag_checked_downslope == 0)
        { // not found keep adding to path
          vSearchPath.push_back(lCellIndex_current);
          lCellID_downslope = vCell[lCellIndex_current].lCellID_downslope_dominant;
          lCellIndex_current = watershed_find_index_by_cell_id(lCellID_downslope);
          if (lCellIndex_current != -1)
          {
            iFlag_checked_downslope = vCell[lCellIndex_current].iFlag_checked;
          }
          else
          {
            std::cout << "This shouldn't happen!" << std::endl;
          }
        }
        lSubbasin = vCell[lCellIndex_current].lSubbasin;
        for (iIterator_path = vSearchPath.begin(); iIterator_path != vSearchPath.end(); iIterator_path++)
        {
          vCell[*iIterator_path].lSubbasin = lSubbasin;
          vCell[*iIterator_path].iFlag_checked = 1;
          vCell[*iIterator_path].lCellIndex_subbasin = vSubbasin[lSubbasin - 1].vCell.size();
          vSubbasin[lSubbasin - 1].vCell.push_back(vCell[*iIterator_path]);
          vSubbasin[lSubbasin - 1].mCellIdToIndex[vCell[*iIterator_path].lCellID] = vCell[*iIterator_path].lCellIndex_subbasin;

        }
      }
    }
    return error_code;
  }
  int watershed::watershed_update_attribute()
  {
    int error_code = 1;
    int iFlag_found;
    long lCellID;
    long lCellID2;
    long lCellIndex_watershed;
    std::vector<hexagon>::iterator iIterator_self;
    std::vector<hexagon>::iterator iIterator1;
    std::vector<hexagon>::iterator iIterator2;

    for (long lSubbasin = 1; lSubbasin <= nSubbasin; lSubbasin++)
    {
      for (iIterator2 = vSubbasin[lSubbasin - 1].vCell.begin(); iIterator2 != vSubbasin[lSubbasin - 1].vCell.end(); iIterator2++)
      {
        lCellIndex_watershed = (*iIterator2).lCellIndex_watershed;
        //may also use the map to find index
        vCell[lCellIndex_watershed].lSubbasin = (*iIterator2).lSubbasin;
        vCell[lCellIndex_watershed].dDistance_to_subbasin_outlet = (*iIterator2).dDistance_to_subbasin_outlet;
        vCell[lCellIndex_watershed].dDistance_to_watershed_outlet = (*iIterator2).dDistance_to_watershed_outlet;
      }
    }

    return error_code;
  }

  /**
   * calculate the watershed characteristics
   * @return
   */
  int watershed::watershed_calculate_characteristics()
  {
    int error_code = 1;
    long lSegment;
    long lSegment_downstream;
    long lSegmentIndex;
    float dLength;
    float dDistance_to_watershed_outlet;
    float dLength_stream_conceptual;

    std::vector<segment>::iterator iIterator0;
    std::vector<segment>::iterator iIterator2;
    std::vector<subbasin>::iterator iIterator1;

    for (iIterator0 = vSegment.begin(); iIterator0 != vSegment.end(); iIterator0++)
    {
      (*iIterator0).segment_calculate_stream_segment_characteristics();
    }
    for (iIterator0 = vSegment.begin(); iIterator0 != vSegment.end(); iIterator0++)
    {
      if ((*iIterator0).lSegment == nSegment)
      {
        // this is the last segment
        (*iIterator0).dDistance_to_watershed_outlet = 0.0;
      }
      else
      {
        lSegment_downstream = (*iIterator0).lSegment_downstream;
        dDistance_to_watershed_outlet = 0.0; //(*iIterator0).dLength;
        while (lSegment_downstream != nSegment)
        {
          lSegmentIndex = watershed_find_index_by_segment_id(lSegment_downstream);
          dLength = vSegment[lSegmentIndex].dLength;
          dDistance_to_watershed_outlet = dDistance_to_watershed_outlet + dLength;
          lSegment_downstream = vSegment[lSegmentIndex].lSegment_downstream;
        }
        // add last segment
        lSegmentIndex = watershed_find_index_by_segment_id(lSegment_downstream);
        dDistance_to_watershed_outlet = dDistance_to_watershed_outlet + vSegment[lSegmentIndex].dLength;

        (*iIterator0).dDistance_to_watershed_outlet = dDistance_to_watershed_outlet;
      }
    }

    for (long lSubbasin = 1; lSubbasin <= nSubbasin; lSubbasin++)
    {
      lSegment = lSubbasin;
      vSubbasin[lSubbasin - 1].cCell_outlet = vSegment[lSegment - 1].cReach_end;
      vSubbasin[lSubbasin - 1].lCellID_outlet = vSegment[lSegment - 1].cReach_end.lCellID;
      dLength_stream_conceptual = vSegment[lSegment - 1].dLength;
      vSubbasin[lSubbasin - 1].subbasin_calculate_characteristics(dLength_stream_conceptual);
    }

    watershed_calculate_travel_distance();
    watershed_update_attribute();
    watershed_calculate_drainage_area();
    watershed_calculate_total_stream_length();
    watershed_calculate_longest_stream_length();
    watershed_calculate_drainage_density();
    watershed_calculate_average_slope();
    watershed_calculate_topographic_wetness_index();

    // save watershed characteristics to the file

    // std::cout << "The watershed characteristics are calculated successfully!" << std::endl;

    return error_code;
  }

  /**
   * calculate the watershed drainage total area
   * we can either sum up hexagon or sum up subbasin
   * @return
   */
  int watershed::watershed_calculate_drainage_area()
  {
    int error_code = 1;
    int iOption = 2; // sum up subbasin

    float dArea_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    std::vector<subbasin>::iterator iIterator1;
    if (iOption == 1)
    {
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        dArea_total = dArea_total + (*iIterator).dArea;
      }
    }
    else
    {
      for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
      {
        dArea_total = dArea_total + (*iIterator1).dArea;
      }
    }
    dArea = dArea_total;
    return error_code;
  }

  /**
   * calculate the total stream length
   * @return
   */
  int watershed::watershed_calculate_total_stream_length()
  {
    int error_code = 1;
    int iOption = 2; // sum up subbasin

    float dLength_total = 0.0;

    std::vector<hexagon>::iterator iIterator;
    std::vector<segment>::iterator iIterator1;
    if (iOption == 1)
    {
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {

        if ((*iIterator).iFlag_stream == 1)
        {
          // should have calculated dLength_stream_conceptual by now
          dLength_total = dLength_total + (*iIterator).dLength_stream_conceptual;
        }
      }
    }
    else
    {
      for (iIterator1 = vSegment.begin(); iIterator1 != vSegment.end(); iIterator1++)
      {
        dLength_total = dLength_total + (*iIterator1).dLength;
      }
    }

    dLength_stream_conceptual = dLength_total;

    return error_code;
  }

  /**
   * calculate the longest stream length
   * @return
   */
  int watershed::watershed_calculate_longest_stream_length()
  {
    int error_code = 1;

    float dLength_longest = 0.0;
    float dLength_stream_conceptual;

    // loop through head water
    std::vector<segment>::iterator iIterator;

    for (iIterator = vSegment.begin(); iIterator != vSegment.end(); iIterator++)
    {
      dLength_stream_conceptual = (*iIterator).dLength;
      if (dLength_stream_conceptual > dLength_longest)
      {
        dLength_longest = dLength_stream_conceptual;
      }
    }

    dLongest_length_stream = dLength_longest;

    return error_code;
  }

  /**
   * calculate the watershed area to stream length ratio
   * @return
   */
  int watershed::watershed_calculate_drainage_density()
  {
    int error_code = 1;

    float dRatio = 0.0;

    if (dLength_stream_conceptual > 0)
    {
      dRatio = dArea / dLength_stream_conceptual;
    }

    dArea_2_stream_ratio = dRatio;
    dLength_2_area_ratio = 1.0 / dRatio;
    dDrainage_density = dLength_2_area_ratio;

    return error_code;
  }

  /**
   * calculate the mean slope of the watershed
   * we can use either subbasin or each cell
   * @return
   */
  int watershed::watershed_calculate_average_slope()
  {
    int error_code = 1;
    int iOption = 1;
    float dSlope_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    std::vector<subbasin>::iterator iIterator1;

    nCell = vCell.size();
    if (iOption == 1) // by cell
    {
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        dSlope_total = dSlope_total + (*iIterator).dSlope_max_downslope; // should mean slope?
      }
    }
    else // by subbasin
    {
      for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
      {
        dSlope_total = dSlope_total + (*iIterator1).dSlope;
      }
    }

    dSlope = dSlope_total / nCell;
    dSlope_mean = dSlope;
    return error_code;
  }

  /**
   * calculate the TWI index using method from //https://en.wikipedia.org/wiki/Topographic_wetness_index
   * // {\displaystyle \ln {a \over \tan b}}
   * @return
   */
  int watershed::watershed_calculate_topographic_wetness_index()
  {
    int error_code = 1;
    float a;
    float b;
    float c;
    float d;
    float dTwi;
    std::vector<hexagon>::iterator iIterator;
    // can use openmp
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      // if ((*iIterator).iFlag_outlet == 1)
      //{
      //   (*iIterator).dTwi = -1;
      // }
      // else
      //{
      a = float(((*iIterator).dAccumulation));
      b = (*iIterator).dSlope_max_downslope;
      c = tan(b);
      if (a == 0 || c == 0)
      {
        std::cout << a << b << c << std::endl;
      }
      dTwi = log2(a / c);
      if (isnan(float(dTwi)))
      {
        std::cout << a << b << c << std::endl;
      }
      (*iIterator).dTwi = dTwi;
      //}
    }

    return error_code;
  }

  int watershed::watershed_calculate_travel_distance()
  {
    int error_code = 1;
    long lSegment;
    long lSubbasin;
    long lSegment_downstream;
    long lSegmentIndex;
    float dDistance_to_watershed_outlet;
    // calculate confluence travel
    std::vector<hexagon>::iterator iIterator;
    std::vector<segment>::iterator iIterator0;
    std::vector<subbasin>::iterator iIterator1;

    for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
    {
      (*iIterator1).subbasin_calculate_travel_distance();
    }

    for (lSubbasin = 1; lSubbasin <= nSubbasin; lSubbasin++)
    {
      lSegment = lSubbasin;
      if (lSubbasin == nSubbasin)
      {
        for (iIterator = vSubbasin[lSubbasin - 1].vCell.begin(); iIterator != vSubbasin[lSubbasin - 1].vCell.end(); iIterator++)
        {
          (*iIterator).dDistance_to_watershed_outlet = (*iIterator).dDistance_to_subbasin_outlet;
        }
      }
      else
      {
        lSegmentIndex = watershed_find_index_by_segment_id(lSegment);

        dDistance_to_watershed_outlet = vSegment[lSegmentIndex].dDistance_to_watershed_outlet;
        for (iIterator = vSubbasin[lSubbasin - 1].vCell.begin(); iIterator != vSubbasin[lSubbasin - 1].vCell.end(); iIterator++)
        {
          (*iIterator).dDistance_to_watershed_outlet = (*iIterator).dDistance_to_subbasin_outlet + dDistance_to_watershed_outlet;
        }
      }
    }

    return error_code;
  }
  /**
   * save the watershed characteristics in the output
   * @return
   */
  int watershed::watershed_export_characteristics()
  {
    int error_code = 1;
    std::string sLine;
    std::ofstream ofs;
    ofs.open(sFilename_watershed_characteristics.c_str(), ios::out);
    if (ofs.good())
    {
      sLine = "Total number of cells: " + convert_float_to_string(nCell);
      ofs << sLine << std::endl;

      sLine = "Watershed drainage area: " + convert_float_to_string(dArea);
      ofs << sLine << std::endl;

      sLine = "Longest stream length: " + convert_float_to_string(dLongest_length_stream);
      ofs << sLine << std::endl;

      sLine = "Total stream length: " + convert_float_to_string(dLength_stream_conceptual);
      ofs << sLine << std::endl;

      sLine = "Drainage density: " + convert_float_to_string(dDrainage_density);
      ofs << sLine << std::endl;

      sLine = "Average slope: " + convert_float_to_string(dSlope_mean);
      ofs << sLine << std::endl;

      ofs.close();
    }

    return error_code;
  }

  int watershed::watershed_export_segment_characteristics()
  {
    int error_code = 1;
    std::string sLine;
    std::vector<segment>::iterator iIterator1;
    std::ofstream ofs;
    ofs.open(sFilename_segment_characteristics.c_str(), ios::out);
    if (ofs.good())
    {
      sLine = "Segment ID, downstream segment ID, stream order, total length, elevation drop, average slope";
      ofs << sLine << std::endl;
      for (iIterator1 = vSegment.begin(); iIterator1 != vSegment.end(); iIterator1++)
      {

        sLine = convert_long_to_string((*iIterator1).lSegment) + "," + convert_long_to_string((*iIterator1).lSegment_downstream) + "," + convert_integer_to_string((*iIterator1).iSegment_order) + "," + convert_float_to_string((*iIterator1).dLength) + "," + convert_float_to_string((*iIterator1).dElevation_drop) + "," + convert_float_to_string((*iIterator1).dSlope_mean);
        ofs << sLine << std::endl;
      }
      ofs.close();
    }
    return error_code;
  }

  int watershed::watershed_export_subbasin_characteristics()
  {
    int error_code = 1;
    std::string sLine;
    std::vector<subbasin>::iterator iIterator1;
    std::ofstream ofs;
    ofs.open(sFilename_subbasin_characteristics.c_str(), ios::out);
    if (ofs.good())
    {
      sLine = "Subbasin ID, Outlet ID, number of cell, total area, average slope, area_to_length, drainage density";
      ofs << sLine << std::endl;
      for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
      {
        sLine = convert_integer_to_string((*iIterator1).lSubbasin) + "," + convert_long_to_string((*iIterator1).lCellID_outlet) + "," + convert_long_to_string((*iIterator1).nCell) + "," + convert_float_to_string((*iIterator1).dArea) + "," + convert_float_to_string((*iIterator1).dSlope_mean) + "," + convert_float_to_string((*iIterator1).dArea_2_stream_ratio) + "," + convert_float_to_string((*iIterator1).dDrainage_density) + ",";
        ofs << sLine << std::endl;
      }
      ofs.close();
    }
    return error_code;
  }

  int watershed::watershed_export_json()
  {
    int error_code = 1;
    jsonmodel::mesh cMesh;
   
    //for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    for (const hexagon& pHexagon : vCell)
    {
      if (pHexagon.iFlag_watershed == 1)
      {
        cell pCell;
        pCell.dLongitude_center_degree = pHexagon.dLongitude_center_degree;
        pCell.dLatitude_center_degree =pHexagon.dLatitude_center_degree;
        pCell.dSlope_between = pHexagon.dSlope_max_downslope;
        pCell.dSlope_profile = pHexagon.dSlope_elevation_profile0;
        pCell.dDistance_to_downslope = pHexagon.dDistance_to_downslope;
        pCell.dDistance_to_subbasin_outlet = pHexagon.dDistance_to_subbasin_outlet;
        pCell.dDistance_to_watershed_outlet = pHexagon.dDistance_to_watershed_outlet;
        pCell.dElevation_mean = pHexagon.dElevation_mean;
        pCell.dElevation_raw = pHexagon.dElevation_raw;
        pCell.dElevation_profile0 = pHexagon.dElevation_profile0;
        pCell.dLength = pHexagon.dLength_stream_conceptual;
        pCell.dLength_flowline = pHexagon.dLength_stream_burned;
        pCell.dArea = pHexagon.dArea;
        pCell.lCellID = pHexagon.lCellID;
        pCell.lStream_segment = pHexagon.lSegment;
        pCell.lSubbasin = pHexagon.lSubbasin;
        pCell.lStream_segment_burned = pHexagon.lStream_segment_burned; // flag for burned stream
        pCell.lCellID_downslope = pHexagon.lCellID_downslope_dominant;
        pCell.dAccumulation = pHexagon.dAccumulation;
        pCell.vVertex = pHexagon.vVertex;
        pCell.nVertex = pCell.vVertex.size();
        cMesh.aCell.push_back(pCell);
      }
    }

    cMesh.SerializeToFile(sFilename_watershed_json.c_str());
    return error_code;
  }

  int watershed::watershed_export_stream_edge_json()
  {
    int error_code = 1;
    std::vector<segment>::iterator iIterator1;
    std::vector<hexagon>::iterator iIterator;

    jsonmodel::mesh cMesh;
    for (iIterator1 = vSegment.begin(); iIterator1 != vSegment.end(); iIterator1++)
    {
      for (iIterator = (*iIterator1).vReach_segment.begin(); iIterator != (*iIterator1).vReach_segment.end(); iIterator++)
      {
        cell pCell;
        pCell.dLongitude_center_degree = (*iIterator).dLongitude_center_degree;
        pCell.dLatitude_center_degree = (*iIterator).dLatitude_center_degree;
        pCell.dSlope_between = (*iIterator).dSlope_max_downslope;
        pCell.dSlope_profile = (*iIterator).dSlope_elevation_profile0;
        pCell.dDistance_to_downslope = (*iIterator).dDistance_to_downslope;
        pCell.dDistance_to_subbasin_outlet = (*iIterator).dDistance_to_subbasin_outlet;
        pCell.dDistance_to_watershed_outlet = (*iIterator).dDistance_to_watershed_outlet;
        pCell.dElevation_mean = (*iIterator).dElevation_mean;
        pCell.dElevation_raw = (*iIterator).dElevation_raw;
        pCell.dElevation_profile0 = (*iIterator).dElevation_profile0;
        pCell.dLength = (*iIterator).dLength_stream_conceptual;
        pCell.dLength_flowline = (*iIterator).dLength_stream_burned;
        pCell.dArea = (*iIterator).dArea;
        pCell.lCellID = (*iIterator).lCellID;
        pCell.lStream_segment = (*iIterator).lSegment;
        pCell.lSubbasin = (*iIterator).lSubbasin;
        pCell.lStream_segment_burned = (*iIterator).lStream_segment_burned; // flag for burned stream
        pCell.lCellID_downslope = (*iIterator).lCellID_downslope_dominant;
        pCell.dAccumulation = (*iIterator).dAccumulation;
        pCell.vVertex = (*iIterator).vVertex;
        pCell.nVertex = pCell.vVertex.size();
        cMesh.aCell.push_back(pCell);
      }
    }
    cMesh.SerializeToFile(sFilename_watershed_stream_edge_json.c_str());
    return error_code;
  }

  long watershed::watershed_find_index_by_cell_id(long lCellID_in)
  {  
    auto iIterator = mCellIdToIndex.find(lCellID_in);
    if (iIterator != mCellIdToIndex.end())
    {
      return iIterator->second;
    }
    else
    {
      return -1;
    }
  }

  long watershed::watershed_find_index_by_segment_id(long lSegment_in)
  {   
    auto iIterator = mSegmentIdToIndex.find(lSegment_in);
    if (iIterator != mSegmentIdToIndex.end())
    {
      return iIterator->second;
    }
    else
    {
      return -1;
    }    
  }

  long watershed::watershed_find_index_by_subbasin_id(long lSegment_in)
  {    
    auto iIterator = mSubbasinIdToIndex.find(lSegment_in);
    if (iIterator != mSubbasinIdToIndex.end())
    {
      return iIterator->second;
    }
    else
    {
      return -1;
    }
  }

} // namespace hexwatershed
