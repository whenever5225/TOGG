#include "index_ssg.h"

#include <omp.h>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <boost/dynamic_bitset.hpp>
#include <chrono>
#include <cmath>
#include <queue>
#include <set>

#include "exceptions.h"
#include "parameters.h"

constexpr double kPi = std::acos(-1);

namespace efanna2e {

#define _CONTROL_NUM 100

IndexSSG::IndexSSG(const size_t dimension, const size_t n, Metric m,
                   Index *initializer)
    : Index(dimension, n, m), initializer_{initializer} {}

IndexSSG::~IndexSSG() {}

void IndexSSG::InitClusterPara(const size_t cl_num) {
  cluster_num = cl_num;
  m_means = new float*[cluster_num];
  for (size_t i = 0; i < cluster_num; i++) {
    m_means[i] = new float[dimension_];
    std::memset(m_means[i], 0, sizeof(float) * dimension_);
  }
  m_initMode = InitRandom;
  m_maxIterNum = 100;
  m_endError = 0.001;
}
void IndexSSG::Save(const char *filename) {
  std::ofstream out(filename, std::ios::binary | std::ios::out);
  assert(nc_.size() == nd_);

  out.write((char *)&width, sizeof(unsigned));
  unsigned n_ep=eps_.size();
  out.write((char *)&n_ep, sizeof(unsigned));
  out.write((char *)eps_.data(), n_ep*sizeof(unsigned));
  for (unsigned i = 0; i < nd_; i++) {
    unsigned cluster_num_ = nc_[i].size();
    out.write((char *)&cluster_num_, sizeof(unsigned));
    for (unsigned j = 0; j < cluster_num_; j++) {
      unsigned cnum = nc_[i][j].size();
      out.write((char *)&cnum, sizeof(unsigned));
      out.write((char *)nc_[i][j].data(), cnum * sizeof(unsigned));
    }
  }
  out.close();
}

void IndexSSG::Load(const char *filename) {
  std::ifstream in(filename, std::ios::binary);
  in.read((char *)&width, sizeof(unsigned));
  unsigned n_ep=0;
  in.read((char *)&n_ep, sizeof(unsigned));
  eps_.resize(n_ep);
  in.read((char *)eps_.data(), n_ep*sizeof(unsigned));
  // width=100;
  // unsigned cc = 0;
  std::vector < std::vector <unsigned> > tmp;
  bool flag = true;
  while (!in.eof()) {
   unsigned cluster_num_;
    in.read((char *)&cluster_num_, sizeof(unsigned));
    if (flag && cluster_num_ > 1){
      cluster_num = cluster_num_;
      flag = false;
    }
    tmp.resize(cluster_num_);
    for (unsigned j = 0; j < cluster_num_; j++) {
      unsigned cnum;
      in.read((char *)&cnum, sizeof(unsigned));
      tmp[j].resize(cnum);
      in.read((char *)tmp[j].data(), cnum * sizeof(unsigned));
    }
    nc_.push_back(tmp);
  }
  // cc /= nd_;
  // std::cerr << "Average Degree = " << cc << std::endl;
}

void IndexSSG::Load_nn_graph(const char *filename) {
  std::ifstream in(filename, std::ios::binary);
  unsigned k;
  in.read((char *)&k, sizeof(unsigned));
  in.seekg(0, std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  size_t num = (unsigned)(fsize / (k + 1) / 4);
  in.seekg(0, std::ios::beg);

  final_graph_.resize(num);
  final_graph_.reserve(num);
  unsigned kk = (k + 3) / 4 * 4;
  for (size_t i = 0; i < num; i++) {
    in.seekg(4, std::ios::cur);
    final_graph_[i].resize(k);
    final_graph_[i].reserve(kk);
    in.read((char *)final_graph_[i].data(), k * sizeof(unsigned));
  }
  in.close();
}

void IndexSSG::get_neighbors(const unsigned q, const Parameters &parameter,
                             std::vector<Neighbor> &pool) {
  boost::dynamic_bitset<> flags{nd_, 0};
  unsigned L = parameter.Get<unsigned>("L");
  flags[q] = true;
  for (unsigned i = 0; i < final_graph_[q].size(); i++) {
    unsigned nid = final_graph_[q][i];
    for (unsigned nn = 0; nn < final_graph_[nid].size(); nn++) {
      unsigned nnid = final_graph_[nid][nn];
      if (flags[nnid]) continue;
      flags[nnid] = true;
      float dist = distance_->compare(data_ + dimension_ * q,
                                      data_ + dimension_ * nnid, dimension_);
      pool.push_back(Neighbor(nnid, dist, true));
      if (pool.size() >= L) break;
    }
    if (pool.size() >= L) break;
  }
}

void IndexSSG::get_neighbors(const float *query, const Parameters &parameter,
                             std::vector<Neighbor> &retset,
                             std::vector<Neighbor> &fullset) {
  unsigned L = parameter.Get<unsigned>("L");

  retset.resize(L + 1);
  std::vector<unsigned> init_ids(L);
  // initializer_->Search(query, nullptr, L, parameter, init_ids.data());
  std::mt19937 rng(rand());
  GenRandom(rng, init_ids.data(), L, (unsigned)nd_);

  boost::dynamic_bitset<> flags{nd_, 0};
  L = 0;
  for (unsigned i = 0; i < init_ids.size(); i++) {
    unsigned id = init_ids[i];
    if (id >= nd_) continue;
    // std::cout<<id<<std::endl;
    float dist = distance_->compare(data_ + dimension_ * (size_t)id, query,
                                    (unsigned)dimension_);
    retset[i] = Neighbor(id, dist, true);
    flags[id] = 1;
    L++;
  }

  std::sort(retset.begin(), retset.begin() + L);
  int k = 0;
  while (k < (int)L) {
    int nk = L;

    if (retset[k].flag) {
      retset[k].flag = false;
      unsigned n = retset[k].id;

      for (unsigned m = 0; m < final_graph_[n].size(); ++m) {
        unsigned id = final_graph_[n][m];
        if (flags[id]) continue;
        flags[id] = 1;

        float dist = distance_->compare(query, data_ + dimension_ * (size_t)id,
                                        (unsigned)dimension_);
        Neighbor nn(id, dist, true);
        fullset.push_back(nn);
        if (dist >= retset[L - 1].distance) continue;
        int r = InsertIntoPool(retset.data(), L, nn);

        if (L + 1 < retset.size()) ++L;
        if (r < nk) nk = r;
      }
    }
    if (nk <= k)
      k = nk;
    else
      ++k;
  }
}

void IndexSSG::init_graph(const Parameters &parameters) {
  float *center = new float[dimension_];
  for (unsigned j = 0; j < dimension_; j++) center[j] = 0;
  for (unsigned i = 0; i < nd_; i++) {
    for (unsigned j = 0; j < dimension_; j++) {
      center[j] += data_[i * dimension_ + j];
    }
  }
  for (unsigned j = 0; j < dimension_; j++) {
    center[j] /= nd_;
  }
  std::vector<Neighbor> tmp, pool;
  // ep_ = rand() % nd_;  // random initialize navigating point
  get_neighbors(center, parameters, tmp, pool);
  ep_ = tmp[0].id;  // For Compatibility
}

void IndexSSG::sync_prune(unsigned q, std::vector<Neighbor> &pool,
                          const Parameters &parameters, float threshold,
                          SimpleNeighbor *cut_graph_) {
  unsigned range = parameters.Get<unsigned>("R");
  width = range;
  unsigned start = 0;

  boost::dynamic_bitset<> flags{nd_, 0};
  for (unsigned i = 0; i < pool.size(); ++i) {
    flags[pool[i].id] = 1;
  }
  for (unsigned nn = 0; nn < final_graph_[q].size(); nn++) {
    unsigned id = final_graph_[q][nn];
    if (flags[id]) continue;
    float dist = distance_->compare(data_ + dimension_ * (size_t)q,
                                    data_ + dimension_ * (size_t)id,
                                    (unsigned)dimension_);
    pool.push_back(Neighbor(id, dist, true));
  }

  std::sort(pool.begin(), pool.end());
  std::vector<Neighbor> result;
  if (pool[start].id == q) start++;
  result.push_back(pool[start]);

  while (result.size() < range && (++start) < pool.size()) {
    auto &p = pool[start];
    bool occlude = false;
    for (unsigned t = 0; t < result.size(); t++) {
      if (p.id == result[t].id) {
        occlude = true;
        break;
      }
      float djk = distance_->compare(data_ + dimension_ * (size_t)result[t].id,
                                     data_ + dimension_ * (size_t)p.id,
                                     (unsigned)dimension_);
      float cos_ij = (p.distance + result[t].distance - djk) / 2 /
                     sqrt(p.distance * result[t].distance);
      if (cos_ij > threshold) {
        occlude = true;
        break;
      }
    }
    if (!occlude) result.push_back(p);
  }

  SimpleNeighbor *des_pool = cut_graph_ + (size_t)q * (size_t)range;
  for (size_t t = 0; t < result.size(); t++) {
    des_pool[t].id = result[t].id;
    des_pool[t].distance = result[t].distance;
  }
  if (result.size() < range) {
    des_pool[result.size()].distance = -1;
  }
}

void IndexSSG::InterInsert(unsigned n, unsigned range, float threshold,
                           std::vector<std::mutex> &locks,
                           SimpleNeighbor *cut_graph_) {
  SimpleNeighbor *src_pool = cut_graph_ + (size_t)n * (size_t)range;
  for (size_t i = 0; i < range; i++) {
    if (src_pool[i].distance == -1) break;

    SimpleNeighbor sn(n, src_pool[i].distance);
    size_t des = src_pool[i].id;
    SimpleNeighbor *des_pool = cut_graph_ + des * (size_t)range;

    std::vector<SimpleNeighbor> temp_pool;
    int dup = 0;
    {
      LockGuard guard(locks[des]);
      for (size_t j = 0; j < range; j++) {
        if (des_pool[j].distance == -1) break;
        if (n == des_pool[j].id) {
          dup = 1;
          break;
        }
        temp_pool.push_back(des_pool[j]);
      }
    }
    if (dup) continue;

    temp_pool.push_back(sn);
    if (temp_pool.size() > range) {
      std::vector<SimpleNeighbor> result;
      unsigned start = 0;
      std::sort(temp_pool.begin(), temp_pool.end());
      result.push_back(temp_pool[start]);
      while (result.size() < range && (++start) < temp_pool.size()) {
        auto &p = temp_pool[start];
        bool occlude = false;
        for (unsigned t = 0; t < result.size(); t++) {
          if (p.id == result[t].id) {
            occlude = true;
            break;
          }
          float djk = distance_->compare(
              data_ + dimension_ * (size_t)result[t].id,
              data_ + dimension_ * (size_t)p.id, (unsigned)dimension_);
          float cos_ij = (p.distance + result[t].distance - djk) / 2 /
                         sqrt(p.distance * result[t].distance);
          if (cos_ij > threshold) {
            occlude = true;
            break;
          }
        }
        if (!occlude) result.push_back(p);
      }
      {
        LockGuard guard(locks[des]);
        for (unsigned t = 0; t < result.size(); t++) {
          des_pool[t] = result[t];
        }
        if (result.size() < range) {
          des_pool[result.size()].distance = -1;
        }
      }
    } else {
      LockGuard guard(locks[des]);
      for (unsigned t = 0; t < range; t++) {
        if (des_pool[t].distance == -1) {
          des_pool[t] = sn;
          if (t + 1 < range) des_pool[t + 1].distance = -1;
          break;
        }
      }
    }
  }
}

void IndexSSG::Link(const Parameters &parameters, SimpleNeighbor *cut_graph_) {
  /*
  std::cerr << "Graph Link" << std::endl;
  unsigned progress = 0;
  unsigned percent = 100;
  unsigned step_size = nd_ / percent;
  std::mutex progress_lock;
  */
  unsigned range = parameters.Get<unsigned>("R");
  std::vector<std::mutex> locks(nd_);

  float angle = parameters.Get<float>("A");
  float threshold = std::cos(angle / 180 * kPi);

#pragma omp parallel
  {
    // unsigned cnt = 0;
    std::vector<Neighbor> pool, tmp;
#pragma omp for schedule(dynamic, 100)
    for (unsigned n = 0; n < nd_; ++n) {
      pool.clear();
      tmp.clear();
      get_neighbors(n, parameters, pool);
      sync_prune(n, pool, parameters, threshold, cut_graph_);
      /*
      cnt++;
      if (cnt % step_size == 0) {
        LockGuard g(progress_lock);
        std::cout << progress++ << "/" << percent << " completed" << std::endl;
      }
      */
    }

#pragma omp for schedule(dynamic, 100)
    for (unsigned n = 0; n < nd_; ++n) {
      InterInsert(n, range, threshold, locks, cut_graph_);
    }
  }
}

void IndexSSG::Build(size_t n, const float *data,
                     const Parameters &parameters) {
  std::string nn_graph_path = parameters.Get<std::string>("nn_graph_path");
  unsigned range = parameters.Get<unsigned>("R");
  Load_nn_graph(nn_graph_path.c_str());
  data_ = data;
  init_graph(parameters);
  SimpleNeighbor *cut_graph_ = new SimpleNeighbor[nd_ * (size_t)range];
  Link(parameters, cut_graph_);
  final_graph_.resize(nd_);

  for (size_t i = 0; i < nd_; i++) {
    SimpleNeighbor *pool = cut_graph_ + i * (size_t)range;
    unsigned pool_size = 0;
    for (unsigned j = 0; j < range; j++) {
      if (pool[j].distance == -1) {
        break;
      }
      pool_size = j;
    }
    ++pool_size;
    final_graph_[i].resize(pool_size);
    for (unsigned j = 0; j < pool_size; j++) {
      final_graph_[i][j] = pool[j].id;
    }
  }

  DFS_expand(parameters);
  nc_.resize(nd_);
  SetInitMode(IndexSSG::InitUniform);
  unsigned max, min, avg;
  max = 0;
  min = nd_;
  avg = 0;
  for (size_t i = 0; i < nd_; i++) {
    auto size = final_graph_[i].size();
    if (size > cluster_num) {
      size_t* labels = new size_t[size];
      Cluster(final_graph_[i].data(), size, labels);

      
      std::set <size_t> mset;
      for (size_t u = 0; u < size; u++) {
        mset.insert(labels[u]);
      }
      size_t tmp_c_num = mset.size();
      nc_[i].resize(tmp_c_num);
    
      std::set <size_t> :: iterator it = mset.begin();
      for (size_t j = 0; j < tmp_c_num || it != mset.end(); j++) {  //选取数据集中距离类中心最近的点作为新的类中心
        float min_dist = -1;
        unsigned* new_cluster_center = new unsigned[cluster_num];
        std::memset(new_cluster_center, 0, sizeof(unsigned) * cluster_num);
        for (size_t k = 0; k < size; k++) {
          if (labels[k] == *it) {
            float temp = distance_ -> compare(data_ + final_graph_[i][k] * dimension_, m_means[*it], dimension_);
            if (temp < min_dist || min_dist == -1) {
              min_dist = temp;
              new_cluster_center[*it] = final_graph_[i][k];
            }
          }
        }
        nc_[i][j].push_back(new_cluster_center[*it]); //新类中心作为该类的第一个元素
        if (j != *it) {
          for (size_t u = 0; u < size; u++) {
            if (labels[u] == *it) {
              labels[u] = j;
            }
          }
        }

        delete new_cluster_center;
        mset.erase(it++);
      }
      // if (i == 952837) {
      //   std::cout << "ni30: " << nc_[i][3][0] << std::endl;
      // }
      for (size_t j = 0; j < size; j++) {
        if (nc_[i][labels[j]][0] != final_graph_[i][j]) {
          nc_[i][labels[j]].push_back(final_graph_[i][j]);
        }
      }
      delete labels;
    }
    else {  //邻居个数不大于聚类个数，所有邻居共同组成一个类
      nc_[i].resize(1);
      for (size_t j = 0; j < size; j++) {
        nc_[i][0].push_back(final_graph_[i][j]);
      }
    }
    // else {  //邻居个数不大于聚类个数，直接划分为邻居个数个类，每个类放一个邻居
    //   nc_[i].resize(size);
    //   for (size_t j = 0; j < size; j++) {
    //     nc_[i][j].push_back(final_graph_[i][j]);
    //   }ssh-keygen -f "/home/tdlab/.ssh/known_hosts:3" -R "[202.107.204.57]:20002"
    // }
    max = max < size ? size : max;
    min = min > size ? size : min;
    avg += size;
  }
  avg /= 1.0 * nd_;
  printf("Degree Statistics: Max = %d, Min = %d, Avg = %d\n",
         max, min, avg);

  /* Buggy!
  strong_connect(parameters);

  max = 0;
  min = nd_;
  avg = 0;
  for (size_t i = 0; i < nd_; i++) {
    auto size = final_graph_[i].size();
    max = max < size ? size : max;
    min = min > size ? size : min;
    avg += size;
  }
  avg /= 1.0 * nd_;
  printf("Degree Statistics(After TreeGrow): Max = %d, Min = %d, Avg = %d\n",
         max, min, avg);
  */

  has_built = true;
}

void IndexSSG::Search(const float *query, const float *x, size_t K,
                      const Parameters &parameters, unsigned *indices) {
  const unsigned L = parameters.Get<unsigned>("L_search");
  data_ = x;
  std::vector<Neighbor> retset(L + 1);
  std::vector<unsigned> init_ids(L);
  boost::dynamic_bitset<> flags{nd_, 0};
  std::mt19937 rng(rand());
  GenRandom(rng, init_ids.data(), L, (unsigned)nd_);
  // assert(eps_.size() < L);
  for(unsigned i=0; i<eps_.size() && i < L; i++){
    init_ids[i] = eps_[i];
  }

  for (unsigned i = 0; i < L; i++) {
    unsigned id = init_ids[i];
    float dist = distance_->compare(data_ + dimension_ * id, query,
                                    (unsigned)dimension_);
    retset[i] = Neighbor(id, dist, true);
    flags[id] = true;
  }

  std::sort(retset.begin(), retset.begin() + L);
  int k = 0;
  while (k < (int)L) {
    int nk = L;

    if (retset[k].flag) {
      retset[k].flag = false;
      unsigned n = retset[k].id;

      for (unsigned m = 0; m < final_graph_[n].size(); ++m) {
        unsigned id = final_graph_[n][m];
        if (flags[id]) continue;
        flags[id] = 1;
        float dist = distance_->compare(query, data_ + dimension_ * id,
                                        (unsigned)dimension_);
        if (dist >= retset[L - 1].distance) continue;
        Neighbor nn(id, dist, true);
        int r = InsertIntoPool(retset.data(), L, nn);

        if (r < nk) nk = r;
      }
    }
    if (nk <= k)
      k = nk;
    else
      ++k;
  }
  for (size_t i = 0; i < K; i++) {
    indices[i] = retset[i].id;
  }
}

void IndexSSG::SearchWithOptGraph(const float *query, size_t K,
                                  const Parameters &parameters,
                                  unsigned *indices) {
  unsigned L = parameters.Get<unsigned>("L_search");
  DistanceFastL2 *dist_fast = (DistanceFastL2 *)distance_;

  std::vector<Neighbor> retset(L + 1);
  std::vector<unsigned> init_ids(L);
  std::mt19937 rng(rand());
  GenRandom(rng, init_ids.data(), L, (unsigned)nd_);
  // assert(eps_.size() <= L);
  for(unsigned i=0; i<eps_.size() && i < L; i++){
    init_ids[i] = eps_[i];
  }

  boost::dynamic_bitset<> flags{nd_, 0};
  for (unsigned i = 0; i < init_ids.size(); i++) {
    unsigned id = init_ids[i];
    if (id >= nd_) continue;
    _mm_prefetch(opt_graph_ + node_size * id, _MM_HINT_T0);
  }
  L = 0;
  for (unsigned i = 0; i < init_ids.size(); i++) {
    unsigned id = init_ids[i];
    if (id >= nd_) continue;
    float *x = (float *)(opt_graph_ + node_size * id);
    float norm_x = *x;
    x++;
    float dist = dist_fast->compare(x, query, norm_x, (unsigned)dimension_);
    dist_cout++;
    retset[i] = Neighbor(id, dist, true);
    flags[id] = true;
    L++;
  }
  // std::cout<< "test1" <<std::endl;

  std::sort(retset.begin(), retset.begin() + L);
  int k = 0;
  while (k < (int)L) {
    int nk = L;

    if (retset[k].flag) {
      retset[k].flag = false;
      unsigned n = retset[k].id;
      // std::cout << "n: " << n << std::endl;

      _mm_prefetch(opt_graph_ + node_size * n + data_len, _MM_HINT_T0);
      unsigned *neighbors = (unsigned *)(opt_graph_ + node_size * n + data_len);
      // std::cout << "ntest1\n";
      unsigned cluster_num_= *neighbors;
      neighbors++;
      unsigned *cnum = new unsigned[cluster_num_];
      for (size_t i = 0; i < cluster_num_; i++) { //每个类的邻居数
        // std::cout << "n_cnum" << i << ": " << *neighbors << std::endl;
        cnum[i] = *neighbors;
        neighbors++;
        // std::cout << "cnum" << i << ": " << cnum[i] << std::endl;
      }
      unsigned MaxM = 0;
      float min_cluster_dist = -1;  //查询点距最小类的距离
      unsigned *op_cluster = NULL;  //查询点选中的类的起始位置
      unsigned cluster_offset = 0;
      
      // std::cout << "ntest2\n";
      for (size_t i = 0; i < cluster_num_; i++) {
        unsigned id = *(neighbors + cluster_offset);  //2019.11.13
        float norm = *((const float*)(opt_graph_ + id * node_size));  //2019.11.13
        float temp = dist_fast -> compare(query, (const float*)(opt_graph_ + id * node_size + sizeof(float)), norm, (unsigned)dimension_);  //2019.11.13
        flags[id] = 1;  //2019.11.13
        dist_cout++;  //2019.11.13
        if (temp < retset[L - 1].distance) {  //2019.11.13
          Neighbor nn(id, temp, true);
          int r = InsertIntoPool(retset.data(), L, nn);
          if (r < nk) nk = r;
        }
        if (temp < min_cluster_dist || min_cluster_dist == -1) {
          min_cluster_dist = temp;
          op_cluster = neighbors + cluster_offset;
          MaxM = cnum[i];
        }
        cluster_offset += cnum[i];
      }
      
      neighbors = op_cluster;
      delete cnum;
      //std::cout << "ntest3\n";
      for (unsigned m = 0; m < MaxM; ++m)
        _mm_prefetch(opt_graph_ + node_size * neighbors[m], _MM_HINT_T0);
      for (unsigned m = 0; m < MaxM; ++m) {
        unsigned id = neighbors[m];
        if (flags[id]) continue;
        flags[id] = 1;
        float *data = (float *)(opt_graph_ + node_size * id);
        float norm = *data;
        data++;
        float dist =
            dist_fast->compare(query, data, norm, (unsigned)dimension_);
            dist_cout++;
        if (dist >= retset[L - 1].distance) continue;
        Neighbor nn(id, dist, true);
        int r = InsertIntoPool(retset.data(), L, nn);

        // if(L+1 < retset.size()) ++L;
        if (r < nk) nk = r;
      }
    }
    if (nk <= k)
      k = nk;
    else
      ++k;
  }
  for (size_t i = 0; i < L; i++) {  //重置候选集中的点为未访问
    retset[i].flag = true;
  }
  k = 0;
  while (k < (int)L) {
    int nk = L;

    if (retset[k].flag) {
      retset[k].flag = false;
      unsigned n = retset[k].id;

      _mm_prefetch(opt_graph_ + node_size * n + data_len, _MM_HINT_T0);
      unsigned *neighbors = (unsigned *)(opt_graph_ + node_size * n + data_len);
      // std::cout << "test1\n";
      unsigned cluster_num_= *neighbors;
      neighbors++;
      unsigned MaxM = 0;
      for (size_t i = 0; i < cluster_num_; i++) { //邻居总数
        MaxM += *neighbors;
        neighbors++;
      }

      for (unsigned m = 0; m < MaxM; ++m)
        _mm_prefetch(opt_graph_ + node_size * neighbors[m], _MM_HINT_T0);
      for (unsigned m = 0; m < MaxM; ++m) {
        unsigned id = neighbors[m];
        if (flags[id]) continue;
        flags[id] = 1;
        float *data = (float *)(opt_graph_ + node_size * id);
        float norm = *data;
        data++;
        float dist =
            dist_fast->compare(query, data, norm, (unsigned)dimension_);
            dist_cout++;
        if (dist >= retset[L - 1].distance) continue;
        Neighbor nn(id, dist, true);
        int r = InsertIntoPool(retset.data(), L, nn);

        // if(L+1 < retset.size()) ++L;
        if (r < nk) nk = r;
      }
    }
    if (nk <= k)
      k = nk;
    else
      ++k;
  }
  for (size_t i = 0; i < K; i++) {
    indices[i] = retset[i].id;
  }
}

void IndexSSG::OptimizeGraph(float *data) {  // use after build or load

  data_ = data;
  data_len = (dimension_ + 1) * sizeof(float);
  neighbor_len = (width + cluster_num + 1) * sizeof(unsigned);
  node_size = data_len + neighbor_len;
  opt_graph_ = (char *)malloc(node_size * nd_);
  DistanceFastL2 *dist_fast = (DistanceFastL2 *)distance_;

  for (unsigned i = 0; i < nd_; i++) {
    char *cur_node_offset = opt_graph_ + i * node_size;
    float cur_norm = dist_fast->norm(data_ + i * dimension_, dimension_);
    std::memcpy(cur_node_offset, &cur_norm, sizeof(float));
    std::memcpy(cur_node_offset + sizeof(float), data_ + i * dimension_,
                data_len - sizeof(float));

    cur_node_offset += data_len;
    unsigned cluster_num_ = nc_[i].size();
    std::memcpy(cur_node_offset, &cluster_num_, sizeof(unsigned));
    cur_node_offset += sizeof(unsigned);
    for (unsigned j = 0; j < cluster_num_; j++) {
      unsigned cnum = nc_[i][j].size();
      std::memcpy(cur_node_offset, &cnum, sizeof(unsigned));
      cur_node_offset += sizeof(unsigned);
    }
    for (unsigned j = 0; j < cluster_num_; j++) {
      unsigned cnum = nc_[i][j].size();
      // std::memcpy(cur_node_offset, &cnum, sizeof(unsigned));
      // cur_node_offset += sizeof(unsigned);
      std::memcpy(cur_node_offset, nc_[i][j].data(), sizeof(unsigned) * cnum);
      cur_node_offset += sizeof(unsigned) * cnum;
      std::vector<unsigned>().swap(nc_[i][j]);
    }
    std::vector<std::vector<unsigned> >().swap(nc_[i]);
  }
  free(data);
  data_ = nullptr;
  std::vector<std::vector<std::vector<unsigned> > >().swap(nc_);
  // std::cout << "OptimizeGraph\n";
}

void IndexSSG::DFS(boost::dynamic_bitset<> &flag,
                   std::vector<std::pair<unsigned, unsigned>> &edges,
                   unsigned root, unsigned &cnt) {
  unsigned tmp = root;
  std::stack<unsigned> s;
  s.push(root);
  if (!flag[root]) cnt++;
  flag[root] = true;
  while (!s.empty()) {
    unsigned next = nd_ + 1;
    for (unsigned i = 0; i < final_graph_[tmp].size(); i++) {
      if (flag[final_graph_[tmp][i]] == false) {
        next = final_graph_[tmp][i];
        break;
      }
    }
    // std::cout << next <<":"<<cnt <<":"<<tmp <<":"<<s.size()<< '\n';
    if (next == (nd_ + 1)) {
      unsigned head = s.top();
      s.pop();
      if (s.empty()) break;
      tmp = s.top();
      unsigned tail = tmp;
      if (check_edge(head, tail)) {
        edges.push_back(std::make_pair(head, tail));
      }
      continue;
    }
    tmp = next;
    flag[tmp] = true;
    s.push(tmp);
    cnt++;
  }
}

void IndexSSG::findroot(boost::dynamic_bitset<> &flag, unsigned &root,
                        const Parameters &parameter) {
  unsigned id = nd_;
  for (unsigned i = 0; i < nd_; i++) {
    if (flag[i] == false) {
      id = i;
      break;
    }
  }

  if (id == nd_) return;  // No Unlinked Node

  std::vector<Neighbor> tmp, pool;
  get_neighbors(data_ + dimension_ * id, parameter, tmp, pool);
  std::sort(pool.begin(), pool.end());

  bool found = false;
  for (unsigned i = 0; i < pool.size(); i++) {
    if (flag[pool[i].id]) {
      // std::cout << pool[i].id << '\n';
      root = pool[i].id;
      found = true;
      break;
    }
  }
  if (!found) {
    for (int retry = 0; retry < 1000; ++retry) {
      unsigned rid = rand() % nd_;
      if (flag[rid]) {
        root = rid;
        break;
      }
    }
  }
  final_graph_[root].push_back(id);
}

bool IndexSSG::check_edge(unsigned h, unsigned t) {
  bool flag = true;
  for (unsigned i = 0; i < final_graph_[h].size(); i++) {
    if (t == final_graph_[h][i]) flag = false;
  }
  return flag;
}

void IndexSSG::strong_connect(const Parameters &parameter) {
  unsigned n_try = parameter.Get<unsigned>("n_try");
  std::vector<std::pair<unsigned, unsigned>> edges_all;
  std::mutex edge_lock;

#pragma omp parallel for
  for (unsigned nt = 0; nt < n_try; nt++) {
    unsigned root = rand() % nd_;
    boost::dynamic_bitset<> flags{nd_, 0};
    unsigned unlinked_cnt = 0;
    std::vector<std::pair<unsigned, unsigned>> edges;

    while (unlinked_cnt < nd_) {
      DFS(flags, edges, root, unlinked_cnt);
      // std::cout << unlinked_cnt << '\n';
      if (unlinked_cnt >= nd_) break;
      findroot(flags, root, parameter);
      // std::cout << "new root"<<":"<<root << '\n';
    }

    LockGuard guard(edge_lock);

    for (unsigned i = 0; i < edges.size(); i++) {
      edges_all.push_back(edges[i]);
    }
  }
  unsigned ecnt = 0;
  for (unsigned e = 0; e < edges_all.size(); e++) {
    unsigned start = edges_all[e].first;
    unsigned end = edges_all[e].second;
    unsigned flag = 1;
    for (unsigned j = 0; j < final_graph_[start].size(); j++) {
      if (end == final_graph_[start][j]) {
        flag = 0;
      }
    }
    if (flag) {
      final_graph_[start].push_back(end);
      ecnt++;
    }
  }
  for (size_t i = 0; i < nd_; ++i) {
    if (final_graph_[i].size() > width) {
      width = final_graph_[i].size();
    }
  }
}

void IndexSSG::DFS_expand(const Parameters &parameter) {
  unsigned n_try = parameter.Get<unsigned>("n_try");
  unsigned range = parameter.Get<unsigned>("R");

  std::vector<unsigned> ids(nd_);
  for(unsigned i=0; i<nd_; i++){
    ids[i]=i;
  }
  std::random_shuffle(ids.begin(), ids.end());
  for(unsigned i=0; i<n_try; i++){
    eps_.push_back(ids[i]);
    //std::cout << eps_[i] << '\n';
  }
#pragma omp parallel for
  for(unsigned i=0; i<n_try; i++){
    unsigned rootid = eps_[i];
    boost::dynamic_bitset<> flags{nd_, 0};
    std::queue<unsigned> myqueue;
    myqueue.push(rootid);
    flags[rootid]=true;

    std::vector<unsigned> uncheck_set(1);

    while(uncheck_set.size() >0){
      while(!myqueue.empty()){
        unsigned q_front=myqueue.front();
        myqueue.pop();

        for(unsigned j=0; j<final_graph_[q_front].size(); j++){
          unsigned child = final_graph_[q_front][j];
          if(flags[child])continue;
          flags[child] = true;
          myqueue.push(child);
        }
      }

      uncheck_set.clear();
      for(unsigned j=0; j<nd_; j++){
        if(flags[j])continue;
        uncheck_set.push_back(j);
      }
      //std::cout <<i<<":"<< uncheck_set.size() << '\n';
      if(uncheck_set.size()>0){
        for(unsigned j=0; j<nd_; j++){
          if(flags[j] && final_graph_[j].size()<range){
            final_graph_[j].push_back(uncheck_set[0]);
            break;
          }
        }
        myqueue.push(uncheck_set[0]);
        flags[uncheck_set[0]]=true;
      }
    }
  }
}
void IndexSSG::Cluster(unsigned* m_neighbors, int N, size_t *Label)
{
	int size = 0;
	size = N;

	// assert(size >= cluster_num);

	// Initialize model
	Init(m_neighbors, N);	//初始化类中心，m_means[]

	// Recursion
  std::vector <bool> cflag(cluster_num, 0);
	float* x = new float[dimension_];	// Sample data
	int label = -1;		// Class index
	float iterNum = 0;
	float lastCost = 0;
	float currCost = 0;
	int unchanged = 0;
	bool loop = true;
	int* counts = new int[cluster_num];	//每个类包含的数据点的个数
	float** next_means = new float*[cluster_num];	// New model for reestimation
	for(size_t i = 0; i < cluster_num; i++)
	{
		next_means[i] = new float[dimension_];
	}
	while(loop)
	{
	    //clean buffer for classification
		std::memset(counts, 0, sizeof(int) * cluster_num);
		for(size_t i = 0; i < cluster_num; i++)
		{
			std::memset(next_means[i], 0, sizeof(float) * dimension_);	//新的类中心的各维数据初始化为0
		}

		lastCost = currCost;
		currCost = 0;

		// Classification
		for(int i = 0; i < size; i++)
		{
			for(size_t j = 0; j < dimension_; j++)
				x[j] = *(data_ + (*(m_neighbors + i)) * dimension_ + j);

			currCost += GetLabel(x, &label, cflag);	//到所属类的最小距离
			counts[label]++;
			for(size_t d = 0; d < dimension_; d++)	//属于某个类的所有数据点相应维的值累加
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for(size_t i = 0; i < cluster_num; i++)	//重新评估类中心
		{
			if(counts[i] > 0)
			{
				for(size_t d = 0; d < dimension_; d++)
				{
					next_means[i][d] /= counts[i];
				}
				std::memcpy(m_means[i], next_means[i], sizeof(float) * dimension_);
			}
		}
		// Terminal conditions
		iterNum++;
		if(fabs(lastCost - currCost) < m_endError * lastCost)
		{
			unchanged++;
		}
		if(iterNum >= m_maxIterNum || unchanged >= 3)
		{
			loop = false;
		}

		//DEBUG

	}
	// Output the label file
	for(int i = 0; i < size; i++)
	{
		for(size_t j = 0; j < dimension_; j++)
            x[j] = *(data_ + (*(m_neighbors + i) * dimension_) + j);
		GetLabel(x,&label, cflag);
		Label[i] = label;
	}
	delete counts;
	delete x;
	delete next_means;
}

void IndexSSG::Init(unsigned* m_neighbors, int N)	//初始化类中心
{
	int size = N;
	if(m_initMode ==  InitRandom)
	{
		int inteval = size / cluster_num;
    float* sample = new float[dimension_];
		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for(size_t i = 0; i < cluster_num; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
      for (size_t j = 0; j < dimension_; j++) {
        sample[j] = *(data_ + (*(m_neighbors + select)) * dimension_ + j);
      }
      std::memcpy(m_means[i], sample, sizeof(float) * dimension_);
    }
	}
	else if(m_initMode == InitUniform)
	{
    float* sample = new float[dimension_];
		for(size_t i = 0; i < cluster_num; i++)
		{
			int select = i * size / cluster_num;
      for (size_t j = 0; j < dimension_; j++) {
        sample[j] = *(data_ + (*(m_neighbors + select)) * dimension_ + j);
      }
      std::memcpy(m_means[i], sample, sizeof(float) * dimension_);
		}
	}
	else if(m_initMode == InitManual)
	{
		// Do nothing
	}
}

float IndexSSG::GetLabel(const float* sample, int* label, std::vector <bool> &cflag)	//返回sample到所有类中心的最小距离
{
	float dist = -1;
  
  
	for(size_t i = 0; i < cluster_num; i++)	//计算sample到每个类中心的距离
	{
    float temp = distance_->compare(sample, m_means[i], dimension_);
		if(temp - dist < -1e-6 || dist == -1)
		{
			dist = temp;
			*label = i;
		}
    else if (temp - dist <= 1e-6 || temp -dist >= -1e-6) {
      if (cflag[*label]&&cflag[i] == false) {
        *label = i;
      }
    }
	}
  cflag[*label] = true;
	return dist;
}

}  // namespace efanna2e
