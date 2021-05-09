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

#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "n2/hnsw.h"

using namespace std;

float* load_data(char* filename, unsigned& num, unsigned& dim) {
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open()) {
    std::cerr << "Open file error" << std::endl;
    exit(-1);
  }

  in.read((char*)&dim, 4);

  in.seekg(0, std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = (unsigned)(fsize / (dim + 1) / 4);

  float* data = new float[(size_t)num * (size_t)dim];

  in.seekg(0, std::ios::beg);
  for (size_t i = 0; i < num; i++) {
    in.seekg(4, std::ios::cur);
    in.read((char*)(data + i * dim), dim * sizeof(float));
  }
  in.close();

  return data;
}

int main(int argc, char** argv)
{
    if (argc < 8) {
        std::cout << "./run data_file query_file groundtruth_file hnsw_path M MaxM0 efConstruction"
                << std::endl;
        exit(-1);
    }
    random_device rd;
    mt19937 mt(rd());
    normal_distribution<double> dist(0.0, 1.0);
    unsigned points_num, dim;
    float* data_load = nullptr;
    load_data(argv[1], points_num, dim);

    n2::Hnsw index(dim, "L2");
    for(int i=0; i < points_num; ++i){
        vector<float> v(dim);
        memcpy(v.data(), data_load + i * dim, sizeof(dim));
        index.AddData(v);
    }
    vector<pair<string, string>>
        configs = {{"M", "5"}, {"MaxM0", "10"}, {"NumThread", "4"}};
    index.SetConfigs(configs);
    index.Fit();
    index.SaveModel("test.n2");

    n2::Hnsw otherway(F, "L2");
    for(int i=0; i < 1000; ++i){
        vector<float> v(F);
        generate(v.begin(), v.end(), [&mt, &dist] { return dist(mt); });
        otherway.AddData(v);
    }
    otherway.Build(5, 10, -1, 4);
    n2::Hnsw index2;
    index2.LoadModel("test.n2");
    int search_id = 1,
        k = 3;
    vector<pair<int, float>> result;
    index2.SearchById(search_id, k, -1, result);
    cout << "[SearchById]: K-NN for " << search_id << " ";
    for(auto ret : result){
        cout << "(" << ret.first << "," << ret.second << ") ";
    }
    cout << endl;

    vector<float> v(F);
    generate(v.begin(), v.end(), [&mt, &dist] { return dist(mt); });
    index2.SearchByVector(v, k, -1, result);
    cout << "[SearchByVector]: K-NN for [";
    for(auto e : v){
        cout << e << ",main";
    }
    cout << "] ";
    for(auto ret : result){
        cout << "(" << ret.first << "," << ret.second << ") ";
    }
    cout << endl;

    return 0;
}
