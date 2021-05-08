import numpy as np


def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname):
    return ivecs_read(fname).view('float32')

if __name__ == "__main__":

    source = fvecs_read("/home/martin/Project/experiment/data_set/sift1M_128/sift_base.fvecs")
    # labels = ivecs_read("/home/martin/Project/experiment/data_set/sift1M_128/siftsmall_groundtruth.ivecs")
    querys = fvecs_read("/home/martin/Project/experiment/data_set/sift1M_128/sift_query.fvecs")


    # size = len(source)
    # print(source[8000])

    # for line in source:
    #     print(line)

    # source = fvecs_read("/home/tdlab/dataset/sift1M/sift_base.fvecs")

    # print(len(source[0]))
    # print(labels[100])

    print(len(source))

    print(len(querys))

    # for i in source[0]:
    #     print(i+1000)

    # for i in range(len(source[0])):
    #     print(i)

    # file = ivecs_read("/home/tdlab/test/quan_Result")
    # print(file)