#pragma once 

#include <iostream>

namespace Chesslib2 {
enum class FenField {
    white,
    hasEP,
    WCastleL,
    WCastleR,
    BCastleL,
    BCastleR
};

struct FEN {
    static constexpr uint64_t FenEnpassant(std::string_view FEN) {
        uint64_t i = 0;

        while (FEN[i++] != ' ')
        {

        }
        char wb = FEN[i++];
        i++;

        //Castling
        while (FEN[i++] != ' ')
        {

        }

        //En Passant
        char EorMinus = FEN[i++];
        if (EorMinus != '-') {
            if (wb == 'w') {
                //Todo where to store Enpassant
                int EPpos = 32 + ('h' - EorMinus);
                return 1ull << EPpos;
            }
            if (wb == 'b') {
                //Todo where to store Enpassant
                int EPpos = 24 + ('h' - EorMinus);
                return 1ull << EPpos;
            }
        }
        return 0;
    }


    template<FenField field>
    static constexpr bool FenInfo(std::string_view FEN) {
        uint64_t i = 0;
        char c{};

        while ((c = FEN[i++]) != ' ')
        {

        }
        char wb = FEN[i++];

        //White
        if constexpr (field == FenField::white) {
            if (wb == 'w') return true;
            else return false;
        }
        i++;

        //Castling
        while ((c = FEN[i++]) != ' ')
        {
            if constexpr (field == FenField::WCastleR) {
                if (c == 'K') return true;
            }
            if constexpr (field == FenField::WCastleL) {
                if (c == 'Q') return true;
            }
            if constexpr (field == FenField::BCastleR) {
                if (c == 'k') return true;
            }
            if constexpr (field == FenField::BCastleL) {
                if (c == 'q') return true;
            }
        }
        if constexpr (field == FenField::WCastleR || field == FenField::WCastleL || field == FenField::BCastleR || field == FenField::BCastleL)
        {
            return false;
        }

        //En Passant
        char EorMinus = FEN[i++];
        if (EorMinus != '-') {
            if (wb == 'w') {
                //Todo where to store Enpassant
                //int EPpos = 32 + ('h' - EorMinus);
                if constexpr (field == FenField::hasEP) return true;
            }
            if (wb == 'b') {
                //Todo where to store Enpassant
                //int EPpos = 24 + ('h' - EorMinus);
                if constexpr (field == FenField::hasEP) return true;
            }
        }
        if constexpr (field == FenField::hasEP) return false;
    }

    /// Transform FEN character 'n' or 'Q' into bitmap where the bits correspond to the field
    static constexpr uint64_t FenToBmp(std::string_view FEN, char p)
    {
        uint64_t i = 0;
        char c{};
        int Field = 63;

        uint64_t result = 0;
        while ((c = FEN[i++]) != ' ')
        {
            uint64_t P = 1ull << Field;
            switch (c) {
            case '/': Field += 1; break;
            case '1': break;
            case '2': Field -= 1; break;
            case '3': Field -= 2; break;
            case '4': Field -= 3; break;
            case '5': Field -= 4; break;
            case '6': Field -= 5; break;
            case '7': Field -= 6; break;
            case '8': Field -= 7; break;
            default:
                if (c == p) result |= P; //constexpr parsing happens here
            }
            Field--;
        }
        return result;
    }
};
};