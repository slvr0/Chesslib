#!/usr/bin/env python3

from state_representation import *

class Worker :
    def __init__(self, batch : [GameHistory]):
        self.batch = batch

    def on_receive_batch(self, batch):
        pass

    def on_init_selfplay(self):
        pass

    def send_batch_for_training(self):
        pass

    #this will be called in gradient tape environment, so anything processessed will be graphed
    def __call__(self, network):
        pass

    def execute_work(self, conf = None) :
        if conf : #set parameters for each job
            pass
