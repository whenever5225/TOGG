#include "n2/neighbor_cluster.h"
#include <set>
#include <cstring>
#include <cmath>
#include <xmmintrin.h>

namespace n2 {

NeighborCluster::NeighborCluster(const size_t clusters, const size_t dimension, float **m_means)
: clusters_(clusters), dimension_(dimension), m_means_(m_means) {}

void NeighborCluster::NeighborsClusterPartition(std::vector<HnswNode*> & neighbors, std::vector<int> &cnum) {
    int nsize = neighbors.size();
    std::vector<std::vector<HnswNode*>> new_cluster(clusters_);
    if (nsize > clusters_) {
      size_t* labels = new size_t[nsize];
      // std::vector<HnswNode*>  neighbors_(neighbors);
      Cluster(neighbors, labels);
      
      std::set <size_t> mset;
      for (size_t u = 0; u < nsize; u++) {
        mset.insert(labels[u]);
      }
      std::set <size_t> :: iterator it = mset.begin();
      for (size_t j = 0; j < mset.size() || it != mset.end(); j++) {  //选取数据集中距离类中心最近的点作为新的类中心
        float min_dist = -1;
        HnswNode* new_cluster_center;
        for (size_t k = 0; k < nsize; k++) {
          if (labels[k] == *it) {
            float temp = DistanceEva(neighbors[k]->GetData().data(), m_means_[*it], dimension_);
            if (temp < min_dist || min_dist == -1) {
              min_dist = temp;
              new_cluster_center = neighbors[k];
            }
          }
        }
        new_cluster[*it].push_back(new_cluster_center);
        if (j != *it) {
          for (size_t u = 0; u < nsize; u++) {
            if (labels[u] == *it) {
              labels[u] = j;
            }
          }
        }
        // delete new_cluster_center;
        mset.erase(it++);
      }

      for (size_t j = 0; j < nsize; j++) {
        if (new_cluster[labels[j]][0] != neighbors[j]) {
          new_cluster[labels[j]].push_back(neighbors[j]);
        }
      }
      delete labels;
    }else{
      new_cluster.resize(1);
      for (size_t j = 0; j < nsize; j++) {
        new_cluster[0].push_back(neighbors[j]);
      }
    }
    cnum.resize(new_cluster.size());
    int k = 0;
    for (size_t i = 0; i < cnum.size(); i++) {
      cnum[i] = new_cluster[i].size();
      for (size_t j = 0; j < new_cluster[i].size(); j++) {
        neighbors[k++] = new_cluster[i][j];
      }
      std::vector<HnswNode*>().swap(new_cluster[i]);
    }

    std::vector<std::vector<HnswNode*>>().swap(new_cluster);
}

void NeighborCluster::Cluster(const std::vector<HnswNode*> & neighbors, size_t *Label)
{
	int size = neighbors.size();

	// Initialize model
	Init(neighbors);	//初始化类中心，m_means[]

	// Recursion
  std::vector <bool> cflag(clusters_, 0);
	float* x = new float[dimension_];	// Sample data
	int label = -1;		// Class index
	float iterNum = 0;
	float lastCost = 0;
	float currCost = 0;
	int unchanged = 0;
	bool loop = true;
	int* counts = new int[clusters_];	//每个类包含的数据点的个数
	float** next_means = new float*[clusters_];	// New model for reestimation
	for(size_t i = 0; i < clusters_; i++)
	{
		next_means[i] = new float[dimension_];
	}
	while(loop)
	{
	    //clean buffer for classification
		std::memset(counts, 0, sizeof(int) * clusters_);
		for(size_t i = 0; i < clusters_; i++)
		{
			std::memset(next_means[i], 0, sizeof(float) * dimension_);	//新的类中心的各维数据初始化为0
		}


		lastCost = currCost;
		currCost = 0;

		// Classification
		for(int i = 0; i < size; i++)
		{
			for(size_t j = 0; j < dimension_; j++) {
        x[j] = neighbors[i]->GetData()[j];
      }

      currCost += GetLabel(x, &label);	//到所属类的最小距离
			counts[label]++;
			for(size_t d = 0; d < dimension_; d++)	//属于某个类的所有数据点相应维的值累加
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for(size_t i = 0; i < clusters_; i++)	//重新评估类中心
		{
			if(counts[i] > 0)
			{
				for(size_t d = 0; d < dimension_; d++)
				{
					next_means[i][d] /= counts[i];
				}
				std::memcpy(m_means_[i], next_means[i], sizeof(float) * dimension_);
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
            x[j] = neighbors[i]->GetData()[j];
		GetLabel(x,&label);
		Label[i] = label;
	}
	delete counts;
	delete x;
	delete next_means;
}

void NeighborCluster::Init(const std::vector<HnswNode*> & neighbors)	//初始化类中心
{
	int size = neighbors.size();
    float* sample = new float[dimension_];
		for(size_t i = 0; i < clusters_; i++)
		{
			int select = i * size / clusters_;
      for (size_t j = 0; j < dimension_; j++) {
        sample[j] = neighbors[select]->GetData()[j];
      }
      std::memcpy(m_means_[i], sample, sizeof(float) * dimension_);
		}
}

float NeighborCluster::GetLabel(const float* sample, int* label)	//返回sample到所有类中心的最小距离
{
	float dist = -1;
	for(size_t i = 0; i < clusters_; i++)	//计算sample到每个类中心的距离
	{
    float temp = DistanceEva(sample, (float *)m_means_[i], dimension_);
		if(temp < dist || dist == -1)
		{
			dist = temp;
			*label = i;
		}
	}
	return dist;
}

float NeighborCluster::DistanceEva(const float* a, const float* b, const size_t d) {
  float sum = 0.0;
  for (size_t i = 0; i < d; i++) {
    sum += (*(a + i) - *(b + i)) * (*(a + i) - *(b + i));
  }
  return sum;
}

} // namespace n2
