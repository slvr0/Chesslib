#!/usr/bin/env python3
import chess
import numpy as np

from coresystem.state_representation import *
from coresystem.data_preprocessor import *
from environment.db_importer import *
from coresystem.output_classifier import *

#tomorrow we shall finish this class, that consumes a condition var controlled thread, listening on job context que entries
#and processes data and sends to agent for training!

class Worker :
    def __init__(self):
        self.preprocessor   = DataPreprocessor()
        self.job_queue      = None
        self.job_cond       = None
        self.classifier     = NetworkOutputClassifier()


    def preprocess_batch(self, states : [chess.Board], history_full : [str]) -> np.array:
        neural_inputs = []

        for i, state in enumerate(states) :
            start = 0 if i <= 10 else i - 10
            hist = history_full[start: i + 1]
            hist.reverse()
            neural_inputs.append(self.preprocessor.convolve_into_network_planes(state, hist, state.turn))

        return np.array(neural_inputs)

    def purge_output(self, target, output):
        pass

    def __call__(self, job : JobContext):
        return self.preprocess_batch(job.statechain,
                              job.movehist)


    def execute_work(self, conf = None) :
        if conf : #set parameters for each job
            pass

