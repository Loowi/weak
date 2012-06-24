#include "weak.h"

// Get the bitboard which encodes a mask of all squares threatened by the specified side in the
// chess set.
BitBoard
AllThreats(ChessSet *chessSet, Side side)
{
  // TODO: Do for more than pawns, knights, rooks, bishops.

  return AllPawnThreats(chessSet, side) | AllKnightThreats(chessSet, side) |
    AllRookThreats(chessSet, side) | AllBishopThreats(chessSet, side);
}

bool
Checked(ChessSet *chessSet, Side side)
{
  BitBoard king;
  switch(side) {
  case White:
    king = chessSet->White.King;
    break;
  case Black:
    king = chessSet->Black.King;
    break;
  default:
    panic("Invalid side %d.", side);
  }

  return (AllThreats(chessSet, OPPOSITE(side))&king) != EmptyBoard;
}

BitBoard
ChessSetOccupancy(ChessSet *chessSet)
{
  return SetOccupancy(&chessSet->White) + SetOccupancy(&chessSet->Black);
}

Piece
ChessSetPieceAt(ChessSet *chessSet, Side side, Position pos)
{
  switch(side) {
  case White:
    return SetPieceAt(&chessSet->White, pos);
  case Black:
    return SetPieceAt(&chessSet->Black, pos);
  }

  panic("Unrecognised side %d.", side);
  return MissingPiece;
}

void
ChessSetPlacePiece(ChessSet *chessSet, Side side, Piece piece, Position pos)
{
  switch(side) {
  case White:
    SetPlacePiece(&chessSet->White, piece, pos);
    break;
  case Black:
    SetPlacePiece(&chessSet->Black, piece, pos);
    break;
  default:
    panic("Invalid side %d.", side);
  }
}

void
ChessSetRemovePiece(ChessSet *chessSet, Side side, Piece piece, Position pos)
{
  switch(side) {
  case White:
    SetRemovePiece(&chessSet->White, piece, pos);
    break;
  case Black:
    SetRemovePiece(&chessSet->Black, piece, pos);
    break;
  default:
    panic("Invalid side %d.", side);
  }  
}

BitBoard
EmptySquares(ChessSet *chessSet)
{
  return ~ChessSetOccupancy(chessSet);
}

ChessSet
NewChessSet()
{
  ChessSet ret;

  ret.White = NewWhiteSet();
  ret.Black = NewBlackSet();

  return ret;
}
