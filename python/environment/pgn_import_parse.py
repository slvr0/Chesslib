
#!/usr/bin/env python3

import chess
import dataclasses
import stockfish
import numpy as np
from datetime import date
import struct
from functools import partial
import re
from typing import Dict
import os
import multiprocessing as mp
from queue import Queue
from collections import deque
import threading
from coresystem.policy_index import policy_index
from coresystem.data_preprocessor import extract_bits, boardnotations, boardnotations_mirror


#mtx = mp.Lock()
std_mtx = mp.Lock()

BUFFER_FORMAT_V1 = '= 1f125HH5HH256s'

_empty_bin  = 0x0
_full_bin   = 0xFFFFFFFFFFFFFFFF

PLYFILL_V = [
    0x0,
    0xFF,
    0xFFFF,
    0xFFFFFFFF,
    0xFFFFFFFFFFFFFFFF
]

PLYFILL_RATIO_VEC = [
    range(0, 1),
    range(1, 12),
    range(13, 25),
    range(26, 38),
    range(39, 1000)
]

class V1TrainingSample :
    q : float
    legal_moves : [int]
    played : int # Mirrored from point of view of active player
    played_uci : str
    qx : (int)
    result : int
    input : np.array

    def __init__(self, buffer = None):
        if buffer : self.unpack_from_buffer(buffer)

    def unpack_from_buffer(self, bfr):
        self.q = bfr[0]
        self.legal_moves = [m for m in bfr[1:126] if m != 0]
        self.played = bfr[126:127][0]
        self.played_uci = policy_index[self.played]
        self.qx = bfr[127:132]
        self.result = bfr[132:133][0]
        self.input = np.unpackbits(np.frombuffer(bfr[133:][0],
                                                  dtype=np.ubyte, count=256)).astype(np.uint64).reshape((32, 8, 8))
        self.input = np.flip(self.input, axis=(1, 2))

class V1FileUnpacker :
    @staticmethod
    def unpack_v1_training_batch(filename):
        with open(filename, "rb") as file :
            struct_fmt = BUFFER_FORMAT_V1
            struct_len = struct.calcsize(struct_fmt)
            struct_unpack = struct.Struct(struct_fmt).unpack_from
            return [V1TrainingSample(struct_unpack(chunk)) for chunk in iter(partial(file.read, struct_len), b'')]

class V1MetricDataParser :
    @staticmethod
    def parse_metrics(board, sf, winner, move_played, file):
        """
        uint16_t legal              125
        uint16_t moveplayed         2
        float q_target              4
        uint16_t q1                 2
        uint16_t q2                 2
        uint16_t q3                 2
        uint16_t q4                 2
        uint16_t q5                 2
        uint16_t result             2
        """

        """
        Stockfish evaluates position and top 5 moves , used as target predictions
        """
        def make_trans(uci):
            t = uci.maketrans("12345678abcdefgh", "87654321hgfedcba")
            return uci[:4].translate(t)

        q = sf.get_evaluation()['value'] * .01  # convert from centipawns
        qs = [
            policy_index.index(m['Move'].strip('n')) if board.turn
            else policy_index.index(make_trans(m['Move'].strip('n'))) for m in sf.get_top_moves(5)]

        """
        Chesslib extracts all legal move indices, speeds up masking during training predictions
        """
        # replacement for stockfish somewhere?Sunfish?Something Non UCI
        legal_moves = [policy_index.index(
            m.uci().strip('n')) if board.turn
                       else policy_index.index(make_trans(m.uci().strip('n'))) for m in board.generate_legal_moves()]

        nmoves = len(legal_moves)
        padwith = 125 - nmoves
        """
        Parse chunks to file
        """
        file.write(struct.pack('f', q))  # 4

        for move in legal_moves:
            file.write(struct.pack('H', int(move)))
        for i in range(padwith):
            file.write(struct.pack('H', _empty_bin))

        #this still needs to be stripped of n in promotion...
        move_played_index = policy_index.index(board.parse_san(move_played).uci().strip('n')) if board.turn \
            else policy_index.index(make_trans(board.parse_san(move_played).uci().strip('n')))
        file.write(struct.pack('H', move_played_index))  # 2

        padq = 5 - len(qs)
        for q in qs: file.write(struct.pack('H', q))  # 2
        for _ in range(padq): file.write(struct.pack('H', 0))  # 2

        file.write(struct.pack('H', winner))  # 2

class V1NeuralPlaneParser:
    @staticmethod
    def parse_state(board, file, history, move_index):
        write_as = 'Q'
        white_acts = board.turn
        extract_from = board if white_acts else board.copy()
        if not white_acts:
            extract_from.apply_transform(chess.flip_vertical)
            extract_from.apply_transform(chess.flip_horizontal)

        ap = extract_from.occupied_co[1] if white_acts else extract_from.occupied_co[0]
        op = extract_from.occupied_co[0] if white_acts else extract_from.occupied_co[1]

        apwn = extract_from.pawns & ap
        akn = extract_from.knights & ap
        absh = extract_from.bishops & ap
        arook = extract_from.rooks & ap
        aqn = extract_from.queens & ap
        aking = extract_from.kings & ap
        opwn = extract_from.pawns & op
        okn = extract_from.knights & op
        obsh = extract_from.bishops & op
        orook = extract_from.rooks & op
        oqn = extract_from.queens & op
        oking = extract_from.kings & op

        file.write(struct.pack(write_as, apwn))
        file.write(struct.pack(write_as, akn))
        file.write(struct.pack(write_as, absh))
        file.write(struct.pack(write_as, arook))
        file.write(struct.pack(write_as, aqn))
        file.write(struct.pack(write_as, aking))
        file.write(struct.pack(write_as, opwn))
        file.write(struct.pack(write_as, okn))
        file.write(struct.pack(write_as, obsh))
        file.write(struct.pack(write_as, orook))
        file.write(struct.pack(write_as, oqn))
        file.write(struct.pack(write_as, oking))

        file.write(struct.pack(write_as, _empty_bin))  # separate edges

        """
        active/observer
        """
        if white_acts:
            file.write(struct.pack(write_as, _full_bin))
            file.write(struct.pack(write_as, _empty_bin))
        else:
            file.write(struct.pack(write_as, _empty_bin))
            file.write(struct.pack(write_as, _full_bin))

        """
        Parse castle state
        """
        c64 = extract_bits(board.castling_rights)
        woo = True if c64.__contains__(7) else False
        wooo = True if c64.__contains__(0) else False
        boo = True if c64.__contains__(63) else False
        booo = True if c64.__contains__(56) else False
        if white_acts:
            file.write(struct.pack(write_as, _full_bin if woo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if wooo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if boo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if booo else _empty_bin))
        else:
            file.write(struct.pack(write_as, _full_bin if boo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if booo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if woo else _empty_bin))
            file.write(struct.pack(write_as, _full_bin if wooo else _empty_bin))
        """
        Parse enp sq
        """
        enp_s = board.ep_square
        if enp_s is None:
            file.write(struct.pack(write_as, _empty_bin))
        else:
            if white_acts:
                file.write(struct.pack(write_as, 1 << enp_s))
            else:
                file.write(struct.pack(write_as, 1 << 63 - enp_s))

        """
        Parse rule 50 
        """
        r50 = board.halfmove_clock
        fill_percent = int((r50 / 50) * 64)
        if fill_percent == 0:
            file.write(struct.pack(write_as, PLYFILL_V[0]))
        elif fill_percent in range(1, 12):
            file.write(struct.pack(write_as, PLYFILL_V[1]))
        elif fill_percent in range(13, 25):
            file.write(struct.pack(write_as, PLYFILL_V[2]))
        elif fill_percent in range(26, 38):
            file.write(struct.pack(write_as, PLYFILL_V[3]))
        else:
            file.write(struct.pack(write_as, _full_bin))
        """
        Parse fullmove as ply
        """
        fullmoves = board.ply()
        fill_percent = int((fullmoves / 100) * 64)
        if fill_percent == 0:
            file.write(struct.pack(write_as, PLYFILL_V[0]))
        elif fill_percent in range(1, 12):
            file.write(struct.pack(write_as, PLYFILL_V[1]))
        elif fill_percent in range(13, 25):
            file.write(struct.pack(write_as, PLYFILL_V[2]))
        elif fill_percent in range(26, 38):
            file.write(struct.pack(write_as, PLYFILL_V[3]))
        else:
            file.write(struct.pack(write_as, _full_bin))
        """
        history
        """

        start = 0 if move_index <= 9 else move_index - 9
        hist = history[start: move_index + 1]
        hist.reverse()

        wact = white_acts
        num_hist_planes = 0

        for i, histinfo in enumerate(hist):
            fidx = boardnotations.index(histinfo[0:2]) if wact else boardnotations_mirror.index(histinfo[0:2])
            tidx = boardnotations.index(histinfo[2:4]) if wact else boardnotations_mirror.index(histinfo[2:4])

            file.write(struct.pack('Q', (1 << fidx) | (1 << tidx)))
            num_hist_planes += 1
            wact = not wact  # swap act/obs in hist chain

        for append_empty_bin in range(num_hist_planes, 10):
            file.write(struct.pack(write_as, _empty_bin))

@dataclasses.dataclass
class PGNMetaDataExtraction:
    __eol: bool = False
    result_map : Dict[str, int] = dataclasses.field(default_factory = lambda : ({'10' : 1, '01' : 2, '1212' : 3}))

    def __call__(self, file):
        self.__eol = False
        res_literal = " "
        """
        A pgn starts with N rows encapsulated with [start, end], this meta data is ended with an empty line
        """
        while (True):
            parse_meta = file.readline()
            if parse_meta == "" : self.__eol = True

            if "[Result" in parse_meta:
                if "*" in parse_meta:
                    res_literal = "1212"  # parse undecisive outcome as draw
                else:
                    res_literal = re.sub('\D', '', parse_meta)
            if (parse_meta == "\n"): break

        assert res_literal !=  " "  # we've not managed to declare an outcome of the game
        return self.__eol, self.result_map[res_literal]

@dataclasses.dataclass
class PGNGameHistoryExtraction:
    __eol : bool = False
    def __call__(self, file):
        self.__eol = False
        moves_full = []

        """
        A continuous string of moves characterized mainly of <moveindex>.<fromsquare><tosquare><x>'captures'<=>'promotion'<promowhat><+>'declarescheck'
        Sites also embed this annoying opening tag , like for example 1. e4 c5 { B20 Sicilian Defense } . 
        Last entry is result, then separates next game with an empty line
        """
        while (True):
            linecontent = file.readline()
            if linecontent == "" :
                self.__eol = True
                break
            elif (linecontent == "\n"):
                break

            moves_p_line = linecontent.strip('\n').split(' ')
            moves_p_line = [m.lstrip('1234567890.') for m in moves_p_line]
            moves_full.extend(moves_p_line)

        moves_full = [move for move in moves_full if move != ''][:-1]

        #special case in for example lichess, removes embedded opening theory
        if '{' in moves_full :
            rf, rt = moves_full.index('{'), moves_full.index('}')
            del moves_full[rf:rt+1]

        return self.__eol, moves_full

class PGNTools :
    @staticmethod
    def peek_entries(readfile : str) -> int :
        meta_extraction = PGNMetaDataExtraction()
        move_extraction = PGNGameHistoryExtraction()
        __eof = False
        n = 0
        with open(readfile, "r", encoding='latin-1') as file:
            while (not __eof ):
                __eof, _ = meta_extraction(file)
                __eof, _ = move_extraction(file)

                rblank = file.readline()
                if rblank == "": __eof = True
                n += 1
        return n

class PGNReader :
    def __init__(self):
        self.meta_extraction: PGNMetaDataExtraction = PGNMetaDataExtraction()
        self.move_extraction: PGNGameHistoryExtraction = PGNGameHistoryExtraction()
        self.condvar = mp.Condition(lock=std_mtx)
        self.jobque: mp.Queue = mp.Queue(maxsize=15000)

    def read_pgn_to_v1_chunks(self, readfile, nfiles = 0):
        __eof = False
        processed_games = 0
        with open(readfile, "r", encoding='latin-1') as file:
            while (not __eof):
                __eof, winner = self.meta_extraction(file)
                __eof, moves = self.move_extraction(file)

                # reads final blank line to close the file before reading another entry pair
                rblank = file.readline()
                if rblank == "": __eof = True

                processed_games += 1
                if processed_games > nfiles:
                    self.jobque.put((moves, winner))

                # with self.condvar :
                #     print("push job!")
                #     self.jobque.put((moves, winner))
                #     self.condvar.notify()


set_sf_str = lambda sf, strength : sf.set_elo_rating(strength)

@dataclasses.dataclass
class V1ChunkParser:
    condvar : mp.Condition
    jobque : mp.Queue
    pathdir: str
    thread_id: int
    b: chess.Board = chess.Board()

    def __post_init__(self):
        pass

    def create_parse_file(self, file, gameindex):
        file = file.replace("pgn", "txt")
        name = file[:-4]
        syntax = file[-4:]
        assert syntax == ".txt"

        fp = os.path.join(self.pathdir, name + "_{}".format(self.thread_id) + "_" + str(gameindex)\
                          + "_" + str(date.today()) + syntax)
        print(fp)
        return open(fp, "wb")

    def process_v1_jobs_onthread(self, filename):
        sf = stockfish.Stockfish()
        sf.set_elo_rating(2200)
        print("sf doesnt block")

        processed = 0
        while True:
            if self.jobque.empty() :
                print("jobque is empty, {} thread exiting".format(self.thread_id))
                break

            with self.condvar:
                self.condvar.wait_for(lambda : not self.jobque.empty())
                print("Thread {} acquired the lock".format(self.thread_id))
                job = self.jobque.get()

            moves   = job[0]
            winner  = job[1]

            self.b.reset()
            #with std_mtx :
            sf.set_fen_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                                    send_ucinewgame_token=True)

            with self.create_parse_file(filename, processed) as parse_file :
                history = []
                for moveindex, move in enumerate(moves):
                    V1MetricDataParser.parse_metrics(self.b, sf, winner, move, parse_file)
                    V1NeuralPlaneParser.parse_state(self.b, parse_file, history, moveindex)
                    san_uci = self.b.push_san(move).uci()
                    sf.set_fen_position(self.b.fen())
                    print(san_uci)
                    history.append(san_uci)
            processed += 1







