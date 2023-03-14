#!/usr/bin/env python3

import tensorflow as tf

from network_interface import *
from configs.network_config import *



L2_REG = tf.keras.regularizers.l2(l=0.5 * (0.0001))
SE_R = 32
DEFAULT_ACTIVATION = 'relu'


def process_inner_loop(self, x, y, z, q, m):
    with tf.GradientTape() as tape:
        outputs = self.model(x, training=True)
        policy = outputs[0]
        value = outputs[1]
        policy_loss = self.policy_loss_fn(y, policy)
        reg_term = sum(self.model.losses)
        if self.wdl:
            value_ce_loss = self.value_loss_fn(self.qMix(z, q), value)
            value_loss = value_ce_loss
        else:
            value_mse_loss = self.mse_loss_fn(self.qMix(z, q), value)
            value_loss = value_mse_loss
        if self.moves_left:
            moves_left = outputs[2]
            moves_left_loss = self.moves_left_loss_fn(m, moves_left)
        else:
            moves_left_loss = tf.constant(0.)
        total_loss = self.lossMix(policy_loss, value_loss, moves_left_loss,
                                  reg_term)
        if self.loss_scale != 1:
            total_loss = self.optimizer.get_scaled_loss(total_loss)
    if self.wdl:
        mse_loss = self.mse_loss_fn(self.qMix(z, q), value)
    else:
        value_loss = self.value_loss_fn(self.qMix(z, q), value)
    metrics = [
        policy_loss,
        value_loss,
        moves_left_loss,
        reg_term,
        total_loss,
        # Google's paper scales MSE by 1/4 to a [0, 1] range, so do the same to
        # get comparable values.
        mse_loss / 4.0,
    ]
    return metrics, tape.gradient(total_loss, self.model.trainable_weights)

#renormalized input option removed
def batch_norm(input, name, scale=False):
        return tf.keras.layers.BatchNormalization(
            epsilon=1e-5,
            axis=1,
            center=True,
            scale=scale,
            name=name)(input)


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
    assert channels % SE_R == 0 #hmm
    pooled = tf.keras.layers.GlobalAveragePooling2D(
        data_format='channels_first')(inputs)
    squeezed = tf.keras.layers.Activation(DEFAULT_ACTIVATION)(tf.keras.layers.Dense(
        channels / SE_R,
        kernel_initializer='glorot_normal',
        kernel_regularizer=L2_REG ,
        name=name + '/se/dense1')(pooled))
    excited = tf.keras.layers.Dense(2 * channels,
                                    kernel_initializer='glorot_normal',
                                    kernel_regularizer=SE_R,
                                    name=name + '/se/dense2')(squeezed)

    return ApplySqueezeExcitation()([inputs, excited])



def conv_block(        inputs,
                       filter_size,
                       output_channels,
                       name,
                       bn_scale=False):
            conv = tf.keras.layers.Conv2D(output_channels,
                                          filter_size,
                                          use_bias=False,
                                          padding='same',
                                          kernel_initializer='glorot_normal',
                                          kernel_regularizer=l2_regulizer,
                                          data_format='channels_first',
                                          name=name + '/conv2d')(inputs)
            return tf.keras.layers.Activation('relu')(self.batch_norm(
                conv, name=name + '/bn', scale=bn_scale))
