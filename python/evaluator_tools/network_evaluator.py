#!/usr/bin/env python3
import dataclasses

import chess
import numpy as np
import stockfish
import tensorflow as tf

from coresystem.data_preprocessor import DataPreprocessor
from coresystem.policy_index import policy_index
from coresystem.output_classifier import NetworkOutputClassifier

from datetime import time, date
import os

@dataclasses.dataclass
class NetworkEval :
    model : tf.keras.Model
    ai_strength : int
    write_to : str = "eval_playouts"

    def eval_by_playout(self, engine_white = True):
        sf = stockfish.Stockfish()
        sf.set_elo_rating(self.ai_strength)
        board = chess.Board()
        neural_processor = DataPreprocessor()
        classifer = NetworkOutputClassifier()
        history = []
        writer = None
        max_moves = 10
        
        def make_trans(uci):
            t = uci.maketrans("12345678abcdefgh", "87654321hgfedcba")
            return uci[:4].translate(t)

        def engine_acts(step):
            start = 0 if step <= 10 else step - 10
            hist = history[start: step + 1]
            hist.reverse()
            neural_input = neural_processor.convolve_into_network_planes(board, hist, engine_white)
            legal_moves = [policy_index.index(
                m.uci().strip('n')) if board.turn
                           else policy_index.index(make_trans(m.uci().strip('n'))) for m in
                           board.generate_legal_moves()]
            legal = np.ones(shape=1858)
            legal[legal_moves] = 0
            policy, value = self.model(np.expand_dims(neural_input, 0))
            policy = np.ma.masked_array(policy,mask=legal)
            return policy_index[np.argmax(policy)]

        def ai_acts() :
            return sf.get_best_move()

        def update_from_move(move):
            move = board.push_san(move).uci()
            history.append(move)
            sf.make_moves_from_current_position([move])

        def state_terminal():
            if board.is_checkmate() or board.is_stalemate(): return True
            return False

        def write_pgn():
            with open(os.path.join(self.write_to, "playout_{}.txt".format(date.today())), "w") as file_open :

                file_open.write("[Event "'"BL2-N8990"'"] \n")
                file_open.write("[Site "'"Germany"'"]\n")
                file_open.write("[Date "'"1990.??.??"'"]\n")
                file_open.write("[Round "'"?"'"]\n")
                file_open.write("[White "'"Cheela Zero"'"]\n")
                file_open.write("[Black "'"Blowfish"'"]\n")
                file_open.write("[Result "'"*"'"]\n")
                file_open.write("[WhiteElo "'""'"]\n")
                file_open.write("[BlackElo "'""'"]\n")
                file_open.write("[ECO "'"A42"'"]\n")
                file_open.write("\n")

                ply = 1
                for i, hist in enumerate(history) :
                    if i % 2 == 0:
                        file_open.write("{}.{} ".format(ply, hist))
                        ply += 1
                    else :
                        file_open.write("{} ".format(hist))
        """
        process one round
        """

        step = 0
        while step < max_moves :
            if engine_white:
                m = engine_acts(step)
                update_from_move(m)
                if(state_terminal()) : break
                m = ai_acts()
                update_from_move(m)
                if(state_terminal()) : break
            else:
                m = ai_acts()
                update_from_move(m)
                if(state_terminal()) : break
                m = engine_acts(step)
                update_from_move(m)
                if(state_terminal()) : break
            step+=1
        write_pgn()












