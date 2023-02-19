#pragma once

#include <iostream>

#include "gtest/gtest.h"

#include "../src/blowfish/defs.h"

#include "../src/mcts_new/mcts_nodemodel.h"
#include "../src/mcts_new/mcts_node_expander.h"


TEST(ZobristFullScope, ZobristStartPositionFindAll) {

    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    std::string a2a3 = "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1";
    std::string g1f3 = "rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 1 1";
    std::string e2e4 = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1";
    
    auto startpos = CREATE(startpos_fen);
    auto startpos_sanity = CREATE(startpos_fen);

    auto b1new = CREATE(a2a3);
    auto b2new = CREATE(g1f3);
    auto b3new = CREATE(e2e4);


    //easiest tool to use is the mcts nodemodel creation
    MCTSNodeModel* n1 = new MCTSNodeModel(startpos, NodeInfo(0,0));
    MCTSNodeInserter* ninsert = new MCTSNodeInserter(new MCTSNodeTreeStatistics(10));

    MCTSNodeExpansion expander(ninsert);

    expander.Expand(n1);

    //make a list of zobrists in expanded position
    //assert that b123 keys are found in this list

    std::vector<unsigned long long int> zkeys;
    for(auto & edge : n1->GetEdges()) {
        zkeys.push_back(edge->GetBoard().z_);
    }

    /* works for now
    
    print(" // Original keys // ");
    print(startpos.z_);
    print(startpos_sanity.z_);

    print(" // Manully created positions // ");
    print(b1new.z_);
    print(b2new.z_);
    print(b3new.z_);

    print(" // Expanded keys // ");
    for(auto key : zkeys) {
        print(key);
    }
    print(" // EO Expanded keys // ");
    */

    auto b1niterptr = std::find(zkeys.begin(), zkeys.end(), b1new.z_);
    auto b2niterptr = std::find(zkeys.begin(), zkeys.end(), b2new.z_);
    auto b3niterptr = std::find(zkeys.begin(), zkeys.end(), b3new.z_);


    ASSERT_NE(b1niterptr, zkeys.end());
    ASSERT_NE(b2niterptr, zkeys.end());
    ASSERT_NE(b3niterptr, zkeys.end());

}

TEST(ZobristFullScope, ZobristComplexFullTest) {
    //test more compllicated stuff

    MCTSNodeInserter* ninsert = new MCTSNodeInserter(new MCTSNodeTreeStatistics(10));
    MCTSNodeExpansion expander(ninsert);

    std::string kiwipep_fen_wenp_b = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1";

    auto k1_norm_capt = "r3k2r/p1ppqpb1/1n2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBbPPP/R3K2R w KQkq - 0 2";
    auto k2_enp_capt = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/p1N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2";
    auto k3_castl_long = "2kr3r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQ - 1 2";
    auto k4_castl_sh = "r4rk1/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQ - 1 2";
    auto k5_pawn_capt = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w KQkq - 0 2";
    auto k6_kn_capt = "r3k2r/p1ppqpb1/bn2p1p1/3PN3/Pp2n3/2N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2";

    auto k1 = CREATE(k1_norm_capt);
    auto k2 = CREATE(k2_enp_capt);
    auto k3 = CREATE(k3_castl_long);
    auto k4 = CREATE(k4_castl_sh);
    auto k5 = CREATE(k5_pawn_capt);
    auto k6 = CREATE(k6_kn_capt);

    auto kiwinew = CREATE(kiwipep_fen_wenp_b);
    MCTSNodeModel* n2 = new MCTSNodeModel(kiwinew, NodeInfo(0,0));
    expander.Expand(n2);

    std::vector<unsigned long long int> zkeys2;
    for(auto & edge : n2->GetEdges()) {
        zkeys2.push_back(edge->GetBoard().z_);
    }

    auto k1niterptr = std::find(zkeys2.begin(), zkeys2.end(), k1.z_);
    auto k2niterptr = std::find(zkeys2.begin(), zkeys2.end(), k2.z_);
    auto k3niterptr = std::find(zkeys2.begin(), zkeys2.end(), k3.z_);
    auto k4niterptr = std::find(zkeys2.begin(), zkeys2.end(), k4.z_);
    auto k5niterptr = std::find(zkeys2.begin(), zkeys2.end(), k5.z_);
    auto k6niterptr = std::find(zkeys2.begin(), zkeys2.end(), k6.z_);

    print(" // Original keys // ");
    print(kiwinew.z_);
   
    print(" // Manully created positions // ");
    print(k1.z_);
    print(k2.z_);
    print(k3.z_);
    print(k4.z_);
    print(k5.z_);
    print(k6.z_);

    print(" // Expanded keys // ");
    for(auto key : zkeys2) {
        print(key);
    }
    print(" // EO Expanded keys // ");

    ASSERT_NE(k1niterptr, zkeys2.end());
    ASSERT_NE(k2niterptr, zkeys2.end());
    ASSERT_NE(k3niterptr, zkeys2.end());
    ASSERT_NE(k4niterptr, zkeys2.end());
    ASSERT_NE(k5niterptr, zkeys2.end());
    ASSERT_NE(k6niterptr, zkeys2.end());


}

TEST(ZobristFullScope, ZobristPromotionTest) {
    //test more compllicated stuff

    MCTSNodeInserter* ninsert = new MCTSNodeInserter(new MCTSNodeTreeStatistics(10));
    MCTSNodeExpansion expander(ninsert);

    std::string promotion_fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";

    auto k1_capt_promo_kn = "n1n5/PPPk4/8/8/8/8/4Kp1p/5N1n w - - 0 2";
    auto k2_move_promo_q = "n1n5/PPPk4/8/8/8/8/4Kp1p/5NqN w - - 0 2";


    auto k1 = CREATE(k1_capt_promo_kn);
    auto k2 = CREATE(k2_move_promo_q);


    auto position = CREATE(promotion_fen);
    MCTSNodeModel* n3 = new MCTSNodeModel(position, NodeInfo(0,0));
    expander.Expand(n3);

    std::vector<unsigned long long int> zkeys2;
    for(auto & edge : n3->GetEdges()) {
        zkeys2.push_back(edge->GetBoard().z_);
    }

    auto k1niterptr = std::find(zkeys2.begin(), zkeys2.end(), k1.z_);
    auto k2niterptr = std::find(zkeys2.begin(), zkeys2.end(), k2.z_);

    print(" // Original keys // ");
    print(position.z_);
   
    print(" // Manully created positions // ");
    print(k1.z_);
    print(k2.z_);

    print(" // Expanded keys // ");
    for(auto key : zkeys2) {
        print(key);
    }
    print(" // EO Expanded keys // ");

    ASSERT_NE(k1niterptr, zkeys2.end());
    ASSERT_NE(k2niterptr, zkeys2.end());
}