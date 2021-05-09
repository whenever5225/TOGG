#ifndef EFANNA2E_INDEX_NSG_H
#define EFANNA2E_INDEX_NSG_H

#include "util.h"
#include "parameters.h"
#include "neighbor.h"
#include "index.h"
#include <cassert>
#include <unordered_map>
#include <string>
#include <sstream>
#include <boost/dynamic_bitset.hpp>
#include <stack>

namespace efanna2e {

class IndexNSG : public Index {
 public:
 enum InitMode
  {
    InitRandom,
    InitManual,
    InitUniform,
  };
  explicit IndexNSG(const size_t dimension, const size_t n, Metric m, Index *initializer);


  virtual ~IndexNSG();

  virtual void Save(const char *filename)override;
  virtual void Load(const char *filename)override;


  virtual void Build(size_t n, const float *data, const Parameters &parameters) override;

  virtual void Search(
      const float *query,
      const float *x,
      size_t k,
      const Parameters &parameters,
      unsigned *indices) override;
  void SearchWithOptGraph(
      const float *query,
      size_t K,
      const Parameters &parameters,
      unsigned *indices);
  void OptimizeGraph(float* data);
  void InitDistCount(){dist_cout = 0;}
  size_t GetDistCount(){return dist_cout;}
  void InitClusterPara(const size_t cl_num);
  void SetMean(int i, const float* u){ memcpy(m_means[i], u, sizeof(float) * dimension_); }
  void SetInitMode(int i)				{ m_initMode = i;}
  void SetMaxIterNum(int i)			{ m_maxIterNum = i; }
  void SetEndError(float f)			{ m_endError = f; }
  float* GetMean(int i)	{ return m_means[i]; }
  int GetInitMode()		{ return m_initMode; }
  int GetMaxIterNum()		{ return m_maxIterNum; }
  float GetEndError()	{ return m_endError; }

  void Init(unsigned* m_neighbors, int N);
  void Cluster(unsigned* m_neighbors, int N, size_t *Label);

  protected:
    typedef std::vector<std::vector<unsigned > > CompactGraph;
    typedef std::vector<SimpleNeighbors > LockGraph;
    typedef std::vector<nhood> KNNGraph;

    CompactGraph final_graph_;

    Index *initializer_;
    void init_graph(const Parameters &parameters);
    void get_neighbors(
        const float *query,
        const Parameters &parameter,
        std::vector<Neighbor> &retset,
        std::vector<Neighbor> &fullset);
    void get_neighbors(
        const float *query,
        const Parameters &parameter,
        boost::dynamic_bitset<>& flags,
        std::vector<Neighbor> &retset,
        std::vector<Neighbor> &fullset);
    //void add_cnn(unsigned des, Neighbor p, unsigned range, LockGraph& cut_graph_);
    void InterInsert(unsigned n, unsigned range, std::vector<std::mutex>& locks, SimpleNeighbor* cut_graph_);
    void sync_prune(unsigned q, std::vector<Neighbor>& pool, const Parameters &parameter, boost::dynamic_bitset<>& flags, SimpleNeighbor* cut_graph_);
    void Link(const Parameters &parameters, SimpleNeighbor* cut_graph_);
    void Load_nn_graph(const char *filename);
    void tree_grow(const Parameters &parameter);
    void DFS(boost::dynamic_bitset<> &flag, unsigned root, unsigned &cnt);
    void findroot(boost::dynamic_bitset<> &flag, unsigned &root, const Parameters &parameter);
    float GetLabel(const float* x, int* label);


  private:
    unsigned width;
    unsigned ep_;
    std::vector<std::mutex> locks;
    char* opt_graph_;
    size_t node_size;
    size_t data_len;
    size_t neighbor_len;
    KNNGraph nnd_graph;
    size_t dist_cout;
    float** m_means;
    size_t m_initMode;
    size_t m_maxIterNum;
    float m_endError;
};
}

#endif //EFANNA2E_INDEX_NSG_H
