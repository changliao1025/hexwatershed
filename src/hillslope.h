//
// Created by Liao, Chang on 2020-09-07.
//

#pragma once
#include <iostream>
#include <unordered_map>
#include "hexagon.h"
#include "./json/JSONBase.h"
#include "./json/mesh.h"
#include "./json/cell.h"
#include "./json/basin.h"
#include "./json/multibasin.h"
using namespace std;
namespace hexwatershed
  {
      class hillslope
      {
      public:
          hillslope();
          ~hillslope();
          int iFlag_headwater; // 1: headwater, 0: not headwater
          int nHillslope; 
          long lSubbasin; //each subbasin should have the same index with its segment      
          long lHillslope; 
          long nCell;        
          double dArea;
          float dSlope;
          float dSlope_mean;
          float dSlope_max;
          float dSlope_min;        
          float dDrainage_density;
          std::vector <hexagon> vCell; //all the cells         
          std::unordered_map<long, long> mCellIdToIndex;     
          int hillslope_calculate_characteristics();   

         
      };
  } // namespace hexwatershed
