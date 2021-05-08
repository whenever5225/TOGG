# TOGG: Two-Stage Routing with Optimized Guided Search and Greedy Algorithm On Proximity Graph

## Introduction

Two-Stage Routing with Optimized Guided Search and Greedy Algorithm is more efficient and effective routing strategy than state-of-the-art method, including [SIGKDD'2011](https://dl.acm.org/doi/10.1145/2020408.2020576), [CVPR'2016](https://www.cv-foundation.org/openaccess/content_cvpr_2016/html/Harwood_FANNG_Fast_Approximate_CVPR_2016_paper.html), [PR'2019](https://www.sciencedirect.com/science/article/abs/pii/S0031320319302730), and [TPAMI'2021](https://ieeexplore.ieee.org/abstract/document/9383170). At present, TOGG has been deployed in several state-of-the-art proximity graph algorithms (such as [HNSW](https://github.com/kakao/n2), [NSG](https://github.com/ZJULearning/nsg), and [NSSG](https://github.com/ZJULearning/ssg)), and experiments on multiple real world datasets have verified the efficiency and effectiveness of TOGG.

## Routing Strategies

We evaluate four advanced routing strategies applied to the proximity graph algorithms, and their papers are given in the following table. In addition, we propose TOGG, including TOGG that uses OGS-KDT + OGA (TOGG-KDT) and OGS-KMC + OGA (TOGG-KMC); they achieve better performance of eﬀiciency and effectiveness than state-of-the-art work.

```
OGS-KDT: optimized Guided Search based on modified KD-Tree
OGS-KMC: optimized Guided Search based on k-means clustering
OGA: optimized greedy algorithm
```

| Strategy    | Paper Source                                                 | Year |
| ----------- | ------------------------------------------------------------ | ---- |
| SIGKDD'2011 | [SIGKDD](https://dl.acm.org/doi/10.1145/2020408.2020576)     | 2011 |
| CVPR'2016   | [CVPR](https://www.cv-foundation.org/openaccess/content_cvpr_2016/html/Harwood_FANNG_Fast_Approximate_CVPR_2016_paper.html) | 2016 |
| PR'2019     | [PR](https://www.sciencedirect.com/science/article/abs/pii/S0031320319302730) | 2019 |
| TPAMI'2021  | [TPAMI](https://ieeexplore.ieee.org/abstract/document/9383170) | 2021 |

## Proximity Graph Algorithms

We applied both TOGG to three state-of-the-art proximity graphs, i.e., HNSW, NSG, and NSSG, and then their scalability were tested on four real-world datasets. Note that greedy algorithm (GA) is the routing strategy currently used by these proximity graph algorithms.

| Algorithm | Paper Source                                                 | Year |
| --------- | ------------------------------------------------------------ | ---- |
| HNSW      | [TPAMI](https://ieeexplore.ieee.org/abstract/document/8594636) | 2018 |
| NSG       | [VLDB](http://www.vldb.org/pvldb/vol12/p461-fu.pdf)          | 2019 |
| NSSG      | [TPAMI](https://ieeexplore.ieee.org/abstract/document/9383170) | 2021 |

## Datasets

Our experiment involves four real-world datasets popularly deployed by existing works. All datasets are pre-split into base data and query data and come with groundtruth data in the form of the top 100 neighbors.

| dataset   | base_num | base_dim | query_num | query_dim | groundtruth_num/query | download                                                     |
| --------- | -------- | -------- | --------- | --------- | --------------------- | ------------------------------------------------------------ |
| SIFT1M    | 1000000  | 128      | 10000     | 128       | 100                   | [sift.tar.gz](http://corpus-texmex.irisa.fr/)(161MB)         |
| GIST1M    | 1000000  | 960      | 1000      | 960       | 100                   | [gist.tar.gz](http://corpus-texmex.irisa.fr/)(2.6GB)         |
| GloVe-100 | 1183514  | 100      | 10000     | 100       | 100                   | [glove-100.tar.gz](http://downloads.zjulearning.org.cn/data/glove-100.tar.gz)(424MB) |
| MNIST     | 60000    | 784      | 10000     | 100       | 100                   | [mnist.tar.gz](https://drive.google.com/file/d/1UBIx5Ci0ZvDLgqhWn_pG6qxp4tlvhTax/view?usp=sharing)(19.8MB) |

## Parameters

```
b: interval factor (TOGG-KDT), it controls the size of the partition interval, thereby affecting the number of neighbors in each subtree.
m: the number of clusters (TOGG-KMC), it controls the number of neighbor clusters, which affects the number of neighbors in each neighbor cluster.
```

To obtain the optimal value of b (0 ≤ b < infinity) or m (1 ≤ m ≤ c, c is maximum outdegree), we first traverse the values in a large interval within their value range, and then reduce the interval and traverse the value within a small range until an optimal value is obtained.

## Usage

### Prerequisites

- GCC 4.9+ with OpenMP
- CMake 2.8+
- Boost 1.55+

### Compile on Linux

**Download:**

```shell
$ git clone https://github.com/whenever5225/TOGG.git
```

**Locate directory:**

```shell
$ cd TOGG/routing_evalutaion	#for routing strategies evaluation
$ cd TOGG/algorithms/A/B/	#A is GA or TOGG-KDT or TOGG-KMC, B is hnsw or nsg or nssg, for scalability evaluation
$ mkdir build && cd build/
```

Compile:

```shell
$ cmake ..
$ make -j
```

### Comparing with Prior Routing Strategies

Before building index, you should set the root directory for the dataset in `TOGG/routing_evaluation/test/evaluation.cpp` first. Then, you can run the following instructions for build graph index.

```shell
cd TOGG/routing_evaluation/build/test/
./evaluation strategy_name dataset_name build [other optional information]
```

After the build is completed, the graph index will be written in the current folder in binary format (for index size). The index construction time can be viewed from the output log information. With the index built, you can run the following commands to perform the search. Related information about the search such as distance evaluation times, candidate set size, memory load can be obtained or calculated according to the output log information.

```shell
cd TOGG/routing_evaluation/build/test/
./evaluation strategy_name dataset_name search [other optional information]
```

### Scalability Evaluation

Before building index, you should set the root directory for the dataset in `TOGG/algorithms/A/B/test/evaluation.cpp` first. Then, you can run the following instructions for build graph index.

```shell
cd TOGG/algorithms/A/B/build/test/
./evaluation dataset_name build [other optional information]	#see evaluation.cpp file for more detail
```

After the build is completed, the graph index will be written in the current folder in binary format (for index size). The index construction time can be viewed from the output log information. With the index built, you can run the following commands to perform the search. Related information about the search such as distance evaluation times, candidate set size, memory load can be obtained or calculated according to the output log information.

```shell
cd TOGG/algorithms/A/B/build/test/
./evaluation dataset_name search [other optional information]	#see evaluation.cpp file for more detail
```

## Results

![image-20210508170359383](/Users/wmz/Library/Application Support/typora-user-images/image-20210508170359383.png)

## Acknowledgements

Thanks to everyone who provided references for this project. Special thanks to [n2](https://github.com/kakao/n2), [nsg](https://github.com/ZJULearning/nsg), and [ssg](https://github.com/ZJULearning/ssg) for their assistance in the necessary implementation of this project.

