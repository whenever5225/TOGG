//
// Created by 付聪 on 2017/6/21.
//

#include <chrono>

#include "index_random.h"
#include "index_ssg.h"
#include "util.h"

void save_result(char* filename, std::vector<std::vector<unsigned> >& results) {
  std::ofstream out(filename, std::ios::binary | std::ios::out);

  for (unsigned i = 0; i < results.size(); i++) {
    unsigned GK = (unsigned)results[i].size();
    out.write((char*)&GK, sizeof(unsigned));
    out.write((char*)results[i].data(), GK * sizeof(unsigned));
  }
  out.close();
}

float cal_recall(std::vector<std::vector<unsigned> > results, std::vector<std::vector<unsigned> > true_data, unsigned num, unsigned k) {
  float mean_acc = 0;
  for(size_t i = 0; i < num; i++) {
    float acc = 0;
    for(size_t j = 0; j < k; j++) {
      for(size_t m = 0; m < k; m++) {
        if(results[i][j] == true_data[i][m]) {
          acc++;
          break;
        }
      }
    }
    mean_acc += acc / k;
  }
  return mean_acc / num ;
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

int main(int argc, char** argv) {
  if (argc < 8) {
    std::cout << "./run data_file query_file groundtruth_file ssg_path L K result_path [seed]"
              << std::endl;
    exit(-1);
  }

  if (argc == 9) {
    unsigned seed = (unsigned)atoi(argv[8]);
    srand(seed);
    std::cerr << "Using Seed " << seed << std::endl;
  }

  std::cerr << "Data Path: " << argv[1] << std::endl;

  unsigned points_num, dim;
  float* data_load = nullptr;
  data_load = efanna2e::load_data(argv[1], points_num, dim);
  data_load = efanna2e::data_align(data_load, points_num, dim);

  std::cerr << "Query Path: " << argv[2] << std::endl;

  unsigned query_num, query_dim;
  float* query_load = nullptr;
  query_load = efanna2e::load_data(argv[2], query_num, query_dim);
  query_load = efanna2e::data_align(query_load, query_num, query_dim);

  assert(dim == query_dim);

  efanna2e::IndexRandom init_index(dim, points_num);
  efanna2e::IndexSSG index(dim, points_num, efanna2e::FAST_L2,
                           (efanna2e::Index*)(&init_index));

  // std::cerr << "SSG Path: " << argv[4] << std::endl;
  // std::cerr << "Result Path: " << argv[7] << std::endl;

  index.Load(argv[4]);
  index.OptimizeGraph(data_load);
  std::vector<std::vector<unsigned> > true_data;
  load_ivecs_data(argv[3], true_data);

  unsigned L = (unsigned)atoi(argv[5]);
  unsigned K = (unsigned)atoi(argv[6]);

  // std::cerr << "L = " << L << ", ";
  // std::cerr << "K = " << K << std::endl;

 std::ofstream out("testingL_result.txt");

  for (unsigned LL = 10199; LL < L; LL -= 1) {
    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", LL);
    std::vector<std::vector<unsigned> > res(query_num);
    for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
    index.InitDistCount();

    // Warm up
    for (int loop = 0; loop < 3; ++loop) {
      for (unsigned i = 0; i < 10; ++i) {
        index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
      }
    }
    for (unsigned i = 0; i < query_num; i++) {
      index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
    }
    size_t DistCount = index.GetDistCount();
    float recall = cal_recall(res, true_data, query_num, K);
    out << "L: " << LL << " " << "NDC: " << DistCount << " " << "recall@" << K << ": " << recall << std::endl;
  }
  out.close();

  return 0;
}
