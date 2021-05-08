
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "../../include/n2/hnsw.h"

using namespace std;

struct recall_speedup {
  float r, s;
    recall_speedup() = default;
    recall_speedup(float r, float s) : r{r}, s{s} {}

    inline bool operator<(const recall_speedup &other) const {
        return r < other.r;
    }
};

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

void load_ivecs_data(const char* filename, std::vector<std::vector<unsigned> >& results) {
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open()) {
    std::cout << "open file error" << std::endl;
    exit(-1);
  }
  unsigned dim, num;
  in.read((char*)&dim, 4);
  //std::cout<<"data dimension: "<<dim<<std::endl;
  in.seekg(0, std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = (unsigned)(fsize / (dim + 1) / 4);
  results.resize(num);
  for (unsigned i = 0; i < num; i++) results[i].resize(dim);

  in.seekg(0, std::ios::beg);
  for (size_t i = 0; i < num; i++) {
    in.seekg(4, std::ios::cur);
    in.read((char*)results[i].data(), dim * 4);
  }
  in.close();
}

float cal_recall(std::vector<vector<pair<int, float> > > results, std::vector<std::vector<unsigned> > true_data, unsigned num, int k) {
  float mean_acc = 0;
  for(int i = 0; i < num; i++) {
    float acc = 0;
    for(int j = 0; j < k; j++) {
      for(int m = 0; m < k; m++) {
        if((unsigned)results[i][j].first == true_data[i][m]) {
          acc++;
          break;
        }
      }
    }
    mean_acc += acc / k;
  }
  return mean_acc / num ;
}

void get_range(const float* data, unsigned dim, unsigned num, float* &range) {
  float *max = new float[(size_t)dim];
  float *min = new float[(size_t)dim];
  range = new float[(size_t)dim];
  for(size_t i = 0; i < dim; i++) { //初始化为第一行数据
    max[i] = min[i] = data[i];
  }
  for(size_t i = 0; i < dim; i++) {
    for(size_t j = i; j < num * dim; j += dim) {
      if(max[i] < data[j]) max[i] = data[j];
      if(min[i] > data[j]) min[i] = data[j];
    }
  }
  for (size_t i = 0; i < dim; i++) {
      range[i] = max[i] - min[i];
  }
}

int main(int argc, char** argv)
{
    if (argc < 12) {
        std::cout << "./run data_file query_file groundtruth_file hnsw_path M MaxM0 efConstruction K L P test_result"
                << std::endl;
        exit(-1);
    }

    unsigned points_num, dim;
    float* data_load = nullptr;
    data_load = load_data(argv[1], points_num, dim);
    float* range = NULL;
    get_range(data_load, dim, points_num, range);

    float P = (float)atof(argv[10]);

    n2::Hnsw index(dim, "L2");
    index.Load_range(range);
    index.InitRangeProportion(P);
    for(int i=0; i < points_num; ++i){
        vector<float> v(dim);
        for (int j = 0; j < dim; j++) {
            v[j] = *(data_load + i * dim + j);
        }
        index.AddData(v);
    }
    vector<pair<string, string>>
        configs = {{"M", argv[5]}, {"MaxM0", argv[6]}, {"NumThread", "32"}, {"efConstruction", argv[7]}};
    index.SetConfigs(configs);
    auto s = std::chrono::high_resolution_clock::now();
    index.Fit();
    index.Partition_neighbors();
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    std::cout << "Build Time: " << diff.count() << "\n";
    index.SaveModel(argv[4]);


    n2::Hnsw index2;
    index2.LoadModel(argv[4]);

    unsigned query_num, query_dim;
    float* query_load = nullptr;
    query_load = load_data(argv[2], query_num, query_dim);
    assert(dim == query_dim);
    std::vector<std::vector<unsigned> > true_data;
    load_ivecs_data(argv[3], true_data);

    int k = (int)atoi(argv[8]);
    int L = (int)atoi(argv[9]);
    std::ofstream out(argv[11]);
    index2.Load_range(range);
    // std::vector <recall_speedup> rs; 
    for (int LL = 6100; LL <= L; LL -= 1) {
      index2.InitComparisonTimes();
      vector<vector<pair<int, float> > > result(query_num);
      for (int i = 0; i < query_num; i++) {
        vector<float> q(query_dim);
        for (int j = 0; j < query_dim; j++) {
          q[j] = *(query_load + i * query_dim + j);
        }
        index2.SearchByVector(q, k, LL, 2, result[i]);
      }
      size_t DistCount = index2.GetComparisonTimes();
      // float speedup = (float)(points_num * query_num) / (float)DistCount;
      float acc = cal_recall(result, true_data, query_num, k);
      // std::cout << "speedup: " << speedup << " " << "acc: " << acc << "\n";
      // rs.push_back(recall_speedup(acc, speedup));
      out << "L: " << LL<< " " << "NDC: " << DistCount << " " << "recall@" << k << ": " << acc << std::endl;
    }
    // float cur_perf = evaluate_performance(rs, 0.95, 0.99);
    // std::cout << "Current Performance Value: " << cur_perf << "\n";
    out.close();
    
    return 0;
}
