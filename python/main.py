from coresystem.coresys import *
from coresystem.data_preprocessor import *
from coresystem.state_representation import *

from configs.generalconfig import *

import chess

if __name__ == '__main__':
    generalconf = create_general_conf()
    core = CoreSystem(generalconf)

    chessboard = chess.Board("8/8/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq a3 3 4")

    print(chessboard.pawns & chessboard.occupied_co[0]) #zero index in occupied_co is black


    print(chessboard.ply())

    print(chessboard.ep_square)

    print(boardnotations[63 - chessboard.ep_square])


