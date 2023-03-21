#!/usr/bin/env python3

import tensorflow as tf
import numpy as np
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'


from coresystem.output_classifier import NetworkOutputClassifier
from environment.pgn_import_parse import V1TrainingSample
from coresystem.model_ac_v1 import create_net_topology_ac_v1

class Metric:
    def __init__(self, short_name, long_name, suffix='', **kwargs):
        self.short_name = short_name
        self.long_name = long_name
        self.suffix = suffix
        self.value = 0.0
        self.count = 0

    def assign(self, value):
        self.value = value
        self.count = 1

    def accumulate(self, value):
        if self.count > 0:
            self.value = self.value + value
            self.count = self.count + 1
        else:
            self.assign(value)

    def merge(self, other):
        assert self.short_name == other.short_name
        self.value = self.value + other.value
        self.count = self.count + other.count

    def get(self):
        if self.count == 0:
            return self.value
        return self.value / self.count

    def reset(self):
        self.value = 0.0
        self.count = 0

class NetworkAgent :
    def __init__(self, learning_rate, global_steps, gconf, netconf, model_path = None):

        self.optimizer = tf.keras.optimizers.Adam(learning_rate = learning_rate)
        self.learning_rate = learning_rate
        self.global_steps = global_steps
        self.cfg = gconf
        self.netconf = netconf
        self.model_path = model_path
        self.moves_left = False
        self.model = self.load_model()
        self.wdl = tf.expand_dims(tf.constant([1.0, 0.0, -1.0]), 1)

        self.q_rat = 0.95
        self.qmix = lambda z, q: q * self.q_rat + z * (1 - self.q_rat)
        self.training_epoch = 0

        self.train_metrics = [
            Metric('P', 'Policy Loss'),
            Metric('V', 'Value Loss'),
            Metric('ML', 'Moves Left Loss'),
            Metric('Reg', 'Reg term'),
            Metric('Total', 'Total Loss'),
            Metric('V MSE', 'MSE Loss')
        ]

        self.test_metrics = [
            Metric('P', 'Policy Loss'),
            Metric('V', 'Value Loss'),
            Metric('ML', 'Moves Left Loss'),
            Metric('V MSE', 'MSE Loss' ),
            Metric('P Acc', 'Policy Accuracy', suffix='%'),
            Metric('V Acc', 'Value Accuracy', suffix='%'),
            Metric('ML Mean', 'Moves Left Mean Error'),
            Metric('P Entropy', 'Policy Entropy'),
            Metric('P UL', 'Policy UL'),
        ]

        """
        think about checkpoint manager later
        """
        # self.checkpoint = tf.train.Checkpoint(optimizer=self.orig_optimizer,
        #                                       model=self.model,
        #                                       global_step=self.global_step,
        #                                       swa_count=self.swa_count)
        # self.checkpoint.listed = self.swa_weights
        # self.manager = tf.train.CheckpointManager(
        #     self.checkpoint,
        #     directory=self.root_dir,
        #     max_to_keep=50,
        #     keep_checkpoint_every_n_hours=24,
        #     checkpoint_name=self.cfg['name'])

        #define checkpoint

        #define loss scaling

        #define output classifer masking

        #define swa

        #define renorm func

        #define eval metrics vector, plugin factor here

        #define gpu strategy , not necessary
        # if self.cfg['gpu'] == 'all':
        #     gpus = tf.config.experimental.list_physical_devices('GPU')
        #     for gpu in gpus:
        #         tf.config.experimental.set_memory_growth(gpu, True)
        #     self.strategy = tf.distribute.MirroredStrategy()
        #     tf.distribute.experimental_set_strategy(self.strategy)
        # else:
        #     gpus = tf.config.experimental.list_physical_devices('GPU')
        #     print(gpus)
        #     tf.config.experimental.set_visible_devices(gpus[self.cfg['gpu']],
        #                                                'GPU')
        #     tf.config.experimental.set_memory_growth(gpus[self.cfg['gpu']],
        #                                              True)
        #     self.strategy = None


        #global step
        self.global_step = tf.Variable(0,
                                       name='global_step',
                                       trainable=False,
                                       dtype=tf.int64)

    def save_model(self):
        tf.keras.models.save_model(
            model=self.model,
            filepath=self.model_path,
            overwrite=True,
            save_format='tf'
        )

    def load_model(self):
        if os.path.exists(self.model_path) :
            print(self.model_path)
            return tf.keras.models.load_model(
                filepath=self.model_path
            )

        else: return create_net_topology_ac_v1(netconf=self.netconf)



    def set_moves_left_loss_fn(self, topology):
        pass

    def policy_loss_fn(self, target, output):
        ntarget = NetworkOutputClassifier.correct_policy_ch_comp(target, output)
        policy_cross_entropy = tf.nn.softmax_cross_entropy_with_logits(
            labels=tf.stop_gradient(ntarget), logits=output, axis=1)
        return tf.reduce_mean(input_tensor=policy_cross_entropy)

    def policy_accuracy_fn(self, target, output):
        #target, output = NetworkOutputClassifier.correct_policy_ch_comp(target, output)
        return tf.reduce_mean(
            tf.cast(
                tf.equal(tf.argmax(input=target, axis=1),
                         tf.argmax(input=output, axis=1)), tf.float32))

    def moves_left_mean_error_fn(self, target, output):
        output = tf.cast(output, tf.float32)
        return tf.reduce_mean(tf.abs(target - output))

    def policy_entropy(self, target, output):
        target, output = NetworkOutputClassifier.correct_policy_ch_comp(target, output)
        softmaxed = tf.nn.softmax(output)
        return tf.math.negative(
            tf.reduce_mean(
                tf.reduce_sum(tf.math.xlogy(softmaxed, softmaxed),
                              axis=1)))

    def policy_uniform_loss(self, target, output):
        uniform = tf.where(tf.greater_equal(target, 0),
                           tf.ones_like(target), tf.zeros_like(target))
        balanced_uniform = uniform / tf.reduce_sum(
            uniform, axis=1, keepdims=True)
        target, output = NetworkOutputClassifier.correct_policy_ch_comp(target, output)
        policy_cross_entropy = \
            tf.nn.softmax_cross_entropy_with_logits(labels=tf.stop_gradient(balanced_uniform),
                                                    logits=output)
        return tf.reduce_mean(input_tensor=policy_cross_entropy)

    """
    Loss on value head
    """
    def value_loss_fn(self, target, output):
        output = tf.cast(output, tf.float32)
        value_cross_entropy = tf.nn.softmax_cross_entropy_with_logits(
            labels=tf.stop_gradient(target), logits=output)
        return tf.reduce_mean(input_tensor=value_cross_entropy)

    """
    Loss on value head
    [Alpha does a matmul with [1, 0, -1] wdl vector], we have scalar comparison simply until we implement WDL head
    """
    def mse_loss_fn(self, target, output):
        output = tf.cast(output, tf.float32)
        scalar_z_conv = tf.matmul(tf.nn.softmax(output), self.wdl)
        scalar_target = tf.matmul(target, self.wdl)
        return tf.reduce_mean(input_tensor=tf.math.squared_difference(
            scalar_target, scalar_z_conv))

    def accuracy_fn(self, target, output):
        output = tf.cast(output, tf.float32)
        return tf.reduce_mean(
            tf.cast(
                tf.equal(tf.argmax(input=target, axis=1),
                         tf.argmax(input=output, axis=1)), tf.float32))


    def make_model(self, topology, load_existing = ""):
        pass


    """
    must implement checkpoint manager
    """
    def restore(self):
        if self.manager.latest_checkpoint is not None:
            print("Restoring from {0}".format(self.manager.latest_checkpoint))
            self.checkpoint.restore(self.manager.latest_checkpoint)


    def loss_mix_fn(self, policy, value, moves_left, reg_term):
        """
        pol_loss_w, val_loss_w, moves_loss_w, reg_term_w are scaling factors defined in config input.
        Im not sure what these should be set too.
        """
        pol_loss_w = 1.0
        val_loss_w = 1.0
        moves_loss_w = 0.0
        reg_term_w = 0.0

        return pol_loss_w * policy + val_loss_w * value + moves_loss_w * moves_left + reg_term_w * reg_term

    """
    We dont train with WDL yet
    """
    #@tf.function()
    def inner_train_process(self, train_batch : [V1TrainingSample]):

        save_rate = 50
        eval_buffer_len = 100
        qx_dec_fac = 0.85
        batch_size = len(train_batch)
        decay = [1.0, 0.85, 0.7, 0.55, 0.4]

        output_classifier = NetworkOutputClassifier()
        mseloss = tf.keras.losses.MeanSquaredError()

        acc_pol_loss = tf.Variable(0.0)
        acc_value_loss = tf.Variable(0.0)
        acc_loss = tf.Variable(0.0)

        for sample in train_batch:
            Q = sample.q
            legal_moves = sample.legal_moves
            played = sample.played
            played_uci = sample.played_uci
            Qx = sample.qx
            result = sample.result
            state = sample.input

            train_steps = 0
            with tf.GradientTape() as tape:
                policy, value = self.model(tf.expand_dims(state, axis=0))
                legal_vector, targets  = np.zeros(shape=1858), np.zeros(shape=1858)
                legal_vector[legal_moves] = 1
                for i, qx in enumerate(Qx): targets[qx] = decay[i]
                targets[played] = .85 #just something to get started
                targets = np.expand_dims(targets, 0)
                policy_clean = output_classifier.correct_policy_ch_comp(legal_vector, policy)
                policy_loss = self.policy_loss_fn(targets, policy_clean)
                value_loss = mseloss(Q, value)
                loss = value_loss + policy_loss
                accuracy = self.accuracy_fn(targets, policy_clean)

                # Compute the gradients from the loss
                grads = tape.gradient(loss, self.model.trainable_variables)
                # Apply the gradients to the model's parameters
                self.optimizer.apply_gradients(zip(grads, self.model.trainable_variables))

                train_steps += 1
                acc_pol_loss.assign_add(policy_loss)
                acc_value_loss.assign_add(value_loss)
                acc_loss.assign_add(loss)

            self.training_epoch += 1
            loss_v = loss.numpy()


        return  acc_pol_loss.numpy() / train_steps , \
                acc_value_loss.numpy() / train_steps, \
                acc_loss.numpy() / train_steps


            #if something something save model

    @tf.function()
    def evalute_loss(self, target, outcome):
        pass

    @tf.function()
    def read_weights(self):
        return [w.read_value() for w in self.model.weights]

    @tf.function()
    def merge_grads(self, grads, new_grads):
        return [tf.math.add(a, b) for (a, b) in zip(grads, new_grads)]

    @tf.function()
    def strategy_merge_grads(self, grads, new_grads):
        return self.strategy.run(self.merge_grads, args=(grads, new_grads))

    # # x,y,z,q,m
    # with tf.GradientTape() as tape:
    #     outputs = self.model(x, training=True)
    #     policy = outputs[0]
    #     value = outputs[1]
    #
    #     policy_loss = self.policy_loss_fn(y, policy)
    #     reg_term = sum(self.model.losses)
    #     value_mse_loss = self.mse_loss_fn(self.qMix(z, q), value)
    #     value_loss = value_mse_loss
    #     moves_left_loss = tf.constant(0.)  # for now
    #     total_loss = self.loss_mix_fn(policy_loss, value_loss, moves_left_loss,
    #                                   reg_term)
    #
    # metrics = [
    #     policy_loss,
    #     value_loss,
    #     moves_left_loss,
    #     reg_term,
    #     total_loss,
    # ]
    # return metrics, tape.gradient(total_loss, self.model.trainable_weights)







