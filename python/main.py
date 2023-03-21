#!/usr/bin/env python3
import multiprocessing

from environment.db_importer import DirectoryPool
from environment.pgn_import_parse import V1FileUnpacker

import os
import tqdm
import collections
import statistics

import stockfish

from coresystem.training_nexus import TrainingNexus
from coresystem.coresys import CoreSystem
from configs.network_config import create_network_conf_a1
from configs.generalconfig import create_general_conf
from coresystem.model_ac_v1 import create_net_topology_ac_v1
from coresystem.agent import NetworkAgent

from evaluator_tools.network_evaluator import NetworkEval

def train_from_packedfiles() :
    batches = []
    path = "training_processed/modern"

    netconf = create_network_conf_a1()
    globalconf = create_general_conf()


    agent = NetworkAgent(0.005, 1, globalconf, netconf, "train_model_v0_0")

    save_every = 20
    max_episodes = 10000
    min_episodes_criterion = 10000
    t = tqdm.trange(max_episodes)
    collected_loss = collections.deque(maxlen=min_episodes_criterion)

    agent.load_model()
    for t_index, element in zip(t, os.listdir(path)) :
        training_batch = V1FileUnpacker.unpack_v1_training_batch(os.path.join(path, element))
        ploss, vloss, tloss = agent.inner_train_process(training_batch)

        collected_loss.append(tloss)
        running_loss = statistics.mean(collected_loss)

        t.set_postfix(
            mean_policy_loss=ploss,
            mean_value_loss=vloss,
            mean_total_loss=tloss,
            running_loss=running_loss

        )
        if t_index % save_every == 0: agent.save_model()

def evaluate_model_wplayout() :
    netconf = create_network_conf_a1()
    globalconf = create_general_conf()
    model = create_net_topology_ac_v1(netconf)
    agent = NetworkAgent(model, 0.005, 1, globalconf, "train_model_v0_0")

    neteval = NetworkEval(agent.model, 1200)
    neteval.eval_by_playout(True)


"""
state, policy pi, result, q, m
x,y,z,q,m

"""
if __name__ == '__main__':
    # v1chunk_parser = V1ChunkParser(pathdir="training_processed", thread_id=0 , nreads=1)
    # filename = "Modern3.pgn"
    # entries = v1chunk_parser.peek(filename) #move peek to the Process creation tool
    # v1chunk_parser.read_pgn_to_v1_chunks(filename, 0)
    #
    #
    # print(training_batch[0].q)

    #dir_pool = DirectoryPool("modern", 8)
    #dir_pool.process_one_file("Modern.pgn")

    netconf = create_network_conf_a1()
    globalconf = create_general_conf()

    agent = NetworkAgent(0.005, 1, globalconf, netconf, "train_model_v0_1")
    agent.save_model()

    #train_from_packedfiles()
    #evaluate_model_wplayout()







