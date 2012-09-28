/*
  Weak, a chess engine derived from Stockfish.

  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2012 Marco Costalba, Joona Kiiski, Tord Romstad (Stockfish authors)
  Copyright (C) 2011-2012 Lorenzo Stoakes

  Weak is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Weak is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "magic.h"
#include "weak.h"

static double weights[] = { 0, 100, 350, 350, 500, 900 };

static int getCentreControl(Piece, BitBoard, Side, BitBoard);

// Value of game position for white.
int
Eval(Game *game)
{
  ChessSet *chessSet = &game->ChessSet;
  BitBoard occupancy = chessSet->Occupancy;
  BitBoard pieceBoard;
  // Use a very rough heuristic to see whether we're in the endgame, if so we don't care
  // about control of the centre.
  int centreMult = PopCount(chessSet->Occupancy) <= 10 ? 0 : 1;

  int them, us;
  Piece piece;
  Side side = game->WhosTurn;
  Side opposite = OPPOSITE(side);

  // Very basic evaluation for now.

  us = them = 0;

  // Grade check mate as worth 15*queens for both sides.

  // Material and central control.
  for(piece = Pawn; piece <= Queen; piece++) {
    pieceBoard = chessSet->Sets[side].Boards[piece];
    us += weights[piece]*PopCount(pieceBoard);
    us += centreMult*getCentreControl(piece, pieceBoard, side, occupancy);

    pieceBoard = chessSet->Sets[opposite].Boards[piece];
    them += weights[piece]*PopCount(pieceBoard);
    them += centreMult*getCentreControl(piece, pieceBoard, opposite, occupancy);
  }

  if(Stalemated(game)) {
    // If checkmate, worth -15*queen, on both sides.
    if(Checked(game)) {
      us -= 13500;
      them += 13500;
    } else {
      // If draw, then that is worth -8*queen.
      us -= 7200;
      them += 7200;
    }
  }

  return us - them;
}

static int
getCentreControl(Piece piece, BitBoard pieceBoard, Side side, BitBoard occupancy)
{
  // Slow. We like control of the centre.

  int ret = 0;
  Position pos;

  while(pieceBoard) {
    pos = PopForward(&pieceBoard);

    switch(piece) {
    case Pawn:
      ret += PopCount(CentralSquaresMask&PawnAttacksFrom(pos, side));
      break;
    case Knight:
      ret += PopCount(CentralSquaresMask&KnightAttacksFrom(pos));
      break;
    case Bishop:
      ret += PopCount(CentralSquaresMask&BishopAttacksFrom(pos, occupancy));
      break;
    case Rook:
      ret += PopCount(CentralSquaresMask&RookAttacksFrom(pos, occupancy));
      break;
    case Queen:
      ret += PopCount(CentralSquaresMask&
                      (RookAttacksFrom(pos, occupancy)|BishopAttacksFrom(pos, occupancy)));
      break;
    case King:
      ret += PopCount(CentralSquaresMask&KingAttacksFrom(pos));
      break;
    default:
      panic("Impossible.");
    }
  }

  return ret;
}
