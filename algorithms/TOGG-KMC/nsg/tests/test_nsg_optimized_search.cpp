//
// Created by 付聪 on 2017/6/21.
//

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>

void load_data(char* filename, float*& data, unsigned& num,
               unsigned& dim) {  // load data with sift10K pattern
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open()) {
    std::cout << "open file error" << std::endl;
    exit(-1);
  }
  in.read((char*)&dim, 4);
  // std::cout<<"data dimension: "<<dim<<std::endl;
  in.seekg(0, std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = (unsigned)(fsize / (dim + 1) / 4);
  data = new float[(size_t)num * (size_t)dim];

  in.seekg(0, std::ios::beg);
  for (size_t i = 0; i < num; i++) {
    in.seekg(4, std::ios::cur);
    in.read((char*)(data + i * dim), dim * 4);
  }
  in.close();
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

void save_result(const char* filename,
                 std::vector<std::vector<unsigned> >& results) {
  std::ofstream out(filename, std::ios::binary | std::ios::out);

  for (unsigned i = 0; i < results.size(); i++) {
    unsigned GK = (unsigned)results[i].size();
    out.write((char*)&GK, sizeof(unsigned));
    out.write((char*)results[i].data(), GK * sizeof(unsigned));
  }
  out.close();
}
int main(int argc, char** argv) {
  if (argc != 8) {
    std::cout << argv[0]
              << " data_file query_file groundtruth_file nsg_path search_L search_K result_path"
              << std::endl;
    exit(-1);
  }
  float* data_load = NULL;
  unsigned points_num, dim;
  load_data(argv[1], data_load, points_num, dim);
  float* query_load = NULL;
  unsigned query_num, query_dim;
  load_data(argv[2], query_load, query_num, query_dim);
  assert(dim == query_dim);

  unsigned L = (unsigned)atoi(argv[5]);
  unsigned K = (unsigned)atoi(argv[6]);

  if (L < K) {
    std::cout << "search_L cannot be smaller than search_K!" << std::endl;
    exit(-1);
  }

  data_load = efanna2e::data_align(data_load, points_num, dim);
  query_load = efanna2e::data_align(query_load, query_num, query_dim);
  efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
  index.Load(argv[4]);
  index.OptimizeGraph(data_load);
  std::vector<std::vector<unsigned> > true_data;
  load_ivecs_data(argv[3], true_data);

   std::ofstream out("testingL_result.txt");

  for (unsigned LL = K + 1; LL < L; LL += 1) {
    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", LL);
    paras.Set<unsigned>("P_search", LL);
    std::vector<std::vector<unsigned> > res(query_num);
    for (unsigned i = 0; i < query_num; i++) res[i].resize(K);
    index.InitDistCount();

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
