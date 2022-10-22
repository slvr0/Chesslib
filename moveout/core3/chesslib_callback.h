#pragma once

namespace Chesslib2 {
class MoveCallback 
{
    public :         

        void AppendMove() {                        
            counter++;
        }

        int DisplayNumMoves() {
            return counter;
        }

    private : 
    int counter = 0;

};
};