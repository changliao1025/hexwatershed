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
    iWatershed = -1;
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
    int iWatershed;
    long lCellID_downstream;
    long lCellIndex_downstream;
    std::vector<hexagon>::iterator iIterator_self;

    iCount = 0;

    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      // we only consider cells within the watershed
      if ((*iIterator_self).iFlag_stream == 1 && (*iIterator_self).iWatershed == iWatershed)
      {
        lCellID_downstream = (*iIterator_self).lCellID_downslope_dominant;
        lCellIndex_downstream = watershed_find_index_by_cell_id(lCellID_downstream);
        if (lCellIndex_downstream != -1)
        {
          (vCell.at(lCellIndex_downstream)).vUpstream.push_back((*iIterator_self).lCellID); // use id instead of index
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
      if ((*iIterator_self).nUpstream > 1 && (*iIterator_self).iWatershed == iWatershed && (*iIterator_self).iFlag_stream == 1)
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
    // sort cannot be used directly here

    return error_code;
  }
  int watershed::watershed_define_stream_segment()
  {
    int error_code = 1;
    long lCellIndex_outlet; // this is a local variable for each subbasin
    int iFlag_confluence;

    int iUpstream;
    int iWatershed;
    long lCellIndex_current;
    long lCellID_current;
    long lCellID_upstream;

    // nSegment = nSegment;
    lCellIndex_outlet = watershed_find_index_by_cell_id(lCellID_outlet);
    iFlag_confluence = vCell.at(lCellIndex_outlet).iFlag_confluence;
    lCellIndex_current = vCell.at(lCellIndex_outlet).lCellIndex;
    vSegment.clear();
    segment cSegment;
    std::vector<hexagon> vReach_segment;
    vCell.at(lCellIndex_outlet).iFlag_last_reach = 1;
    iSegment_current = nSegment;
    vCell.at(lCellIndex_outlet).iSegment = iSegment_current;
    vReach_segment.push_back(vCell.at(lCellIndex_outlet));
    if (iFlag_confluence == 1) // the outlet is actually a confluence
    {
      cSegment.vReach_segment = vReach_segment;
      cSegment.nReach = 1;
      cSegment.cReach_start = vReach_segment.front();
      cSegment.cReach_end = vReach_segment.back();
      cSegment.iSegment = nSegment;
      cSegment.iSegmentIndex = nSegment - cSegment.iSegment;
      cSegment.nSegment_upstream = cSegment.cReach_start.nUpstream;
      cSegment.iFlag_has_upstream = 1;
      cSegment.iFlag_has_downstream = 0;
      cSegment.iWatershed = iWatershed;
      vSegment.push_back(cSegment);
    }
    else
    {
      while (iFlag_confluence != 1)
      {
        iUpstream = vCell.at(lCellIndex_current).nUpstream;
        if (iUpstream == 1) //
        {
          lCellID_upstream = (vCell.at(lCellIndex_current)).vUpstream[0];
          lCellIndex_current = watershed_find_index_by_cell_id(lCellID_upstream);
          vCell.at(lCellIndex_current).iSegment = iSegment_current;
          vReach_segment.push_back(vCell.at(lCellIndex_current));
        }
        else
        { // headwater
          vCell.at(lCellIndex_current).iSegment = iSegment_current;
          vCell.at(lCellIndex_current).iFlag_first_reach = 1;
          lCellID_current = vCell.at(lCellIndex_current).lCellID;
          vReach_segment.push_back(vCell.at(lCellIndex_current));
          iFlag_confluence = 1;
        }
      }

      // this is the last reach of a segment
      std::reverse(vReach_segment.begin(), vReach_segment.end());
      cSegment.vReach_segment = vReach_segment;
      cSegment.nReach = vReach_segment.size();
      cSegment.cReach_start = vReach_segment.front();
      cSegment.cReach_end = vReach_segment.back();
      cSegment.iSegment = nSegment;
      cSegment.iSegmentIndex = nSegment - cSegment.iSegment;
      cSegment.nSegment_upstream = cSegment.cReach_start.nUpstream;
      cSegment.iFlag_has_upstream = 1;
      cSegment.iWatershed = iWatershed;
      cSegment.iFlag_has_downstream = 0;
      if (cSegment.cReach_start.iFlag_headwater == 1)
      {
        cSegment.iFlag_headwater = 1;
      }
      vSegment.push_back(cSegment);
    }
    iSegment_current = iSegment_current - 1;
    watershed_tag_confluence_upstream(lCellID_current);
    // in fact the segment is ordered by default already, just reservely
    std::sort(vSegment.begin(), vSegment.end());

    return error_code;
  }
  int watershed::watershed_tag_confluence_upstream(long lCellID_confluence)
  {
    int error_code = 1;
    int nUpstream;
    int iFlag_first_reach;
    int iFlag_confluence;
    int iSegment_confluence;
    long lCellID_upstream;
    long lCellIndex_upstream;
    segment cSegment; // be careful with the scope
    std::vector<long> vUpstream;
    std::vector<long>::iterator iterator_upstream;
    std::vector<hexagon> vReach_segment;

    long lCellIndex_confluence = watershed_find_index_by_cell_id(lCellID_confluence);

    // if may not be necessary to use these flags in this algorithm because it will only search for the segment
    vUpstream = vCell.at(lCellIndex_confluence).vUpstream;
    iSegment_confluence = vCell.at(lCellIndex_confluence).iSegment;
    for (iterator_upstream = vUpstream.begin();
         iterator_upstream != vUpstream.end();
         iterator_upstream++)
    {
      lCellID_upstream = *iterator_upstream;
      lCellIndex_upstream = watershed_find_index_by_cell_id(lCellID_upstream);
      iFlag_first_reach = 0;
      // remember that it is possible a segment only has one reach
      iFlag_confluence = vCell.at(lCellIndex_upstream).iFlag_confluence;
      vCell.at(lCellIndex_upstream).iFlag_last_reach = 1;
      // use last reach to find next stream segment
      vCell.at(lCellIndex_upstream).iSegment_downstream = vCell.at(lCellIndex_confluence).iSegment;
      vReach_segment.clear();

      // if the immediate upstream is also confluence: 1-1, we can quickly setup then move on
      // to the confluence
      if (iFlag_confluence == 1)
      {
        // continuous confluence, in this case, we need to set only one reach and move on
        nUpstream = (vCell.at(lCellIndex_upstream)).nUpstream;
        // we need to set 4 importnat attributes
        vCell.at(lCellIndex_upstream).iSegment = iSegment_current;
        (vCell.at(lCellIndex_upstream)).iFlag_first_reach = 1;
        //(vCell.at(lCellIndex_upstream)).iFlag_last_reach = 1;
        (vCell.at(lCellIndex_upstream)).iFlag_headwater = 0;

        vReach_segment.push_back(vCell.at(lCellIndex_upstream));
        // it has only one reach
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = vReach_segment.size();
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.iSegment = iSegment_current;
        cSegment.iSegmentIndex = nSegment - cSegment.iSegment;
        cSegment.iFlag_has_downstream = 1;
        cSegment.iFlag_has_upstream = 1;
        cSegment.iFlag_headwater = 0;
        cSegment.iWatershed = iWatershed;
        cSegment.iSegment_downstream = iSegment_confluence;
        // add the segment to the watershed object
        vSegment.push_back(cSegment);

        // update segment index
        iSegment_current = iSegment_current - 1;
        watershed_tag_confluence_upstream(lCellID_upstream);
      }
      else
      {
        // if there is at least one reach that is not a confluence: 1-0-1 or 1-0-2
        while (iFlag_confluence != 1) // 1-0-1
        {
          // it has only one upstream
          nUpstream = (vCell.at(lCellIndex_upstream)).nUpstream;
          (vCell.at(lCellIndex_upstream)).iSegment = iSegment_current;
          if (nUpstream == 0)
          {
            // this is the headwater, 1-0-2
            iFlag_first_reach = 1;
            (vCell.at(lCellIndex_upstream)).iFlag_first_reach = 1;
            (vCell.at(lCellIndex_upstream)).iFlag_headwater = 1;
            vReach_segment.push_back(vCell.at(lCellIndex_upstream));
            break;
          }
          else
          {
            // 1-0-0
            (vCell.at(lCellIndex_upstream)).iFlag_last_reach = 0;
            (vCell.at(lCellIndex_upstream)).iFlag_first_reach = 0;
            (vCell.at(lCellIndex_upstream)).iFlag_headwater = 0;
            vReach_segment.push_back(vCell.at(lCellIndex_upstream));
            // we are on the stream segment and there is only one upstream
            // move to upstream
            lCellID_upstream = (vCell.at(lCellIndex_upstream)).vUpstream[0];
            lCellIndex_upstream = watershed_find_index_by_cell_id(lCellID_upstream);
            iFlag_confluence = vCell.at(lCellIndex_upstream).iFlag_confluence;
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
          (vCell.at(lCellIndex_upstream)).iSegment = iSegment_current;
          (vCell.at(lCellIndex_upstream)).iFlag_last_reach = 0;
          (vCell.at(lCellIndex_upstream)).iFlag_first_reach = 1;
          (vCell.at(lCellIndex_upstream)).iFlag_headwater = 0;
          vReach_segment.push_back(vCell.at(lCellIndex_upstream));

          cSegment.iFlag_has_upstream = 1;
          cSegment.iFlag_headwater = 0;
        }

        std::reverse(vReach_segment.begin(), vReach_segment.end());
        cSegment.vReach_segment = vReach_segment;
        cSegment.nReach = vReach_segment.size();
        cSegment.cReach_start = vReach_segment.front();
        cSegment.cReach_end = vReach_segment.back();
        cSegment.iSegment = iSegment_current;
        cSegment.iSegmentIndex = nSegment - cSegment.iSegment;
        cSegment.iFlag_has_downstream = 1;
        cSegment.iWatershed = iWatershed;
        cSegment.iSegment_downstream = iSegment_confluence;
        vSegment.push_back(cSegment);
        iSegment_current = iSegment_current - 1;
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
    int iSegment;
    // rebuild stream topology
    std::vector<segment>::iterator iIterator_segment_self;
    std::vector<segment>::iterator iIterator_segment;
    for (iIterator_segment_self = vSegment.begin();
         iIterator_segment_self != vSegment.end();
         iIterator_segment_self++)
    {
      iSegment = (*iIterator_segment_self).iSegment_downstream;
      for (iIterator_segment = vSegment.begin();
           iIterator_segment != vSegment.end();
           iIterator_segment++)
      {
        if (iSegment == (*iIterator_segment).iSegment)
        {
          (*iIterator_segment).vSegment_upstream.push_back((*iIterator_segment_self).iSegment);
        }
      }
    }
    for (iIterator_segment = vSegment.begin();
         iIterator_segment != vSegment.end();
         iIterator_segment++)
    {
      (*iIterator_segment).nSegment_upstream = (*iIterator_segment).vSegment_upstream.size();
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
    int iSegment;
    int iUpstream;
    int iStream_order_max;
    int iFlag_all_upstream_done;

    std::vector<int> vStream_order;

    std::vector<segment>::iterator iIterator_segment;

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
            iSegment = (*iIterator_segment).vSegment_upstream.at(iUpstream);
            if (vSegment.at(iSegment - 1).iSegment_order == -1)
            {
              iFlag_all_upstream_done = 0;
            }
            else
            {
              vStream_order.push_back(vSegment.at(iSegment - 1).iSegment_order);
            }
          }

          if (iFlag_all_upstream_done == 1)
          {
            if (vStream_order.at(0) == vStream_order.at(1))
            {
              (*iIterator_segment).iSegment_order = vStream_order.at(0) + 1;
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

    return error_code;
  }
  int watershed::watershed_define_subbasin()
  {
    int error_code = 1;
    int iFound_outlet;
    int iSubbasin;
    long lCellIndex_self;
    long lCellIndex_current;
    long lCellID_outlet;
    long lCellIndex_outlet; // local outlet
    long lCellID_downslope;
    long lCellIndex_downslope;
    long lCellIndex_accumulation;
    std::vector<long> vAccumulation;
    std::vector<long>::iterator iterator_accumulation;
    std::vector<hexagon> vConfluence_copy(vConfluence);
    std::vector<hexagon>::iterator iIterator_self;
    std::vector<long>::iterator iIterator_upstream;
    std::vector<hexagon>::iterator iIterator_current;

    // the whole watershed first
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      if ((*iIterator_self).iWatershed == iWatershed) // not using flag anymore
      {
        (*iIterator_self).iSubbasin = nSegment;
      }
    }

    // now starting from the confluences loop, vConfluence_copy is only usable for one watershed
    while (vConfluence_copy.size() != 0)
    {
      iterator_accumulation = max_element(std::begin(vAccumulation), std::end(vAccumulation));
      lCellIndex_accumulation = std::distance(vAccumulation.begin(), iterator_accumulation);
      std::vector<long> vUpstream((vConfluence_copy.at(lCellIndex_accumulation)).vUpstream);
      for (iIterator_upstream = vUpstream.begin(); iIterator_upstream != vUpstream.end(); iIterator_upstream++)
      {
        // use the watershed method again here
        lCellID_outlet = *iIterator_upstream;
        lCellIndex_outlet = watershed_find_index_by_cell_id(lCellID_outlet);
        iSubbasin = (vCell.at(lCellIndex_outlet)).iSegment;
        (vCell.at(lCellIndex_outlet)).iSubbasin = iSubbasin;
        for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
        {
          iFound_outlet = 0;
          lCellIndex_current = (*iIterator_self).lCellIndex;
          while (iFound_outlet != 1)
          {
            lCellID_downslope = vCell.at(lCellIndex_current).lCellID_downslope_dominant;
            if (lCellID_outlet == lCellID_downslope)
            {
              iFound_outlet = 1;
              (*iIterator_self).iSubbasin = iSubbasin;
            }
            else
            {
              lCellIndex_current = watershed_find_index_by_cell_id(lCellID_downslope);
              if (lCellIndex_current == -1)
              {
                // this one does not belong in this watershed
                iFound_outlet = 1;
              }
              else
              {
                lCellID_downslope = vCell.at(lCellIndex_current).lCellID_downslope_dominant;
                if (lCellID_downslope == -1)
                {
                  // this is the outlet
                  iFound_outlet = 1;
                }
                else
                {
                  // std::cout << lCellID_downslope << std::endl;
                }
              }
            }
          }
        }
      }

      // remove the confluence now
      vAccumulation.erase(iterator_accumulation);
      vConfluence_copy.erase(vConfluence_copy.begin() + lCellIndex_accumulation);
      // repeat
    }

    // assign watershed subbasin cell, maybe later?
    vSubbasin.clear();
    for (int iSubbasin = 1; iSubbasin <= nSegment; iSubbasin++)
    {
      subbasin cSubbasin;
      cSubbasin.iSubbasin = iSubbasin;
      vSubbasin.push_back(cSubbasin);
    }
    for (iIterator_self = vCell.begin(); iIterator_self != vCell.end(); iIterator_self++)
    {
      int iSubbasin = (*iIterator_self).iSubbasin;
      if (iSubbasin >= 1 && iSubbasin <= nSegment)
      {
        (vSubbasin[iSubbasin - 1]).vCell.push_back(*iIterator_self);
      }
      else
      {
        if (iSubbasin != -1)
        {
          std::cout << "Something is wrong" << std::endl;
        }
      }
    }

    return error_code;
  }
  int watershed::watershed_update_attribute()
  {
    int error_code = 1;

    for (int iSegment = 1; iSegment < nSegment; iSegment++)
    {
      vSubbasin.at(iSegment - 1).cCell_outlet = vSegment.at(iSegment - 1).cReach_end;
      vSubbasin.at(iSegment - 1).lCellID_outlet = vSegment.at(iSegment - 1).cReach_end.lCellID;
    }

    return error_code;
  }

  /**
   * calculate the watershed characteristics
   * @return
   */
  int watershed::calculate_watershed_characteristics()
  {
    int error_code = 1;
    int iSegment_downstream;
    int iSegmentIndex;
    float dLength;
    float dDistance_to_watershed_outlet;
    float dLength_stream_conceptual;

    std::vector<segment>::iterator iIterator0;
    std::vector<segment>::iterator iIterator2;
    std::vector<subbasin>::iterator iIterator1;

    for (iIterator0 = vSegment.begin(); iIterator0 != vSegment.end(); iIterator0++)
    {
      (*iIterator0).calculate_stream_segment_characteristics();
      dDistance_to_watershed_outlet = 0.0;
      iSegment_downstream = (*iIterator0).iSegment_downstream;
      while (iSegment_downstream != nSegment)
      {
        iSegmentIndex = watershed_find_index_by_segment_id(iSegment_downstream);
        dLength = vSegment.at(iSegmentIndex).dLength;
        dDistance_to_watershed_outlet = dDistance_to_watershed_outlet + dLength;
      }

      (*iIterator0).dDistance_to_watershed_outlet = dDistance_to_watershed_outlet;
    }

    for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
    {
      dLength_stream_conceptual = vSegment.at((*iIterator1).iSubbasin - 1).dLength;
      (*iIterator1).calculate_subbasin_characteristics(dLength_stream_conceptual);
    }

    calculate_travel_distance();

    calculate_watershed_drainage_area();
    calculate_watershed_total_stream_length();
    calculate_watershed_longest_stream_length();
    calculate_watershed_drainage_density();
    calculate_watershed_average_slope();
    calculate_topographic_wetness_index();

    // save watershed characteristics to the file

    std::cout << "The watershed characteristics are calculated successfully!" << std::endl;

    return error_code;
  }

  /**
   * calculate the watershed drainage total area
   * we can either sum up hexagon or sum up subbasin
   * @return
   */
  int watershed::calculate_watershed_drainage_area()
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
  int watershed::calculate_watershed_total_stream_length()
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
  int watershed::calculate_watershed_longest_stream_length()
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
  int watershed::calculate_watershed_drainage_density()
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
  int watershed::calculate_watershed_average_slope()
  {
    int error_code = 1;
    int iOption = 1;
    float dSlope_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    std::vector<subbasin>::iterator iIterator1;
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
  int watershed::calculate_topographic_wetness_index()
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

  int watershed::calculate_travel_distance()
  {
    int error_code = 1;
    int iSegment;
    int iSubbasin;
    // calculate confluence travel
    std::vector<hexagon>::iterator iIterator;

    std::vector<subbasin>::iterator iIterator1;
    for (iIterator = vConfluence.begin(); iIterator != vConfluence.end(); iIterator++)
    {
      iSubbasin = (*iIterator).iSubbasin;
    }
    for (iIterator1 = vSubbasin.begin(); iIterator1 != vSubbasin.end(); iIterator1++)
    {
      (*iIterator1).calculate_travel_distance();
    }

    return error_code;
  }
  /**
   * save the watershed characteristics in the output
   * @return
   */
  int watershed::save_watershed_characteristics()
  {
    int error_code = 1;
    std::string sLine;

    std::ofstream ofs;
    ofs.open(sFilename_watershed_characteristics.c_str(), ios::out);
    if (ofs.good())
    {
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

  int watershed::save_segment_characteristics()
  {
    int error_code = 1;
    std::string sLine;
    std::vector<segment>::iterator iIterator1;
    std::ofstream ofs;
    ofs.open(sFilename_segment_characteristics.c_str(), ios::out);
    if (ofs.good())
    {

      sLine = "Segment ID, stream order, total length, elevation drop, average slope";
      ofs << sLine << std::endl;
      for (iIterator1 = vSegment.begin(); iIterator1 != vSegment.end(); iIterator1++)
      {

        sLine = convert_integer_to_string((*iIterator1).iSegment) + "," + convert_integer_to_string((*iIterator1).iSegment_order) + "," + convert_float_to_string((*iIterator1).dLength) + "," + convert_float_to_string((*iIterator1).dElevation_drop) + "," + convert_float_to_string((*iIterator1).dSlope_mean);
        ofs << sLine << std::endl;
      }
      ofs.close();
    }
    return error_code;
  }

  int watershed::save_subbasin_characteristics()
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
        sLine = convert_integer_to_string((*iIterator1).iSubbasin) + "," + convert_long_to_string((*iIterator1).lCellID_outlet) + "," + convert_long_to_string((*iIterator1).nCell) + "," + convert_float_to_string((*iIterator1).dArea) + "," + convert_float_to_string((*iIterator1).dSlope_mean) + "," + convert_float_to_string((*iIterator1).dArea_2_stream_ratio) + "," + convert_float_to_string((*iIterator1).dDrainage_density) + ",";
        ofs << sLine << std::endl;
      }
      ofs.close();
    }
    return error_code;
  }

  long watershed::watershed_find_index_by_cell_id(long lCellID_in)
  {
    long lCellIndex = -1;
    std::vector<hexagon>::iterator iIterator;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).lCellID == lCellID_in)
      {
        lCellIndex = (*iIterator).lCellIndex;
        break;
      }
    }

    return lCellIndex;
  }

  int watershed::watershed_find_index_by_segment_id(int iSegment_in)
  {
    int iSegmentIndex = -1;
    std::vector<segment>::iterator iIterator;
    for (iIterator = vSegment.begin(); iIterator != vSegment.end(); iIterator++)
    {
      if ((*iIterator).iSegment == iSegment_in)
      {
        iSegmentIndex = (*iIterator).iSegmentIndex;
        break;
      }
    }

    return iSegmentIndex;
  }
  int watershed::watershed_find_index_by_subbasin_id(int iSegment_in)
  {
    int iSegmentIndex = -1;
    std::vector<subbasin>::iterator iIterator;
    for (iIterator = vSubbasin.begin(); iIterator != vSubbasin.end(); iIterator++)
    {
      if ((*iIterator).iSubbasin == iSegment_in)
      {
        iSegmentIndex = (*iIterator).iSubbasinIndex;
        break;
      }
    }

    return iSegmentIndex;
  }
} // namespace hexwatershed