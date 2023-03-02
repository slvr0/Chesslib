#!/usr/bin/env python3
import dataclasses
from dataclasses import dataclass

from coresystem.state_representation import *

#0 - 10 , r : 0 , 11 - 20 r : 1,2 , 21 - 30 : r 1,2,3,4 : 31 - 40 : r 1,2,3,4,5,6 : 41-50 FULL

__R0 = 0xFF
__R1 = 0xFF << 8
__R2 = 0xFF << 16
__R3 = 0xFF << 24
__R4 = 0xFF << 32
__R5 = 0xFF << 40
__R6 = 0xFF << 48
__R7 = 0xFF << 56

PLY_FILL_MOVE = [
    __R0 | __R1,
    __R0 | __R1 | __R2 | __R3,
    __R0 | __R1 | __R2 | __R3 | __R4 | __R5 ,
    0xFFFFFFFF
]





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

    #Should these be mirrored?
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
        return state.ply
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

#makes a neural input structure from a chain of input planes
class NeuralInput :
    planes : [InputPlane]

class DataPreprocessor :
    def __init__(self):

        self.processor = LibChessDataInterpreter()

    def fill_actor(self, state, white_acts = True):
        actormask = self.processor.get_white(state) if white_acts else \
                    self.processor.get_black(state)
        return (
            InputPlane(0, self.processor.get_pawns_uint64(state) & actormask),
            InputPlane(1, self.processor.get_knights_uint64(state) & actormask),
            InputPlane(2, self.processor.get_bishops_uint64(state) & actormask),
            InputPlane(3, self.processor.get_rooks_uint64(state) & actormask),
            InputPlane(4, self.processor.get_queens_uint64(state) & actormask),
            InputPlane(5, self.processor.get_king_uint64(state) & actormask)
        )

    def fill_observer(self, state, white_acts = True):
        actormask = self.processor.get_black(state) if white_acts else \
                    self.processor.get_white(state)
        return (
            InputPlane(6, self.processor.get_pawns_uint64(state) & actormask),
            InputPlane(7, self.processor.get_knights_uint64(state) & actormask),
            InputPlane(8, self.processor.get_bishops_uint64(state) & actormask),
            InputPlane(9, self.processor.get_rooks_uint64(state) & actormask),
            InputPlane(10, self.processor.get_queens_uint64(state) & actormask),
            InputPlane(11, self.processor.get_king_uint64(state) & actormask)
        )

    def fill_active(self, white_acts = True):
        return (
            InputPlane(12, 0xFFFFFFFF),
            InputPlane(13, 0x0),
        ) if white_acts else \
        (
            InputPlane(12, 0x0),
            InputPlane(13, 0xFFFFFFFF),
        )

    #remember the same logic, actors castling right should have the same input slot
    def fill_castle_planes(self, state, white_acts = True):
        woo, wooo, boo, booo = self.processor.get_castling(state)
        return (
            InputPlane(14, 0xFFFFFFFF if woo else 0x0),
            InputPlane(15, 0xFFFFFFFF if wooo else 0x0),
            InputPlane(16, 0xFFFFFFFF if boo else 0x0),
            InputPlane(17, 0xFFFFFFFF if booo else 0x0),
        ) if white_acts else \
        (
            InputPlane(14, 0xFFFFFFFF if boo else 0x0),
            InputPlane(15, 0xFFFFFFFF if booo else 0x0),
            InputPlane(16, 0xFFFFFFFF if woo else 0x0),
            InputPlane(17, 0xFFFFFFFF if wooo else 0x0),
        )

    def fill_enp_plane(self, state, white_acts = True):
        enp_square = self.processor.get_enp_sq(state)
        if enp_square == None : return InputPlane(18, 0x0)
        else :
            return  InputPlane(18, 1 << enp_square) if white_acts else \
                    InputPlane(18, 1 << 63 - enp_square)

    #0 - 10 , r : 0 , 11 - 20 r : 1,2 , 21 - 30 : r 1,2,3,4 : 31 - 40 : r 1,2,3,4,5,6 : 41-50 FULL ?? are u high
    def fill_rule_50_plane(self, state):
        r50 = self.processor.get_rule50(state)
        if r50 in range(0, 12)      : return InputPlane(19, PLY_FILL_MOVE[0])
        elif r50 in range(13, 25)   : return InputPlane(19, PLY_FILL_MOVE[1])
        elif r50 in range(26, 38)   : return InputPlane(19, PLY_FILL_MOVE[2])
        else   : return InputPlane(19, PLY_FILL_MOVE[3])



    # similar to above, #0 - 10 , r : 0 , 11 - 20 r : 1,2 , 21 - 30 : r 1,2,3,4 : 31 - 40 : r 1,2,3,4,5,6 : 41-50 FULL
    def fill_plymoves(self, state):
        plyn = self.processor.get_ply(state) #scale up abit
        if plyn in range(0, 12)      : return InputPlane(20, PLY_FILL_MOVE[0])
        elif plyn in range(13, 25)   : return InputPlane(20, PLY_FILL_MOVE[1])
        elif plyn in range(26, 38)   : return InputPlane(20, PLY_FILL_MOVE[2])
        else   : return InputPlane(20, PLY_FILL_MOVE[3])

    #fix tomorrow, e3e4 = indexes from / to , mirror for black , insert plane
    #most recent moves in front, store backwards chain
    def fill_hist_plane(self, history, white_acts = True):
        planes = []
        wact = white_acts
        for i, histinfo in enumerate(history):

            fidx = boardnotations.index(histinfo[0:2]) if wact else boardnotations_mirror.index(histinfo[0:2])
            tidx = boardnotations.index(histinfo[2:4]) if wact else boardnotations_mirror.index(histinfo[2:4])

            planes.append(InputPlane(20+i, 1 << fidx | 1 << tidx))

            #print(wact, histinfo[0:2], fidx, histinfo[2:4], tidx)

            wact = not wact #swap act/obs in hist chain

        return planes

    def convolve_into_network_planes(self, state, history : [str],
                                     white_acts : bool = True ) -> [InputPlane]:

        active_planes       = self.fill_actor(state, white_acts) # 6
        observer_planes     = self.fill_observer(state, white_acts) # 6
        actor_obs_planes    = self.fill_active(white_acts) # 2
        castle_planes       = self.fill_castle_planes(state, white_acts) # 4
        enp_plane           = self.fill_enp_plane(state, white_acts) # 1
        r50_plane           = self.fill_rule_50_plane(state) # 1
        ply_plane           = self.fill_plymoves(state) # 1
        hist_planes         = self.fill_hist_plane(history, white_acts) # 11

        return [
            active_planes,
            observer_planes,
            actor_obs_planes,
            castle_planes,
            enp_plane,
            r50_plane,
            ply_plane,
            hist_planes
        ]














