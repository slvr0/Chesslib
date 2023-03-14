#!/usr/bin/env python3

import tensorflow as tf


from configs.network_config import *

from coresystem.action_space import *

INPUT_DIMS = (32,8,8)
L2_REG = tf.keras.regularizers.l2(l=0.5 * (0.0001))
SE_R = 32
DEFAULT_ACTIVATION = 'relu'
KERNEL_FILTERS = (3,3)
STRIDE_DEF = 1
RESIDUAL_FILTERS = 32 #im not sure ? gotta learn what this should be
POLICY_HEAD_FORMAT = "policy_conv"

class ApplyPolicyMap(tf.keras.layers.Layer):
    def __init__(self, **kwargs):
        super(ApplyPolicyMap, self).__init__(**kwargs)
        self.fc1 = tf.constant(make_map()) #this creates a translation from policy index to the 8x8x80 neural output
        #ie the network thinks in terms of vector movements, we translate it to a index ie e2e4 -> 256 etc.

    def call(self, inputs):
        h_conv_pol_flat = tf.reshape(inputs, [-1, 80 * 8 * 8])
        return tf.matmul(h_conv_pol_flat,
                         tf.cast(self.fc1, h_conv_pol_flat.dtype))

class ApplySqueezeExcitation(tf.keras.layers.Layer):
    def __init__(self, **kwargs):
        super(ApplySqueezeExcitation, self).__init__(**kwargs)

    def build(self, input_dimens):
        self.reshape_size = input_dimens[1][1]

    def call(self, inputs):
        x = inputs[0]
        excited = inputs[1]
        gammas, betas = tf.split(tf.reshape(excited,
                                            [-1, self.reshape_size, 1, 1]),
                                 2,
                                 axis=1)
        return tf.nn.sigmoid(gammas) * x + betas

def squeeze_excitation(inputs, channels, name):
    assert channels % SE_R == 0
    pooled = tf.keras.layers.GlobalAveragePooling2D(
        data_format='channels_first')(inputs)
    squeezed = tf.keras.layers.Activation(DEFAULT_ACTIVATION)(tf.keras.layers.Dense(
        channels / SE_R,
        kernel_initializer='glorot_normal',
        kernel_regularizer=L2_REG ,
        name=name + '/se/dense1')(pooled))
    excited = tf.keras.layers.Dense(2 * channels,
                                    kernel_initializer='glorot_normal',
                                    kernel_regularizer=L2_REG,
                                    name=name + '/se/dense2')(squeezed)

    return ApplySqueezeExcitation()([inputs, excited])

# 2-layer dense feed-forward network in encoder blocks
def ffn(inputs, emb_size, dff, name):
    dense1 = tf.keras.layers.Dense(dff, kernel_initializer='glorot_normal', activation='selu',
                                   name=name + "/dense1")(inputs)
    return tf.keras.layers.Dense(emb_size, kernel_initializer='glorot_normal', name=name + "/dense2")(dense1)

def batch_norm(input, name, scale=False):
        return tf.keras.layers.BatchNormalization(
            epsilon=1e-5,
            axis=1,
            center=True,
            scale=scale,
            name=name)(input)

def conv_layer(inputs,
                   filter_size,
                   output_channels,
                   name,
                   bn_scale=False) :

    conv = tf.keras.layers.Conv2D(output_channels,
                                  filter_size,
                                  use_bias=False,
                                  padding='same',
                                  kernel_initializer='glorot_normal',
                                  kernel_regularizer=L2_REG,
                                  data_format='channels_first',
                                  name=name + '/conv2d')(inputs)
    return tf.keras.layers.Activation('relu')(batch_norm(
        conv, name=name + '/bn', scale=bn_scale))

def conv_block(inputs,
               filter_size,
               output_channels,
               name,
               bn_scale=False):
    conv = tf.keras.layers.Conv2D(output_channels,
                                  filter_size,
                                  use_bias=False,
                                  padding='same',
                                  kernel_initializer='glorot_normal',
                                  kernel_regularizer=L2_REG,
                                  data_format='channels_first',
                                  name=name + '/conv2d')(inputs)
    return tf.keras.layers.Activation(DEFAULT_ACTIVATION)(batch_norm(
        conv, name=name + '/bn', scale=bn_scale))

def residual_block(inputs, channels, name):
    conv1 = tf.keras.layers.Conv2D(channels,
                                   3,
                                   use_bias=False,
                                   padding='same',
                                   kernel_initializer='glorot_normal',
                                   kernel_regularizer=L2_REG,
                                   data_format='channels_first',
                                   name=name + '/1/conv2d')(inputs)
    out1 = tf.keras.layers.Activation(DEFAULT_ACTIVATION)(batch_norm(conv1,name +'/1/bn', scale=False))


    conv2 = tf.keras.layers.Conv2D(channels,
                                   3,
                                   use_bias=False,
                                   padding='same',
                                   kernel_initializer='glorot_normal',
                                   kernel_regularizer=L2_REG,
                                   data_format='channels_first',
                                   name=name + '/2/conv2d')(out1)

    out2 = squeeze_excitation(batch_norm(conv2,
                                        name + '/2/bn',
                                        scale=True),
                                        channels,
                                        name=name + '/se')
    return tf.keras.layers.Activation(DEFAULT_ACTIVATION)(tf.keras.layers.add(
        [inputs, out2]))

