//
// Created by Liao, Chang on 2020-09-07.
//

#include "subbasin.h"

namespace hexwatershed
{

  subbasin::subbasin()
  {
    iFlag_outlet = 0;
    lSubbasinIndex = -1;
    iFlag_headwater = 0;
    nHillslope = 2;
    lHillslope_headwater = -1;
    dWidth_hillslope_headwater=0.0;
    dLength_hillslope_headwater=0.0;
    dSlope_hillslope_headwater=0.0;
    // vHillslope.clear();
  }
  subbasin::~subbasin()
  {
  }
  int subbasin::subbasin_define_hillslope()
  {
    int error_code = 1;
    int iFlag_checked;
    int iFlag_left_hillslope, iFlag_right_hillslope, iFlag_headwater_hillslope;
    int iFlag_checked_downslope;
    long lCellID_upslope, lCellID_downslope;
    long lCellIndex_buffer;
    long lCellIndex_upslope, lCellIndex_downslope;
    long lCellIndex_current;
    float dLongitude_start, dLatitude_start;
    float dLongitude_channel_center, dLatitude_channel_center;
    float dLongitude_channel_upstream, dLatitude_channel_upstream;
    float dLongitude_channel_downstream, dLatitude_channel_downstream;
    float dAngle, dAngle1, dAngle2;
    std::vector<long>::iterator iIterator1;
    std::vector<hexagon>::iterator iIterator;
    // each subbasin have two or three hillslopes
    std::vector<long> vCellID_channel;
    std::vector<long> vCellID_buffer_left;
    std::vector<long> vCellID_buffer_right;
    std::vector<long> vCellID_buffer_hill;
    std::vector<long> vSearchPath;
    std::vector<long>::iterator iIterator_path;
    // define the list of segment cells in the upstream to downstream order?
    // can this be passed by?

    // find the buffer zone, but also be careful about the headwater
    if (iFlag_headwater == 1)
    {
      // set the headwater as checked
      lCellIndex_current = mCellIdToIndex[lCellID_headwater];
      vCell[lCellIndex_current].iFlag_checked = 1;
      vCell[lCellIndex_current].iFlag_headwater_hillslope = 1;

      for (iIterator = vCell_segment.begin() + 1; iIterator != vCell_segment.end(); iIterator++) // skip the first one because it is the headwater
      {
        vCellID_channel.push_back((*iIterator).lCellID);
      }
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        lCellID_downslope = (*iIterator).lCellID_downslope_dominant;
        // check if this downstream cell is in the channel
        if ((*iIterator).iFlag_stream != 1) // it is not a channel cell
        {
          if (std::find(vCellID_channel.begin(), vCellID_channel.end(), lCellID_downslope) != vCellID_channel.end())
          {
            // but its downslope is on the channel
            vCellID_buffer_hill.push_back((*iIterator).lCellID);
            (*iIterator).iFlag_checked = 1;
          }
          else //
          {
            (*iIterator).iFlag_checked = 0;
          }
        }
        else
        {
          (*iIterator).iFlag_checked = 1;
        }
      }
    }
    else
    {
      for (iIterator = vCell_segment.begin(); iIterator != vCell_segment.end(); iIterator++)
      {
        vCellID_channel.push_back((*iIterator).lCellID);
      }
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        lCellID_downslope = (*iIterator).lCellID_downslope_dominant;
        // check if this downstream cell is in the channel
        if ((*iIterator).iFlag_stream != 1) // it is not a channel cell
        {
          if (std::find(vCellID_channel.begin(), vCellID_channel.end(), lCellID_downslope) != vCellID_channel.end())
          {
            vCellID_buffer_hill.push_back((*iIterator).lCellID);
            (*iIterator).iFlag_checked = 1;
          }
          else
          {
            (*iIterator).iFlag_checked = 0;
          }
        }
        else
        {
          (*iIterator).iFlag_checked = 1;
        }
      }
    }

    // find the left and right hillslope from the buffer
    for (iIterator1 = vCellID_buffer_hill.begin(); iIterator1 != vCellID_buffer_hill.end(); iIterator1++)
    {
      lCellIndex_buffer = mCellIdToIndex[*iIterator1];
      dLongitude_start = vCell[lCellIndex_buffer].dLongitude_center_radian;
      dLatitude_start = vCell[lCellIndex_buffer].dLatitude_center_radian;
      // find the channel cell
      lCellID_downslope = vCell[lCellIndex_buffer].lCellID_downslope_dominant;
      lCellIndex_downslope = mCellIdToIndex[lCellID_downslope];
      dLongitude_channel_center = vCell[lCellIndex_downslope].dLongitude_center_radian;
      dLatitude_channel_center = vCell[lCellIndex_downslope].dLatitude_center_radian;
      // find upstream and downstream cells
      if (lCellID_downslope == lCellID_headwater)
      {
        // the first cell
        // find the downstream cell
        lCellID_downslope = vCell[lCellIndex_downslope].lCellID_downslope_dominant;
        lCellIndex_downslope = mCellIdToIndex[lCellID_downslope];
        dLongitude_channel_downstream = vCell[lCellIndex_downslope].dLongitude_center_radian;
        dLatitude_channel_downstream = vCell[lCellIndex_downslope].dLatitude_center_radian;
        // convert to radian

        // now calculate the angle
        dAngle = calculate_angle_between_lon_lat_radian(1, dLongitude_start, dLatitude_start,
                                                        dLongitude_channel_center, dLatitude_channel_center,
                                                        dLongitude_channel_downstream, dLatitude_channel_downstream);
        if (dAngle <= 180.0)
        {
          // left hillslope

          vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
          vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
          vCellID_buffer_left.push_back((*iIterator1));
        }
        else
        {
          vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
          vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
          vCellID_buffer_right.push_back((*iIterator1));
        }
      }
      else
      {
        if (lCellID_downslope == lCellID_outlet)
        {
          // the last cell
          // get location first

          // find the upstream cell using the vSegment vector
          // locate the last cell in the vSegment vector
          if (vCell_segment.size() >= 2)
          {
            // Get the second-to-last element
            lCellID_upslope = (*(vCell_segment.rbegin() + 1)).lCellID;
            lCellIndex_upslope = mCellIdToIndex[lCellID_upslope];

            dLongitude_channel_upstream = vCell[lCellIndex_upslope].dLongitude_center_radian;
            dLatitude_channel_upstream = vCell[lCellIndex_upslope].dLatitude_center_radian;

            // downstream cell
            if (iFlag_outlet == 1)
            {
              dAngle = calculate_angle_between_lon_lat_radian(1, dLongitude_start, dLatitude_start,
                                                              dLongitude_channel_center, dLatitude_channel_center,
                                                              dLongitude_channel_downstream, dLatitude_channel_downstream);

              if (dAngle <= 180)
              {

                vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                vCellID_buffer_left.push_back((*iIterator1));
              }
              else
              {
                vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                vCellID_buffer_right.push_back((*iIterator1));
              }
            }
            else
            {
              lCellID_downslope = cCell_outlet_downslope.lCellID;

              // we dont have information about a cell that is downslope of the outlet cell. unless we pass it
              dLongitude_channel_downstream = cCell_outlet_downslope.dLongitude_center_radian;
              dLatitude_channel_downstream = cCell_outlet_downslope.dLatitude_center_radian;
              // now calculate the angles
              dAngle1 = calculate_angle_between_lon_lat_radian(1, dLongitude_channel_upstream, dLatitude_channel_upstream,
                                                               dLongitude_channel_center, dLatitude_channel_center,
                                                               dLongitude_channel_downstream, dLatitude_channel_downstream);

              dAngle2 = calculate_angle_between_lon_lat_radian(1, dLongitude_start, dLatitude_start,
                                                               dLongitude_channel_center, dLatitude_channel_center,
                                                               dLongitude_channel_downstream, dLatitude_channel_downstream);

              if (dAngle1 <= 180)
              {
                if (dAngle2 <= 180)
                {
                  // left hillslope
                  if (dAngle2 >= dAngle1)
                  {
                    vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                    vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                    vCellID_buffer_left.push_back((*iIterator1));
                  }
                  else
                  {
                    vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                    vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                    vCellID_buffer_right.push_back((*iIterator1));
                  }
                }
                else
                {
                  vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                  vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                  vCellID_buffer_right.push_back((*iIterator1));
                }
              }
              else
              {
                if (dAngle2 <= 180)
                {
                  // left hillslope
                  vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                  vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                  vCellID_buffer_left.push_back((*iIterator1));
                }
                else
                {
                  if (dAngle2 > dAngle1)
                  {
                    // left hillslope
                    vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                    vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                    vCellID_buffer_left.push_back((*iIterator1));
                  }
                  else
                  {
                    vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                    vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                    vCellID_buffer_right.push_back((*iIterator1));
                  }
                }
              }
            }
          }
        }
        else // middle cell
        {
          // we need to calculate two angles

          // the first angle is between upstream-downstream of the river course
          // upstream cell
          // use iterator to find the upstream cell
          for (auto iIterator_segment = vCell_segment.begin(); iIterator_segment != vCell_segment.end(); iIterator_segment++)
          {
            if ((*iIterator_segment).lCellID_downslope_dominant == lCellID_downslope)
            {
              lCellID_upslope = (*(iIterator_segment)).lCellID;
              lCellIndex_upslope = mCellIdToIndex[lCellID_upslope];
              dLongitude_channel_upstream = vCell[lCellIndex_upslope].dLongitude_center_radian;
              dLatitude_channel_upstream = vCell[lCellIndex_upslope].dLatitude_center_radian;
              break;
            }
          }
          // downstream cell
          lCellID_downslope = vCell[lCellIndex_downslope].lCellID_downslope_dominant;
          lCellIndex_downslope = mCellIdToIndex[lCellID_downslope];
          dLongitude_channel_downstream = vCell[lCellIndex_downslope].dLongitude_center_radian;
          dLatitude_channel_downstream = vCell[lCellIndex_downslope].dLatitude_center_radian;
          // now calculate the angle
          dAngle1 = calculate_angle_between_lon_lat_radian(1, dLongitude_channel_upstream, dLatitude_channel_upstream,
                                                           dLongitude_channel_center, dLatitude_channel_center,
                                                           dLongitude_channel_downstream, dLatitude_channel_downstream);

          dAngle2 = calculate_angle_between_lon_lat_radian(1, dLongitude_start, dLatitude_start,
                                                           dLongitude_channel_center, dLatitude_channel_center,
                                                           dLongitude_channel_downstream, dLatitude_channel_downstream);
          if (dAngle1 <= 180)
          {
            if (dAngle2 <= 180)
            {
              // left hillslope
              if (dAngle2 >= dAngle1)
              {
                vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                vCellID_buffer_left.push_back((*iIterator1));
              }
              else
              {
                vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                vCellID_buffer_right.push_back((*iIterator1));
              }
            }
            else
            {
              vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
              vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
              vCellID_buffer_right.push_back((*iIterator1));
            }
          }
          else
          {
            if (dAngle2 <= 180)
            {
              // left hillslope
              vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
              vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
              vCellID_buffer_left.push_back((*iIterator1));
            }
            else
            {
              if (dAngle2 > dAngle1)
              {
                // left hillslope
                vCell[lCellIndex_buffer].iFlag_left_hillslope = 1;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 0;
                vCellID_buffer_left.push_back((*iIterator1));
              }
              else
              {
                vCell[lCellIndex_buffer].iFlag_left_hillslope = 0;
                vCell[lCellIndex_buffer].iFlag_right_hillslope = 1;
                vCellID_buffer_right.push_back((*iIterator1));
              }
            }
          }
        }
      }
    }

    // now screening the remaining cells
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      iFlag_checked = (*iIterator).iFlag_checked;
      lCellIndex_current = (*iIterator).lCellIndex_subbasin;
      if (iFlag_checked == 0)
      {
        vSearchPath.clear();
        iFlag_checked_downslope = 0;
        while (iFlag_checked_downslope == 0)
        { // not found keep adding to path
          vSearchPath.push_back(lCellIndex_current);
          lCellID_downslope = vCell[lCellIndex_current].lCellID_downslope_dominant;
          lCellIndex_current = mCellIdToIndex[lCellID_downslope];
          if (lCellIndex_current != -1)
          {
            iFlag_checked_downslope = vCell[lCellIndex_current].iFlag_checked;
          }
          else
          {
            std::cout << "This shouldn't happen!" << std::endl;
          }
        }
        // now set the flag
        iFlag_left_hillslope = vCell[lCellIndex_current].iFlag_left_hillslope;
        iFlag_right_hillslope = vCell[lCellIndex_current].iFlag_right_hillslope;
        iFlag_headwater_hillslope = vCell[lCellIndex_current].iFlag_headwater_hillslope;
        for (iIterator_path = vSearchPath.begin(); iIterator_path != vSearchPath.end(); iIterator_path++)
        {
          vCell[*iIterator_path].iFlag_left_hillslope = iFlag_left_hillslope;
          vCell[*iIterator_path].iFlag_right_hillslope = iFlag_right_hillslope;
          vCell[*iIterator_path].iFlag_headwater_hillslope = iFlag_headwater_hillslope;
          vCell[*iIterator_path].iFlag_checked = 1;
        }
      }
    }

    // now calculate left and right hillslope width and count
    dWidth_hillslope_left = 0.0;
    for (iIterator1 = vCellID_buffer_left.begin(); iIterator1 != vCellID_buffer_left.end(); iIterator1++)
    {
      lCellIndex_buffer = mCellIdToIndex[*iIterator1];
      dWidth_hillslope_left = dWidth_hillslope_left + vCell[lCellIndex_buffer].dLength_edge_mean;
    }

    dWidth_hillslope_right = 0.0;
    for (iIterator1 = vCellID_buffer_right.begin(); iIterator1 != vCellID_buffer_right.end(); iIterator1++)
    {
      lCellIndex_buffer = mCellIdToIndex[*iIterator1];
      dWidth_hillslope_right = dWidth_hillslope_right + vCell[lCellIndex_buffer].dLength_edge_mean;
    }
    // for headwater, it is consider a convergence, a special method is needed

    // calculate left and right cell count
    nCell_hillslope_left = 0;
    nCell_hillslope_right = 0;
    nCell_hillslope_headwater = 0;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).iFlag_left_hillslope == 1)
      {
        nCell_hillslope_left = nCell_hillslope_left + 1;
      }
      else
      {
        if ((*iIterator).iFlag_right_hillslope == 1)
        {
          nCell_hillslope_right = nCell_hillslope_right + 1;
        }
        else
        {
          if ((*iIterator).iFlag_headwater_hillslope == 1)
          {
            nCell_hillslope_headwater = nCell_hillslope_headwater + 1;
          }
        }
      }
    }

    /*
    hillslope cHillslope_left;
    hillslope cHillslope_right;
    hillslope cHillslope_headwater;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).iFlag_left_hillslope == 1)
      {
        cHillslope_left.vCell.push_back((*iIterator));
      }
      if ((*iIterator).iFlag_right_hillslope == 1)
      {
        cHillslope_right.vCell.push_back((*iIterator));
      }
      if ((*iIterator).iFlag_headwater_hillslope == 1)
      {
        cHillslope_headwater.vCell.push_back((*iIterator));
      }
    }

    vHillslope.clear();
    vHillslope.push_back(cHillslope_left);
    vHillslope.push_back(cHillslope_right);
    if (iFlag_headwater == 1)
    {
      cHillslope_headwater.iFlag_headwater = 1;
      vHillslope.push_back(cHillslope_headwater);
    }
    else
    {

    }
    */
    if (iFlag_headwater == 1)
    {
      nHillslope = 3;
    }
    else
    {
      nHillslope = 2;
    }
    return error_code;
  }
  
  int subbasin::subbasin_calculate_characteristics(float dLength_stream_conceptual)
  {
    int error_code = 1;
    subbasin_calculate_total_area();
    subbasin_calculate_slope();
    subbasin_calculate_drainage_density(dLength_stream_conceptual);
    subbasin_calculate_hillslope_characteristics();
    return error_code;
  }

  int subbasin::subbasin_calculate_total_area()
  {
    int error_code = 1;
    double dArea_total = 0.0;
    std::vector<hexagon>::iterator iIterator;
    nCell = vCell.size();

    dArea_hillslope_left = 0.0;
    dArea_hillslope_right = 0.0;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).dArea < 0.0)
      {
        std::cout << "Something is wrong" << std::endl;
      }
      else
      {
        if ((*iIterator).iFlag_left_hillslope == 1)
        {
          dArea_hillslope_left = dArea_hillslope_left + (*iIterator).dArea;
        }
        if ((*iIterator).iFlag_right_hillslope == 1)
        {
          dArea_hillslope_right = dArea_hillslope_right + (*iIterator).dArea;
        }
        dArea_total = dArea_total + (*iIterator).dArea;
      }
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
    float dSlope_left = 0.0;
    float dSlope_right = 0.0;
    std::vector<hexagon>::iterator iIterator;
    if (iOption == 1) // by cell
    {
      for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
      {
        dSlope_total = dSlope_total + (*iIterator).dSlope_max_downslope; // should mean slope?
        if ((*iIterator).iFlag_left_hillslope == 1)
        {
          dSlope_left = dSlope_left + (*iIterator).dSlope_max_downslope;
        }
        if ((*iIterator).iFlag_right_hillslope == 1)
        {
          dSlope_right = dSlope_right + (*iIterator).dSlope_max_downslope;
        }
      }
    }
    dSlope = dSlope_total / nCell;

    dSlope_mean = dSlope;
    dSlope_hillslope_left_mean = dSlope_left / nCell_hillslope_left;
    dSlope_hillslope_right_mean = dSlope_right / nCell_hillslope_right;
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
    int iFlag_channel;
    long lCellID_current;
    long lCellIndex;
    float dDistance_to_subbasin_channel_temp = 0.0;
    float dDistance_to_subbasin_outlet_temp = 0.0;
    std::vector<hexagon>::iterator iIterator;
    for (iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      lCellID_current = (*iIterator).lCellID;
      if (lCellID_current != lCellID_outlet)
      {
        dDistance_to_subbasin_channel_temp = (*iIterator).dDistance_to_downslope;   
        dDistance_to_subbasin_outlet_temp = (*iIterator).dDistance_to_downslope;
        lCellID_current = (*iIterator).lCellID_downslope_dominant;
        while (lCellID_current != lCellID_outlet)
        {
          lCellIndex = subbasin_find_index_by_cellid(lCellID_current);
          lCellID_current = vCell[lCellIndex].lCellID_downslope_dominant;
          iFlag_channel = vCell[lCellIndex].iFlag_stream;
          if (iFlag_channel != 1)
          {        
            dDistance_to_subbasin_channel_temp = dDistance_to_subbasin_channel_temp + vCell[lCellIndex].dDistance_to_downslope;
          }
          dDistance_to_subbasin_outlet_temp = dDistance_to_subbasin_outlet_temp + vCell[lCellIndex].dDistance_to_downslope;
        }

        (*iIterator).dDistance_to_channel = dDistance_to_subbasin_channel_temp;
        (*iIterator).dDistance_to_subbasin_outlet = dDistance_to_subbasin_outlet_temp;
      }
      else
      {
        (*iIterator).dDistance_to_subbasin_outlet = 0.0;
      }
    }
    return error_code;
  }

  // in the next version, we will use the hillslope class to calculate the hillslope characteristics
  int subbasin::subbasin_calculate_hillslope_characteristics()
  {
    int error_code = 1;
    // zonal statistics
    // elevation profile
    int nElevation_profile = 11;
    float p;
    std::vector<float> vElevation_left, vElevation_right;
    //std::array<float, 11> aElevation_profile_left;
    //std::array<float, 11> aElevation_profile_right;
    // std::vector<hillslope>::iterator iIterator;
    /*
    for (iIterator = vHillslope.begin(); iIterator != vHillslope.end(); iIterator++)
    {
      (*iIterator).hillslope_calculate_characteristics();
    }*/
    // list of characteristics needed: total area, mean slope, segment length, average width

    // width is already calculated

    // area is already calculated

    // length
    dLength_hillslope_left = dArea_hillslope_left / dWidth_hillslope_left;
    dLength_hillslope_right = dArea_hillslope_right / dWidth_hillslope_right;
    // gather all the elevation data
    for (auto iIterator = vCell.begin(); iIterator != vCell.end(); iIterator++)
    {
      if ((*iIterator).iFlag_left_hillslope == 1)
      {
        vElevation_left.push_back((*iIterator).dElevation_mean);
      }
      else
      {
        if ((*iIterator).iFlag_right_hillslope == 1)
        {
          vElevation_right.push_back((*iIterator).dElevation_mean);
        }
      }
    }
    // create an array of 10 elements for both left and right hillslope
    // for first and last of the elevation profile are min and max
    aElevation_profile_left[0] = *std::min_element(vElevation_left.begin(), vElevation_left.end());
    aElevation_profile_left[10] = *std::max_element(vElevation_left.begin(), vElevation_left.end());
    for (int i = 1; i < nElevation_profile; i++)
    {
      p = i * 10.0;
      aElevation_profile_left[i] = data::percentile(vElevation_left, p);
      aElevation_profile_right[i] = data::percentile(vElevation_right, p);
    }
    //how to save this data? maybe using the subbasin class.
    //now calculate the hillslope slope using the elevation profile
    dSlope_hillslope_left = ( aElevation_profile_left[10] - aElevation_profile_left[0]  )/dLength_hillslope_left;
    dSlope_hillslope_right = ( aElevation_profile_right[10] - aElevation_profile_right[0]  )/dLength_hillslope_right;

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