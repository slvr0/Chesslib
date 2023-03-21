#include "v0model.h"

V0NetworkModel::V0NetworkModel(const std::string loadpath) {
    LoadFromCheckpoint(loadpath);
}

bool V0NetworkModel::LoadFromCheckpoint(const std::string & path) {
    model_ = std::make_unique<cppflow::model> (path);

    return true;
}