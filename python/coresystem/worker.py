#!/usr/bin/env python3
import chess
import numpy as np

from coresystem.state_representation import *
from coresystem.data_preprocessor import *
from environment.db_importer import *

#tomorrow we shall finish this class, that consumes a condition var controlled thread, listening on job context que entries
#and processes data and sends to agent for training!

class Worker :
    def __init__(self):
        self.preprocessor = DataPreprocessor()


    def on_receive_batch(self, batch):
        pass

    def on_init_selfplay(self):
        pass

    def preprocess_batch(self, states : [chess.Board], history_full : [str], res : GameResult, id : int) -> [NeuralInput]:
        neural_inputs = []

        for i, state in enumerate(states) :
            start = 0 if i <= 10 else i - 10
            hist = history_full[start: i + 1]
            hist.reverse()

            ninput_planes = self.preprocessor.convolve_into_network_planes(state, hist, state.turn)

            planes_unpacked = []
            for plane in ninput_planes :

                if(isinstance(plane, InputPlane)) : planes_unpacked.append(plane)
                else : planes_unpacked.extend(plane)









    #this will be called in gradient tape environment, so anything processessed will be graphed
    def __call__(self, job : JobContext):
        self.preprocess_batch(job.statechain,
                              job.movehist,
                              job.result,
                              job.id)




    def execute_work(self, conf = None) :
        if conf : #set parameters for each job
            pass

    def connect_to_env(self):
        pass