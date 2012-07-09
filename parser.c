#include <ctype.h>
#include <string.h>
#include "weak.h"

// Parse a FEN string into a game object.
// See http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
Game
ParseFen(char *fen)
{
  bool seenKing[2] = { false, false };
  char chr;
  int i, len;
  // Make file/rank integers so we can make them negative to detect errors. Both are unsigned. TODO: fix.
  int file, rank;
  Game ret = NewEmptyGame(false, White);
  Piece piece;
  Position pos;
  Side side;

  if(fen == NULL) {
    panic("Null char pointer in ParseFen().");
  }

  len = strlen(fen);

  if(len < 25) {
    panic("Invalid FEN '%s' - too short. Expect at least 25 characters.", fen);
  }

  rank = Rank8;
  file = FileA;
  for(i = 0; i < len; i++) {
    chr = fen[i];

    if(chr >= '1' && chr <= '8') {
      file += (chr - '0');
      continue;
    } else if(chr == '/') {
      rank--;
      file = FileA;
      continue;
    } else if(rank == Rank1 && chr == ' ') {
      break;
    } else {
      side = isupper(chr) ? White : Black;
      switch(chr) {
      case 'P':
      case 'p':
        piece = Pawn;
        break;
      case 'R':
      case 'r':
        piece = Rook;
        break;
      case 'N':
      case 'n':
        piece = Knight;
        break;
      case 'B':
      case 'b':
        piece = Bishop;
        break;
      case 'Q':
      case 'q':
        piece = Queen;
        break;
      case 'K':
      case 'k':
        piece = King;
        seenKing[side] = true;
        break;
      default:
        panic("Unrecognised character '%c' at position %d.", chr, i);
      }
    }

    if(rank < 0) {
      panic("Too many ranks at position %d.", i);
    }
    if(file < 0) {
      panic("Too many pieces in the file at position %d.", i);
    }

    pos = POSITION(rank, file);
    ret.ChessSet.Sets[side].Boards[piece] |= POSBOARD(pos);

    file++;
  }
  if(i+2 >= len) {
    panic("FEN string does not contain enough characters to determine position.");
  }
  i++;

  // Who's turn.

  switch(fen[i]) {
  case 'w':
    ret.WhosTurn = White;
    break;
  case 'b':
    ret.WhosTurn = Black;
    break;
  default:
    panic("No turn indicator at position %d.", i);
  }

  i += 2;
  if(fen[i] != '-') {
    for(; i < len && i != ' '; i++) {
      chr = fen[i];

      switch(chr) {
      case 'K':
        ret.CastleKingSideWhite = true;
        break;
      case 'k':
        ret.CastleKingSideBlack = true;
        break;
      case 'Q':
        ret.CastleQueenSideWhite = true;
        break;
      case 'q':
        ret.CastleQueenSideBlack = true;
        break;
      case ' ':
        goto done;
      default:
        panic("Unrecognised character '%c' at position %d.", chr, i);
      }
    }
  }

  if(i == len) {
    panic("FEN '%s' ended without en passant square or half/full clock times.", fen);
  }

  // TODO: Implement parsing of en passant square + clock times.

 done:

  // Set up occupancy/empty square values.
  for(side = White; side <= Black; side++) {
    for(piece = Pawn; piece <= King; piece++) {
      ret.ChessSet.Occupancy |= ret.ChessSet.Sets[side].Boards[piece];
      ret.ChessSet.Sets[side].Occupancy |= ret.ChessSet.Sets[side].Boards[piece];
    }
    ret.ChessSet.Sets[side].EmptySquares = ~ret.ChessSet.Sets[side].Occupancy;
  }
  ret.ChessSet.EmptySquares = ~ret.ChessSet.Occupancy;

  return ret;
}
