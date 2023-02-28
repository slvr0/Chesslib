#!/usr/bin/env python3

from coresystem.worker import Worker
from db_importer import DBImporter

class SupervisedEnvironment :
    def __init__(self, sv_conf):
        self.sv_conf = sv_conf
        self.learn_root = ""
        self.visited_paths = [] #subdirs under learning root path


    #purpose is to maybe split up training data into multiple paths, will be excessive amount
    def chunk_training_data(self):
        pass

    def generate_job(self, db_importer : DBImporter) -> Worker:


        game_hist = db_importer.import_from(self.learn_root + "g1")
        return Worker(
            batch = game_hist

        )




    #not sure if this is a thing
    def __hash__(self):
        raise NotImplemented()



