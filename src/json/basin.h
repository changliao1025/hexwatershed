#pragma once

#include "JSONBase.h"
#include <list>
#include <vector>
namespace jsonmodel
{
  class basin : public JSONBase
  {
  public:
    basin();
    virtual ~basin();

    virtual bool Deserialize(const rapidjson::Value& obj);
    virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;

    int iFlag_dam;
    int iFlag_flowline;
    int iFlag_accumulation_threshold_provided;
    int iFlag_accumulation_threshold_ratio;

    float dLongitude_outlet_degree;
    float dLatitude_outlet_degree;
    float dBreach_threshold;
    //float dAccumulation_max;
    float dAccumulation_threshold_ratio;
    float dAccumulation_threshold_value;

    long lCellID_outlet;
    long lBasinID;


  private:

  };
}