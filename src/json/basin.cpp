#include <string>
#include "basin.h"

namespace jsonmodel
{
  basin::basin()
  {
    dLongitude_outlet_degree=-9999;
    dLatitude_outlet_degree=-9999;
    iFlag_accumulation_threshold_ratio = 0;

    dAccumulation_threshold_ratio = 0.001;
    dAccumulation_threshold_value = 2.5E7;
    dBreach_threshold = 5.0; // unit in meter

    lCellID_outlet =-1;
  }

  basin::~basin()
  {

  }


  bool basin::Deserialize(const rapidjson::Value & obj)
  {
    std::string sKey;
    float dValue;
    sKey = "dLongitude_outlet_degree";
    if (obj.HasMember(sKey.c_str()))
      {
        this->dLongitude_outlet_degree= obj[sKey.c_str()].GetFloat();
      }
    sKey = "dLatitude_outlet_degree";
    if (obj.HasMember(sKey.c_str()))
      {
        this->dLatitude_outlet_degree= obj[sKey.c_str()].GetFloat();
      }

    sKey = "dBreach_threshold";
    if (obj.HasMember(sKey.c_str()))
      {
        this->dBreach_threshold = obj[sKey.c_str()].GetFloat();
      }

    sKey = "dAccumulation_threshold";
    if (obj.HasMember(sKey.c_str()))
      {
        dValue = obj[sKey.c_str()].GetFloat();

        if (dValue >= 1.0)
          {
            //this is provided as the actual accumulation value;
            this->iFlag_accumulation_threshold_ratio = 0;
            this->dAccumulation_threshold_value = dValue;
          }
        else
          {
            //this is provided as a ratio
            this->iFlag_accumulation_threshold_ratio = 1;
            this->dAccumulation_threshold_ratio = dValue;
            if (this->dAccumulation_threshold_ratio < 0.0)
              {
                this->dAccumulation_threshold_ratio = 0.001;
              }
          }

      }

    sKey = "lBasinID";
    if (obj.HasMember(sKey.c_str()))
      {
        this->lBasinID= obj[sKey.c_str()].GetInt64();
      }

    sKey = "lCellID_outlet";
    if (obj.HasMember(sKey.c_str()))
      {
        this->lCellID_outlet= obj[sKey.c_str()].GetInt64();
      }


    return true;
  }

  bool basin::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> * writer) const
  {
    //writer->SetIndent('\n',2);
    writer->StartObject();

    writer->String("lBasinID");
    writer->Int64(lBasinID);

    writer->String("dLongitude_outlet_degree");
    writer->Double(dLongitude_outlet_degree);

    writer->String("dLatitude_outlet_degree");
    writer->Double(dLatitude_outlet_degree);

    writer->String("lCellID_outlet");
    writer->Int64(lCellID_outlet);

    writer->EndObject();

    return true;
  }
}
