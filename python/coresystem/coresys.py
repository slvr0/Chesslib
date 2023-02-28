#!/usr/bin/env python3

class CoreSystem:
    def __init__(self, optionsdict):
        self.connected = False

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

def create_coresystem(optionsdict) :
    return CoreSystem(optionsdict) #??
