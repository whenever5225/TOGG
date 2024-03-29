//
// Created by MurphySL on 2020/10/23.
//

#include "weavess/builder.h"
#include "weavess/component.h"
#include <set>
//#include "weavess/matplotlibcpp.h"

//namespace plt = matplotlibcpp;

namespace weavess {

    /**
     * load dataset and parameters
     * @param data_file *_base.fvecs
     * @param query_file *_query.fvecs
     * @param ground_file *_groundtruth.ivecs
     * @param parameters
     * @return pointer of builder
     */
    IndexBuilder *IndexBuilder::load(char *data_file, char *query_file, char *ground_file, Parameters &parameters) {
        auto *a = new ComponentLoad(final_index_);

        a->LoadInner(data_file, query_file, ground_file, parameters);

        std::cout << "base data len : " << final_index_->getBaseLen() << std::endl;
        std::cout << "base data dim : " << final_index_->getBaseDim() << std::endl;
        std::cout << "query data len : " << final_index_->getQueryLen() << std::endl;
        std::cout << "query data dim : " << final_index_->getQueryDim() << std::endl;
        std::cout << "ground truth data len : " << final_index_->getGroundLen() << std::endl;
        std::cout << "ground truth data dim : " << final_index_->getGroundDim() << std::endl;
        std::cout << "=====================" << std::endl;

        std::cout << final_index_->getParam().toString() << std::endl;
        std::cout << "=====================" << std::endl;

        return this;
    }

    /**
    * build init graph
    * @param type init type
    * @param debug Whether to output graph index information (will have a certain impact on performance)
    * @return pointer of builder
    */
    IndexBuilder *IndexBuilder::init(TYPE type, bool debug) {
        s = std::chrono::high_resolution_clock::now();
        ComponentInit *a = nullptr;

        if (type == INIT_RAND) {
            std::cout << "__INIT : RAND__" << std::endl;
            a = new ComponentInitRand(final_index_);
        } else if (type == INIT_KDT) {
            std::cout << "__INIT : KDT__" << std::endl;
            a = new ComponentInitKDT(final_index_);
            //a = new ComponentInitKDTree(final_index_);
        } else if (type == INIT_IEH) {
            std::cout << "__INIT : IEH__" << std::endl;
            a = new ComponentInitIEH(final_index_);
        } else if (type == INIT_NSW) {
            std::cout << "__INIT : NSW__" << std::endl;
            a = new ComponentInitNSW(final_index_);
        } else if (type == INIT_HNSW) {
            std::cout << "__INIT : HNSW__" << std::endl;
            a = new ComponentInitHNSW(final_index_);
        } else if (type == INIT_ANNG) {
            std::cout << "__INIT : ANNG__" << std::endl;
            a = new ComponentInitANNG(final_index_);
        } else if (type == INIT_SPTAG_KDT) {
            std::cout << "__INIT : SPTAG_KDT__" << std::endl;
            a = new ComponentInitSPTAG_KDT(final_index_);
        } else if (type == INIT_SPTAG_BKT) {
            std::cout << "__INIT : SPTAG_BKT__" << std::endl;
            a = new ComponentInitSPTAG_BKT(final_index_);
        } else if (type == INIT_IEH) {
            std::cout << "__INIT : IEH__" << std::endl;
            a = new ComponentInitIEH(final_index_);
        } else if (type == INIT_FANNG) {
            std::cout << "__INIT : FANNG__" << std::endl;
            a = new ComponentInitFANNG(final_index_);
        } else if (type == INIT_HCNNG) {
            std::cout << "__INIT : HCNNG__" << std::endl;
            a = new ComponentInitHCNNG(final_index_);
        } else if (type == INIT_RANDOM) {
            std::cout << "__INIT : RANDOM__" << std::endl;
            a = new ComponentInitRandom(final_index_);
        } else if (type == INIT_KNNG) {
            std::cout << "__INIT : KNNG__" << std::endl;
            a = new ComponentInitKNNG(final_index_);
        } else {
            std::cerr << "__INIT : WRONG TYPE__" << std::endl;
            exit(-1);
        }

        a->InitInner();

        // if (debug) {
        //     //print_graph();
        //     std::unordered_map<unsigned, unsigned> in_degree;
        //     std::unordered_map<unsigned, unsigned> out_degree;
        //     degree_info(in_degree, out_degree);
        //     conn_info();
        // }

        e = std::chrono::high_resolution_clock::now();

        std::cout << "__INIT FINISH__" << std::endl;

        return this;
    }

    /**
     * build refine graph
     * @param type refine type
     * @param debug
     * @return
     */
    IndexBuilder *IndexBuilder::refine(TYPE type, bool debug) {
        ComponentRefine *a = nullptr;

        if (type == REFINE_NN_DESCENT) {
            std::cout << "__REFINE : KGRAPH__" << std::endl;
            a = new ComponentRefineNNDescent(final_index_);
        } else if (type == REFINE_NSG) {
            std::cout << "__REFINE : NSG__" << std::endl;
            a = new ComponentRefineNSG(final_index_);
        } else if (type == REFINE_SSG) {
            std::cout << "__REFINE : NSSG__" << std::endl;
            a = new ComponentRefineSSG(final_index_);
        } else if (type == REFINE_DPG) {
            std::cout << "__REFINE : DPG__" << std::endl;
            a = new ComponentRefineDPG(final_index_);
        } else if (type == REFINE_VAMANA) {
            std::cout << "__REFINE : VAMANA__" << std::endl;
            a = new ComponentRefineVAMANA(final_index_);
        } else if (type == REFINE_EFANNA) {
            std::cout << "__REFINE : EFANNA__" << std::endl;
            a = new ComponentRefineEFANNA(final_index_);
        } else if (type == REFINE_SPTAG_BKT) {
            std::cout << "__REFINE : SPTAG_BKT__" << std::endl;
            a = new ComponentRefineSPTAG_BKT(final_index_);
        } else if (type == REFINE_SPTAG_KDT) {
            std::cout << "__REFINE : SPTAG_KDT__" << std::endl;
            a = new ComponentRefineSPTAG_KDT(final_index_);
        } else if (type == REFINE_FANNG) {
            std::cout << "__REFINE : FANNG__" << std::endl;
            a = new ComponentRefineFANNG(final_index_);
        } else if (type == REFINE_PANNG) {
            std::cout << "__REFINE : PANNG__" << std::endl;
            a = new ComponentRefinePANNG(final_index_);
        } else if (type == REFINE_ONNG) {
            std::cout << "__REFINE : ONNG__" << std::endl;
            a = new ComponentRefineONNG(final_index_);
        } else if (type == REFINE_KDRG) {
            std::cout << "__REFINE : KDRG__" << std::endl;
            a = new ComponentRefineKDRG(final_index_);
        } else if (type == REFINE_GUIDED) {
            std::cout << "__REFINE : GUIDED__" << std::endl;
            a = new ComponentRefineGuided(final_index_);
        } else if (type == REFINE_TOGGKDT) {
            std::cout << "__REFINE : TOGGKDT__" << std::endl;
            a = new ComponentRefineTOGGKDT(final_index_);
        } else if (type == REFINE_TOGGKMC) {
            std::cout << "__REFINE : TOGGKMC__" << std::endl;
            a = new ComponentRefineTOGGKMC(final_index_);
        } else {
            std::cerr << "__REFINE : WRONG TYPE__" << std::endl;
        }

        a->RefineInner();

        std::cout << "===================" << std::endl;
        std::cout << "__REFINE : FINISH__" << std::endl;
        std::cout << "===================" << std::endl;
        e = std::chrono::high_resolution_clock::now();
        // if (debug) {
        //     //print_graph();
        //     // degree
        //     std::unordered_map<unsigned, unsigned> in_degree;
        //     std::unordered_map<unsigned, unsigned> out_degree;
        //     degree_info(in_degree, out_degree);

        //     conn_info();
        // }

        return this;
    }

    /**
     * offline search
     * @param entry_type
     * @param route_type
     * @return
     */
    IndexBuilder *IndexBuilder::search(TYPE entry_type, TYPE route_type, TYPE L_type) {
        std::cout << "__SEARCH__" << std::endl;

        unsigned K = final_index_->getParam().get<unsigned>("search_K");
        // unsigned K = 100;

        final_index_->getParam().set<unsigned>("K_search", K);

        // std::vector<Index::Neighbor> pool;
        std::vector<std::vector<unsigned>> res;

        // ENTRY
        ComponentSearchEntry *a = nullptr;
        if (entry_type == SEARCH_ENTRY_RAND) {
            std::cout << "__SEARCH ENTRY : RAND__" << std::endl;
            a = new ComponentSearchEntryRand(final_index_);
        } else if (entry_type == SEARCH_ENTRY_CENTROID) {
            std::cout << "__SEARCH ENTRY : CENTROID__" << std::endl;
            a = new ComponentSearchEntryCentroid(final_index_);
        } else if (entry_type == SEARCH_ENTRY_SUB_CENTROID) {
            std::cout << "__SEARCH ENTRY : SUB_CENTROID__" << std::endl;
            a = new ComponentSearchEntrySubCentroid(final_index_);
        } else if (entry_type == SEARCH_ENTRY_KDT) {
            std::cout << "__SEARCH ENTRY : KDT__" << std::endl;
            a = new ComponentSearchEntryKDT(final_index_);
        } else if (entry_type == SEARCH_ENTRY_KDT_SINGLE) {
            std::cout << "__SEARCH ENTRY : KDT SINGLE__" << std::endl;
            a = new ComponentSearchEntryKDTSingle(final_index_);
        } else if (entry_type == SEARCH_ENTRY_NONE) {
            std::cout << "__SEARCH ENTRY : NONE__" << std::endl;
            a = new ComponentSearchEntryNone(final_index_);
        } else if (entry_type == SEARCH_ENTRY_HASH) {
            std::cout << "__SEARCH ENTRY : HASH__" << std::endl;
            a = new ComponentSearchEntryHash(final_index_);
        } else if (entry_type == SEARCH_ENTRY_VPT) {
            std::cout << "__SEARCH ENTRY : VPT__" << std::endl;
            a = new ComponentSearchEntryVPT(final_index_);
        } else if (entry_type == SEARCH_ENTRY_GUIDED_CENTROID) {
            std::cout << "__SEARCH ENTRY : GUIDED_CENTROID__" << std::endl;
            a = new ComponentSearchEntryGuidedCentroid(final_index_);
        } else if (entry_type == SEARCH_ENTRY_TOGGKDT_CENTROID) {
            std::cout << "__SEARCH ENTRY : TOGGKDT_CENTROID__" << std::endl;
            a = new ComponentSearchEntryTOGGKDTCentroid(final_index_);
        } else if (entry_type == SEARCH_ENTRY_TOGGKMC_CENTROID) {
            std::cout << "__SEARCH ENTRY : TOGGKMC_CENTROID__" << std::endl;
            a = new ComponentSearchEntryTOGGKMCCentroid(final_index_);
        } else {
            std::cerr << "__SEARCH ENTRY : WRONG TYPE__" << std::endl;
            exit(-1);
        }

        // ROUTE
        ComponentSearchRoute *b = nullptr;
        if (route_type == ROUTER_GREEDY) {
            std::cout << "__ROUTER : GREEDY__" << std::endl;
            b = new ComponentSearchRouteGreedy(final_index_);
        } else if (route_type == ROUTER_NSW) {
            std::cout << "__ROUTER : NSW__" << std::endl;
            b = new ComponentSearchRouteNSW(final_index_);
        } else if (route_type == ROUTER_HNSW) {
            std::cout << "__ROUTER : HNSW__" << std::endl;
            b = new ComponentSearchRouteHNSW(final_index_);
        } else if (route_type == ROUTER_IEH) {
            std::cout << "__ROUTER : IEH__" << std::endl;
            b = new ComponentSearchRouteIEH(final_index_);
        } else if (route_type == ROUTER_BACKTRACK) {
            std::cout << "__ROUTER : BACKTRACK__" << std::endl;
            b = new ComponentSearchRouteBacktrack(final_index_);
        } else if (route_type == ROUTER_GUIDE) {
            std::cout << "__ROUTER : GUIDED__" << std::endl;
            b = new ComponentSearchRouteGuided(final_index_);
        } else if (route_type == ROUTER_SPTAG_KDT) {
            std::cout << "__ROUTER : SPTAG_KDT__" << std::endl;
            b = new ComponentSearchRouteSPTAG_KDT(final_index_);
        } else if (route_type == ROUTER_SPTAG_BKT) {
            std::cout << "__ROUTER : SPTAG_BKT__" << std::endl;
            b = new ComponentSearchRouteSPTAG_BKT(final_index_);
        } else if (route_type == ROUTER_NGT) {
            std::cout << "__ROUTER : NGT__" << std::endl;
            b = new ComponentSearchRouteNGT(final_index_);
        } else if (route_type == ROUTER_TOGGKDT) {
            std::cout << "__ROUTER : TOGGKDT__" << std::endl;
            b = new ComponentSearchRouteTOGGKDT(final_index_);
        } else if (route_type == ROUTER_TOGGKMC) {
            std::cout << "__ROUTER : TOGGKMC__" << std::endl;
            b = new ComponentSearchRouteTOGGKMC(final_index_);
        } else {
            std::cerr << "__ROUTER : WRONG TYPE__" << std::endl;
            exit(-1);
        }

        if (L_type == L_SEARCH_SET_RECALL) {
            std::set<unsigned> visited;
            unsigned sg = 100000;
            float acc_set = 0.99;
            bool flag = false;
            int L_sl = 1;
            unsigned L = 1000;
            visited.insert(L);
            unsigned L_min = 0x7fffffff;
            while (true) {
                // unsigned L_pre = L;
                std::cout << "SEARCH_L : " << L << std::endl;
                if (L < K) {
                    std::cout << "search_L cannot be smaller than search_K! " << std::endl;
                    exit(-1);
                }

                final_index_->getParam().set<unsigned>("L_search", L);

                auto s1 = std::chrono::high_resolution_clock::now();

                res.clear();
                res.resize(final_index_->getBaseLen());
#pragma omp parallel for
                for (unsigned i = 0; i < final_index_->getQueryLen(); i++) {
                    // pool.clear();
                    // if (i == 5070) continue; // only for hnsw search on glove-100
                    std::vector<Index::Neighbor> pool;

                    a->SearchEntryInner(i, pool);

                    b->RouteInner(i, pool, res[i]);

                }

                auto e1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = e1 - s1;
                // std::cout << "search time: " << diff.count() << "\n";

                //float speedup = (float)(index_->n_ * query_num) / (float)distcount;
                std::cout << "DistCount: " << final_index_->getDistCount() << std::endl;
                // std::cout << "HopCount: " << final_index_->getHopCount() << std::endl;
                final_index_->resetDistCount();
                final_index_->resetHopCount();

                int cnt = 0;
                for (unsigned i = 0; i < final_index_->getGroundLen(); i++) {
                    if (res[i].size() == 0) continue;
                    for (unsigned j = 0; j < K; j++) {
                        unsigned k = 0;
                        for (; k < K; k++) {
                            if (res[i][j] == final_index_->getGroundData()[i * final_index_->getGroundDim() + k])
                                break;
                        }
                        if (k == K)
                            cnt++;
                    }
                }

                float acc = 1 - (float) cnt / (final_index_->getGroundLen() * K);
                std::cout << K << " NN accuracy: " << acc << std::endl;
                if (acc_set - acc <= 0) {
                    if (L_min > L) L_min = L;
                    // if (L == K || L_sl == 1) {
                    if (L_sl == 1) {
                        break;
                    } else {
                        if (flag == false) {
                            L_sl < 0 ? L_sl-- : L_sl++;
                            flag = true;
                        }

                        L_sl /= 2;

                        if (L_sl == 0) {
                            break;
                        }
                        L_sl < 0 ? L_sl : L_sl = -L_sl;
                    }
                }else {
                    if (L_min < L) break;
                    L_sl = (int)(sg * (acc_set - acc));
                    if (L_sl == 0) L_sl++;
                    flag = false;
                }
                L += L_sl;
                if (visited.count(L)) {
                    break;
                }else {
                    visited.insert(L);
                }
            }
            std::cout << "L_min: " << L_min << std::endl;
        }else if (L_type == L_SEARCH_ASCEND) {
            unsigned L_st = 1;
            unsigned L_st2 = K;
            unsigned iter_num = 10;
            for (unsigned i = 0; i < iter_num; i++) {
                unsigned L = L_st + L_st2;
                L_st = L_st2;
                L_st2 = L;
                std::cout << "SEARCH_L : " << L << std::endl;
                if (L < K) {
                    std::cout << "search_L cannot be smaller than search_K! " << std::endl;
                    exit(-1);
                }

                final_index_->getParam().set<unsigned>("L_search", L);

                auto s1 = std::chrono::high_resolution_clock::now();

                res.clear();
                res.resize(final_index_->getBaseLen());
                final_index_->resetDistCount();
                final_index_->resetHopCount();

                for (unsigned i = 0; i < final_index_->getQueryLen(); i++) {
                    //std::cout << i << std::endl;
                    std::vector<Index::Neighbor> pool;

                    // std::cout << "test0\n";
                    a->SearchEntryInner(i, pool);
                    // std::cout << "test1\n";

//                    for(int i = 0; i < pool.size(); i ++) {
//                        std::cout << pool[i].id << "|" << pool[i].distance << " ";
//                    }
//                    std::cout << std::endl;
//                    std::cout << "entry finish" << std::endl;

                    b->RouteInner(i, pool, res[i]);
                    // std::cout << "test2\n";
                }

                auto e1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = e1 - s1;
                std::cout << "search time: " << diff.count() << "\n";

                //float speedup = (float)(index_->n_ * query_num) / (float)distcount;
                std::cout << "DistCount: " << final_index_->getDistCount() << std::endl;
                std::cout << "HopCount: " << final_index_->getHopCount() << std::endl;
                //结果评估
                int cnt = 0;
                unsigned test_K = K;
                for (unsigned i = 0; i < final_index_->getGroundLen(); i++) {
                    for (unsigned j = 0; j < test_K; j++) {
                        unsigned k = 0;
                        for (; k < test_K; k++) {
                            if (res[i][j] == final_index_->getGroundData()[i * final_index_->getGroundDim() + k])
                                break;
                        }
                        if (k == test_K)
                            cnt++;
                    }

                    //                for(unsigned j = 0; j < K; j ++) {
                    //                    std::cout << res[i][j] << " ";
                    //                }
                    //                std::cout << std::endl;
                    //                for(unsigned j = 0; j < K; j ++) {
                    //                    std::cout << final_index_->getGroundData()[i * final_index_->getGroundDim() + j] << " ";
                    //                }
                    //                std::cout << std::endl;
                }

                float acc = 1 - (float) cnt / (final_index_->getGroundLen() * K);
                std::cout << test_K << " NN accuracy: " << acc << std::endl;
                // if (acc > 0.99) break;
                // else if (i == iter_num - 1 && (float)(final_index_->getBaseLen() * final_index_->getQueryLen() / final_index_->getDistCount()) > 10) iter_num++;
            }
        } else if (L_type == L_SEARCH_ASSIGN) {

            unsigned L = final_index_->getParam().get<unsigned>("L_search");
            std::cout << "SEARCH_L : " << L << std::endl;
            if (L < K) {
                std::cout << "search_L cannot be smaller than search_K! " << std::endl;
                exit(-1);
            }

            auto s1 = std::chrono::high_resolution_clock::now();

            res.clear();
            res.resize(final_index_->getBaseLen());

            for (unsigned i = 0; i < final_index_->getQueryLen(); i++) {
                // pool.clear();
                // if (i == 5070) continue; // only for hnsw search on glove-100
                std::vector<Index::Neighbor> pool;

                a->SearchEntryInner(i, pool);

                b->RouteInner(i, pool, res[i]);

            }

            auto e1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = e1 - s1;
            std::cout << "search time: " << diff.count() << "\n";

            //float speedup = (float)(index_->n_ * query_num) / (float)distcount;
            std::cout << "DistCount: " << final_index_->getDistCount() << std::endl;
            std::cout << "HopCount: " << final_index_->getHopCount() << std::endl;
            final_index_->resetDistCount();
            final_index_->resetHopCount();
            int cnt = 0;
            for (unsigned i = 0; i < final_index_->getGroundLen(); i++) {
                if (res[i].size() == 0) continue;
                for (unsigned j = 0; j < K; j++) {
                    unsigned k = 0;
                    for (; k < K; k++) {
                        if (res[i][j] == final_index_->getGroundData()[i * final_index_->getGroundDim() + k])
                            break;
                    }
                    if (k == K)
                        cnt++;
                }
            }

            float acc = 1 - (float) cnt / (final_index_->getGroundLen() * K);
            std::cout << K << " NN accuracy: " << acc << std::endl;
        }

        e = std::chrono::high_resolution_clock::now();
        std::cout << "__SEARCH FINISH__" << std::endl;

        return this;
    }

    void IndexBuilder::print_graph() {
        std::cout << "=====================" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << i << " : " << final_index_->getFinalGraph()[i].size() << std::endl;
            for (int j = 0; j < final_index_->getFinalGraph()[i].size(); j++) {
                std::cout << final_index_->getFinalGraph()[i][j].id << "|"
                          << final_index_->getFinalGraph()[i][j].distance << " ";
            }
            std::cout << std::endl;
        }
    }

    IndexBuilder *IndexBuilder::print_index_info(TYPE type) {
        std::unordered_map<unsigned, unsigned> in_degree;
        std::unordered_map<unsigned, unsigned> out_degree;
        degree_info(in_degree, out_degree, type);
        conn_info(type);
        graph_quality(type);
        return this;

    }

    void IndexBuilder::degree_info(std::unordered_map<unsigned, unsigned> &in_degree,
                                   std::unordered_map<unsigned, unsigned> &out_degree, TYPE type) {
        unsigned max_out_degree = 0, min_out_degree = 1e6;
        double avg_out_degree = 0.0;

        unsigned max_in_degree = 0, min_in_degree = 1e6;
        double avg_in_degree = 0.0;

        if (type == INDEX_HNSW || type == INDEX_NSW) {
            for (size_t i = 0; i < final_index_->getBaseLen(); i++) {
                auto size = (unsigned) final_index_->nodes_[i]->GetFriends(0).size();
                out_degree[size]++;
                max_out_degree = max_out_degree < size ? size : max_out_degree;
                min_out_degree = min_out_degree > size ? size : min_out_degree;
                avg_out_degree += size;
            }
        }else if (type == INDEX_HCNNG) {
            for (size_t i = 0; i < final_index_->getBaseLen(); i++) {
                auto size = final_index_->Tn[i].left.size() + final_index_->Tn[i].right.size();
                out_degree[size]++;
                max_out_degree = max_out_degree < size ? size : max_out_degree;
                min_out_degree = min_out_degree > size ? size : min_out_degree;
                avg_out_degree += size;
            }
        }else {
            for (size_t i = 0; i < final_index_->getBaseLen(); i++) {
                unsigned size;
                if (final_index_->getParam().get<std::string>("exc_type") == "build") {
                    size = final_index_->getFinalGraph()[i].size();
                }else {
                    size = final_index_->getLoadGraph()[i].size();
                }
                out_degree[size]++;
                max_out_degree = max_out_degree < size ? size : max_out_degree;
                min_out_degree = min_out_degree > size ? size : min_out_degree;
                avg_out_degree += size;

                // for (unsigned j = 0; j < final_index_->getFinalGraph()[i].size(); j++) {
                //     in_degree[final_index_->getFinalGraph()[i][j].id]++;
                //     max_in_degree = max_in_degree < in_degree[final_index_->getFinalGraph()[i][j].id]
                //                     ? in_degree[final_index_->getFinalGraph()[i][j].id] : max_in_degree;
                //     min_in_degree = min_in_degree > in_degree[final_index_->getFinalGraph()[i][j].id]
                //                     ? in_degree[final_index_->getFinalGraph()[i][j].id] : min_in_degree;
                // }
            }
        }

        // for (auto it : in_degree) {
        //     avg_in_degree += it.second;
        // }

        avg_out_degree /= final_index_->getBaseLen();
        // avg_in_degree /= final_index_->getBaseLen();
        printf("Degree Statistics: Max out degree = %d, Min out degree= %d, Avg out degree = %lf\n", max_out_degree,
               min_out_degree, avg_out_degree);
        // printf("Degree Statistics: Max in degree = %d, Min in degree= %d, Avg in degree = %lf\n", max_in_degree,
        //        min_in_degree, avg_in_degree);
    }

    void IndexBuilder::conn_info(TYPE type) {
        std::vector<unsigned> root{0};
        if (type == INDEX_NSG || type == INDEX_VAMANA) {
            root[0] = final_index_->ep_;
        }else if (type == INDEX_SSG) {
            root.resize(final_index_->eps_.size());
            for (unsigned i = 0; i < final_index_->eps_.size(); i++) {
                root[i] = final_index_->eps_[i];
            }
        }else if (type == INDEX_HNSW) {
            root[0] = final_index_->enterpoint_->GetId();
        }
        
        boost::dynamic_bitset<> flags{final_index_->getBaseLen(), 0};

        unsigned conn = 0;
        unsigned unlinked_cnt = 0;

        while (unlinked_cnt < final_index_->getBaseLen()) {
            conn++;
            for (unsigned i = 0; i < root.size(); i++) {
                DFS(flags, root[i], unlinked_cnt, type);
            }
            if (unlinked_cnt >= final_index_->getBaseLen()) break;
            root.clear();
            findRoot(flags, root);
        }
        printf("Conn Statistics: conn = %d\n", conn);
    }

    void IndexBuilder::findRoot(boost::dynamic_bitset<> &flag, std::vector<unsigned> &root) {
        unsigned id = final_index_->getBaseLen();
        for (unsigned i = 0; i < final_index_->getBaseLen(); i++) {
            if (!flag[i]) {
                id = i;
                break;
            }
        }
        root.push_back(id);
    }

    void IndexBuilder::DFS(boost::dynamic_bitset<> &flag, unsigned root, unsigned &cnt, TYPE type) {
        if (type == INDEX_HNSW) {   // BFS
            weavess::HNSW::HnswNode *tmp = final_index_->nodes_[root];
            std::queue<weavess::HNSW::HnswNode *> q;
            q.push(tmp);
            if (!flag[tmp->GetId()]) cnt++;
            flag[tmp->GetId()] = true;
            
            while (!q.empty()) {
                tmp = q.front();
                q.pop();
                for (int l = tmp->GetLevel() - 1; l >= 0; l--) {
                    for (size_t i = 0; i < tmp->GetFriends(l).size(); i ++) {
                        if (!flag[(unsigned)tmp->GetFriends(l)[i]->GetId()]) {
                            flag[(unsigned)tmp->GetFriends(l)[i]->GetId()] = true;
                            cnt++;
                            q.push(tmp->GetFriends(l)[i]);
                        }
                    }
                }
            }
            return;
        }else if (type == INDEX_NSW) {
            weavess::HNSW::HnswNode *tmp = final_index_->nodes_[root];
            std::queue<weavess::HNSW::HnswNode *> q;
            q.push(tmp);
            if (!flag[tmp->GetId()]) cnt++;
            flag[tmp->GetId()] = true;
            
            while (!q.empty()) {
                tmp = q.front();
                q.pop();
                for (size_t i = 0; i < tmp->GetFriends(0).size(); i ++) {
                    if (!flag[(unsigned)tmp->GetFriends(0)[i]->GetId()]) {
                        flag[(unsigned)tmp->GetFriends(0)[i]->GetId()] = true;
                        cnt++;
                        q.push(tmp->GetFriends(0)[i]);
                    }
                }
            }
            return;
        }
        
        unsigned tmp = root;
        std::stack<unsigned> s;
        s.push(root);
        if (!flag[root]) cnt++;
        flag[root] = true;
        
        while (!s.empty()) {
            unsigned next = final_index_->getBaseLen() + 1;
            if (type == INDEX_HCNNG) {
                bool lf_flag = false;
                for (unsigned i = 0; i < final_index_->Tn[tmp].left.size(); i++) {
                    if (!flag[final_index_->Tn[tmp].left[i]]) {
                        next = final_index_->Tn[tmp].left[i];
                        lf_flag = true;
                        break;
                    }
                }
                if (!lf_flag) {
                    for (unsigned i = 0; i < final_index_->Tn[tmp].right.size(); i++) {
                        if (!flag[final_index_->Tn[tmp].right[i]]) {
                            next = final_index_->Tn[tmp].right[i];
                            break;
                        }
                    }
                }
            }else {
                if (final_index_->getParam().get<std::string>("exc_type") == "build") {
                    for (unsigned i = 0; i < final_index_->getFinalGraph()[tmp].size(); i++) {
                        if (!flag[final_index_->getFinalGraph()[tmp][i].id]) {
                            next = final_index_->getFinalGraph()[tmp][i].id;
                            break;
                        }
                    }
                }else {
                    for (unsigned i = 0; i < final_index_->getLoadGraph()[tmp].size(); i++) {
                        if (!flag[final_index_->getLoadGraph()[tmp][i]]) {
                            next = final_index_->getLoadGraph()[tmp][i];
                            break;
                        }
                    }
                }
            }

            // std::cout << next <<":"<<cnt <<":"<<tmp <<":"<<s.size()<< '\n';
            if (next == (final_index_->getBaseLen() + 1)) {
                s.pop();
                if (s.empty()) break;
                tmp = s.top();
                continue;
            }
            tmp = next;
            flag[tmp] = true;
            s.push(tmp);
            cnt++;
        }
    }

    /**
     * graph quality analyst
     */
    void IndexBuilder::graph_quality(TYPE type) {
        std::string exact_knng_path(final_index_->getParam().get<std::string>("exact_knng_path"));
        load_graph(weavess::TYPE::INDEX_EXACT_KNNG, &exact_knng_path[0]);   // load exact knng
        float mean_quality = 0;
        unsigned control_point_num = 10;

        if (type == INDEX_HNSW || type == INDEX_NSW) {
            for (unsigned i = 0; i < final_index_->nodes_.size(); i++) {
                float quality = 0;
                auto &g = final_index_->nodes_[i];
                auto &v = final_index_->getExactGraph()[(unsigned)g->GetId()];
                if (v[0] == (unsigned)g->GetId()) v.erase(v.begin());  // skip itself
                for (unsigned j = 0; j < control_point_num && j < v.size(); j++) {
                    for (unsigned k = 0; k < g->GetFriends(0).size(); k++) {
                        if ((unsigned)g->GetFriends(0)[k]->GetId() == v[j]) {
                            quality++;
                            break;
                        }
                    }
                }
                mean_quality += quality / std::min(std::min(control_point_num, (unsigned)g->GetFriends(0).size()), (unsigned)v.size());
            }
            std::cout << "Graph Quality : " << mean_quality / final_index_->nodes_.size() <<std::endl;
        }else if (type == INDEX_HCNNG) {
            for (unsigned i = 0; i < final_index_->Tn.size(); i++){
                unsigned quality = 0;
                auto &g = final_index_->Tn[i];
                auto &v = final_index_->getExactGraph()[i];
                if (v[0] == i) v.erase(v.begin());
                for (unsigned j = 0; j < control_point_num && j < v.size(); j++) {
                    for (unsigned k = 0; k < g.left.size(); k++) {
                        if (g.left[k] == v[j]) {
                        quality++;
                        break;
                        }
                    }
                    for (unsigned k = 0; k < g.right.size(); k++) {
                        if (g.right[k] == v[j]) {
                        quality++;
                        break;
                        }
                    }
                }
                mean_quality += (float)quality / (float)std::min(std::min(control_point_num, (unsigned)(g.left.size() + g.right.size())), (unsigned)v.size());
            }
            std::cout << "Graph Quality : " << mean_quality / final_index_->Tn.size() <<std::endl;
        }else {
            for (unsigned i = 0; i < final_index_->getLoadGraph().size(); i++){
                unsigned quality = 0;
                auto &g = final_index_->getLoadGraph()[i];
                auto &v = final_index_->getExactGraph()[i];
                if (v[0] == i) v.erase(v.begin());
                for (unsigned j = 0; j < control_point_num && j < v.size(); j++) {
                    for (unsigned k = 0; k < g.size(); k++) {
                        if (g[k] == v[j]) {
                        quality++;
                        break;
                        }
                    }
                }
                mean_quality += (float)quality / (float)std::min(std::min(control_point_num, (unsigned)g.size()), (unsigned)v.size());
            }
            std::cout << "Graph Quality : " << mean_quality / final_index_->getLoadGraph().size() <<std::endl;
        }
    }


    /**
    * peak_memory（VmPeak）
    */
    void IndexBuilder::peak_memory_footprint() {

        unsigned iPid = (unsigned)getpid();

        std::cout<<"PID: "<<iPid<<std::endl;

        std::string status_file = "/proc/" + std::to_string(iPid) + "/status";
        std::ifstream info(status_file);
        if (!info.is_open()) {
            std::cout << "memory information open error!" << std::endl;
        }
        std::string tmp;
        while(getline(info, tmp)) {
            if (tmp.find("Name:") != std::string::npos || tmp.find("VmPeak:") != std::string::npos || tmp.find("VmHWM:") != std::string::npos)
            std::cout << tmp << std::endl;
        }
        info.close();
        
    }

    int DepthFirstWrite(std::fstream& out, struct Index::Node *root){
        if(root==nullptr) return 0;
        int left_cnt = DepthFirstWrite(out, root->Lchild);
        int right_cnt = DepthFirstWrite(out, root->Rchild);

        //std::cout << root->StartIdx <<":" << root->EndIdx<< std::endl;
        out.write((char *)&(root->DivDim), sizeof(root->DivDim));
        out.write((char *)&(root->DivVal), sizeof(root->DivVal));
        out.write((char *)&(root->StartIdx), sizeof(root->StartIdx));
        out.write((char *)&(root->EndIdx), sizeof(root->EndIdx));
        out.write((char *)&(root->Lchild), sizeof(root->Lchild));
        out.write((char *)&(root->Rchild), sizeof(root->Rchild));
        return (left_cnt + right_cnt + 1);
    }

    struct Index::Node* DepthFirstBuildTree(std::vector<struct Index::Node *>& tree_nodes){
        std::vector<Index::Node*> root_serial;
        typename std::vector<struct Index::Node*>::iterator it = tree_nodes.begin();
        for( ; it!=tree_nodes.end(); it++){
            Index::Node* tmp = *it;
            size_t rsize = root_serial.size();
            if(rsize<2){
                root_serial.push_back(tmp);
                //continue;
            }
            else{
                Index::Node *last1 = root_serial[rsize-1];
                Index::Node *last2 = root_serial[rsize-2];
                if(last1->EndIdx == tmp->EndIdx && last2->StartIdx == tmp->StartIdx){
                    tmp->Rchild = last1;
                    tmp->Lchild = last2;
                    root_serial.pop_back();
                    root_serial.pop_back();
                }
                root_serial.push_back(tmp);
            }

        }
        if(root_serial.size()!=1){
            std::cout << "Error constructing trees" << std::endl;
            return NULL;
        }
        return root_serial[0];
    }

    Index::VPNodePtr deser(std::ifstream &in) {
        Index::VPNodePtr new_node(new Index::VPNodeType);

        bool m_leaf_node;
        in.read((char*)&m_leaf_node, sizeof(bool));
        new_node->set_leaf_node(m_leaf_node);

        if(m_leaf_node) {
            unsigned objects_size;
            in.read((char*)&objects_size, sizeof(unsigned));
            new_node->m_objects_list.resize(objects_size);
            in.read((char*)new_node->m_objects_list.data(), sizeof(unsigned) * objects_size);
        } else {
            size_t m_branches_count;
            in.read((char*)&m_branches_count, sizeof(size_t));
            new_node->set_branches_count(m_branches_count);
            unsigned m_value;
            in.read((char*)&m_value, sizeof(unsigned));
            new_node->set_value(m_value);

            unsigned mu_size;
            in.read((char*)&mu_size, sizeof(unsigned));
            new_node->m_mu_list.resize(mu_size);
            std::vector<float> test(mu_size);
            in.read((char*)(new_node->m_mu_list.data()), sizeof(float) * mu_size);

            unsigned child_size;
            in.read((char*)&child_size, sizeof(unsigned));
            new_node->m_child_list.reserve(child_size);
            while(child_size --) {
                Index::VPNodePtr node = deser(in);
                new_node->m_child_list.push_back(node);
            }
        }

        return new_node;
    }

    int Inorder(std::fstream& out, Index::VPNodePtr root) {
        bool m_leaf_node = root->get_leaf_node();
        out.write((char *)&m_leaf_node, sizeof(bool));

        if(m_leaf_node) {
            unsigned objects_size = root->m_objects_list.size();
            out.write((char *)&objects_size, sizeof(unsigned));
            out.write((char *)root->m_objects_list.data(), objects_size * sizeof (unsigned));
            return 1;
        } else {
            size_t m_branches_count = root->get_branches_count();
            unsigned m_value = root->get_value();
            out.write((char *)&m_branches_count, sizeof(size_t));
            out.write((char *)&m_value, sizeof(unsigned));

            unsigned mu_size = root->m_mu_list.size();
            out.write((char *)&mu_size, sizeof(unsigned));
            out.write((char *) root->m_mu_list.data(), mu_size * sizeof(float));

            unsigned child_size = root->m_child_list.size();
            out.write((char *)&child_size, sizeof(unsigned));

            int cnt = 0;
            for(int i = 0; i < child_size; i ++ ) {
                cnt += Inorder(out, root->m_child_list[i]);
            }

            return cnt + 1;
        }
    }

    IndexBuilder *IndexBuilder::save_graph(TYPE type, char *graph_file) {
        std::fstream out(graph_file, std::ios::binary | std::ios::out);
        out.write((char *) &final_index_->ep_, sizeof(unsigned));
        if (type == INDEX_STG1 || type == INDEX_STG2 || type == INDEX_STG4) {
            for (unsigned i = 0; i < final_index_->getBaseLen(); i++) {
                unsigned GK = (unsigned) final_index_->getFinalGraph()[i].size();
                std::vector<unsigned> tmp;
                for (unsigned j = 0; j < GK; j++) {
                    tmp.push_back(final_index_->getFinalGraph()[i][j].id);
                }
                out.write((char *) &GK, sizeof(unsigned));
                out.write((char *) tmp.data(), GK * sizeof(unsigned));
            }
        }
        else if (type == INDEX_STG3) {
            assert(final_index_->Tn.size() == final_index_->getBaseLen());
            for (unsigned i = 0; i < final_index_->getBaseLen(); i++) {
                out.write((char *)&(final_index_->Tn[i].div_dim), sizeof(unsigned));
                unsigned left_len = (unsigned)final_index_->Tn[i].left.size();
                unsigned right_len = (unsigned)final_index_->Tn[i].right.size();
                out.write((char *)&left_len, sizeof(unsigned));
                out.write((char *)&right_len, sizeof(unsigned));
                out.write((char *)final_index_->Tn[i].left.data(), left_len * sizeof(unsigned));
                out.write((char *)final_index_->Tn[i].right.data(), right_len * sizeof(unsigned));
            }
            return this;
        }
        else if (type == INDEX_TOGGKDT) {
            assert(final_index_->Tn.size() == final_index_->getBaseLen());
            out.write((char*)&final_index_->r_proportion, sizeof(float));
            out.write((char*)final_index_->range_, final_index_->getGroundDim() * sizeof(float));
            for (unsigned i = 0; i < final_index_->getBaseLen(); i++) {
                out.write((char *)&(final_index_->Tn[i].div_dim), sizeof(unsigned));
                unsigned left_len = (unsigned)final_index_->Tn[i].left.size();
                unsigned common_len = (unsigned)final_index_->Tn[i].common.size();
                unsigned right_len = (unsigned)final_index_->Tn[i].right.size();
                out.write((char *)&left_len, sizeof(unsigned));
                out.write((char *)&common_len, sizeof(unsigned));
                out.write((char *)&right_len, sizeof(unsigned));
                out.write((char *)final_index_->Tn[i].left.data(), left_len * sizeof(unsigned));
                out.write((char *)final_index_->Tn[i].common.data(), common_len * sizeof(unsigned));
                out.write((char *)final_index_->Tn[i].right.data(), right_len * sizeof(unsigned));
            }
        }
        else if (type == INDEX_TOGGKMC) {
            for (unsigned i = 0; i < final_index_->getBaseLen(); i++) {
                unsigned cluster_num_ = final_index_->nc_[i].size();
                out.write((char *)&cluster_num_, sizeof(unsigned));
                for (unsigned j = 0; j < cluster_num_; j++) {
                    unsigned cnum = final_index_->nc_[i][j].size();
                    out.write((char *)&cnum, sizeof(unsigned));
                    out.write((char *)final_index_->nc_[i][j].data(), cnum * sizeof(unsigned));
                }
            }
        }
        out.close();

        //std::vector<std::vector<Index::SimpleNeighbor>>().swap(final_index_->getFinalGraph());
        return this;
    }

    IndexBuilder *IndexBuilder::load_graph(TYPE type, char *graph_file) {
        std::ifstream in(graph_file, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "load graph error" << std::endl;
            exit(-1);
        }
        in.read((char *) &final_index_->ep_, sizeof(unsigned));
        if (type == INDEX_STG1 || type == INDEX_STG2 || type == INDEX_STG4) {
            while (!in.eof()) {
                unsigned GK;
                in.read((char *) &GK, sizeof(unsigned));
                if (in.eof()) break;
                std::vector<unsigned> tmp(GK);
                in.read((char *) tmp.data(), GK * sizeof(unsigned));
                final_index_->getLoadGraph().push_back(tmp);
            }
        }
        else if (type == INDEX_STG3) {
            while (!in.eof()) {
                unsigned left_len, right_len;
                Index::Tnode tmp;
                in.read((char *)&tmp.div_dim, sizeof(unsigned));
                in.read((char *)&left_len, sizeof(unsigned));
                in.read((char *)&right_len, sizeof(unsigned));
                if (in.eof()) break;
                tmp.left.resize(left_len);
                tmp.right.resize(right_len);
                in.read((char *)tmp.left.data(), left_len * sizeof(unsigned));
                in.read((char *)tmp.right.data(), right_len * sizeof(unsigned));
                final_index_->Tn.push_back(tmp);
            }
        }
        else if (type == INDEX_TOGGKDT) {
            in.read((char*)&final_index_->r_proportion, sizeof(float));
            final_index_->range_ = new float[(size_t)final_index_->getBaseDim()];
            in.read((char*)final_index_->range_, final_index_->getGroundDim() * sizeof(float));
            while (!in.eof()) {
                unsigned left_len, right_len, common_len;
                Index::Tnode tmp;
                in.read((char *)&tmp.div_dim, sizeof(unsigned));
                in.read((char *)&left_len, sizeof(unsigned));
                in.read((char *)&common_len, sizeof(unsigned));
                in.read((char *)&right_len, sizeof(unsigned));
                if (in.eof()) break;
                tmp.left.resize(left_len);
                tmp.common.resize(common_len);
                tmp.right.resize(right_len);
                in.read((char *)tmp.left.data(), left_len * sizeof(unsigned));
                in.read((char *)tmp.common.data(), common_len * sizeof(unsigned));
                in.read((char *)tmp.right.data(), right_len * sizeof(unsigned));
                final_index_->Tn.push_back(tmp);
            }
        }
        else if (type == INDEX_TOGGKMC) {
            while (!in.eof()) {
                std::vector < std::vector <unsigned> > tmp;
                unsigned cluster_num_;
                in.read((char *)&cluster_num_, sizeof(unsigned));
                tmp.resize(cluster_num_);
                for (unsigned j = 0; j < cluster_num_; j++) {
                unsigned cnum;
                in.read((char *)&cnum, sizeof(unsigned));
                tmp[j].resize(cnum);
                in.read((char *)tmp[j].data(), cnum * sizeof(unsigned));
                }
                final_index_->nc_.push_back(tmp);
            }
        }
        std::cout << "load graph successfully!" << std::endl;

        return this;
    }


}
