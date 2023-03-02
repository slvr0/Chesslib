#!/usr/bin/env python3

import chess
import os
import chess
import dataclasses
import collections

from coresystem.state_representation import *


@dataclasses.dataclass
class JobContext :
    statechain : [chess.Board]
    movehist : [str]
    result : GameResult
    id : int

    def __eq__(self, other):
        if isinstance(other, JobContext):
            return self.id == other.id
        else : return NotImplemented()

class DBImporter :
    def __init__(self, import_callback):
        self.import_callback = import_callback
        self.root_dir = "training/"

    def create_jobcontext(self, statechain: [chess.Board], movehist: [str], result: GameResult, id : int) -> JobContext:
        return JobContext(statechain, movehist, result, id)

    def import_from(self, pathdir):
        map_result = {
            '1-0': GameResult.WHITE_WIN,
            '0-1': GameResult.BLACK_WIN,
            '1/2-1/2': GameResult.DRAW
        }

        mockboard = chess.Board()
        fullpath = os.path.join(self.root_dir, pathdir)
        processed_games = 0
        if os.path.isdir(fullpath):
            for file in os.listdir(fullpath):
                with open(os.path.join(fullpath, file), "r") as file:

                    while (not file.closed):

                        for i in range(11):
                            _ = file.readline()

                        mockboard.reset()
                        states = []
                        storedmovetransitions = []
                        while (True):
                            linecontent = file.readline()
                            if (linecontent == "\n" or linecontent == ''):
                                break

                            moves = linecontent.strip('\n').split(' ')
                            store_res = moves[-1]
                            moves = [m.lstrip('1234567890.') for m in moves]
                            for m in moves:
                                if m == '': break
                                states.append(mockboard.copy())
                                storedmovetransitions.append(mockboard.push_san(m).uci())

                        gameresult = map_result[store_res]
                        processed_games += 1


                        self.import_callback(self.create_jobcontext(states, storedmovetransitions, gameresult, processed_games))




                        if processed_games == 1000: exit(1)

