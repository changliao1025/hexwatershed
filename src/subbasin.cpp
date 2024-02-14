//
// Created by Liao, Chang on 2020-09-07.
//

#include "subbasin.h"

namespace hexwatershed
{

  subbasin::subbasin()
  {
    lSubbasinIndex = -1;
    iFlag_headwater = 0;
  }
  subbasin::~subbasin()
  {
  }
  int subbasin::subbasin_define_hillslope()
  {
    int error_code = 1;
    long lCellID_downslope;
    long lCellIndex_downstream
    float dLongitude_start, dLatitude_start;
    float dLongitude_channel_center, dLatitude_channel_center;
    float dLongitude_channel_upstream, dLatitude_channel_upstream;
    float dLongitude_channel_downstream, dLatitude_channel_downstream;
    float dAngle;
    std::vector<hexagon>::iterator iIterator;
    //each subbasin have two or three hillslopes
    std::vector<long> vCellID_channel;
    std::vector<hexagon> vCell_buffer;
    std::vector<hexagon> vCell_left;
    std::vector<hexagon> vCell_right;
    std::vector<hexagon> vCell_head;
    //define the list of segment cells in the upstream to downstream order?
    //can this be passed by?
    
    for (iIterator = vCell_segment.begin(); iIterator != vCell_segment.end(); iIterator++)
    {
      vCellID_channel.push_back((*iIterator).lCellID);      
    }
    //find the buffer zone
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      lCellID_downslope = (*iIterator).lCellID_downslope_dominant;
      //check if this downstream cell is in the channel      
      if (std::find(vCellID_channel.begin(), vCellID_channel.end(), lCellID_downslope) != vCellID_channel.end())
      {
        vCell_buffer.push_back((*iIterator));
      }
    }
    //find the left and right hillslope from the buffer
    for (iIterator = vCell_buffer.begin(); iIterator != vCell_buffer.end(); iIterator++)
    {
      dLongitude_start = (*iIterator).dLongitude_center_radian;
      dLatitude_start = (*iIterator).dLatitude_center_radian;

      //find the channel cell
      lCellID_downslope = (*iIterator).lCellID_downslope_dominant;
      lCellIndex = mCellIdToIndex[lCellID_downslope];

      dLongitude_channel_center = vCell[lCellIndex].dLongitude_center_radian;
      dLatitude_channel_center = vCell[lCellIndex].dLatitude_center_radian;
      //find upstream and downstream cells
      if (lCellID == lCellID_start)
      {
        //the first cell
        //find the downstream cell
        lCellID_downslope = vCell[lCellIndex].lCellID_downslope_dominant;
        lCellIndex_downstream = mCellIdToIndex[lCellID_downslope];
        dLongitude_channel_downstream = vCell[lCellIndex_downstream].dLongitude_center_radian;
        dLatitude_channel_downstream = vCell[lCellIndex_downstream].dLatitude_center_radian;
        //convert to radian

        //now calculate the angle

        dAngle = calculate_angle_between_lon_lat_radian(dLongitude_start, dLatitude_start,
         dLongitude_channel_center, dLatitude_channel_center, 
         dLongitude_channel_downstream, dLatitude_channel_downstream);
         if (dAngle < 180.0)
         {
           //left hillslope
           (*iIterator).iFlag_left_hill = 1;
           (*iIterator).iFlag_right_hill = 0;
           vCell_left.push_back((*iIterator));
         }
         else
         {
           (*iIterator).iFlag_left_hill = 0;
           (*iIterator).iFlag_right_hill = 1;
           vCell_right.push_back((*iIterator));
         }
      }
      else
      {
        if (lCellID != lCellID_outlet)
        {
          //the last cell
        }
        else
        {

        }
      }


    }



    if (iFlag_headwater ==1) //3 hillslope: left, right, head
    {
      
    }
    else //2 hillslopes, left and right
    {
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      
    }
    }
    return error_code;
  }
  int subbasin::subbasin_calculate_characteristics(float dLength_stream_conceptual)
  {
    int error_code = 1;
    subbasin_calculate_total_area();
    subbasin_calculate_slope();
    subbasin_calculate_drainage_density(dLength_stream_conceptual);
    return error_code;
  }

  int subbasin::subbasin_calculate_total_area()
  {
    int error_code = 1;
    double dArea_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    nCell = vCell.size();

    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).dArea < 0.0)
      {
        std::cout << "Something is wrong" << std::endl;
      }
      dArea_total = dArea_total + (*iIterator).dArea;
    }
    dArea = dArea_total;
    if (dArea < 0.0)
    {
      std::cout << "Something is wrong" << std::endl;
    }

    return error_code;
  }

  int subbasin::subbasin_calculate_slope()
  {
    int error_code = 1;
    int iOption = 1;
    float dSlope_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    if (iOption == 1) // by cell
    {
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        dSlope_total = dSlope_total + (*iIterator).dSlope_max_downslope; // should mean slope?
      }
    }
    dSlope = dSlope_total / nCell;
    dSlope_mean = dSlope;
    return error_code;
  }
  int subbasin::subbasin_calculate_drainage_density(float dLength_stream_conceptual)
  {
    int error_code = 1;
    dArea_2_stream_ratio = dArea / dLength_stream_conceptual;

    dLength_2_area_ratio = 1.0 / dArea_2_stream_ratio;
    dDrainage_density = dLength_2_area_ratio;

    return error_code;
  }
  int subbasin::subbasin_calculate_travel_distance()
  {
    int error_code = 1;
    long lCellID_current;
    long lCellIndex;
    float dDistance_to_subbasin_outlet_temp = 0.0;
    std::vector<hexagon>::iterator iIterator;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      lCellID_current = (*iIterator).lCellID;
      if (lCellID_current != lCellID_outlet)
      {
        dDistance_to_subbasin_outlet_temp = (*iIterator).dDistance_to_downslope;
        lCellID_current = (*iIterator).lCellID_downslope_dominant;
        while (lCellID_current != lCellID_outlet)
        {
          lCellIndex = subbasin_find_index_by_cellid(lCellID_current);
          lCellID_current = vCell[lCellIndex].lCellID_downslope_dominant;
          dDistance_to_subbasin_outlet_temp = dDistance_to_subbasin_outlet_temp + vCell[lCellIndex].dDistance_to_downslope;
        }

        (*iIterator).dDistance_to_subbasin_outlet = dDistance_to_subbasin_outlet_temp;
      }
      else
      {
        (*iIterator).dDistance_to_subbasin_outlet = 0.0;
      }
    }
    return error_code;
  }
  long subbasin::subbasin_find_index_by_cellid(long lCellID_in)
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
} // namespace hexwatershed