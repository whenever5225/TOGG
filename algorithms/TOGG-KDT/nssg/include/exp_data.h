#include <string>
#include <vector>
#include <string.h>
#include <iostream>

void set_para(std::string dataset, efanna2e::Parameters &parameters) {
    unsigned L, R, A;
    if (dataset == "siftsmall") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "sift1M"  || dataset == "sift1M_sample") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "gist" || dataset == "gist_sample") {
        L = 500, R = 70, A = 60;   // nsg
    }else if (dataset == "glove-100" || dataset == "glove_sample") {
        L = 500, R = 50, A = 60;   // nsg
    }else if (dataset == "audio") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "crawl") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "msong") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "uqv") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "enron") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "mnist") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "c_1") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "c_10") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "c_100") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "d_8") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "d_128") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "n_10000") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "n_1000000") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "s_1") {
        L = 100, R = 50, A = 60;   // nsg
    }else if (dataset == "s_10") {
        L = 100, R = 50, A = 60;   // nsg
    }else {
        std::cout << "input dataset error!\n";
        exit(-1);
    }
    std::string nn_graph_path("/ddisk/wmz/TOGG/algorithms/GA/efanna_graph/build/tests/" + dataset + "_knn.graph");
    parameters.Set<unsigned>("L", L);
    parameters.Set<unsigned>("R", R);
    parameters.Set<unsigned>("A", A);
    parameters.Set<unsigned>("n_try", 10);
    parameters.Set<std::string>("nn_graph_path", nn_graph_path);
}

void set_data_path(std::string dataset, std::string &base_path, std::string &query_path, std::string &ground_path) {
    // dataset root path
    std::string dataset_root = "/home_bak/yq/ANNS/dataset/";
    base_path = dataset_root;
    query_path = dataset_root;
    ground_path = dataset_root;
    if (dataset == "siftsmall") {
        base_path.append(R"(siftsmall/siftsmall_base.fvecs)");
        query_path.append(R"(siftsmall/siftsmall_query.fvecs)");
        ground_path.append(R"(siftsmall/siftsmall_groundtruth.ivecs)");

    }else if (dataset == "sift1M") {
        base_path.append(R"(sift1M/sift_base.fvecs)");
        query_path.append(R"(sift1M/sift_query.fvecs)");
        ground_path.append(R"(sift1M/sift_groundtruth.ivecs)");

    }else if (dataset == "gist") {
        base_path.append(R"(gist/gist_base.fvecs)");
        query_path.append(R"(gist/gist_query.fvecs)");
        ground_path.append(R"(gist/gist_groundtruth.ivecs)");

    }else if (dataset == "glove-100") {
        base_path.append(R"(glove-100/glove-100_base.fvecs)");
        query_path.append(R"(glove-100/glove-100_query.fvecs)");
        ground_path.append(R"(glove-100/glove-100_groundtruth.ivecs)");

    }else if (dataset == "audio") {
        base_path.append(R"(audio/audio_base.fvecs)");
        query_path.append(R"(audio/audio_query.fvecs)");
        ground_path.append(R"(audio/audio_groundtruth.ivecs)");

    }else if (dataset == "crawl") {
        base_path.append(R"(crawl/crawl_base.fvecs)");
        query_path.append(R"(crawl/crawl_query.fvecs)");
        ground_path.append(R"(crawl/crawl_groundtruth.ivecs)");

    }else if (dataset == "msong") {
        base_path.append(R"(msong/msong_base.fvecs)");
        query_path.append(R"(msong/msong_query.fvecs)");
        ground_path.append(R"(msong/msong_groundtruth.ivecs)");

    }else if (dataset == "uqv") {
        base_path.append(R"(uqv/uqv_base.fvecs)");
        query_path.append(R"(uqv/uqv_query.fvecs)");
        ground_path.append(R"(uqv/uqv_groundtruth.ivecs)");

    }else if (dataset == "enron") {
        base_path.append(R"(enron/enron_base.fvecs)");
        query_path.append(R"(enron/enron_query.fvecs)");
        ground_path.append(R"(enron/enron_groundtruth.ivecs)");

    }else if (dataset == "mnist") {
        base_path.append(R"(mnist/mnist_base.fvecs)");
        query_path.append(R"(mnist/mnist_query.fvecs)");
        ground_path.append(R"(mnist/mnist_groundtruth.ivecs)");

    }else if (dataset == "c_1") {
        base_path.append(R"(c_1/random_base_n100000_d32_c1_s5.fvecs)");
        query_path.append(R"(c_1/random_query_n1000_d32_c1_s5.fvecs)");
        ground_path.append(R"(c_1/random_ground_truth_n1000_d32_c1_s5.ivecs)");

    }else if (dataset == "c_10") {
        base_path.append(R"(c_10/random_base_n100000_d32_c10_s5.fvecs)");
        query_path.append(R"(c_10/random_query_n1000_d32_c10_s5.fvecs)");
        ground_path.append(R"(c_10/random_ground_truth_n1000_d32_c10_s5.ivecs)");

    }else if (dataset == "c_100") {
        base_path.append(R"(c_100/random_base_n100000_d32_c100_s5.fvecs)");
        query_path.append(R"(c_100/random_query_n1000_d32_c100_s5.fvecs)");
        ground_path.append(R"(c_100/random_ground_truth_n1000_d32_c100_s5.ivecs)");

    }else if (dataset == "d_8") {
        base_path.append(R"(d_8/random_base_n100000_d8_c10_s5.fvecs)");
        query_path.append(R"(d_8/random_query_n1000_d8_c10_s5.fvecs)");
        ground_path.append(R"(d_8/random_ground_truth_n1000_d8_c10_s5.ivecs)");

    }else if (dataset == "d_128") {
        base_path.append(R"(d_128/random_base_n100000_d128_c10_s5.fvecs)");
        query_path.append(R"(d_128/random_query_n1000_d128_c10_s5.fvecs)");
        ground_path.append(R"(d_128/random_ground_truth_n1000_d128_c10_s5.ivecs)");

    }else if (dataset == "n_10000") {
        base_path.append(R"(n_10000/random_base_n10000_d32_c10_s5.fvecs)");
        query_path.append(R"(n_10000/random_query_n100_d32_c10_s5.fvecs)");
        ground_path.append(R"(n_10000/random_ground_truth_n100_d32_c10_s5.ivecs)");

    }else if (dataset == "n_1000000") {
        base_path.append(R"(n_1000000/random_base_n1000000_d32_c10_s5.fvecs)");
        query_path.append(R"(n_1000000/random_query_n10000_d32_c10_s5.fvecs)");
        ground_path.append(R"(n_1000000/random_ground_truth_n10000_d32_c10_s5.ivecs)");

    }else if (dataset == "s_1") {
        base_path.append(R"(s_1/random_base_n100000_d32_c10_s1.fvecs)");
        query_path.append(R"(s_1/random_query_n1000_d32_c10_s1.fvecs)");
        ground_path.append(R"(s_1/random_ground_truth_n1000_d32_c10_s1.ivecs)");

    }else if (dataset == "s_10") {
        base_path.append(R"(s_10/random_base_n100000_d32_c10_s10.fvecs)");
        query_path.append(R"(s_10/random_query_n1000_d32_c10_s10.fvecs)");
        ground_path.append(R"(s_10/random_ground_truth_n1000_d32_c10_s10.ivecs)");

    }else if (dataset == "sift1M_sample") {
        base_path.append(R"(sift1M/sift_sample_base.fvecs)");
        query_path.append(R"(sift1M/sift_sample_query.fvecs)");
        ground_path.append(R"(sift1M/sift_sample_groundtruth.ivecs)");

    }else if (dataset == "glove_sample") {
        base_path.append(R"(glove-100/glove-100_sample_base.fvecs)");
        query_path.append(R"(glove-100/glove-100_sample_query.fvecs)");
        ground_path.append(R"(glove-100/glove-100_sample_groundtruth.ivecs)");
    }else if (dataset == "gist_sample") {
        base_path.append(R"(gist/gist_sample_base.fvecs)");
        query_path.append(R"(gist/gist_sample_query.fvecs)");
        ground_path.append(R"(gist/gist_sample_groundtruth.ivecs)");
    }
    else {
        std::cout << "dataset input error!\n";
        exit(-1);
    }
}