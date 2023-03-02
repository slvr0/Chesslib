#!/usr/bin/env python3

from enum import Enum, IntEnum

class GameResult(Enum) :
    WHITE_WIN  = 1
    BLACK_WIN  = 2
    DRAW       = 3
    RULE_50    = 4
    STALEMATE  = 5
    INSUFF_MAT = 6

class PositionState :
    def __init__(self, state, last_move, meta):
        self.current_state = state
        self.last_move = last_move
        self.meta = meta #castle rule50 #enp

class GameHistory :
    def __init__(self, history : [PositionState], result : GameResult):
        self.hist_moves = history
        self.res = result



