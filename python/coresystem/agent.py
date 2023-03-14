#!/usr/bin/env python3

import tensorflow as tf

class NetworkAgent :
    def __init__(self, network_topology, cfg):
        self.cfg = cfg
        self.wdl = None
        self.moves_left = True

        self.moves_left_loss_fn = None
        self.policy_loss_fn = None
        self.value_loss_fn = None

        #define checkpoint

        #define loss scaling

        #define output classifer masking

        #define swa

        #define renorm func

        #define eval metrics vector, plugin factor here

        #define gpu strategy
        if self.cfg['gpu'] == 'all':
            gpus = tf.config.experimental.list_physical_devices('GPU')
            for gpu in gpus:
                tf.config.experimental.set_memory_growth(gpu, True)
            self.strategy = tf.distribute.MirroredStrategy()
            tf.distribute.experimental_set_strategy(self.strategy)
        else:
            gpus = tf.config.experimental.list_physical_devices('GPU')
            print(gpus)
            tf.config.experimental.set_visible_devices(gpus[self.cfg['gpu']],
                                                       'GPU')
            tf.config.experimental.set_memory_growth(gpus[self.cfg['gpu']],
                                                     True)
            self.strategy = None


        #global step
        self.global_step = tf.Variable(0,
                                       name='global_step',
                                       trainable=False,
                                       dtype=tf.int64)


    def set_moves_left_loss_fn(self, topology):
        pass

    def set_policy_loss_fn(self, topology):
        pass

    def set_value_loss_fn(self, topology):
        pass


    def make_model(self, topology, load_existing = ""):
        pass

    @tf.function()
    def inner_train_process(self, x,y,z,q,m):
        pass

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







