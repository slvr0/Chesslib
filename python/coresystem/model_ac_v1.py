#!/usr/bin/env python3

import tensorflow as tf


from configs.network_config import *

from coresystem.network_factory_tools import *


def create_net_topology_ac_v1(netconf = None) :
    wdl_active = False
    consider_moves_left = False

    input_1 = tf.keras.Input(shape=INPUT_DIMS, name='my_input_1')


    #1 conv block
    flow = conv_block( inputs = input_1,
                       filter_size=3,
                       output_channels=RESIDUAL_FILTERS,
                       name='my_input_1',
                       bn_scale=True)

    #N residual blocks
    flow = residual_block(flow,
                           RESIDUAL_FILTERS,
                           name='residual_{}'.format(1))

    #connect policy head (lets start with convolutional, i dont want flattened

    if POLICY_HEAD_FORMAT == "policy_conv":
        print("[policy convolutional head configured]")
        conv_pol = conv_block(flow,
                               filter_size=3,
                               output_channels=RESIDUAL_FILTERS,

                               name='policy1')

        conv_pol2 = tf.keras.layers.Conv2D(
            80,
            3,
            use_bias=True,
            padding='same',
            kernel_initializer='glorot_normal',
            kernel_regularizer=L2_REG,
            bias_regularizer=L2_REG,
            data_format='channels_first',
            name='policy')(conv_pol)

        pout = ApplyPolicyMap()(conv_pol2)
    else : pout = None


    #connect valuehead

    # Value head
    conv_val = conv_block(flow,
                               filter_size=1,
                               output_channels=32,
                               name='value')

    h_conv_val_flat = tf.keras.layers.Flatten()(conv_val)

    h_fc2 = tf.keras.layers.Dense(128,
                                  kernel_initializer='glorot_normal',
                                  kernel_regularizer=L2_REG,
                                  activation=DEFAULT_ACTIVATION,
                                  name='value/dense1')(h_conv_val_flat)
    if wdl_active:
        vout = tf.keras.layers.Dense(3,
                                      kernel_initializer='glorot_normal',
                                      kernel_regularizer=L2_REG,
                                      bias_regularizer=L2_REG,
                                      name='value/dense2')(h_fc2)
    else:
        vout = tf.keras.layers.Dense(1,
                                      kernel_initializer='glorot_normal',
                                      kernel_regularizer=L2_REG,
                                      activation='tanh',
                                      name='value/dense2')(h_fc2)

    # # Moves left head
    # if consider_moves_left:
    #     conv_mov = conv_block(flow,
    #                                filter_size=1,
    #                                output_channels=8,
    #                                name='moves_left')
    #
    #     h_conv_mov_flat = tf.keras.layers.Flatten()(conv_mov)
    #
    #     h_fc4 = tf.keras.layers.Dense(
    #         128,
    #         kernel_initializer='glorot_normal',
    #         kernel_regularizer=L2_REG,
    #         activation=DEFAULT_ACTIVATION,
    #         name='moves_left/dense1')(h_conv_mov_flat)
    #
    #     mlh_out = tf.keras.layers.Dense(1,
    #                                   kernel_initializer='glorot_normal',
    #                                   kernel_regularizer=L2_REG,
    #                                   activation='relu',
    #                                   name='moves_left/dense2')(h_fc4)
    # else :
    #     mlh_out = None

    ac_v1 = tf.keras.Model(inputs=[input_1], outputs=[pout, vout])

    ac_v1.compile()

    return ac_v1