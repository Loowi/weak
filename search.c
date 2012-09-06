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

#define SHOW_LINES

#if defined(SHOW_LINES)
#include <stdio.h>
#endif

#include <time.h>
#include "weak.h"

static double quiesce(Game*, double, double, uint64_t*);
static double negaMax(Game*, double, double, int, uint64_t*, int);

static Move lines[200][10];

Move
Search(Game *game, uint64_t *count, int depth)
{
  double max, val;
  int i;
#if defined(SHOW_LINES)
  int selectedLine;
#endif
  Move moves[INIT_MOVE_LEN];
  Move best;
  Move *start = moves;
  Move *curr, *end;

  end = AllMoves(moves, game);

  *count = end-start;

  // Iterate through all moves looking for the best, whose definition
  // varies based on who's turn it is.

  max = SMALL;
  best = INVALID_MOVE;

  for(i = 0, curr = start; curr != end; i++, curr++) {
    DoMove(game, *curr);

#if defined(SHOW_LINES)
    lines[i][depth-1] = *curr;
#endif

    val = -negaMax(game, SMALL, BIG, depth-1, count, i);

    if(val > max) {
      max = val;
      best = *curr;
#if defined(SHOW_LINES)
      selectedLine = i;
#endif
    }

    Unmove(game);
  }

  if(best == INVALID_MOVE) {
    panic("No move selected!");
  }

#if defined(SHOW_LINES)
  for(i = depth-1; i >= 0; i--) {
    printf("%s ", StringMove(lines[selectedLine][i]));
  }

  printf("\n");
#endif

  return best;
}

static double
negaMax(Game *game, double alpha, double beta, int depth, uint64_t *count, int lineInd)
{
  double val;
  Move moves[INIT_MOVE_LEN];
  Move *start = moves;
  Move *curr, *end;

  if(depth == 0) {
    return quiesce(game, alpha, beta, count);
  }

  end = AllMoves(moves, game);

  // If no moves available, all we can do is eval. Stalemate or checkmate.
  if(end == moves) {
    return Eval(game);
  }

  *count += end - start;

  for(curr = start; curr != end; curr++) {
    DoMove(game, *curr);

    val = -negaMax(game, -beta, -alpha, depth-1, count, lineInd);

    Unmove(game);

    if(val >= alpha) {
#if defined(SHOW_LINES)    
    lines[lineInd][depth-1] = *curr;    
#endif

      alpha = val;
    }    

    // Fail high.
    if(val >= beta) {
      return val;
    }
  }

  return alpha;
}

// Quiescent search. See http://chessprogramming.wikispaces.com/Quiescence+Search
static double
quiesce(Game *game, double alpha, double beta, uint64_t *count)
{
  double val;
  double standPat = Eval(game);
  Move buffer[INIT_MOVE_LEN];
  Move move;
  Move *end;
  Move *curr = buffer;

  // Fail high.
  if(standPat >= beta) {
    return beta;
  }

  if(alpha < standPat) {
    alpha = standPat;
  }

  end = AllCaptures(curr, game);

  *count += end-curr;

  for(; curr != end; curr++) {
    move = *curr;

    DoMove(game, move);
    val = -quiesce(game, -beta, -alpha, count);
    Unmove(game);

    // Fail high.
    if(val >= beta) {
      return beta;
    }

    if(val > alpha) {
      val = alpha;
    }
  }

  return alpha;
}
