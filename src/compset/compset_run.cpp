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
    long lCelllIndex_neighbor;
    long lCellID_downslope_neighbor;
    long lCellID_neighbor;

    std::vector<hexagon>::iterator iIterator_self;
    // std::vector<long>::iterator iIterator_neighbor;
    std::vector<int> vFlag(vCell_active.size());
    std::vector<int> vFinished(vCell_active.size());
    std::fill(vFlag.begin(), vFlag.end(), 0);
    std::fill(vFinished.begin(), vFinished.end(), 0);

    // the initial run

    lFlag_total = std::accumulate(vFlag.begin(), vFlag.end(), 0);

    while (lFlag_total != vCell_active.size())
    {
      for (iIterator_self = vCell_active.begin(); iIterator_self != vCell_active.end(); iIterator_self++)
      {

        if (vFlag.at((*iIterator_self).lCellIndex) == 1)
        {
          // this hexagon is finished
          continue;
        }
        else
        {
          // check whether one or more of the neighbors flow to itself
          iFlag_has_upslope = 0;
          iFlag_all_upslope_done = 1;
          for (int i = 0; i < (*iIterator_self).nNeighbor_land; i++)
          {
            lCellID_neighbor = (*iIterator_self).vNeighbor_land.at(i);

            lCelllIndex_neighbor = compset_find_index_by_cellid(lCellID_neighbor);

            lCellID_downslope_neighbor = (vCell_active.at(lCelllIndex_neighbor)).lCellID_downslope_dominant;

            if (lCellID_downslope_neighbor == (*iIterator_self).lCellID)
            {
              // there is one upslope neighbor found
              iFlag_has_upslope = 1;
              if (vFlag.at(lCelllIndex_neighbor) == 1)
              {
                // std::cout << "==" << lCelllIndex_neighbor << std::endl;
              }
              else
              {
                iFlag_all_upslope_done = 0;
              }
            }
            else
            {
            }
          }

          // there are the ones have no upslope at all

          if (iFlag_has_upslope == 0)
          {
            vFlag.at((*iIterator_self).lCellIndex) = 1;
          }
          else
          {
            // these ones have upslope,
            if (iFlag_all_upslope_done == 1)
            {
              // and they are finished scanning
              for (int i = 0; i < (*iIterator_self).nNeighbor_land; i++)
              {
                lCellID_neighbor = (*iIterator_self).vNeighbor_land.at(i);
                lCelllIndex_neighbor = compset_find_index_by_cellid(lCellID_neighbor);
                lCellID_downslope_neighbor = (vCell_active.at(lCelllIndex_neighbor)).lCellID_downslope_dominant;

                if (lCellID_downslope_neighbor == (*iIterator_self).lCellID)
                {
                  // std::cout << "===" << lCelllIndex_neighbor << std::endl;
                  // std::cout << "====" << lCellID_downslope_neighbor << std::endl;
                  // this one accepts upslope and the upslope is done
                  (*iIterator_self).dAccumulation =
                      (*iIterator_self).dAccumulation + vCell_active.at(lCelllIndex_neighbor).dAccumulation;
                }
                else
                {
                  // this neighbor does not flow here, sorry
                }
              }
              vFlag.at((*iIterator_self).lCellIndex) = 1;
            }
            else
            {
              // we have wait temporally
            }
          }
        }
      }
      lFlag_total = std::accumulate(vFlag.begin(), vFlag.end(), 0);
    }
    return error_code;
  }

  /**
   * define the stream network using flow accumulation value
   * @return
   */
  int compset::compset_define_stream_grid()
  {
    int error_code = 1;
    long lCellIndex_self;
    // in watershed hydrology, a threshold is usually used to define the network
    // here we use similar method
    float dAccumulation_min = 0.0;
    float dAccumulation_max = 0.0;
    float dAccumulation_threshold;
    long lCellID_outlet;
    long lCellIndex_outlet;
    std::vector<hexagon>::iterator iIterator_self;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        if (iFlag_multiple_outlet == 0)
        {
          // use outlet id as largest
          lCellID_outlet = aBasin.at(0).lCellID_outlet;
          lCellIndex_outlet = compset_find_index_by_cellid(lCellID_outlet);
          dAccumulation_threshold = 0.05 * vCell_active.at(lCellIndex_outlet).dAccumulation;
          // openmp may  not work for std container in earlier C++
          //#pragma omp parallel for private(lCellIndex_self)
          for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
          {
            if ((vCell_active.at(lCellIndex_self)).dAccumulation >= dAccumulation_threshold)
            {
              (vCell_active.at(lCellIndex_self)).iFlag_stream = 1;
              (vCell_active.at(lCellIndex_self)).dLength_stream_conceptual = (vCell_active.at(lCellIndex_self)).dResolution_effective;
            }
            else
            {
              (vCell_active.at(lCellIndex_self)).iFlag_stream = 0;
              // we still need its length for MOSART model.
              (vCell_active.at(lCellIndex_self)).dLength_stream_conceptual = (vCell_active.at(lCellIndex_self)).dResolution_effective;
            }
          }
        }
        else // todo: multiple 'pre-defined' outlets
        {
          // define pre-define first
          for (int iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
          {
            lCellID_outlet = aBasin.at(iWatershed - 1).lCellID_outlet;
            lCellIndex_outlet = compset_find_index_by_cellid(lCellID_outlet);
            dAccumulation_threshold = 0.05 * vCell_active.at(lCellIndex_outlet).dAccumulation;
            if (dAccumulation_threshold > dAccumulation_min)
            {
              dAccumulation_min = dAccumulation_threshold;
            }
          }
          // then define normal grid as well
        }
      }
      else
      {
        // find the maximum accumulation
        dAccumulation_max = 0.0;
        for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
        {
          if ((vCell_active.at(lCellIndex_self)).dAccumulation >= dAccumulation_max)
          {
            dAccumulation_max = (vCell_active.at(lCellIndex_self)).dAccumulation;
            lCellIndex_outlet = (vCell_active.at(lCellIndex_self)).lCellIndex;
          }
        }
        dAccumulation_threshold = 0.05 * vCell_active.at(lCellIndex_outlet).dAccumulation;

        for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
        {
          if ((vCell_active.at(lCellIndex_self)).dAccumulation >= dAccumulation_threshold)
          {
            (vCell_active.at(lCellIndex_self)).iFlag_stream = 1;
            (vCell_active.at(lCellIndex_self)).dLength_stream_conceptual = (vCell_active.at(lCellIndex_self)).dResolution_effective;
          }
          else
          {
            (vCell_active.at(lCellIndex_self)).iFlag_stream = 0;
            // we still need its length for MOSART model.
            (vCell_active.at(lCellIndex_self)).dLength_stream_conceptual = (vCell_active.at(lCellIndex_self)).dResolution_effective;
          }
        }
      }
    }
    else
    {
      // global scale simulation
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
    int iWatershed;
    long lCellIndex_self;
    long lCellIndex_current;
    long lCellIndex_downslope;
    long lCellIndex_outlet;
    long lCellID_downslope;
    long lCellID_outlet;
    float dDistance_to_watershed_outlet;
    std::string sWatershed;
    std::string sWorkspace_output_watershed;
    std::vector<float> vAccumulation;
    std::vector<float>::iterator iterator_float;
    std::vector<hexagon>::iterator iIterator_self;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          lCellID_outlet = aBasin.at(iWatershed - 1).lCellID_outlet;
          lCellIndex_outlet = compset_find_index_by_cellid(lCellID_outlet);
          watershed cWatershed;
          sWatershed = convert_integer_to_string(iWatershed, 2);
          cWatershed.sWorkspace_output_watershed = sWorkspace_output_hexwatershed + slash + sWatershed;
          cWatershed.sFilename_watershed_characteristics = cWatershed.sWorkspace_output_watershed + slash + "watershed.json";
          cWatershed.sFilename_segment_characteristics = cWatershed.sWorkspace_output_watershed + slash + "segment.json";
          cWatershed.sFilename_subbasin_characteristics = cWatershed.sWorkspace_output_watershed + slash + "subbasin.json";
          cWatershed.vCell.clear();
          cWatershed.iWatershed = iWatershed;
          cWatershed.lCellID_outlet = lCellID_outlet;
          // we may check the mesh id as well
          vCell_active.at(lCellIndex_outlet).iFlag_outlet = 1;
          for (lCellIndex_self = 0; lCellIndex_self < vCell_active.size(); lCellIndex_self++)
          {
            dDistance_to_watershed_outlet = (vCell_active.at(lCellIndex_self)).dDistance_to_downslope;
            iFound_outlet = 0;
            lCellIndex_current = lCellIndex_self;
            while (iFound_outlet != 1)
            {
              lCellID_downslope = (vCell_active.at(lCellIndex_current)).lCellID_downslope_dominant;
              if (lCellID_outlet == lCellID_downslope)
              {
                iFound_outlet = 1;
                (vCell_active.at(lCellIndex_self)).iFlag_watershed = 1;
                (vCell_active.at(lCellIndex_self)).dDistance_to_watershed_outlet = dDistance_to_watershed_outlet;
                (vCell_active.at(lCellIndex_self)).iWatershed = iWatershed; // single watershed
                cWatershed.vCell.push_back(vCell_active.at(lCellIndex_self));
              }
              else
              {
                lCellIndex_current = compset_find_index_by_cellid(lCellID_downslope);
                dDistance_to_watershed_outlet = dDistance_to_watershed_outlet + (vCell_active.at(lCellIndex_current)).dDistance_to_downslope;
                if (lCellIndex_current >= 0)
                {
                  iFound_outlet = 0;
                }
                else
                {
                  iFound_outlet = 1; // a cell not going in this outlet may be going to a different one
                }
              }
            }
          }
          (vCell_active.at(lCellIndex_outlet)).iFlag_watershed = 1;
          vCell_active.at(lCellIndex_outlet).iWatershed = iWatershed;
          cWatershed.vCell.push_back(vCell_active.at(lCellIndex_outlet));
          vWatershed.push_back(cWatershed);
        }
        // how about other auto-defined watershed?
        // todo
      }
      else
      {
        // without flowline
      }
    }
    else
    {
    }

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
    int iWatershed;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        nSegment_total = 0;
        nConfluence_total = 0;
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).watershed_define_stream_confluence();
        }
      }
    }
    else
    {
    }

    return error_code;
  }

  /**
   * define the stream segment, must use vCell_active
   * @return
   */
  int compset::compset_define_stream_segment()
  {
    int error_code = 1;
    int iWatershed;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).watershed_define_stream_segment();
        }
      }
    }

    return error_code;
  }

  int compset::compset_build_stream_topology()
  {
    int error_code = 1;
    int iWatershed;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).watershed_build_stream_topology();
        }
      }
    }
    return error_code;
  }
  int compset::compset_define_stream_order()
  {
    int error_code = 1;
    int iWatershed;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).watershed_define_stream_order();
        }
      }
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
    int iWatershed;

    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).watershed_define_subbasin();
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

  int compset::compset_calculate_watershed_characteristics()
  {
    int error_code = 1;
    int iWatershed;
    int iFlag_global = cParameter.iFlag_global;
    int iFlag_flowline = cParameter.iFlag_flowline;
    int iFlag_multiple_outlet = cParameter.iFlag_multiple_outlet;
    if (iFlag_global != 1)
    {
      if (iFlag_flowline == 1)
      {
        for (iWatershed = 1; iWatershed <= cParameter.nOutlet; iWatershed++)
        {
          vWatershed.at(iWatershed - 1).calculate_watershed_characteristics();
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
  int compset::update_cell_elevation()
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
  int compset::update_vertex_elevation()
  {
    int error_code = 1;
    long lVertexIndex = 0;
    std::vector<hexagon>::iterator iIterator1;
    std::vector<vertex>::iterator iIterator2;
    std::vector<vertex>::iterator iIterator3;

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

    return error_code;
  }

}
