# -*- coding: utf-8 -*-
from n2 import HnswIndex
import random
import numpy as np


def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname):
    return ivecs_read(fname).view('float32')

if __name__ == "__main__":

    datas = fvecs_read("/home/tdlab/dataset/siftsmall/siftsmall_base.fvecs")
    querys = fvecs_read("/home/tdlab/dataset/siftsmall/siftsmall_query.fvecs")
    # labels = ivecs_read("/home/martin/Project/experiment/data_set/siftsmall/siftsmall_groundtruth.ivecs")

    # datas = fvecs_read("/home/martin/Project/experiment/data_set/sift1M_128/sift_base.fvecs")
    # querys = fvecs_read("/home/martin/Project/experiment/data_set/sift1M_128/sift_query.fvecs")
    # labels = ivecs_read("/home/martin/Project/experiment/data_set/siftsmall/siftsmall_groundtruth.ivecs")

    # file = ivecs_read("/home/tdlab/test/quan_Result")
    # print(file)cd 
    f = 128
    t = HnswIndex(f, "L2")  # HnswIndex(f, "L2 or angular")

    for data in datas:
        t.add_data(data)

    

    t.build(m=20, max_m0=40, n_threads=4)
    t.save('HNSW.n20_40_128_100w_L2')

    u = HnswIndex(f, "L2")
    u.load('HNSW.n20_40_128_100w_L2')

    # search_id = 1
    # k = 100
    # neighbor_ids = u.search_by_id(search_id, k, 200)
    # print(
    #     "[search_by_id]: Nearest neighborhoods of id {}: {}".format(
    #         search_id,
    #         neighbor_ids))

    # example_vector_query = [random.gauss(0, 1) for z in range(f)]
    # nns = u.search_by_vector(example_vector_query, k, 100, 2, include_distances=True)# 2-SearchById_true() 默认-SearchById_()
    # print(
    #     "[search_by_vector]: Nearest neighborhoods of vector {}: {}".format(
    #         example_vector_query, 
    #         nns))
        
    # nnsv = u.search_by_vector_violence(example_vector_query, k, include_distances=True)
    # print(
    #     "[search_by_vector_violence]: Nearest neighborhoods of vector {}: {}".format(
    #         example_vector_query,
    #         nnsv))

    k = 100
    hg = 0
    hd = 100
    for i in range(100):
        nns = u.search_by_vector(querys[i], k, 220, 2, include_distances=True)# 2-SearchById_true() 默认-SearchById_()

        # nnsv = labels[i]
        # print(nnsv)


        nnsv = u.search_by_vector_violence(querys[i], k, include_distances=True)
        # print(nnsv)
        for (key,value) in nns:
            for (keyv,valuev) in nnsv:
                if key == keyv:
                    hg +=1
                    break

    # hg = 0
    # hd = 1
    # hh = hd
    # while hh > 0:
    #     hh -= 1
    #     example_vector_query = querys[10]
    #     nns = u.search_by_vector(example_vector_query, k, 100, 2, include_distances=True)# 2-SearchById_true() 默认-SearchById_()

    #     nnsv = u.search_by_vector_violence(example_vector_query, k, include_distances=True)
    #     for (key,value) in nns:
    #         for (keyv,valuev) in nnsv:
    #             if key == keyv:
    #                 hg +=1
    #                 break
    comprison_times = u.get_comparison_times()
    print("比较次数： {}".format(comprison_times*1.0/hd))

    print("精度为: {}%".format((hg*1.0/(k*hd))*100))