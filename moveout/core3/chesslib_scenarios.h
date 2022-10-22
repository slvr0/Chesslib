#pragma once


//we define the actions possible to take for the move generator, for example pawn attack left , pawn attack right, enp, castle etc.
//each scenario is analyzed and can generate legal move


namespace Chesslib2 {
struct Scenarios {



//copy these scenarios, 

/* 	template<class BoardStatus status, int depth>
	static void Kingmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move<BoardPiece::King, status.WhiteMove>(brd, from, to, to & Enemy<status.WhiteMove>(brd));
		IFPRN std::cout << "Kingmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));

		PerfT<status.KingMove(), depth - 1>(next);
	}

	template<class BoardStatus status, int depth>
	static void KingCastle(const Board& brd, uint64_t kingswitch, uint64_t rookswitch)
	{
		Board next = Board::MoveCastle<status.WhiteMove>(brd, kingswitch, rookswitch);
		IFPRN std::cout << "KingCastle:\n" << _map(kingswitch, rookswitch, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, false);xx
		PerfT<status.KingMove(), depth - 1>(next);
	}

	template<class BoardStatus status, int depth>
	static void PawnCheck(map eking, uint64_t to) {
		constexpr bool white = status.WhiteMove;
		map pl = Pawn_AttackLeft<white>(to & Pawns_NotLeft());
		map pr = Pawn_AttackRight<white>(to & Pawns_NotRight());

		if (eking & (pl | pr)) Movestack::Check_Status[depth - 1] = to;
	}

	template<class BoardStatus status, int depth>
	static void KnightCheck(map eking, uint64_t to) {
		constexpr bool white = status.WhiteMove;

		if (Lookup::Knight(SquareOf(eking)) & to) Movestack::Check_Status[depth - 1] = to;
	}
	

	template<class BoardStatus status, int depth>
	static void Pawnmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move<BoardPiece::Pawn, status.WhiteMove, false>(brd, from, to);
		IFPRN std::cout << "Pawnmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		PawnCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.SilentMove(), depth - 1>(next);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;
	}

	template<class BoardStatus status, int depth>
	static void Pawnatk(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move<BoardPiece::Pawn, status.WhiteMove, true>(brd, from, to);
		IFPRN std::cout << "Pawntake:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		PawnCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.SilentMove(), depth - 1>(next);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;
	}

	template<class BoardStatus status, int depth>
	static void PawnEnpassantTake(const Board& brd, uint64_t from, uint64_t enemy, uint64_t to)
	{
		Board next = Board::MoveEP<status.WhiteMove>(brd, from, enemy, to);
		IFPRN std::cout << "PawnEnpassantTake:\n" << _map(from | enemy, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, true);
		PawnCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.SilentMove(), depth - 1>(next);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;
	}

	template<class BoardStatus status, int depth>
	static void Pawnpush(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move <BoardPiece::Pawn, status.WhiteMove, false>(brd, from, to);
		IFPRN std::cout << "Pawnpush:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));

		Movelist::EnPassantTarget = to;
		PawnCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.PawnPush(), depth - 1>(next);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;
	}

	template<class BoardStatus status, int depth>
	static void Pawnpromote(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next1 = Board::MovePromote<BoardPiece::Queen, status.WhiteMove>(brd, from, to);
		IFPRN std::cout << "Pawnpromote:\n" << _map(from, to, brd, next1) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next1, to & Enemy<status.WhiteMove>(brd));
		PerfT<status.SilentMove(), depth - 1>(next1);

		Board next2 = Board::MovePromote<BoardPiece::Knight, status.WhiteMove>(brd, from, to);
		KnightCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.SilentMove(), depth - 1>(next2);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;

		Board next3 = Board::MovePromote<BoardPiece::Bishop, status.WhiteMove>(brd, from, to);
		PerfT<status.SilentMove(), depth - 1>(next3);
		Board next4 = Board::MovePromote<BoardPiece::Rook, status.WhiteMove>(brd, from, to);
		PerfT<status.SilentMove(), depth - 1>(next4);
	}

	template<class BoardStatus status, int depth>
	static void Knightmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move <BoardPiece::Knight, status.WhiteMove>(brd, from, to, to & Enemy<status.WhiteMove>(brd));
		IFPRN std::cout << "Knightmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		KnightCheck<status, depth>(EnemyKing<status.WhiteMove>(brd), to);
		PerfT<status.SilentMove(), depth - 1>(next);
		Movestack::Check_Status[depth - 1] = 0xffffffffffffffffull;
	}

	template<class BoardStatus status, int depth>
	static void Bishopmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move <BoardPiece::Bishop, status.WhiteMove>(brd, from, to, to & Enemy<status.WhiteMove>(brd));
		IFPRN std::cout << "Bishopmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		PerfT<status.SilentMove(), depth - 1>(next);
	}

	template<class BoardStatus status, int depth>
	static void Rookmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move<BoardPiece::Rook, status.WhiteMove>(brd, from, to, to & Enemy<status.WhiteMove>(brd));
		IFPRN std::cout << "Rookmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		if constexpr (status.CanCastle()) {
			if (status.IsLeftRook(from)) PerfT<status.RookMove_Left(), depth - 1>(next);
			else if (status.IsRightRook(from)) PerfT<status.RookMove_Right(), depth - 1>(next);
			else PerfT<status.SilentMove(), depth - 1>(next);
		}
		else PerfT<status.SilentMove(), depth - 1>(next);
	}

	template<class BoardStatus status, int depth>
	static void Queenmove(const Board& brd, uint64_t from, uint64_t to)
	{
		Board next = Board::Move<BoardPiece::Queen, status.WhiteMove>(brd, from, to, to & Enemy<status.WhiteMove>(brd));
		IFPRN std::cout << "Queenmove:\n" << _map(from, to, brd, next) << "\n";
		//IFDBG Board::AssertBoardMove<status.WhiteMove>(brd, next, to & Enemy<status.WhiteMove>(brd));
		PerfT<status.SilentMove(), depth - 1>(next);
	} */




















};
};
