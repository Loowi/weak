#include <stdio.h>
#include "weak.h"

//#define SHOW_MOVES

static PerftStats initStats(void);

uint64_t
QuickPerft(Game *game, int depth)
{
  int i, len;
  uint64_t ret = 0;
  MoveSlice allMoves;
  Move move;
  Move buffer[INIT_MOVE_LEN];

  allMoves = NewMoveSlice(buffer);
  AllMoves(&allMoves, game);

  len = LenMoves(&allMoves);

  if(depth <= 1) {
#if defined(SHOW_MOVES)
    for(i = 0; i < len; i++) {
      move = allMoves.Vals[i];

      puts(StringMove(&move));
    }
#endif

    return len;
  }

  for(i = 0; i < len; i++) {
    move = allMoves.Vals[i];

    DoMove(game, &move);
    ret += QuickPerft(game, depth - 1);
    Unmove(game);
  }

  return ret;
}

PerftStats
Perft(Game *game, int depth)
{
  int i, len;
  Move move;
  MoveSlice allMoves;
  Move buffer[INIT_MOVE_LEN];
  PerftStats ret, stats;

  if(depth <= 0) {
    panic("Invalid depth %d.", depth);
  }

  ret = initStats();

  allMoves = NewMoveSlice(buffer);
  AllMoves(&allMoves, game);

  len = LenMoves(&allMoves);
  for(i = 0; i < len; i++) {
    move = allMoves.Vals[i];

    if(depth == 1) {
#if defined(SHOW_MOVES)
      puts(StringMove(&move));
#endif
      ret.Count++;
      if(move.Capture) {
        ret.Captures++;
      }
      switch(move.Type) {
      case CastleQueenSide:
      case CastleKingSide:
        ret.Castles++;
        break;
      case EnPassant:
        ret.EnPassants++;
        break;
      case PromoteKnight:
      case PromoteBishop:
      case PromoteRook:
      case PromoteQueen:
        ret.Promotions++;
        break;
      case Normal:
        break;
      default:
        panic("Invalid move type %d.", move.Type);
      }

      DoMove(game, &move);
      if(game->CheckStats.CheckSources != EmptyBoard) {
        ret.Checks++;

        if(Checkmated(game)) {
          ret.Checkmates++;
        }
      }
      Unmove(game);
    } else {
      DoMove(game, &move);
      stats = Perft(game, depth - 1);
      Unmove(game);
      ret.Count += stats.Count;
      ret.Captures += stats.Captures;
      ret.EnPassants += stats.EnPassants;
      ret.Castles += stats.Castles;
      ret.Promotions += stats.Promotions;
      ret.Checks += stats.Checks;
      ret.Checkmates += stats.Checkmates;
    }
  }

  return ret;
}

static PerftStats
initStats()
{
  PerftStats ret;

  ret.Count = 0;
  ret.Captures = 0;
  ret.EnPassants = 0;
  ret.Castles = 0;
  ret.Promotions = 0;
  ret.Checks = 0;
  ret.Checkmates = 0;

  return ret;
}
