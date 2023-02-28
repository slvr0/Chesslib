#!/usr/bin/env python3

import tensorflow as tf

from data_preprocessor import *
from actor_critic_net import *

class INetwork :
    def __init__(self):
        pass

    def store_net(self, networkupdater):
        pass

    #im not sure but perhaps there will be separate nets for white and black. leave the option open
    def load_net(self, networkupdater, white=True):
        pass

    def mask_legal(self, mask):
        pass

    def request(self, state : [InputPlane]) -> tf.Tensor:
        raise NotImplemented()

    def get_trainable_weights(self):
        raise NotImplemented()

class NetworkA1 (INetwork) :
    def __init__(self, network_config):
        super().__init__()
        self.net = create_net_topology_a1(network_config)

