#!/usr/bin/env python3

import chess
import os
import chess
import dataclasses
import collections
import threading
import stockfish
import time


import multiprocessing as mp

from coresystem.state_representation import *
from environment.pgn_import_parse import V1ChunkParser, PGNTools, PGNReader

@dataclasses.dataclass
class JobContext :
    statechain : [chess.Board]
    movehist : [str]
    result : GameResult
    id : int
    origin : str

    def __eq__(self, other):
        if isinstance(other, JobContext):
            return self.id == other.id
        else : return NotImplemented()

#manages a pool of threaded pgn readers
@dataclasses.dataclass
class DirectoryPool :
    path : str
    writer_threads : int = 1
    rootdir: str = "training"

    def existing_replica(self):
        return os.path.isdir(os.path.join(self.rootdir + "_processed", self.path))

    def existing_file(self, file):
        return os.path.isfile(os.path.join(self.rootdir + "_processed", self.path + "/" + file))

    def process_one_file(self, readfile_name):
        read_path = os.path.join(self.rootdir, self.path)
        parsing_path = os.path.join(self.rootdir + "_processed", self.path)
        read_file = os.path.join(read_path, readfile_name)

        if not os.path.exists(parsing_path) :
            os.makedirs(parsing_path)

        #check if file has been previously parsed

        num_files_fptr = lambda dir_path : sum([1 for element in os.listdir(dir_path)
                                                if os.path.isfile(os.path.join(dir_path, element))])
        files = num_files_fptr(parsing_path)
        print(files)

        entries = PGNTools.peek_entries(read_file)
        reader = PGNReader()
        reader.read_pgn_to_v1_chunks(read_file, files)
        writers = []

        for i in range(self.writer_threads):
            writer = V1ChunkParser(reader.condvar,reader.jobque, parsing_path, i)
            th = mp.Process(target=writer.process_v1_jobs_onthread, args=[readfile_name,])
            #th.daemon = True // kills the process after main process exits
            th.start()
            writers.append(th)

        for th in writers:
            th.join()
        return True
