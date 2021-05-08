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

#include "n2/hnsw_node.h"

namespace n2 {

HnswNode::HnswNode(int id, const Data* data, int level, int maxsize, int maxsize0)
: id_(id), data_(data), level_(level), maxsize_(maxsize), maxsize0_(maxsize0) {
    friends_at_layer_.resize(level+1);
    for (int i = 1; i <= level; ++i) {
        friends_at_layer_[i].reserve(maxsize_ + 1);
    }
    friends_at_layer_[0].reserve(maxsize0_ + 1);
}

void HnswNode::CopyHigherLevelLinksToOptIndex(char* mem_offset, long long memory_per_node_higher_level, float r_proportion_, const float* range_) {
    char* mem_data = mem_offset;
    for (int level = 1; level <= level_; ++level) {
        CopyLinksToOptIndex(mem_data, level, r_proportion_, range_);
        mem_data += memory_per_node_higher_level;
    }
}

void HnswNode::CopyDataAndLevel0LinksToOptIndex(char* mem_offset, int higher_level_offset, int M0) {
    char* mem_data = mem_offset;
    *((int*)(mem_data)) = higher_level_offset;
    mem_data += sizeof(int);
    CopyLinksToOptIndex(mem_data, 0);
    mem_data += (sizeof(int) + sizeof(int)*M0); // "0" level doesn't guide
    auto& data = data_->GetData();
    for (int i = 0; i < data.size(); ++i) {
        *((float*)(mem_data)) = (float)data[i];
        mem_data += sizeof(float);
    }
}

void HnswNode::CopyLinksToOptIndex(char* mem_offset, int level) const {
    char* mem_data = mem_offset;
    const auto& neighbors = friends_at_layer_[level];
    *((int*)(mem_data)) = (int)(neighbors.size());
    mem_data += sizeof(int);
    for (size_t i = 0; i < neighbors.size(); ++i) {
        *((int*)(mem_data)) = (int)neighbors[i]->GetId();
        mem_data += sizeof(int);
    }
}

void HnswNode::CopyLinksToOptIndex(char* mem_offset, int level, float r_proportion_, const float* range_) {
    char* mem_data = mem_offset;
    int div_dim, l_size, c_size, r_size;
    auto& neighbors = friends_at_layer_[level];
    NeighborsTree(neighbors, div_dim, l_size, c_size, r_size, r_proportion_, range_);
    *((int*)(mem_data)) = (int)div_dim;
    mem_data += sizeof(int);
    *((int*)(mem_data)) = (int)l_size;
    mem_data += sizeof(int);
    *((int*)(mem_data)) = (int)c_size;
    mem_data += sizeof(int);
    *((int*)(mem_data)) = (int)r_size;
    mem_data += sizeof(int);
    for (int i = 0; i < neighbors.size(); ++i) {
        *((int*)(mem_data)) = (int)neighbors[i]->GetId();
        mem_data += sizeof(int);
    }
}

void HnswNode::NeighborsTree(std::vector<HnswNode*> & neighbors, int &div_dim, int &l_size, int &c_size, int &r_size, 
                                                                    float r_proportion_, const float* range_) {
    auto& data = data_->GetData();
    int dimension_ = data.size();
    int nsize = neighbors.size();
    long long min_diff =1e6, min_diff_dim = -1;
    for (int j = 0; j < dimension_; j++) {
      int lnum = 0, rnum = 0;
      for (int k = 0; k < nsize; k++) {
        auto& ndata = neighbors[k]->GetData();
        if (ndata[j] - data[j] < -r_proportion_* range_[j]) {
          lnum++;
        }
        else if (ndata[j] - data[j]  > r_proportion_ * range_[j]) {
          rnum++;
        }
      }
      long long diff = lnum - rnum;
      if (diff < 0) diff = -diff;
      if (diff < min_diff) {
        min_diff = diff;
        min_diff_dim = j;
      }
    }
    div_dim = min_diff_dim;
    std::vector<HnswNode*> l_tree, c_tree, r_tree;
    for (int k = 0; k < nsize; k++) {
      auto& ndata = neighbors[k]->GetData();
      if (ndata[div_dim] - data[div_dim] < -r_proportion_ * range_[div_dim]) {
        l_tree.push_back(neighbors[k]);
      }
      else if (ndata[div_dim] - data[div_dim]  > r_proportion_ * range_[div_dim]) {
        r_tree.push_back(neighbors[k]);
      }
      else {
        c_tree.push_back(neighbors[k]);
      }
    }
    l_size = l_tree.size(); c_size = c_tree.size(); r_size = r_tree.size();
    int k = 0;
    for (int s = 0; s < l_size && k < nsize; k++, s++) {
      neighbors[k] = l_tree[s];
    }
    for (int s = 0; s < c_size && k < nsize; k++, s++) {
      neighbors[k] = c_tree[s];
    }
    for (int s = 0; s < r_size && k < nsize; k++, s++) {
      neighbors[k] = r_tree[s];
    }
}

} // namespace n2
