from coresystem.coresys import *
from coresystem.data_preprocessor import *
from coresystem.state_representation import *
import re
from configs.generalconfig import *
import chess
import chess.pgn
import os

if __name__ == '__main__':


    generalconf = create_general_conf()
    sv_conf = create_sv_conf()
    core = CoreSystem(generalconf)
    core.init_supervised_training(sv_conf)















