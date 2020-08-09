//
// Created by 付聪 on 2017/6/21.
//

#include "index_random.h"
#include "index_ssg.h"
#include "util.h"

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

int main(int argc, char** argv) {
  if (argc < 8) {
    std::cout << "./run data_file nn_graph_path L R Angle P save_graph_file [seed]"
              << std::endl;
    exit(-1);
  }

  if (argc == 9) {
    unsigned seed = (unsigned)atoi(argv[8]);
    srand(seed);
    std::cout << "Using Seed " << seed << std::endl;
  }

  std::cerr << "Data Path: " << argv[1] << std::endl;

  unsigned points_num, dim;
  float* data_load = nullptr;
  data_load = efanna2e::load_data(argv[1], points_num, dim);
  data_load = efanna2e::data_align(data_load, points_num, dim);
  float* range = NULL;
  get_range(data_load, dim, points_num, range);

  std::string nn_graph_path(argv[2]);
  unsigned L = (unsigned)atoi(argv[3]);
  unsigned R = (unsigned)atoi(argv[4]);
  float A = (float)atof(argv[5]);
  float P = (float)atof(argv[6]);

  std::cout << "L = " << L << ", ";
  std::cout << "R = " << R << ", ";
  std::cout << "Angle = " << A << std::endl;
  std::cout << "P = " << P << std::endl;
  std::cout << "KNNG = " << nn_graph_path << std::endl;

  efanna2e::IndexRandom init_index(dim, points_num);
  efanna2e::IndexSSG index(dim, points_num, efanna2e::L2,
                           (efanna2e::Index*)(&init_index));
  index.Load_range(range);
  index.InitRangeProportion(P);

  efanna2e::Parameters paras;
  paras.Set<unsigned>("L", L);
  paras.Set<unsigned>("R", R);
  paras.Set<float>("A", A);
  paras.Set<unsigned>("n_try", 10);
  paras.Set<std::string>("nn_graph_path", nn_graph_path);

  std::cerr << "Output SSG Path: " << argv[7] << std::endl;

  auto s = std::chrono::high_resolution_clock::now();
  index.Build(points_num, data_load, paras);
  auto e = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = e - s;
  std::cout << "Build Time: " << diff.count() << "\n";

  index.Save(argv[7]);

  return 0;
}
