#!/usr/bin/env python3

import tensorflow as tf

from network_interface import *
from configs.network_config import *

def create_net_topology_a1(network_config) :
    input_1 = tf.keras.Input(shape=(5,), name='my_input_1')

    #x1 = tf.keras.layers.Dense(5, activation=tf.nn.relu)(input_1)

    output_1 = tf.keras.layers.Dense(1, activation=tf.nn.sigmoid, name='my_outputs_1')(input_1)
    #output_2 = tf.keras.layers.Dense(1, activation=tf.nn.sigmoid, name='my_outputs_2')(x1)

    model = tf.keras.Model(inputs=[input_1], outputs=[output_1])

    model.compile()

    return model
