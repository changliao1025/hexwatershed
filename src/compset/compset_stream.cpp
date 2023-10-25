/**
 * @file compset.cpp
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief the realization of the compset class
 * @version 0.1
 * @date 2019-08-02
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "compset.h"

namespace hexwatershed
{
  /**
   * @brief
   *
   * @param lCellIndex_center
   * @return int
   */
  int compset::compset_stream_burning_without_topology(long lCellID_center_in)
  {
    int error_code = 1;
    int iFlag_stream_burned_neighbor;
    int iFlag_stream_burning_treated_neighbor;
    int untreated;
    long lCellID_neighbor;
    long lCellIndex_neighbor;
    long lCellIndex_neighbor2;
    long lCellIndex_center = (mCellIdToIndex.find(lCellID_center_in))->second;
    float dBreach_threshold = cParameter.dBreach_threshold;
    float dElevation_mean_center;
    float dElevation_mean_neighbor;
    std::vector<long>::iterator iIterator_neighbor, iIterator_neighbor2;
    std::vector<long> vNeighbor_land, vNeighbor_land2;
    vCell_active[lCellIndex_center].iFlag_stream_burning_treated = 1;
    vNeighbor_land = vCell_active[lCellIndex_center].vNeighbor_land;
    dElevation_mean_center = vCell_active[lCellIndex_center].dElevation_mean;
    // stream first
    for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
    {
      lCellIndex_neighbor = (mCellIdToIndex.find(*iIterator_neighbor))->second;
      iFlag_stream_burning_treated_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated;
      iFlag_stream_burned_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burned;
      if (iFlag_stream_burned_neighbor == 1)
      {
        if (iFlag_stream_burning_treated_neighbor != 1)
        {
          dElevation_mean_neighbor = vCell_active[lCellIndex_neighbor].dElevation_mean;
          if (dElevation_mean_neighbor < dElevation_mean_center)
          {
            vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + 0.1 + abs(dElevation_mean_center) * 0.001;
          }
          else
          {
            if ((dElevation_mean_neighbor - dElevation_mean_center) > dBreach_threshold)
            {
              vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + dBreach_threshold;
            }
          }

          vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated = 1;
        }
      }
    }

    // go to the next iteration
    for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
    {
      lCellIndex_neighbor = (mCellIdToIndex.find(*iIterator_neighbor))->second;
      iFlag_stream_burned_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burned;
      if (iFlag_stream_burned_neighbor == 1)
      {
        // recursive
        untreated = 0;
        vNeighbor_land2 = vCell_active[lCellIndex_neighbor].vNeighbor_land;

        // for (int j = 0; j < vCell_active[lCellIndex_neighbor].nNeighbor_land; j++)
        for (iIterator_neighbor2 = vNeighbor_land2.begin(); iIterator_neighbor2 != vNeighbor_land2.end(); iIterator_neighbor2++)
        {
          lCellIndex_neighbor2 = (mCellIdToIndex.find(*iIterator_neighbor2))->second;
          if (vCell_active[lCellIndex_neighbor2].iFlag_stream_burned == 1)
          {
            if (vCell_active[lCellIndex_neighbor2].iFlag_stream_burning_treated != 1)
            {
              untreated = untreated + 1;
            }
          }
        }
        if (untreated > 0)
        {
          compset_stream_burning_without_topology(*iIterator_neighbor);
        }
      }
    }
    // land second
    // it might have modified, so need update
    dElevation_mean_center = vCell_active[lCellIndex_center].dElevation_mean;
    for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
    {
      lCellIndex_neighbor = (mCellIdToIndex.find(*iIterator_neighbor))->second;
      iFlag_stream_burned_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burned;
      iFlag_stream_burning_treated_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated;
      if (iFlag_stream_burned_neighbor != 1)
      {
        if (iFlag_stream_burning_treated_neighbor != 1)
        {
          dElevation_mean_neighbor = vCell_active[lCellIndex_neighbor].dElevation_mean;
          if (dElevation_mean_neighbor < dElevation_mean_center)
          {
            vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + abs(dElevation_mean_center) * 0.01 + 0.01;
            // we may increase the elevation again in the depression filling step
            vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated = 1;
          }
          else
          {
          }
        }
      }
    }

    return error_code;
  }

  /**
   * @brief
   *
   * @param lCellIndex_center
   * @return int
   */
  int compset::compset_stream_burning_with_topology(long lCellID_center_in)
  {
    int error_code = 1;
    int iFlag_elevation_profile = cParameter.iFlag_elevation_profile;
    int iFlag_stream_burned_neighbor;
    int iFlag_stream_burning_treated_neighbor;
    int iStream_order_center;
    int iStream_order_neighbor;
    long lCellIndex_neighbor;
    long lIndex_center_next;
    long lCellID_current;
    long lCellID_downstream_burned;
    long lCellIndex_center = (mCellIdToIndex.find(lCellID_center_in))->second;
    float dBreach_threshold = cParameter.dBreach_threshold;
    float dElevation_mean_center;
    float dElevation_mean_neighbor;
    float dDifference_dummy;
    float dElevation_profile0_center;
    float dElevation_profile0_neighbor;
    std::vector<long> vNeighbor_land;
    std::vector<long>::iterator iIterator_neighbor;    
    vCell_active[lCellIndex_center].iFlag_stream_burning_treated = 1;
    vNeighbor_land = vCell_active[lCellIndex_center].vNeighbor_land;
    dElevation_mean_center = vCell_active[lCellIndex_center].dElevation_mean;
    dElevation_profile0_center = vCell_active[lCellIndex_center].dElevation_profile0;
    // stream elevation
    iStream_order_center = vCell_active[lCellIndex_center].iStream_order_burned;
    lCellID_current = vCell_active[lCellIndex_center].lCellID;
    // stream first
    // std::cout << lCellID_current << ": " << dElevation_mean_center << std::endl;
    for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor < vNeighbor_land.end(); iIterator_neighbor++)
    {
      lCellIndex_neighbor = (mCellIdToIndex.find(*iIterator_neighbor))->second;
      lCellID_downstream_burned = vCell_active[lCellIndex_neighbor].lCellID_downstream_burned;
      if (lCellID_downstream_burned == lCellID_current)
      {
        iStream_order_neighbor = vCell_active[lCellIndex_neighbor].iStream_order_burned;
        dElevation_mean_neighbor = vCell_active[lCellIndex_neighbor].dElevation_mean;
        vCell_priority_flood.push_back(vCell_active[lCellIndex_neighbor]);
        vCell_active[lCellIndex_neighbor].dElevation_downstream = dElevation_mean_center; // need update after modification
        dDifference_dummy = dElevation_mean_neighbor - dElevation_mean_center;
        if (dDifference_dummy > 0) // should not be equally to 0.0
        {
        }
        else
        {
          // this is a depression, but should we fill or breaching depends on parameter
          if (abs(dDifference_dummy) < dBreach_threshold)
          {
            // if it is slight lower, we will increase  it
            if (iStream_order_neighbor == iStream_order_center)
            {
              vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + abs(dElevation_mean_center) * 0.001 + 0.1;
            }
            else
            {
              vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + abs(dElevation_mean_center) * 0.001 + 0.2;
            }
          }
          else
          {
            // breaching needed//we will use breach algorithm
            compset_breaching_stream_elevation(*iIterator_neighbor);
          }
        }

        // for elevation enabled case
        if (iFlag_elevation_profile == 1)
        {
          dElevation_profile0_neighbor = vCell_active[lCellIndex_neighbor].dElevation_profile0;
          if (dElevation_profile0_neighbor < dElevation_profile0_center)
          {
            vCell_active[lCellIndex_neighbor].dElevation_profile0 =
                dElevation_profile0_center + abs(dElevation_profile0_center) * 0.001 + 1.0;
          }
        }

        // update for next step
        vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated = 1; // this should be enough
        // burn recursively
        lIndex_center_next = lCellIndex_neighbor;

        // std::cout << vCell_active[lCellIndex_neighbor].lCellID << ": " << vCell_active[lCellIndex_neighbor].dElevation_mean << std::endl;
        //  go to the new grid
        if (vCell_active[lIndex_center_next].iFlag_headwater_burned != 1)
        {
          compset_stream_burning_with_topology(vCell_active[lCellIndex_neighbor].lCellID);
        }
      }
    }

    // it might have modified, so need update
    dElevation_mean_center = vCell_active[lCellIndex_center].dElevation_mean;

    // land second
    for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
    {
      lCellIndex_neighbor = (mCellIdToIndex.find(*iIterator_neighbor))->second;
      iFlag_stream_burned_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burned;
      iFlag_stream_burning_treated_neighbor = vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated;

      if (iFlag_stream_burned_neighbor != 1)
      {
        if (iFlag_stream_burning_treated_neighbor != 1)
        {

          vCell_priority_flood.push_back(vCell_active[lCellIndex_neighbor]);

          dElevation_mean_neighbor = vCell_active[lCellIndex_neighbor].dElevation_mean;

          if (dElevation_mean_neighbor <= dElevation_mean_center) // should not be equally to 0.0 as well
          {
            vCell_active[lCellIndex_neighbor].dElevation_mean =
                dElevation_mean_center + abs(dElevation_mean_center) * 0.001 + 1.0;
          }
          else
          {
            if ((dElevation_mean_neighbor - dElevation_mean_center) > dBreach_threshold)
            {
              vCell_active[lCellIndex_neighbor].dElevation_mean = dElevation_mean_center + dBreach_threshold;
            }
          }
          vCell_active[lCellIndex_neighbor].iFlag_stream_burning_treated = 1;

          // if elevation profile is turned on
          if (iFlag_elevation_profile == 1)
          {
            dElevation_profile0_neighbor = vCell_active[lCellIndex_neighbor].dElevation_profile0;
            if (dElevation_profile0_neighbor < dElevation_profile0_center)
            {
              vCell_active[lCellIndex_neighbor].dElevation_profile0 =
                  dElevation_profile0_center + abs(dElevation_profile0_center) * 0.001 + 1.0;
            }
          }
        }
      }
    }

    return error_code;
  }

  /**
   * @brief
   *
   * @param lCellID_active
   * @return int
   */
  int compset::compset_breaching_stream_elevation(long lCellID_active_in)
  {
    int error_code = 1;
    int iFlag_finished = 0;
    long lCellIndex_active;
    long lCellIndex2, lCellIndex3;
    long lCellID_downstream;
    long lCellID_downstream2;
    long lCellID_next;
    float dElevation_upstream;
    float dElevation_downstream;        
    float dDifference_dummy;
    lCellIndex_active = (mCellIdToIndex.find(lCellID_active_in))->second;
    while (iFlag_finished != 1)
    {
      lCellID_downstream = vCell_active[lCellIndex_active].lCellID_downstream_burned;
      dElevation_upstream = vCell_active[lCellIndex_active].dElevation_mean;
      if (lCellID_downstream != -1)
      {
        lCellIndex2 = (mCellIdToIndex.find(lCellID_downstream))->second;
        dElevation_downstream = vCell_active[lCellIndex2].dElevation_mean;
        dDifference_dummy = dElevation_upstream - dElevation_downstream;
        if (dDifference_dummy <= 0.0) // how about equals to 0.0?
        {
          vCell_active[lCellIndex2].dElevation_mean = dElevation_upstream - 0.01;
          vCell_active[lCellIndex_active].dElevation_mean = dElevation_downstream + 0.01;
          vCell_active[lCellIndex_active].dElevation_downstream = vCell_active[lCellIndex2].dElevation_mean;
          // find out the next downstream elevation
          lCellID_downstream2 = vCell_active[lCellIndex2].lCellID_downstream_burned;
          if (lCellID_downstream2 != -1)
          {
            lCellIndex3 = (mCellIdToIndex.find(lCellID_downstream2))->second;
            dDifference_dummy = vCell_active[lCellIndex2].dElevation_mean - vCell_active[lCellIndex3].dElevation_mean;
            if (dDifference_dummy <= 0.0) // another depression
            {
              lCellID_next = lCellID_downstream;
              compset_breaching_stream_elevation(lCellID_next);
            }
            else
            {
              iFlag_finished = 1;
              break;
            }
          }
          else
          {
            iFlag_finished = 1;
            break;
          }
        }
        else
        {
          iFlag_finished = 1;
        }
      }
      else
      {
        iFlag_finished = 1;
        break;
      }
    }

    return error_code;
  }

}
