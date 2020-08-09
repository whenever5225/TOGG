#ifndef EFANNA2E_INDEX_SSG_H
#define EFANNA2E_INDEX_SSG_H

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>

#include "index.h"
#include "neighbor.h"
#include "parameters.h"
#include "util.h"

namespace efanna2e {

class IndexSSG : public Index {
 public:
 enum InitMode
  {
    InitRandom,
    InitManual,
    InitUniform,
  };
  explicit IndexSSG(const size_t dimension, const size_t n, Metric m,
                    Index *initializer);

  virtual ~IndexSSG();

  virtual void Save(const char *filename) override;
  virtual void Load(const char *filename) override;

  virtual void Build(size_t n, const float *data,
                     const Parameters &parameters) override;

  virtual void Search(const float *query, const float *x, size_t k,
                      const Parameters &parameters, unsigned *indices) override;
  void SearchWithOptGraph(const float *query, size_t K,
                          const Parameters &parameters, unsigned *indices);
  void OptimizeGraph(float *data);
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
  typedef std::vector<std::vector<unsigned>> CompactGraph;
  typedef std::vector<SimpleNeighbors> LockGraph;
  typedef std::vector<nhood> KNNGraph;

  CompactGraph final_graph_;
  Index *initializer_;

  void init_graph(const Parameters &parameters);
  void get_neighbors(const float *query, const Parameters &parameter,
                     std::vector<Neighbor> &retset,
                     std::vector<Neighbor> &fullset);
  void get_neighbors(const unsigned q, const Parameters &parameter,
                     std::vector<Neighbor> &pool);
  void sync_prune(unsigned q, std::vector<Neighbor> &pool,
                  const Parameters &parameter, float threshold,
                  SimpleNeighbor *cut_graph_);
  void Link(const Parameters &parameters, SimpleNeighbor *cut_graph_);
  void InterInsert(unsigned n, unsigned range, float threshold,
                   std::vector<std::mutex> &locks, SimpleNeighbor *cut_graph_);
  void Load_nn_graph(const char *filename);
  void strong_connect(const Parameters &parameter);

  void DFS(boost::dynamic_bitset<> &flag,
           std::vector<std::pair<unsigned, unsigned>> &edges, unsigned root,
           unsigned &cnt);
  bool check_edge(unsigned u, unsigned t);
  void findroot(boost::dynamic_bitset<> &flag, unsigned &root,
                const Parameters &parameter);
  void DFS_expand(const Parameters &parameter);
  float GetLabel(const float* x, int* label);

 private:
  unsigned width;
  unsigned ep_; //not in use
  std::vector<unsigned> eps_;
  std::vector<std::mutex> locks;
  char *opt_graph_;
  size_t node_size;
  size_t data_len;
  size_t neighbor_len;
  KNNGraph nnd_graph;
  float** m_means;
  size_t m_initMode;
  size_t m_maxIterNum;
  float m_endError;
};

}  // namespace efanna2e

#endif  // EFANNA2E_INDEX_SSG_H
