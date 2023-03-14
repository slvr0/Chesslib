#!/usr/bin/env python3


class ModelConfig :
    def __init__(self, *args, **kwargs):
        self.model_params = dict(**kwargs)

    def get(self, arg):
        return self.model_params[arg]


def create_network_conf_a1() :
    return {
        'model_v1' : { ModelConfig({
            ("regulizer", 'l2'),
            ('input_size' , (32,8,8)),
            'out'
        }


        )

        }
    }
