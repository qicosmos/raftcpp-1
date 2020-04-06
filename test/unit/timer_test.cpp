#include "../doctest/doctest.hpp"
#include "../../raft_node.hpp"
#include <chrono>

TEST_CASE("test heartbeat timer") {
    using namespace raftcpp;
    raft_config conf{};
    conf.all_peers = {
        {"127.0.0.1", 8001, 0},
        {"127.0.0.1", 8002, 1},
        {"127.0.0.1", 8003, 2},
    };

    conf.self_addr = conf.all_peers[0];
    raft_node node(conf);
    node.handle_prevote_response({ 0, true });
    CHECK(node.state() == State::CANDIDATE);
    node.handle_vote_response({ 0, true });
    CHECK(node.state() == State::LEADER);

    conf.disable_election_timer = false;
    node.start_heartbeat_timer();
    node.stepdown(0);
}

TEST_CASE("test election timer") {
    using namespace raftcpp;
    raft_config conf{};
    conf.all_peers = {
        {"127.0.0.1", 8001, 0},
        {"127.0.0.1", 8002, 1},
    };

    conf.disable_election_timer = true;
    conf.self_addr = conf.all_peers[0];
    raft_node node(conf);

    raft_config conf2 = conf;
    conf2.self_addr = conf.all_peers[1];
    conf2.disable_election_timer = true;
    raft_node node2(conf2);
    node2.prevote_ack_num_.store(0);

    node.async_run();
    node2.async_run();

    size_t radom_milli = node2.get_random_milli();
    node2.reset_timer(true, radom_milli);

    std::this_thread::sleep_for(std::chrono::milliseconds(radom_milli * 2));

    int pre_ack_num = node2.prevote_ack_num_.load();
    CHECK(pre_ack_num == 2);
    CHECK(node2.state_ != State::FOLLOWER);
}

TEST_CASE("test vote timer") {
    
}

TEST_CASE("test handle repeat vote request") {
    
}

TEST_CASE("test heartbeat timeout") {
    
}