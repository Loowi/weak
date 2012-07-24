#include "weak.h"

static BitBoard singlePushSources(ChessSet*, Side, BitBoard);
static BitBoard doublePushSources(ChessSet*, Side, BitBoard);
static BitBoard singlePushTargets(ChessSet*, Side, BitBoard);
static BitBoard doublePushTargets(ChessSet*, Side, BitBoard, BitBoard);

// Get BitBoard encoding capture sources for *all* pawns on specified side.
BitBoard
AllPawnCaptureSources(ChessSet *chessSet, Side side)
{
  return PawnCaptureSources(chessSet, side, chessSet->Sets[side].Boards[Pawn]);
}

// Get BitBoard encoding capture targets for *all* pawns on specified side.
BitBoard
AllPawnCaptureTargets(ChessSet *chessSet, Side side)
{
  return PawnCaptureSources(chessSet, side, chessSet->Sets[side].Boards[Pawn]);
}

// Get BitBoard encoding push sources for *all* pawns on specified side.
BitBoard
AllPawnPushSources(ChessSet *chessSet, Side side)
{
  return PawnPushSources(chessSet, side, chessSet->Sets[side].Boards[Pawn]);
}

// Get BitBoard encoding push targets for *all* pawns on specified side.
BitBoard
AllPawnPushTargets(ChessSet *chessSet, Side side)
{
  return PawnPushTargets(chessSet, side, chessSet->Sets[side].Boards[Pawn]);
}

BitBoard
PawnAttackersTo(ChessSet *chessSet, Position to)
{
  return (chessSet->Sets[White].Boards[Pawn] &
          PawnAttacksFrom(to, Black)) |
    (chessSet->Sets[Black].Boards[Pawn] &
     PawnAttacksFrom(to, White));
}

BitBoard
PawnAttacksFrom(Position pawn, Side side)
{
  return PawnThreats(POSBOARD(pawn), side);
}

// Get BitBoard encoding specified pawns which are able to capture.
BitBoard
PawnCaptureSources(ChessSet *chessSet, Side side, BitBoard pawns)
{
  BitBoard opposition = chessSet->Sets[OPPOSITE(side)].Occupancy;

  switch(side) {
  case White:
    return pawns & (SoWeOne(opposition) | SoEaOne(opposition));
  case Black:
    return pawns & (NoWeOne(opposition) | NoEaOne(opposition));
  }

  panic("Invalid side %s.", StringSide(side));
  return EmptyBoard;
}

// Get BitBoard encoding squares able to be captured by specified pawns.
BitBoard
PawnCaptureTargets(ChessSet *chessSet, Side side, BitBoard pawns)
{
  BitBoard opposition = chessSet->Sets[OPPOSITE(side)].Occupancy;

  switch(side) {
  case White:
    return (NoWeOne(pawns) | NoEaOne(pawns)) & opposition;
  case Black:
    return (SoWeOne(pawns) | SoEaOne(pawns)) & opposition;
  }

  panic("Invalid side %s.", StringSide(side));
  return EmptyBoard;
}

// Get BitBoard encoding push sources for specified pawns.
BitBoard
PawnPushSources(ChessSet *chessSet, Side side, BitBoard pawns)
{
  return singlePushSources(chessSet, side, pawns) |
    doublePushSources(chessSet, side, pawns);
}

// Get BitBoard encoding push targets for specified pawns.
BitBoard
PawnPushTargets(ChessSet *chessSet, Side side, BitBoard pawns)
{
  BitBoard singlePushes = singlePushTargets(chessSet, side, pawns);

  return singlePushes | doublePushTargets(chessSet, side, pawns, singlePushes);
}

BitBoard
PawnThreats(BitBoard pawns, Side side)
{
  // TODO: Consider en passant.

  switch(side) {
  case White:
    pawns = NoWeOne(pawns) | NoEaOne(pawns);
    return pawns;
  case Black:
    pawns = SoWeOne(pawns) | SoEaOne(pawns);
    return pawns;
  }

  panic("Invalid side %s.", StringSide(side));
  return EmptyBoard;  
}

// Get BitBoard encoding which of specified pawns are able to single push.
static BitBoard
singlePushSources(ChessSet *chessSet, Side side, BitBoard pawns)
{
  // Any pawn which is able to move forward one place must have an empty square
  // immediately in front of it. If we move all empty squares south one place, we end up
  // with a bitmask for all pawns able to move forward one place.  We do this, rather
  // than shift pawns one place forward and intersect with empty squares, because that
  // would result in targets rather than sources.
  switch(side) {
  case White:
    return SoutOne(chessSet->EmptySquares) & pawns;
  case Black:
    return NortOne(chessSet->EmptySquares) & pawns;
  }

  panic("Invalid side %d.", side);
  return EmptyBoard;
}

// Get BitBoard encoding which of specified pawns are able to double push.
static BitBoard
doublePushSources(ChessSet *chessSet, Side side, BitBoard pawns)
{
  BitBoard emptyRank4, emptyRank3And4, emptyRank5, emptyRank5And6;

  BitBoard empty = chessSet->EmptySquares;

  // Uses logic similar to the single push, however we have to a. take into account
  // double push is only possible if the target is rank 4 (5 for black), and b. consider
  // empty squares in both rank 3 and 4 (5 and 6 for black), as the pawn must pass
  // through both these ranks.
  switch(side) {
  case White:
    emptyRank4 = empty & Rank4Mask;
    emptyRank3And4 = empty & SoutOne(emptyRank4);
    return SoutOne(emptyRank3And4) & pawns;
  case Black:
    emptyRank5 = empty & Rank5Mask;
    emptyRank5And6 = empty & NortOne(emptyRank5);
    return NortOne(emptyRank5And6) & pawns;
  }

  panic("Invalid side %d.", side);
  return EmptyBoard;
}

// Get BitBoard encoding single push targets for specified pawns.
static BitBoard
singlePushTargets(ChessSet *chessSet, Side side, BitBoard pawns)
{
  // Take advantage of the fact that shifting off the end of the uint64 simply pushes the
  // bits into oblivion, so we need not worry about pawns on the last rank (assuming this
  // method is used before promotion concerns are applied).
  switch(side) {
  case White:
    return NortOne(pawns) & chessSet->EmptySquares;
  case Black:
    return SoutOne(pawns) & chessSet->EmptySquares;
  }

  panic("Invalid side %d.", side);
  return EmptyBoard;
}

// Get BitBoard encoding double push targets for specified pawns.
static BitBoard
doublePushTargets(ChessSet *chessSet, Side side, BitBoard pawns, BitBoard singlePushes)
{
  // We can only double-push pawns if they are on the 2nd rank and thus end up on the 4th
  // rank (5th for black).
  switch(side) {
  case White:
    return NortOne(singlePushes) & chessSet->EmptySquares & Rank4Mask;
  case Black:
    return SoutOne(singlePushes) & chessSet->EmptySquares & Rank5Mask;
  }

  panic("Invalid side %d.", side);
  return EmptyBoard;
}
