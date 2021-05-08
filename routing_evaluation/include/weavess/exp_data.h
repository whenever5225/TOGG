#include "parameters.h"
#include <string.h>
#include <iostream>

void set_para(std::string dataset, weavess::Parameters &parameters) {
    unsigned K, L, Iter, S, R, L_refine, R_refine, C;
    if (dataset == "siftsmall") {
        K = 25, L = 50, Iter = 6, S = 10, R = 100, L_refine = 100, R_refine = 50, C = 500;   // nsg
    }else if (dataset == "sift1M" || dataset == "sift1M_sample") {
        K = 100, L = 120, Iter = 12, S = 25, R = 300, L_refine = 150, R_refine = 30, C = 400;   // nsg
    }else if (dataset == "gist" || dataset == "gist_sample") {
        K = 400, L = 430, Iter = 12, S = 10, R = 200, L_refine = 500, R_refine = 20, C = 400;   // nsg
    }else if (dataset == "glove-100" || dataset == "glove_sample") {
        K = 400, L = 420, Iter = 12, S = 20, R = 300, L_refine = 150, R_refine = 90, C = 600;   // nsg
    }else if (dataset == "audio") {
        K = 200, L = 230, Iter = 5, S = 10, R = 100, L_refine = 200, R_refine = 30, C = 600;   // nsg
    }else if (dataset == "crawl") {
        K = 400, L = 430, Iter = 12, S = 15, R = 300, L_refine = 250, R_refine = 40, C = 600;   // nsg
    }else if (dataset == "msong") {
        K = 300, L = 310, Iter = 12, S = 25, R = 300, L_refine = 350, R_refine = 20, C = 500;   // nsg
    }else if (dataset == "uqv") {
        K = 300, L = 320, Iter = 6, S = 15, R = 200, L_refine = 350, R_refine = 30, C = 400;   // nsg
    }else if (dataset == "enron") {
        K = 200, L = 200, Iter = 7, S = 25, R = 200, L_refine = 150, R_refine = 60, C = 600;   // nsg
    }else if (dataset == "mnist") {
        K = 25, L = 50, Iter = 8, S = 10, R = 100, L_refine = 100, R_refine = 50, C = 600;   // nsg
    }else if (dataset == "c_1") {
        K = 300, L = 310, Iter = 8, S = 20, R = 200, L_refine = 200, R_refine = 80, C = 400;   // nsg
    }else if (dataset == "c_10") {
        K = 200, L = 200, Iter = 8, S = 20, R = 100, L_refine = 100, R_refine = 80, C = 400;   // nsg
    }else if (dataset == "c_100") {
        K = 400, L = 410, Iter = 8, S = 20, R = 100, L_refine = 400, R_refine = 20, C = 400;   // nsg
    }else if (dataset == "d_8") {
        K = 100, L = 100, Iter = 8, S = 10, R = 100, L_refine = 150, R_refine = 20, C = 600;   // nsg
    }else if (dataset == "d_128") {
        K = 200, L = 210, Iter = 8, S = 10, R = 300, L_refine = 150, R_refine = 20, C = 400;   // nsg
    }else if (dataset == "n_10000") {
        K = 300, L = 300, Iter = 7, S = 15, R = 300, L_refine = 50, R_refine = 20, C = 500;   // nsg
    }else if (dataset == "n_1000000") {
        K = 200, L = 200, Iter = 12, S = 20, R = 100, L_refine = 100, R_refine = 80, C = 400;   // nsg
    }else if (dataset == "s_1") {
        K = 200, L = 220, Iter = 5, S = 25, R = 300, L_refine = 300, R_refine = 20, C = 500;   // nsg
    }else if (dataset == "s_10") {
        K = 300, L = 300, Iter = 9, S = 25, R = 300, L_refine = 200, R_refine = 80, C = 400;   // nsg
    }else {
        std::cout << "input dataset error!\n";
        exit(-1);
    }
    parameters.set<unsigned>("K", K);
    parameters.set<unsigned>("L", L);
    parameters.set<unsigned>("ITER", Iter);
    parameters.set<unsigned>("S", S);
    parameters.set<unsigned>("R", R);

    parameters.set<unsigned>("L_refine", L_refine);
    parameters.set<unsigned>("R_refine", R_refine);
    parameters.set<unsigned>("C_refine", C);
}

void set_data_path(std::string dataset, weavess::Parameters &parameters) {
    // dataset root path
    std::string dataset_root = parameters.get<std::string>("dataset_root");
    std::string base_path(dataset_root);
    std::string query_path(dataset_root);
    std::string ground_path(dataset_root);
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
        dataset_root = "/home_bak/yq/WEAVESS/dataset/sift1M/";
        base_path = dataset_root;
        query_path = dataset_root;
        ground_path = dataset_root;
        base_path.append(R"(sift_sample_base.fvecs)");
        query_path.append(R"(sift_sample_query.fvecs)");
        ground_path.append(R"(sift_sample_groundtruth.ivecs)");
    }else if (dataset == "gist_sample") {
        dataset_root = "/home_bak/yq/WEAVESS/dataset/gist/";
        base_path = dataset_root;
        query_path = dataset_root;
        ground_path = dataset_root;
        base_path.append(R"(gist_sample_base.fvecs)");
        query_path.append(R"(gist_sample_query.fvecs)");
        ground_path.append(R"(gist_sample_groundtruth.ivecs)");
    }else if (dataset == "glove_sample") {
        dataset_root = "/home_bak/yq/WEAVESS/dataset/glove-100/";
        base_path = dataset_root;
        query_path = dataset_root;
        ground_path = dataset_root;
        base_path.append(R"(glove-100_sample_base.fvecs)");
        query_path.append(R"(glove-100_sample_query.fvecs)");
        ground_path.append(R"(glove-100_sample_groundtruth.ivecs)");
    }else {
        std::cout << "dataset input error!\n";
        exit(-1);
    }
    parameters.set<std::string>("base_path", base_path);
    parameters.set<std::string>("query_path", query_path);
    parameters.set<std::string>("ground_path", ground_path);
}

void set_para(std::string stg, std::string dataset, weavess::Parameters &parameters) {
    
    set_data_path(dataset, parameters);
    if (parameters.get<std::string>("exc_type") == "search") {
        return;
    }
    set_para(dataset, parameters);
}