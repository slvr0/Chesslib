#!/usr/bin/env python3

import tensorflow as tf
import numpy as np

from coresystem.policy_index import *

class NetworkOutputClassifier :
    def __init__(self):
        self.mask_legal_moves = True

    def parse(net_out, mirror = False):
        pass

    def correct_policy(self, target, output):
        output = tf.cast(output, tf.float32)
        # Calculate loss on policy head

        # extract mask for legal moves from target policy
        move_is_legal = tf.greater_equal(target, 0)
        # replace logits of illegal moves with large negative value (so that it doesn't affect policy of legal moves) without gradient
        illegal_filler = tf.zeros_like(output) - 1.0e10
        output = tf.where(move_is_legal, output, illegal_filler)

        # y_ still has -1 on illegal moves, flush them to 0
        target = tf.nn.relu(target)
        return target, output

    #target = list[1858]
    @staticmethod
    def correct_policy_ch_comp(target, output):
        output = tf.cast(output, tf.float32)
        # Calculate loss on policy head1

        # extract mask for legal moves from target policy
        move_is_legal = tf.greater(target, 0)

        # replace logits of illegal moves with large negative value (so that it doesn't affect policy of legal moves) without gradient

        illegal_filler = tf.zeros_like(output)
        output = tf.where(move_is_legal, output, illegal_filler)

        return output

    def uci_to_policy(self, ucis : [str]) -> []:
        legal = np.zeros(shape=len(policy_index), dtype=int)
        legal[[policy_index.index(uci) for uci in ucis]] = 1

        return legal

