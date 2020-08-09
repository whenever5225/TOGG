SSG Based on K-means Tree Query Guided Search
======

SSG is a graph-based approximate nearest neighbor search (ANNS) algorithm. It provides a flexible and efficient solution for the metric-free large-scale ANNS on dense real vectors. 
It implements the algorithm of this paper: [Satellite System Graph: Towards the Efficiency Up-Boundary of Graph-Based Approximate Nearest Neighbor Search](https://arxiv.org/abs/1907.06146)


How to use
------

### Compile

* Prerequisite : openmp, cmake, boost
* Compile:
    1. Go to the root directory of faiss, it's under the directory of extern_libraries aside of ours.
    2. Execute the following commands:

```bash
$ cd /path/to/project
$ mkdir -p build && cd build
$ cmake .. && make -j
```

### Building SSG Index

The main interfaces and classes have its respective test codes under directory `tests/`.

Please follow the instructions below to build the SSG index.

#### a) Build a kNN graph

Firstly, we need to prepare a kNN graph.

We suggest you use our [efanna\_graph](https://github.com/ZJULearning/efanna\_graph) to build this kNN graph. But you can also use any alternatives you like, such as KGraph or faiss.

#### b) Convert the kNN graph to an SSG

For example:

```bash
$ cd /path/to/project/build/tests/
$ ./test_ssg_index data_path knn_graph_path L R Angle C ssg_path
```

* **data\_path** is the path of the origin data.
* **knn\_graph\_path** is the path of the pre-built kNN graph.
* **L** controls the quality of the NSG, the larger the better, L > R.
* **R** controls the index size of the graph, the best R is related to the intrinsic dimension of the dataset.
* **Angle** controls the angle between two edges.
* C is the maximum number of neighbor set clusters per node.
* **ssg\_path** is the path where the result SSG stored.

### Approximate Nearest Neighbor Search using SSG

For example:

```bash
$ cd /path/to/project/build/tests/
$ ./test_ssg_optimized_search data_path query_path ssg_path search_L search_K result_path [random_seed]
```

* **data\_path** is the path of the origin data.
* **query\_path** is the path of the query data.
* **ssg\_path** is the path of the pre-built SSG.
* **search\_L** controls the quality of the search results, the larger the better but slower (must larger than search\_K).
* **search\_K** controls the number of neighbors we want to find.
* **result\_path** is the path of the result neighbors.
* **random\_seed** (optional) is the random seed.

> **NOTE:** For now, we only provide interface for search for only one query at a time, and test the performance with single thread.

> **NOTE:** Data alignment is essential for the correctness of our procedure, because we use SIMD instructions for acceleration of numerical computing such as AVX and SSE2.
You should use it to ensure your data elements (feature) is aligned with 8 or 16 int or float. For example, if your features are of dimension 70, then it should be extend to dimension 72. And the last 2 dimension should be filled with 0 to ensure the correctness of the distance computing. And this is what data\_align() does.

> **NOTE:** Only data-type int32 and float32 are supported for now.

### SSG Search

+ `search_L`: range from `search_K` to 2000
+ `random_seed`: 161803398
