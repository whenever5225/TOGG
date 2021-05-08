#include <weavess/builder.h>
#include <weavess/exp_data.h>
#include <iostream>

void STG1(weavess::Parameters parameters) { //sigkdd2011

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false)
                -> save_graph(weavess::TYPE::INDEX_STG1, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_STG1, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_CENTROID, weavess::TYPE::ROUTER_NGT, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }
}

void STG2(weavess::Parameters parameters) {

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false)
                -> save_graph(weavess::TYPE::INDEX_STG2, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_STG2, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_CENTROID, weavess::TYPE::ROUTER_BACKTRACK, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }

}

void STG3(weavess::Parameters parameters) {

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false);
        std::cout << "Original cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_GUIDED,false)
                -> save_graph(weavess::TYPE::INDEX_STG3, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_STG3, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_GUIDED_CENTROID, weavess::TYPE::ROUTER_GUIDE, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }
}

void STG4(weavess::Parameters parameters) {

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false)
                -> save_graph(weavess::TYPE::INDEX_STG4, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_STG4, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_CENTROID, weavess::TYPE::ROUTER_GREEDY, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }
}

void TOGG_KDT(weavess::Parameters parameters) {

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false);
        std::cout << "Original cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_TOGGKDT,false)
                -> save_graph(weavess::TYPE::INDEX_TOGGKDT, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_TOGGKDT, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_TOGGKDT_CENTROID, weavess::TYPE::ROUTER_TOGGKDT, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }
}

void TOGG_KMC(weavess::Parameters parameters) {

    const unsigned num_threads = parameters.get<unsigned>("n_threads");
    std::string base_path = parameters.get<std::string>("base_path");
    std::string query_path = parameters.get<std::string>("query_path");
    std::string ground_path = parameters.get<std::string>("ground_path");
    std::string graph_file = parameters.get<std::string>("graph_file");
    auto *builder = new weavess::IndexBuilder(num_threads);

    if (parameters.get<std::string>("exc_type") == "build") {   // build
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> init(weavess::INIT_RANDOM)
                -> refine(weavess::REFINE_NN_DESCENT, false);
        std::cout << "Init cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_NSG, false);
        std::cout << "Original cost: " << builder->GetBuildTime().count() << std::endl;
        builder -> refine(weavess::REFINE_TOGGKMC,false)
                -> save_graph(weavess::TYPE::INDEX_TOGGKMC, &graph_file[0]);
        std::cout << "Build cost: " << builder->GetBuildTime().count() << std::endl;
    }else if (parameters.get<std::string>("exc_type") == "search") {    // search
        builder -> load(&base_path[0], &query_path[0], &ground_path[0], parameters)
                -> load_graph(weavess::TYPE::INDEX_TOGGKMC, &graph_file[0])
                -> search(weavess::TYPE::SEARCH_ENTRY_TOGGKMC_CENTROID, weavess::TYPE::ROUTER_TOGGKMC, weavess::TYPE::L_SEARCH_ASSIGN);
        builder -> peak_memory_footprint();
    }else {
        std::cout << "exc_type input error!" << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 4 || argc > 6) {
        std::cout << "./evaluation strategy dataset exc_type [L_search]"
                << std::endl;
        exit(-1);
    }
    weavess::Parameters parameters;
    std::string dataset_root = R"(/home_bak/yq/ANNS/dataset/)";
    parameters.set<std::string>("dataset_root", dataset_root);
    parameters.set<unsigned>("n_threads", 16);
    std::string stg(argv[1]);
    std::string dataset(argv[2]);
    std::string exc_type(argv[3]);
    if (argc == 5) {
        if (stg == "togg-kdt" && exc_type == "build") {
            float P = (float)atof(argv[4]);
            parameters.set<float>("P", P);
        } else if (stg == "togg-kmc" && exc_type == "build") {
            unsigned CN = (unsigned)atoi(argv[4]);
            parameters.set<float>("CN", CN); 
        }
        else {
            unsigned L_search = (unsigned)atoi(argv[4]);
            parameters.set<unsigned>("L_search", L_search);
        }
    } else if (argc == 6) {
        unsigned search_K = (unsigned)atoi(argv[4]);
        parameters.set<unsigned>("search_K", search_K);
        unsigned L_search = (unsigned)atoi(argv[5]);
        parameters.set<unsigned>("L_search", L_search);
    }
    std::cout << "strategy: " << stg << std::endl;
    std::cout << "dataset: " << dataset << std::endl;
    std::string graph_file(stg + "_" + dataset + ".graph");
    parameters.set<std::string>("graph_file", graph_file);
    parameters.set<std::string>("exc_type", exc_type);
    set_para(stg, dataset, parameters);

    // stg
    if (stg == "sigkdd2011") {
        STG1(parameters);
    }else if (stg == "cvpr2016") {
        STG2(parameters);
    }else if (stg == "pr2019") {
        STG3(parameters);
    }else if (stg == "tpami2021") {
        STG4(parameters);
    }else if (stg == "togg-kdt") {
        TOGG_KDT(parameters);
    }else if (stg == "togg-kmc") {
        TOGG_KMC(parameters);
    }else {
        std::cout << "alg input error!\n";
        exit(-1);
    }
    return 0;
}
