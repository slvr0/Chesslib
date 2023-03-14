#!/usr/bin/env python3
import dataclasses
from dataclasses import dataclass
import numpy as np

from coresystem.state_representation import *

boardnotations = [
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
]

boardnotations_mirror = boardnotations.copy()
boardnotations_mirror.reverse()

def extract_bits(v):
    flsb = []
    while (v != 0):
        lsb = v & -v
        flsb.append(lsb.bit_length() - 1)
        v &= ~lsb
    return flsb

@dataclasses.dataclass
class HistoryContextObject:
    chain_len = 11
    "number of moves remembered"

    move_history : [(str, bool)]
    "Previous moves in chain, bool represents white acting. otherwise enumeration index must be mirror"

    def as_nn_enumerations(self):
        raise NotImplemented()

class IChessDataInterpreter:
    def __init__(self):
        pass

    def get_active_player(self, state):
        raise NotImplemented()
    def get_pawns_uint64(self, state):
        raise NotImplemented()
    def get_knights_uint64(self, state):
        raise NotImplemented()
    def get_bishops_uint64(self, state):
        raise NotImplemented()
    def get_rooks_uint64(self, state):
        raise NotImplemented()
    def get_queens_uint64(self, state):
        raise NotImplemented()
    def get_king_uint64(self,state):
        raise NotImplemented()
    def get_white(self, state):
        raise NotImplemented()
    def get_black(self, state):
        raise NotImplemented()
    def get_enp_sq(self, state):
        raise NotImplemented()
    def get_castling(self, state):
        raise NotImplemented()
    def get_ply(self, state):
        raise NotImplemented()
    def get_fullmove(self, state):
        raise NotImplemented()
    def get_rule50(self, state):
        raise NotImplemented()

class LibChessDataInterpreter(IChessDataInterpreter) :
    def __init__(self):
        super(LibChessDataInterpreter, self).__init__()
    def get_pawns_uint64(self, state):
        return state.pawns
    def get_knights_uint64(self, state):
        return state.knights
    def get_bishops_uint64(self, state):
        return state.bishops
    def get_rooks_uint64(self, state):
        return state.rooks
    def get_queens_uint64(self, state):
        return state.queens
    def get_king_uint64(self, state):
        return state.kings
    def get_white(self, state):
        return state.occupied_co[1]
    def get_black(self, state):
        return state.occupied_co[0]
    def get_enp_sq(self, state):
        return state.ep_square
    def get_castling(self, state):
        c64 = extract_bits(state.castling_rights)
        woo = True if c64.__contains__(7)   else False
        wooo = True if c64.__contains__(0)  else False
        boo = True if c64.__contains__(63)  else False
        booo = True if c64.__contains__(56) else False
        return woo, wooo, boo, booo
    def get_ply(self, state):
        return state.ply()
    def get_fullmove(self, state):
        return state.fullmove
    def get_rule50(self, state):
        return state.halfmove_clock

@dataclasses.dataclass
class InputPlane :
    planeindex : int
    "represents the plane index"
    mask : int
    "represents the values set in a uint64t"

    def as_numpy(self):
        if self.mask == 0xFFFFFFFF : return np.ones(shape=(8,8), dtype=int)
        else :
            npz = np.zeros(shape=(64), dtype=int)
            npz[extract_bits(self.mask)] = 1
            npz = npz.reshape((8,8))
        return npz

#makes a neural input structure from a chain of input planes
class NeuralInput :
    planes : [InputPlane]

class DataPreprocessor :
    def __init__(self):
        self.processor = LibChessDataInterpreter()

    def fill_actor(self, netin : np.array, state, white_acts = True):
        actormask = self.processor.get_white(state) if white_acts else \
                    self.processor.get_black(state)

        netin[0, extract_bits(self.processor.get_pawns_uint64(state) & actormask)] = 1
        netin[1, extract_bits(self.processor.get_knights_uint64(state) & actormask)] = 1
        netin[2, extract_bits(self.processor.get_bishops_uint64(state) & actormask)] = 1
        netin[3, extract_bits(self.processor.get_rooks_uint64(state) & actormask)] = 1
        netin[4, extract_bits(self.processor.get_queens_uint64(state) & actormask)] = 1
        netin[5, extract_bits(self.processor.get_king_uint64(state) & actormask)] = 1


    def fill_observer(self, netin, state, white_acts = True):
        actormask = self.processor.get_black(state) if white_acts else \
                    self.processor.get_white(state)
        netin[6, extract_bits(self.processor.get_pawns_uint64(state) & actormask)] = 1
        netin[7, extract_bits(self.processor.get_knights_uint64(state) & actormask)] = 1
        netin[8, extract_bits(self.processor.get_bishops_uint64(state) & actormask)] = 1
        netin[9, extract_bits(self.processor.get_rooks_uint64(state) & actormask)] = 1
        netin[10, extract_bits(self.processor.get_queens_uint64(state) & actormask)] = 1
        netin[11, extract_bits(self.processor.get_king_uint64(state) & actormask)] = 1

    def fill_active(self, netin, white_acts = True):
        if white_acts : netin[12, :] = 1
        else : netin[13, :] = 1

    def fill_castle_planes(self, netin, state, white_acts = True):
        woo, wooo, boo, booo = self.processor.get_castling(state)
        if white_acts :
            if woo : netin[14, :] = 1
            if wooo : netin[15, :] = 1
            if boo: netin[16, :] = 1
            if booo: netin[17, :] = 1
        else :
            if boo : netin[14, :] = 1
            if booo : netin[15, :] = 1
            if woo: netin[16, :] = 1
            if wooo: netin[17, :] = 1

    def fill_enp_plane(self, netin, state, white_acts = True):
        enp_square = self.processor.get_enp_sq(state)

        if enp_square :
            if white_acts : netin[18, enp_square] = 1
            else : netin[18, 63 - enp_square] = 1

    def fill_rule_50_plane(self, netin, state):
        r50 = self.processor.get_rule50(state)
        netin[19, 0: int((r50 / 64) * 64)] = 1

    def fill_plymoves(self, netin, state):
        plyn = self.processor.get_ply(state)
        netin[20, 0: int((plyn / 64) * 64)] = 1

    def fill_hist_plane(self, netin, history, white_acts = True):
        planes = []
        wact = white_acts
        for i, histinfo in enumerate(history):
            fidx = boardnotations.index(histinfo[0:2]) if wact else boardnotations_mirror.index(histinfo[0:2])
            tidx = boardnotations.index(histinfo[2:4]) if wact else boardnotations_mirror.index(histinfo[2:4])

            netin[20+i, fidx] = 1
            netin[20+i, tidx] = 1

            wact = not wact #swap act/obs in hist chain

    def convolve_into_network_planes(self, state, history : [str],
                                     white_acts : bool = True ) -> np.array:

        netin = np.zeros(shape=(32,64), dtype=int)

        self.fill_actor(netin, state, white_acts) # 6
        self.fill_observer(netin, state, white_acts) # 6
        self.fill_active(netin, white_acts) # 2
        self.fill_castle_planes(netin, state, white_acts) # 4
        self.fill_enp_plane(netin, state, white_acts) # 1
        self.fill_rule_50_plane(netin, state) # 1
        self.fill_plymoves(netin, state) # 1
        self.fill_hist_plane(netin, history, white_acts) # 11

        return netin.reshape((32, 8, 8))













