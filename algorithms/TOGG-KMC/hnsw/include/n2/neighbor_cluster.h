// Copyright 2017 Kakao Corp. <http://www.kakaocorp.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <vector>
#include <mutex>
#include "hnsw_node.h"

#include "base.h"

namespace n2 {

class NeighborCluster {
public:
    explicit NeighborCluster(const size_t clusters, const size_t dimension, float **m_means);
    void NeighborsClusterPartition(std::vector<HnswNode*> &neighbors, std::vector<int> &cnum);

private:
    void Init(const std::vector<HnswNode*> & neighbors);
    void Cluster(const std::vector<HnswNode*> & neighbors, size_t *Label);
    float GetLabel(const float* x, int* label);
    float DistanceEva(const float* a, const float* b, const size_t d);

public:
    size_t clusters_ = 0;
    size_t dimension_ = 0;
    float **m_means_ = nullptr;
    size_t m_maxIterNum = 100;
    float m_endError = 0.001;
    
};

} // namespace n2
