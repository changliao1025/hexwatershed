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
#include "./compset.h"

namespace hexwatershed
{

  /**
   * calculate the flow accumulation based on flow direction
   * @return
   */
  int compset::compset_calculate_flow_accumulation()
  {
    int error_code = 1;
    int iFlag_has_upslope = 0;
    int iFlag_all_upslope_done; // assume all are done
    long lFlag_total = 0;
    long lCellIndex_neighbor;
    long lCellID_downslope_neighbor;

    std::vector<hexagon>::iterator iIterator_self;
    std::vector<int> vFlag(vCell_active.size());
    std::fill(vFlag.begin(), vFlag.end(), 0);
    std::vector<long> vNeighbor_land;
    std::vector<long>::iterator iIterator_neighbor;

    // track progress with a counter instead of scanning the whole vector each iteration
    long lFlag_total_prev = -1;

    while (lFlag_total != (long)vCell_active.size())
    {
      lFlag_total_prev = lFlag_total;
      for (iIterator_self = vCell_active.begin(); iIterator_self != vCell_active.end(); iIterator_self++)
      {

        if (vFlag[(*iIterator_self).lCellIndex] == 1)
        {
          // this hexagon is finished
          continue;
        }
        else
        {
          // check whether one or more of the neighbors flow to itself
          iFlag_has_upslope = 0;
          iFlag_all_upslope_done = 1;
          vNeighbor_land = (*iIterator_self).vNeighbor_land;
          for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
          {
            // use find() instead of operator[] to avoid inserting default entries
            // for neighbors that are outside the active domain
            auto it = mCellIdToIndex.find(*iIterator_neighbor);
            if (it == mCellIdToIndex.end())
            {
              continue; // neighbor not in active domain, skip
            }
            lCellIndex_neighbor = it->second;
            lCellID_downslope_neighbor = (vCell_active[lCellIndex_neighbor]).lCellID_downslope_dominant;
            if (lCellID_downslope_neighbor == (*iIterator_self).lCellID)
            {
              // there is one upslope neighbor found
              iFlag_has_upslope = 1;
              if (vFlag[lCellIndex_neighbor] != 1)
              {
                iFlag_all_upslope_done = 0;
              }
            }
          }

          // there are the ones have no upslope at all

          if (iFlag_has_upslope == 0)
          {
            vFlag[(*iIterator_self).lCellIndex] = 1;
            lFlag_total++;
          }
          else
          {
            // these ones have upslope,
            if (iFlag_all_upslope_done == 1)
            {
              // and they are finished scanning
              for (iIterator_neighbor = vNeighbor_land.begin(); iIterator_neighbor != vNeighbor_land.end(); iIterator_neighbor++)
              {
                auto it = mCellIdToIndex.find(*iIterator_neighbor);
                if (it == mCellIdToIndex.end())
                {
                  continue; // neighbor not in active domain, skip
                }
                lCellIndex_neighbor = it->second;
                lCellID_downslope_neighbor = (vCell_active[lCellIndex_neighbor]).lCellID_downslope_dominant;

                if (lCellID_downslope_neighbor == (*iIterator_self).lCellID)
                {
                  // this one accepts upslope and the upslope is done
                  (*iIterator_self).dAccumulation =
                      (*iIterator_self).dAccumulation + vCell_active[lCellIndex_neighbor].dAccumulation;
                }
              }
              vFlag[(*iIterator_self).lCellIndex] = 1;
              lFlag_total++;
            }
            else
            {
              // we have to wait temporarily
            }
          }
        }
      }
      // Safety check: if no progress was made in this full pass, there is a cycle
      // in the flow-direction graph that cannot be resolved. Break to avoid an
      // infinite loop and warn the user.
      if (lFlag_total == lFlag_total_prev)
      {
        long lUnresolved = (long)vCell_active.size() - lFlag_total;
        std::cout << "Warning: flow accumulation stalled with "
                  << lUnresolved
                  << " cell(s) unresolved. A cycle may exist in the flow direction graph. "
                  << "Forcing remaining cells to complete." << std::endl;

        // Diagnostic: print details of each unresolved cell to help identify the cycle
        std::cout << "=== Diagnostic: unresolved cells ===" << std::endl;
        std::cout << "CellID\tElev\tDepFilled\tStreamBurned\tWsBndBurned\tStreamBurnTreated\tDownslopeDominant\tNeighbors(ID->theirDownslope)" << std::endl;
        for (iIterator_self = vCell_active.begin(); iIterator_self != vCell_active.end(); iIterator_self++)
        {
          if (vFlag[(*iIterator_self).lCellIndex] != 1)
          {
            const hexagon &cell = *iIterator_self;
            std::cout << cell.lCellID
                      << "\t" << cell.dElevation_mean
                      << "\t" << cell.iFlag_depression_filling_treated
                      << "\t" << cell.iFlag_stream_burned
                      << "\t" << cell.iFlag_watershed_boundary_burned
                      << "\t" << cell.iFlag_stream_burning_treated
                      << "\t" << cell.lCellID_downslope_dominant
                      << "\t[";
            // Print each land neighbor and where that neighbor flows
            for (auto nbr_id : cell.vNeighbor_land)
            {
              auto it2 = mCellIdToIndex.find(nbr_id);
              if (it2 != mCellIdToIndex.end())
              {
                long nbr_idx = it2->second;
                std::cout << nbr_id << "->" << vCell_active[nbr_idx].lCellID_downslope_dominant << " ";
              }
            }
            std::cout << "]" << std::endl;
          }
        }
        std::cout << "=== End diagnostic ===" << std::endl;

        // Force-complete all remaining cells so the program can continue
        for (iIterator_self = vCell_active.begin(); iIterator_self != vCell_active.end(); iIterator_self++)
        {
          if (vFlag[(*iIterator_self).lCellIndex] != 1)
          {
            vFlag[(*iIterator_self).lCellIndex] = 1;
            lFlag_total++;
          }
        }
      }
    }
    return error_code;
  }

  /**
   * this function is used to define at least one watershed by finding the max flow accumulation
   * @return
   */
  int compset::compset_stats_flow_accumulation()
  {
    int error_code = 1;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    int iFlag_stream_grid_option;
    int iFlag_accumulation_threshold_ratio = cParameter.iFlag_accumulation_threshold_ratio;
    int iFlag_accumulation_threshold_ratio_basin = 0;
    long nOutlet;
    long lCellIndex_self;
    long lCellID_outlet;
    long lCellIndex_outlet;
    float dAccumulation;
    float dAccumulation_min = 0.0;
    float dAccumulation_max = 0.0;
    float dAccumulation_threshold = 0.0;
    std::vector<hexagon>::iterator iIterator_self;
    if (iFlag_multiple_outlet == 0) // only one outlet
    {
      if (iFlag_flowline == 1) // user provided flowline
      {
        // maybe we can just get the flow accumulation directly
        aBasin[0].iFlag_flowline = 1;
        lCellIndex_outlet = mCellIdToIndex[aBasin[0].lCellID_outlet];
        dAccumulation_max = (vCell_active[lCellIndex_outlet]).dAccumulation;
        // no watershed yet, so we have to use the basin to save the max accumulation
        aBasin[0].dAccumulation_max = dAccumulation_max;
      }
      else // no flowline provided, so it is based on DEM
      {
        dAccumulation_max = 0.0;
        for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
        {
          if ((vCell_active[lCellIndex_self]).dAccumulation >= dAccumulation_max)
          {
            dAccumulation_max = (vCell_active[lCellIndex_self]).dAccumulation;
            lCellIndex_outlet = (vCell_active[lCellIndex_self]).lCellIndex;
          }
        }
        // also set the outlet id
        lCellID_outlet = vCell_active[lCellIndex_outlet].lCellID;
        // should we update at least one watershed?
        basin pBasin;
        aBasin.clear();
        aBasin.push_back(pBasin);
        // set the id and outlet
        aBasin[0].iFlag_flowline = 0; // the model will define a watershed, but it has no user provided flowline
        aBasin[0].lCellID_outlet = lCellID_outlet;
        aBasin[0].dLongitude_outlet_degree = vCell_active[lCellIndex_outlet].dLongitude_center_degree;
        aBasin[0].dLatitude_outlet_degree = vCell_active[lCellIndex_outlet].dLatitude_center_degree;
        aBasin[0].dAccumulation_max = dAccumulation_max;
        // we also need to update the nOutlet, this is key for the following watershed algorithm
        cParameter.nOutlet = 1;
      }
    }
    else
    {
      if (iFlag_flowline == 1) // user provided more than 1 outlet/flowline
      {
        for (long lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
        {
          lCellID_outlet = aBasin[lWatershed - 1].lCellID_outlet;
          lCellIndex_outlet = mCellIdToIndex[lCellID_outlet];
          aBasin[lWatershed - 1].iFlag_flowline = 1;
          // set the id and outlet
          aBasin[lWatershed - 1].lCellID_outlet = lCellID_outlet;
          aBasin[lWatershed - 1].dLongitude_outlet_degree = vCell_active[lCellIndex_outlet].dLongitude_center_degree;
          aBasin[lWatershed - 1].dLatitude_outlet_degree = vCell_active[lCellIndex_outlet].dLatitude_center_degree;
        }
      }
      else
      {
        // pure dem based watershed definition
        dAccumulation_max = 0.0;
        for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
        {
          if ((vCell_active[lCellIndex_self]).dAccumulation >= dAccumulation_max)
          {
            dAccumulation_max = (vCell_active[lCellIndex_self]).dAccumulation;
          }
        }
        // now we can define the watershed based on the max accumulation?
        // for a large scale simulation, we allow multiple watersheds,
        // but smaller watershed do not have the large accumulation, so we use a threshold
        dAccumulation_threshold = dAccumulation_max * 0.1;
        // criteria for defining the watershed outlet: (1) has no downslope, (2) has accumulation larger than the threshold
        nOutlet = 0;
        cParameter.nOutlet = 0;
        for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
        {
          if ((vCell_active[lCellIndex_self]).dAccumulation >= dAccumulation_threshold && (vCell_active[lCellIndex_self]).lCellID_downslope_dominant == -1)
          {
            // this is a potential outlet
            lCellID_outlet = vCell_active[lCellIndex_self].lCellID;
            lCellIndex_outlet = vCell_active[lCellIndex_self].lCellIndex;
            // we can define a basin here
            basin pBasin;
            aBasin.push_back(pBasin);
            aBasin[cParameter.nOutlet].iFlag_flowline = 0; // the model will define a watershed, but it has no user provided flowline
            aBasin[cParameter.nOutlet].lCellID_outlet = lCellID_outlet;
            aBasin[cParameter.nOutlet].dLongitude_outlet_degree = vCell_active[lCellIndex_outlet].dLongitude_center_degree;
            aBasin[cParameter.nOutlet].dLatitude_outlet_degree = vCell_active[lCellIndex_outlet].dLatitude_center_degree;
            aBasin[cParameter.nOutlet].dAccumulation_max = vCell_active[lCellIndex_self].dAccumulation;
            cParameter.nOutlet++;
          }
        }
      }
    }

    return error_code;
  }

  /**
   * define the watershed boundary using outlet
   * @return
   */
  int compset::compset_define_watershed_boundary()
  {
    int error_code = 1;
    int iFound_outlet;
    int iFlag_flowline;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    long lWatershed;
    long lCellIndex_self;
    long lCellIndex_current;
    long lCellIndex_outlet;
    long lCellIndex_watershed;
    long lCellID_downslope;
    long lCellID_outlet;
    std::string sWatershed;
    std::string sWorkspace_output_watershed;
    std::vector<float>::iterator iterator_float;
    std::vector<hexagon>::iterator iIterator_self;

    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      lCellID_outlet = aBasin[lWatershed - 1].lCellID_outlet;
      lCellIndex_outlet = mCellIdToIndex[lCellID_outlet];
      watershed cWatershed;
      sWatershed = convert_long_to_string(lWatershed, 8); // increase to 8 to include 100 million rivers
      cWatershed.sWorkspace_output_watershed = sWorkspace_output_hexwatershed + slash + sWatershed;
      // make output
      if (path_test(cWatershed.sWorkspace_output_watershed) == 0)
      {
        make_directory(cWatershed.sWorkspace_output_watershed);
      }
      cWatershed.sFilename_watershed_json = cWatershed.sWorkspace_output_watershed + slash + "watershed.json";
      cWatershed.sFilename_watershed_stream_edge_json = cWatershed.sWorkspace_output_watershed + slash + "stream_edge.json";
      cWatershed.sFilename_watershed_characteristics = cWatershed.sWorkspace_output_watershed + slash + "watershed.txt";
      cWatershed.sFilename_segment_characteristics = cWatershed.sWorkspace_output_watershed + slash + "segment.txt";
      cWatershed.sFilename_subbasin_characteristics = cWatershed.sWorkspace_output_watershed + slash + "subbasin.txt";
      cWatershed.sFilename_hillslope_characteristics = cWatershed.sWorkspace_output_watershed + slash + "hillslope.txt";
      cWatershed.vCell.clear();
      cWatershed.lWatershed = lWatershed;
      cWatershed.lCellID_outlet = lCellID_outlet;
      lCellIndex_watershed = 0;
      // we may check the mesh id as well
      vCell_active[lCellIndex_outlet].iFlag_outlet = 1;
      for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
      {
        // if it is already in another watershed, skip it
        if ((vCell_active[lCellIndex_self]).iFlag_watershed == 1)
        {
          continue;
        }
        lCellID_downslope = (vCell_active[lCellIndex_self]).lCellID_downslope_dominant;
        if (lCellID_downslope != -1)
        {
          iFound_outlet = 0;
        }
        else
        {
          iFound_outlet = 1;
        }
        lCellIndex_current = lCellIndex_self;
        while (iFound_outlet != 1)
        {
          lCellID_downslope = (vCell_active[lCellIndex_current]).lCellID_downslope_dominant;
          if (lCellID_outlet == lCellID_downslope)
          {
            iFound_outlet = 1;
            (vCell_active[lCellIndex_self]).iFlag_watershed = 1;
            (vCell_active[lCellIndex_self]).lWatershed = lWatershed;
            (vCell_active[lCellIndex_self]).lCellIndex_watershed = lCellIndex_watershed;
            // only push the cell, not the outlet
            cWatershed.vCell.push_back(vCell_active[lCellIndex_self]);
            cWatershed.mCellIdToIndex[(vCell_active[lCellIndex_self]).lCellID] = lCellIndex_watershed;
            lCellIndex_watershed = lCellIndex_watershed + 1;
          }
          else
          {
            if (lCellID_downslope != -1)
            {
              lCellIndex_current = (mCellIdToIndex.find(lCellID_downslope))->second;
              if (lCellIndex_current >= 0)
              {
                if (vCell_active[lCellIndex_current].lWatershed == lWatershed) // the downslope is already finished
                {
                  (vCell_active[lCellIndex_self]).iFlag_watershed = 1;
                  (vCell_active[lCellIndex_self]).lWatershed = lWatershed;
                  (vCell_active[lCellIndex_self]).lCellIndex_watershed = lCellIndex_watershed;
                  cWatershed.vCell.push_back(vCell_active[lCellIndex_self]);
                  cWatershed.mCellIdToIndex[(vCell_active[lCellIndex_self]).lCellID] = lCellIndex_watershed;
                  lCellIndex_watershed = lCellIndex_watershed + 1;
                  iFound_outlet = 1;
                }
                else
                {
                  iFound_outlet = 0;
                }
              }
              else
              {
                iFound_outlet = 1; // a cell not going in this outlet may be going to a different one
              }
            }
            else
            {
              iFound_outlet = 1; // this cell is going out of domain and it does not belong to any user-defined watersheds.
            }
          }
        }
      }
      // in the last step, we then push in the outlet cell
      vCell_active[lCellIndex_outlet].iFlag_watershed = 1;
      vCell_active[lCellIndex_outlet].lWatershed = lWatershed;
      vCell_active[lCellIndex_outlet].lCellIndex_watershed = lCellIndex_watershed;

      cWatershed.dAccumulation_max = vCell_active[lCellIndex_outlet].dAccumulation;
      // copy parameter as well
      cWatershed.cParameter.iFlag_stream_grid_option = cParameter.iFlag_stream_grid_option;
      cWatershed.cParameter.iFlag_flowline = aBasin[lWatershed - 1].iFlag_flowline; // this one is tricky, because not all have flowline
      cWatershed.cParameter.iFlag_accumulation_threshold_ratio = aBasin[lWatershed - 1].iFlag_accumulation_threshold_ratio;
      cWatershed.cParameter.dAccumulation_threshold_ratio = aBasin[lWatershed - 1].dAccumulation_threshold_ratio;
      cWatershed.cParameter.dAccumulation_threshold_value = aBasin[lWatershed - 1].dAccumulation_threshold_value;

      cWatershed.vCell.push_back(vCell_active[lCellIndex_outlet]);
      cWatershed.mCellIdToIndex[(vCell_active[lCellIndex_outlet]).lCellID] = lCellIndex_watershed;
      cWatershed.dLongitude_outlet_degree = vCell_active[lCellIndex_outlet].dLongitude_center_degree;
      cWatershed.dLatitude_outlet_degree = vCell_active[lCellIndex_outlet].dLatitude_center_degree;
      // also need to copy the cParameter from compset to watershed object (mannually, because some parameter are already set)
      // careful here
      vWatershed.push_back(cWatershed);
    }
    // how about other auto-defined watershed?

    return error_code;
  }

  /**
   * define the stream network using flow accumulation value
   * @return
   */
  int compset::compset_define_stream_grid()
  {
    int error_code = 1;
    long lWatershed;
    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_define_stream_grid();
    }
    // how about the remaining cells?

    return error_code;
  }

  /**
   * define the stream confluence point
   * because we need to topology info, the vCell_active will be used
   * @return
   */
  int compset::compset_define_stream_confluence()
  {
    int error_code = 1;
    long lWatershed;
    nSegment_total = 0;
    nConfluence_total = 0;
    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_define_stream_confluence();
      nConfluence_total = nConfluence_total + vWatershed[lWatershed - 1].nConfluence;
      nSegment_total = nSegment_total + vWatershed[lWatershed - 1].nSegment;
    }
    // how about the remaining cells?

    return error_code;
  }

  /**
   * define the stream segment, must use vCell_active
   * @return
   */
  int compset::compset_define_stream_segment()
  {
    int error_code = 1;
    long lWatershed;
    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_define_stream_segment();
    }

    return error_code;
  }

  int compset::compset_build_stream_topology()
  {
    int error_code = 1;
    long lWatershed;

    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_build_stream_topology();
    }

    return error_code;
  }
  int compset::compset_define_stream_order()
  {
    int error_code = 1;
    long lWatershed;
    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_define_stream_order();
    }

    return error_code;
  }
  /**
   * define subbasin boundary, it requires cell topology, so the vCell_active is used
   * @return
   */
  int compset::compset_define_subbasin()
  {
    int error_code = 1;
    long lWatershed;
    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].watershed_define_subbasin();
    }

    return error_code;
  }

  /**
   * @brief
   *
   * @return int
   */

  int compset::compset_calculate_watershed_characteristics()
  {
    int error_code = 1;
    long lWatershed;
    int iFlag_hillslope = cParameter.iFlag_hillslope;

    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      vWatershed[lWatershed - 1].iFlag_hillslope = iFlag_hillslope;
      vWatershed[lWatershed - 1].watershed_calculate_characteristics();
    }
    return error_code;
  }

  int compset::compset_transfer_watershed_to_domain()
  {
    int error_code = 1;
    long lWatershed;
    long lCellIndex; //, lCellIndex1;
    std::vector<hexagon>::iterator iIterator1;
    std::vector<hexagon>::iterator iIterator2;

    for (lWatershed = 1; lWatershed <= cParameter.nOutlet; lWatershed++)
    {
      watershed cWatershed = vWatershed[lWatershed - 1];
      for (iIterator1 = cWatershed.vCell.begin(); iIterator1 != cWatershed.vCell.end(); iIterator1++)
      {
        lCellIndex = (*iIterator1).lCellIndex;
        if (lCellIndex != -1)
        {
          vCell_active[lCellIndex].iFlag_stream = (*iIterator1).iFlag_stream;
          vCell_active[lCellIndex].dLength_stream_conceptual = (*iIterator1).dLength_stream_conceptual;
          vCell_active[lCellIndex].lSubbasin = (*iIterator1).lSubbasin;
          vCell_active[lCellIndex].lSegment = (*iIterator1).lSegment;
          vCell_active[lCellIndex].dDistance_to_subbasin_outlet = (*iIterator1).dDistance_to_subbasin_outlet;
          // additional attributes may be added here
        }
      }
    }

    return error_code;
  }
  /**
   * @brief
   *
   * @return int
   */
  int compset::compset_update_cell_elevation()
  {
    int error_code = 1;
    std::vector<hexagon>::iterator iIterator1;
    for (iIterator1 = vCell_active.begin(); iIterator1 != vCell_active.end(); ++iIterator1)
    {
      (*iIterator1).update_location();
    }
    return error_code;
  }

  /**
   * @brief
   *
   * @return int
   */
  int compset::compset_update_vertex_elevation()
  {
    int error_code = 1;
    int iFlag_vtk = cParameter.iFlag_vtk;
    std::vector<hexagon>::iterator iIterator1;
    std::vector<vertex>::iterator iIterator2, iIterator3;
    // Build a mapping from vertices to cells
    if (iFlag_vtk == 1)
    {
      for (iIterator2 = vVertex_active.begin(); iIterator2 != vVertex_active.end(); ++iIterator2)
      {
        for (iIterator1 = vCell_active.begin(); iIterator1 != vCell_active.end(); ++iIterator1)
        {
          iIterator3 = std::find((*iIterator1).vVertex.begin(), (*iIterator1).vVertex.end(), (*iIterator2));
          if (iIterator3 != (*iIterator1).vVertex.end())
          {
            // found
            (*iIterator2).dElevation = (*iIterator1).dElevation_mean;
            // update location too
            (*iIterator2).update_location();
            break;
          }
          else
          {
          }
        }
      }
    }

    return error_code;
  }

}
