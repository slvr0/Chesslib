#!/usr/bin/env python3
import multiprocessing

from environment.db_importer import DirectoryPool
from environment.pgn_import_parse import V1FileUnpacker

import os

import stockfish

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

    batches = []
    path = "training_processed/modern"


    for element in os.listdir(path) :
        training_batch = V1FileUnpacker.unpack_v1_training_batch(os.path.join(path, element))






    #
    # print(len(training_batch))

    #print(len(training_batch))











