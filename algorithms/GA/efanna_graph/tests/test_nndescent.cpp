#include <efanna2e/index_graph.h>
#include <efanna2e/index_random.h>
#include <efanna2e/util.h>
#include <efanna2e/exp_data.h>


void load_data(char* filename, float*& data, unsigned& num,unsigned& dim){// load data with sift10K pattern
  std::ifstream in(filename, std::ios::binary);
  if(!in.is_open()){std::cout<<"open file error"<<std::endl;exit(-1);}
  in.read((char*)&dim,4);
  std::cout<<"data dimension: "<<dim<<std::endl;
  in.seekg(0,std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = (unsigned)(fsize / (dim+1) / 4);
  data = new float[num * dim * sizeof(float)];

  in.seekg(0,std::ios::beg);
  for(size_t i = 0; i < num; i++){
    in.seekg(4,std::ios::cur);
    in.read((char*)(data+i*dim),dim*4);
  }
  in.close();
}
int main(int argc, char** argv){
  if (argc != 2) {
    std::cout << "./test_nndescent dataset"
              << std::endl;
    exit(-1);
  }
  std::string dataset(argv[1]);
  //set data
  std::string base_path;
  std::string query_path;
  std::string ground_path;
  set_data_path(dataset, base_path, query_path, ground_path);
  std::string graph_file(dataset + "_knn.graph");

  unsigned points_num, dim;
  float* data_load = nullptr;
  load_data(&base_path[0], data_load, points_num, dim);

  data_load = efanna2e::data_align(data_load, points_num, dim);
  efanna2e::IndexRandom init_index(dim, points_num);
  efanna2e::IndexGraph index(dim, points_num, efanna2e::L2, (efanna2e::Index*)(&init_index));

  efanna2e::Parameters paras;
  set_para(dataset, paras);

  auto s = std::chrono::high_resolution_clock::now();
  index.Build(points_num, data_load, paras);
  auto e = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = e-s;
  std::cout <<"Build Time: "<< diff.count() << "\n";

  index.Save(&graph_file[0]);

  return 0;
}
