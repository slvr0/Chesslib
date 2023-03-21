#ifndef V0_MODEL_H
#define V0_MODEL_H

#include <memory>

#include "tensorflow/c/c_api.h"
#include <cppflow/cppflow.h>


struct InputPlane  {
    uint64_t value_ = 0x0;
   

};


class V0NetworkModel {
public : 
    V0NetworkModel() = default;
    V0NetworkModel(const std::string loadpath); 

    bool LoadFromCheckpoint(const std::string & path);


    auto operator() (cppflow::tensor inputtensor){
        return (*model_)({{"serving_default_my_input_1:0", inputtensor}}, 
                         {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});
    }

private:
    std::unique_ptr<cppflow::model> model_ = nullptr;


}; 



#endif