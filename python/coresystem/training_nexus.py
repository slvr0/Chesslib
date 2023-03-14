#!/usr/bin/env python3

import multiprocessing
from datetime import date

from coresystem.worker import  *
from coresystem.agent import *
from environment.supervised_env import *
from configs.generalconfig import *
from configs.network_config import *
from coresystem.model_ac_v1 import *
from coresystem.output_classifier import *

import chess
import chess.pgn
import stockfish
from coresystem.policy_index import *
import threading

class DataParser :
    def __init__(self, filename):
        self.filename = filename

class DataUnpacker :

    @staticmethod
    def reverse_expand_bits(plane):
        return np.unpackbits(np.array([plane], dtype=np.uint8))[::-1].astype(
            np.float32).tobytes()

    @staticmethod
    def unpack_planes(planes):
        return np.unpackbits(np.frombuffer(planes, dtype=np.uint8)).astype(
            np.float32)

class TrainingNexus :
    def __init__(self, env : SupervisedEnvironment, gconf : dict, netconf : dict):
        self.env = env
        self.gconf = gconf
        self.netconf = netconf
        self.worker = Worker()
        self.model = create_net_topology_ac_v1(self.netconf)
        self.classifier = NetworkOutputClassifier()

    def get_model_summary(self):
        return self.model.summary()

    def init_threaded_parse_chunks(self):
        while True:
            with self.env.thread_cond:
                self.env.thread_cond.wait()
                job = self.env.parse_chunk_queue.popleft()

                origin = job.origin.replace("training", "training_processed")
                parse_to = origin + "_" + str(date.today()) + "_" + str(job.id)

                if(not os.path.exists(origin)) :
                    os.makedirs(origin)

                with open(parse_to) as parse_file :
                    pass

                neural_input = self.worker(job)
                print(parse_to)



    def init_threaded(self):
        while True:
            with self.env.thread_cond:
                self.env.thread_cond.wait()
                job = self.env.job_queue.popleft()
                neural_input = self.worker(job)

                board = chess.Board()
                sf  = stockfish.Stockfish()
                sf.set_elo_rating(2000)

                for state, move, processed in zip(job.statechain, job.movehist, neural_input) :
                    #policy, value, mlh = self.model(np.expand_dims(processed, axis=0))
                    #legalmoves_uci = [move.uci() for move in board.legal_moves]
                    #outputmask = self.classifier.uci_to_policy(legalmoves_uci)

                    # policy_m = self.classifier.correct_policy_ch_comp(outputmask, policy)
                    #
                    # top = sf.get_top_moves(5)
                    #
                    # boost_factor = (0.3, 0.25, 0.2, 0.15, 0.1)
                    #
                    # sftm = np.zeros(shape=len(policy_index))
                    #
                    # for i, tm in enumerate(top):
                    #     nn_id = policy_index.index(tm['Move'])
                    #     sftm[nn_id] = boost_factor[i]

                    #target_p = tf.math.multiply(policy_m, tf.constant(sftm, dtype=tf.float32))
                    #target_p_log = tf.math.log(target_p)

                    target_v = sf.get_evaluation()['value']

                    #train

                    #step chesslib and stockfish

                    #sf.make_moves_from_current_position([move])
                    print(move)





    def abort(self):
        pass

class TrainingNexusFactory:
    def __init__(self, qact_exec, qact_abort):
        self.nexuses = []
        self.threads = []

        self.qact_exec  = qact_exec
        self.qact_abort = qact_abort

    def on_env_create(self, env : SupervisedEnvironment, gconf : dict, netconf : dict):
        self.nexuses.append(TrainingNexus(env, gconf, netconf))

    def execute_nexuses(self):
        for nexus in self.nexuses :
            th = threading.Thread(target=nexus.init_threaded_parse_chunks)
            th.start()
            self.threads.append(th)

        for th in self.threads:
            th.join()

    def abort_nexuses(self):
        for nexus in self.nexuses :
            nexus.abort()


# if workers > 0:
#     print("Using {} worker processes.".format(workers))
#
#     # Start the child workers running
#     self.readers = []
#     self.writers = []
#     parent.processes = []
#     self.chunk_filename_queue = mp.Queue(maxsize=4096)
#     for _ in range(workers):
#         read, write = mp.Pipe(duplex=False)
#         p = mp.Process(target=self.task,
#                        args=(self.chunk_filename_queue, write))
#         p.daemon = True
#         parent.processes.append(p)
#         p.start()
#         self.readers.append(read)
#         self.writers.append(write)
#
#     parent.chunk_process = mp.Process(target=chunk_reader,
#                                       args=(chunks,
#                                             self.chunk_filename_queue))
#     parent.chunk_process.daemon = True
#     parent.chunk_process.start()