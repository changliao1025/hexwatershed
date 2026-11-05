#pragma once

#include <iostream>
#include <list>

#include "basin.h"
namespace jsonmodel {
class multibasin : public JSONBase {
   public:
    // multibasin() {};
    virtual ~multibasin() {};
    virtual bool Deserialize(const std::string& s);

    std::list<basin> aBasin;

   public:
    virtual bool Deserialize(const rapidjson::Value& obj) { return false; };
    virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;

   private:
};
}  // namespace jsonmodel
