#!/usr/bin/env python3
import threading

from environment.supervised_env import SupervisedEnvironment
from configs.sv_hyperparams import create_sv_conf
from coresystem.training_nexus import *

class CoreSystem:
    def __init__(self):
        self.connected = False

        self.nexus_factory = TrainingNexusFactory(None, None)


    def set_c_interface(self, interface):
        self.interface = interface
        self.on_connect() #system connects to c environment

    def on_connect(self):
        self.connected = True
        print(self)

    def on_disconnect(self):
        self.connected = False
        print(self)

    def __str__(self):
        return "Core System , is currently {:} connected to C".format("" if self.connected else "not")



    def initiate_training_environment(self, env, gconf, netconf):
        env_execution_thread = threading.Thread(target=env.process_chunks)
        env_execution_thread.start()

        self.nexus_factory.on_env_create(env, gconf, netconf)
        self.nexus_factory.execute_nexuses()

        env_execution_thread.join()



