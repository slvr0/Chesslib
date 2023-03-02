#!/usr/bin/env python3

from collections import defaultdict, deque

from coresystem.worker import Worker
from coresystem.state_representation import *

from environment.db_importer import *

class SupervisedEnvironment :
    def __init__(self, sv_conf):
        self.sv_conf = sv_conf
        self.learn_root = ""
        self.visited_paths = [] #subdirs under learning root path
        self.import_db = DBImporter(import_callback = self.generate_job)

        self.game_queue = collections.deque(maxlen=1000) #connect workers by hooking to this
        self.worker_pool = []

        #in future connect this to que
        self.worker = Worker()


    #purpose is to maybe split up training data into multiple paths, will be excessive amount
    def process_chunks(self):
        self.import_db.import_from("modern2")

    def generate_job(self, job : JobContext) :
        #self.game_queue.append(job)

        self.worker(job)



    #not sure if this is needed
    def __hash__(self):
        raise NotImplemented()



