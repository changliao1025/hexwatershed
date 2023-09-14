/**
 * @file domain.cpp
 * @author Chang Liao (chang.liao@pnnl.gov)
 * @brief the realization of the domain class
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
   * initialize the model
   * @return
   */
  int compset::compset_initialize_model()
  {
    int error_code = 1;

    // get informaiton from the domain pass variable
    long lVertexIndex = 0;
    long lCellIndex = 0;
    long lCellIndex_outlet;
    long ncell = this->aCell.size();
    float dLongtitue_min = 180;
    float dLongtitue_max = -180;
    float dLatitude_min = 90;
    float dLatitude_max = -90;
    std::string sTime;

    std::vector<cell>::iterator iIterator1;
    std::vector<vertex>::iterator iIterator2;

    vVertex_active.clear();
    vCell_active.clear();
    lVertexIndex = 0;
    // vCell_active.reserve(ncell); //CL: dont remember that this does
    for (iIterator1 = aCell.begin(); iIterator1 != aCell.end(); iIterator1++)
    {
      hexagon pHexagon;
      pHexagon.lCellIndex = lCellIndex;
      pHexagon.lCellID = (*iIterator1).lCellID;
      pHexagon.dElevation_mean = (*iIterator1).dElevation_mean;
      pHexagon.dElevation_raw = (*iIterator1).dElevation_raw;
      pHexagon.dElevation_profile0 = (*iIterator1).dElevation_profile0;
      pHexagon.dLength_stream_burned = (*iIterator1).dLength_flowline;
      pHexagon.dArea = (*iIterator1).dArea;
      pHexagon.dAccumulation = pHexagon.dArea;
      pHexagon.dLongitude_center_degree = (*iIterator1).dLongitude_center_degree;
      pHexagon.dLatitude_center_degree = (*iIterator1).dLatitude_center_degree;
      pHexagon.dLongitude_center_radian = convert_degree_to_radian(pHexagon.dLongitude_center_degree);
      pHexagon.dLatitude_center_radian = convert_degree_to_radian(pHexagon.dLatitude_center_degree);
      pHexagon.vNeighbor = (*iIterator1).aNeighbor;
      pHexagon.vNeighbor_distance = (*iIterator1).aNeighbor_distance;
      pHexagon.vNeighbor_land = (*iIterator1).aNeighbor_land;
      // pHexagon.vNeighbor_ocean = (*iIterator1).aNeighbor_ocean;
      // edge and vertex coordinates are not yet used
      pHexagon.nNeighbor = (*iIterator1).nNeighbor;
      pHexagon.nNeighbor_land = (*iIterator1).nNeighbor_land;
      // pHexagon.nNeighbor_ocean = (*iIterator1).nNeighbor_ocean;
      pHexagon.nEdge = (*iIterator1).nEdge;
      pHexagon.nVertex = (*iIterator1).nVertex;
      pHexagon.vVertex.clear();

      // define the bounding box

      for (int i = 0; i < pHexagon.nVertex; i++)
      {
        vertex pVertex = (*iIterator1).vVertex.at(i);
        pVertex.dElevation = pHexagon.dElevation_mean; // this needs to be updated
        iIterator2 = std::find(vVertex_active.begin(), vVertex_active.end(), pVertex);
        if (iIterator2 != vVertex_active.end())
        {
          // it is already indexed
          pVertex.lVertexIndex = (*iIterator2).lVertexIndex;
        }
        else
        {
          pVertex.lVertexIndex = lVertexIndex;
          lVertexIndex = lVertexIndex + 1;
          vVertex_active.push_back(pVertex);
        }
        if (pVertex.dLongitude_degree < dLongtitue_min)
        {
          dLongtitue_min = pVertex.dLongitude_degree;
        }
        if (pVertex.dLongitude_degree > dLongtitue_max)
        {
          dLongtitue_max = pVertex.dLongitude_degree;
        }
        if (pVertex.dLatitude_degree < dLatitude_min)
        {
          dLatitude_min = pVertex.dLatitude_degree;
        }
        if (pVertex.dLatitude_degree > dLatitude_max)
        {
          dLatitude_max = pVertex.dLatitude_degree;
        }
        pHexagon.vVertex.push_back(pVertex);
        // define the bounding box using the vertex info
        pHexagon.aBoundingBox = {dLongtitue_min, dLatitude_min, dLongtitue_max, dLatitude_max};
      }

      pHexagon.lStream_segment_burned = (*iIterator1).lStream_segment_burned;
      pHexagon.iStream_order_burned = (*iIterator1).iStream_order_burned;
      pHexagon.lCellID_downstream_burned = (*iIterator1).lCellID_downstream_burned;

      if (pHexagon.lStream_segment_burned > 0) // check it starts with 1
      {
        pHexagon.iFlag_stream_burned = 1;
      }

      pHexagon.calculate_effective_resolution();
      // we require the
      if (pHexagon.dLength_stream_burned < pHexagon.dLength_stream_conceptual)
      {
        pHexagon.dLength_stream_burned = pHexagon.dLength_stream_conceptual;
      }

      vCell_active.push_back(pHexagon);
      // build the unordered map from cellid to index
      mCellIdToIndex[pHexagon.lCellID] = lCellIndex;
      lCellIndex = lCellIndex + 1;
    }

    // Convert the time_t object to a string representation
    sTime = get_current_time();
    // Print the current time
    std::cout << "Current time: " << sTime;
    std::cout << "Finished initialization!" << std::endl;
    std::flush(std::cout);
    return error_code;
  }

  /**
   * find the cell index by cell id, this requires the cell id is unique and the map is unchanged
   * @param lCellID_in
   * @return
   */
  long compset::compset_find_index_by_cell_id(long lCellID_in)
  {
    long lCellIndex = -1;
    // the new method uses unordered map to speed up
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
}
