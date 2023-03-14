#!/usr/bin/env python3

import threading

from collections import defaultdict, deque

from coresystem.worker import Worker
from coresystem.state_representation import *

from environment.db_importer import *

import time

class SupervisedEnvironment :
    def __init__(self, sv_conf):
        self.sv_conf = sv_conf
        self.learn_root = ""
        self.visited_paths = [] #subdirs under learning root path


        self.buffer_job_lim = 2000

        self.job_queue = collections.deque(maxlen=10000) #connect workers by hooking to this
            self.parse_chunk_queue = collections.deque(maxlen=10000) #connect workers by hooking to this

        self.thread_cond = threading.Condition()
        #if self.existing_file(parsing_file) : return False

    def parse_to_processed(self):
        pass

    def generate_job(self, job : JobContext, parse_chunks = False) :
        if len(self.job_queue) > self.buffer_job_lim :
            time.sleep(5)

        if parse_chunks : self.parse_chunk_queue.append(job)
        else : self.job_queue.append(job)

        with self.thread_cond:
            #print(len(self.job_queue))
            self.thread_cond.notify() #should be any

    #not sure if this is needed
    def __hash__(self):
        raise NotImplemented()



