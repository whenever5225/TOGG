
#include <set>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

#include "../../include/n2/hnsw.h"
#include "../../include/n2/exp_data.h"

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
    if (argc < 3 || argc > 5) {
      std::cout << "./evaluation dataset exc_type [P] [K] [L]"
                << std::endl;
      exit(-1);
    }
    std::string dataset(argv[1]);
    std::cout << "dataset: " << dataset << std::endl;
    std::string exc_type(argv[2]);
    //set data
    std::string base_path;
    std::string query_path;
    std::string ground_path;
    set_data_path(dataset, base_path, query_path, ground_path);
    std::string graph_file("hnsw_toggkdt_" + dataset + ".graph");

    unsigned points_num, dim;
    float* data_load = nullptr;
    data_load = load_data(&base_path[0], points_num, dim);
    float* range = NULL;
    get_range(data_load, dim, points_num, range);

    if (exc_type == "build") {
      float P = (float)atof(argv[3]);
      std::cout << "P: " << P << std::endl;
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
      vector<pair<string, string>> configs;
      set_para(dataset, configs);
      configs.push_back({"NumThread", "8"});
      index.SetConfigs(configs);
      auto s = std::chrono::high_resolution_clock::now();
      index.Fit();
      index.Partition_neighbors();
      auto e = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = e - s;
      std::cout << "Build Time: " << diff.count() << "\n";
      index.SaveModel(graph_file);
    }else if (exc_type == "search") {
      n2::Hnsw index2;
      index2.LoadModel(graph_file);

      unsigned query_num, query_dim;
      float* query_load = nullptr;
      query_load = load_data(&query_path[0], query_num, query_dim);
      assert(dim == query_dim);
      std::vector<std::vector<unsigned> > true_data;
      load_ivecs_data(&ground_path[0], true_data);
      index2.Load_range(range);

      unsigned K;
      std::string L_type("L_SEARCH_ASCEND");
      if (argc == 4) {
        K = (unsigned)atoi(argv[3]);
        std::cout << "K: " << K << std::endl;
      }else {
        K = 10;
      }
      if (L_type == "L_SEARCH_ASCEND") {
        unsigned L_st = 1;
        unsigned L_st2 = K;
        unsigned iter_num = 8;
        for (unsigned i = 0; i < iter_num; i++) {
          unsigned L = L_st + L_st2;
          L_st = L_st2;
          L_st2 = L;
          std::cout << "SEARCH_L : " << L << std::endl;
          if (L < K) {
            std::cout << "search_L cannot be smaller than search_K! " << std::endl;
            exit(-1);
          }
          index2.InitComparisonTimes();
          vector<vector<pair<int, float> > > result(query_num);
          auto s1 = std::chrono::high_resolution_clock::now();
          for (int i = 0; i < query_num; i++) {
            vector<float> q(query_dim);
            for (int j = 0; j < query_dim; j++) {
              q[j] = *(query_load + i * query_dim + j);
            }
            index2.SearchByVector(q, K, L, 2, result[i]);
          }
          auto e1 = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = e1 - s1;
          std::cout << "search time: " << diff.count() << "\n";
          size_t DistCount = index2.GetComparisonTimes();
          std::cout << "DistCount: " << DistCount << std::endl;
          float recall = cal_recall(result, true_data, query_num, K);
          std::cout << K << " NN accuracy: " << recall << std::endl;
          // if (recall > 0.99) break;
          // else if (i == iter_num - 1 && (float)(points_num * query_num / DistCount) > 10) iter_num++;
        }
      }
      else if (L_type == "L_SEARCH_SET_RECALL") {
        std::set<unsigned> visited;
        unsigned sg = 1000;
        float acc_set = 0.96;
        bool flag = false;
        int L_sl = 1;
        unsigned L = K;
        visited.insert(L);
        unsigned L_min = 0x7fffffff;
        while (true) {
          // std::cout << "SEARCH_L : " << L << std::endl;
          if (L < K) {
            std::cout << "search_L cannot be smaller than search_K! " << std::endl;
            exit(-1);
          }
          index2.InitComparisonTimes();
          vector<vector<pair<int, float> > > result(query_num);

// #pragma omp parallel for
          for (int i = 0; i < query_num; i++) {
            vector<float> q(query_dim);
            for (int j = 0; j < query_dim; j++) {
              q[j] = *(query_load + i * query_dim + j);
            }
            index2.SearchByVector(q, K, L, 2, result[i]);
          }

          float acc = cal_recall(result, true_data, query_num, K);
          if (acc_set - acc <= 0) {
              if (L_min > L) L_min = L;
              if (L == K || L_sl == 1) {
                  break;
              } else {
                  if (flag == false) {
                      L_sl < 0 ? L_sl-- : L_sl++;
                      flag = true;
                  }

                  L_sl /= 2;

                  if (L_sl == 0) {
                      break;
                  }
                  L_sl < 0 ? L_sl : L_sl = -L_sl;
              }
          }else {
              if (L_min < L) break;
              L_sl = (int)(sg * (acc_set - acc));
              if (L_sl == 0) L_sl++;
              flag = false;
          }
          L += L_sl;
          if (visited.count(L)) {
              break;
          }else {
              visited.insert(L);
          }
        }
        std::cout << "L_min: " << L_min << std::endl;
      }
      else if (L_type == "L_SEARCH_ASSIGN") {
        unsigned L = (unsigned)atoi(argv[4]);
        std::cout << "SEARCH_L : " << L << std::endl;
        if (L < K) {
          std::cout << "search_L cannot be smaller than search_K! " << std::endl;
          exit(-1);
        }
        index2.InitComparisonTimes();
        vector<vector<pair<int, float> > > result(query_num);
        auto s1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < query_num; i++) {
          vector<float> q(query_dim);
          for (int j = 0; j < query_dim; j++) {
            q[j] = *(query_load + i * query_dim + j);
          }
          index2.SearchByVector(q, K, L, 2, result[i]);
        }
        auto e1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = e1 - s1;
        std::cout << "search time: " << diff.count() << "\n";
        size_t DistCount = index2.GetComparisonTimes();
        std::cout << "DistCount: " << DistCount << std::endl;
        float recall = cal_recall(result, true_data, query_num, K);
        std::cout << K << " NN accuracy: " << recall << std::endl;
      }
    }
    
    return 0;
}