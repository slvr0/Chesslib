#include "chessboard_generator.h"

#include "chessboard_extractor.h"

Board ChessboardGenerator::CreateFromFen(const std::string& fen) {
     BBoard white_pawn = 0x0;
     BBoard white_knight = 0x0;
     BBoard white_bishop = 0x0;
     BBoard white_rook = 0x0;
     BBoard white_queen = 0x0;
     BBoard white_king = 0x0;
     BBoard black_pawn = 0x0;
     BBoard black_knight = 0x0;
     BBoard black_bishop = 0x0;
     BBoard black_rook = 0x0;
     BBoard black_queen = 0x0;
     BBoard black_king = 0x0;    
     BBoard white = 0x0;
     BBoard black = 0x0;    
     BBoard occ = 0x0;
    
    bool white_acts = true;

    bool white_oo = false;
    bool white_ooo = false;
    bool black_oo = false;
    bool black_ooo = false;

    int  enp = -1;

    int spos = 0;
    for(int i = 56; i>=0;) {
        char c =  fen.at(spos++);  

        if(c == ' ') break;
 
        if(isdigit(c)) { 
            int stepcount = static_cast<int> (c - 48);
            i += stepcount;
        }
        else if(c == '/') {
            i -= 16;
            continue; // we literally dont care
        }        
        else {
            
            switch(c) {
                case 'P' : white_pawn |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'N' : white_knight |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'B' : white_bishop |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'R' : white_rook |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'Q' : white_queen |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'K' : white_king |= 1ULL << i; white |= 1ULL << i; occ |= 1ULL << i ; break;

                case 'p' : black_pawn |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'n' : black_knight |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'b' : black_bishop |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'r' : black_rook |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'q' : black_queen |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;
                case 'k' : black_king |= 1ULL << i; black |= 1ULL << i; occ |= 1ULL << i ; break;

                default : break;
            }
            ++i;
        }        
    }  
    char to_act = fen.at(spos++);
    white_acts= to_act == 'w' ? true : false;    

    ++spos;   

    auto update_castling = [&] (char w) { 
        if(w == 'K') white_oo = true; 
        else if(w == 'Q') white_ooo = true;
        else if(w == 'k') black_oo = true;
        else if(w == 'q') black_ooo = true;
    };    
    if(fen.at(spos) == '-') {
        ++spos;
    }
    else{
        for(int i = 0 ; i < 4 ; ++i) { 
            if(fen.at(spos) == ' ') {
                break; 
            }
            
            update_castling(fen.at(spos++));            
        }
    }    

    if(fen.at(spos) != '-') {                
        std::string notation = fen.substr(spos + 1, 2);
        enp = notation_idx(notation);
        
        spos += 3;
    }
    else spos += 2;

    int rule50 = fen.at(spos + 1) == ' ' ? 
        static_cast<int> (fen.at(spos) - 48) :
        std::stoi(fen.substr(spos, 2));

    spos += fen.at(spos + 1) == ' ' ? 2 : 3;

    int totalmoves = std::stoi(fen.substr(spos, fen.size())); 
    Board b1 = Board(white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king, 
                black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,                
                white_acts, white_oo, white_ooo, black_oo, black_ooo, enp                  
               );
    b1.half_move_ = rule50;
    b1.full_move_ = totalmoves;

    ZHash::GetInstance().GenerateNewZobristKey(b1);
    return b1;
}