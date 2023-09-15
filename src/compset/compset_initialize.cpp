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
    long lVertexIndex = 0;
    long lCellIndex = 0;
    long lCellIndex_outlet;
    long ncell = this->aCell.size();
    //float dLongtitue_min = 180;
    //float dLongtitue_max = -180;
    //float dLatitude_min = 90;
    //float dLatitude_max = -90;
    
    //std::vector<cell>::iterator iIterator1;
    //std::vector<vertex>::iterator iIterator2;
    vVertex_active.clear();
    vCell_active.clear();
    
    vCell_active.reserve(ncell); //CL: dont remember that this does but it speeds up the code
    //for (iIterator1 = aCell.begin(); iIterator1 != aCell.end(); iIterator1++)
    for (const cell& pCell : aCell) 
    {
      hexagon pHexagon;
      pHexagon.lCellIndex = lCellIndex;
      pHexagon.lCellID = pCell.lCellID;
      pHexagon.dElevation_mean = pCell.dElevation_mean;
      pHexagon.dElevation_raw = pCell.dElevation_raw;
      pHexagon.dElevation_profile0 = pCell.dElevation_profile0;
      pHexagon.dLength_stream_burned = pCell.dLength_flowline;
      pHexagon.dArea = pCell.dArea;
      pHexagon.dAccumulation = pHexagon.dArea;
      pHexagon.dLongitude_center_degree = pCell.dLongitude_center_degree;
      pHexagon.dLatitude_center_degree = pCell.dLatitude_center_degree;
      pHexagon.dLongitude_center_radian = convert_degree_to_radian(pHexagon.dLongitude_center_degree);
      pHexagon.dLatitude_center_radian = convert_degree_to_radian(pHexagon.dLatitude_center_degree);
      pHexagon.vNeighbor = pCell.aNeighbor;
      pHexagon.vNeighbor_distance = pCell.aNeighbor_distance;
      pHexagon.vNeighbor_land = pCell.aNeighbor_land;
      // pHexagon.vNeighbor_ocean = pCell.aNeighbor_ocean;
      // edge and vertex coordinates are not yet used
      pHexagon.nNeighbor = pCell.nNeighbor;
      pHexagon.nNeighbor_land = pCell.nNeighbor_land;
      // pHexagon.nNeighbor_ocean = pCell.nNeighbor_ocean;
      pHexagon.nEdge = pCell.nEdge;
      pHexagon.nVertex = pCell.nVertex;
      //pHexagon.vVertex.clear();

      // define the bounding box
      pHexagon.vVertex.reserve(pHexagon.nVertex); 
      //for (int i = 0; i < pHexagon.nVertex; i++)
      for (const vertex& pV : pCell.vVertex)
      {
        vertex pVertex = pV;
        pVertex.dElevation = pHexagon.dElevation_mean; // this needs to be updated
        auto iIterator2 = std::find(vVertex_active.begin(), vVertex_active.end(), pVertex);
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
        //if (pVertex.dLongitude_degree < dLongtitue_min)
        //{
        //  dLongtitue_min = pVertex.dLongitude_degree;
        //}
        //if (pVertex.dLongitude_degree > dLongtitue_max)
        //{
        //  dLongtitue_max = pVertex.dLongitude_degree;
        //}
        //if (pVertex.dLatitude_degree < dLatitude_min)
        //{
        //  dLatitude_min = pVertex.dLatitude_degree;
        //}
        //if (pVertex.dLatitude_degree > dLatitude_max)
        //{
        //  dLatitude_max = pVertex.dLatitude_degree;
        //}
        pHexagon.vVertex.push_back(pVertex);
        }

      // define the bounding box using the vertex info, no longer needed for spatial index
      //pHexagon.aBoundingBox = {dLongtitue_min, dLatitude_min, dLongtitue_max, dLatitude_max};
   

      pHexagon.lStream_segment_burned = pCell.lStream_segment_burned;
      pHexagon.iStream_order_burned = pCell.iStream_order_burned;
      pHexagon.lCellID_downstream_burned = pCell.lCellID_downstream_burned;

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
      lCellIndex++;
    }

    // Convert the time_t object to a string representation
    sTime = get_current_time();        
    sLog = "Finished initialization at " + sTime;
    std::cout << sLog << std::endl;
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
