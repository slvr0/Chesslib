#!/usr/bin/env python3

import tensorflow as tf

class AgentTrainingMethods :
    def __init__(self):
        raise NotImplemented()

    @staticmethod
    def expected_returns(statebatch, conf):
        pass

    @staticmethod
    def huber_loss(target : tf.Tensor, outcome : tf.Tensor):
        return tf.keras.losses.Huber(target, outcome)



