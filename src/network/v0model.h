#ifndef V0_MODEL_H
#define V0_MODEL_H

#include <memory>

#include "../blowfish/defs.h"

#include "tensorflow/c/c_api.h"
#include <cppflow/cppflow.h>

class V0NetworkModel {
public : 
    V0NetworkModel() = default;
    V0NetworkModel(const std::string loadpath); 

    bool LoadFromCheckpoint(const std::string & path);


    auto operator() (cppflow::tensor inputtensor){
        m_assert(model_, "Model is not loaded, abort");
        return (*model_)({{"serving_default_my_input_1:0", inputtensor}}, 
                         {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});
    }

private:
    std::unique_ptr<cppflow::model> model_ = nullptr;


}; 



#endif