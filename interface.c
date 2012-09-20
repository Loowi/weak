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

#include <stdio.h>
#include <time.h>
#include "weak.h"

#define INIT_BUFFER_LENGTH 100

static bool isDrawnWon(Game*);

void
RunInterface(Game *game)
{
  bool capture;
  char *buffer = (char*)allocate(sizeof(char), INIT_BUFFER_LENGTH);
  clock_t ticks;
  Command command;
  double elapsed;
  Move moves[INIT_MOVE_LEN];
  Move *curr, *end;
  Move reply;
  PerftStats stats;
  Piece fromPiece;
  size_t length = INIT_BUFFER_LENGTH;
  uint64_t count;

  while(true) {
    if(getline(&buffer, &length, stdin) < 0) {
      continue;
    }

    command = ParseCommand(buffer);

    switch(command.Type) {
    case CmdBoard:
      puts(StringChessSet(&game->ChessSet));

      break;
    case CmdInvalid:
      puts("Invalid command.");

      break;
    case CmdAnalysis:
    case CmdMove:
      if(command.Type == CmdMove) {
        if(!Legal(game, command.Move)) {
          puts("Invalid move.");
          break;
        }

        DoMove(game, command.Move);

        if(isDrawnWon(game)) {
          return;
        }
      }

      count = 0;

      ticks = clock();
      reply = IterSearch(game, &count, MAX_THINK_SECS);
      ticks = clock() - ticks;
      elapsed = ((double)ticks)/CLOCKS_PER_SEC;
      printf("%fms elapsed, considered %llu moves, %0.3f Mnps\n", 1000*elapsed, count,
             (count/1e6)/elapsed);

      fromPiece = PieceAt(&game->ChessSet, FROM(reply));
      capture = PieceAt(&game->ChessSet, TO(reply)) != MissingPiece;

      if(command.Type == CmdMove) {
        DoMove(game, reply);
      }

      puts(StringMoveFull(reply, fromPiece, capture));

      break;
    case CmdMoves:
      end = AllMoves(moves, game);

      printf("%ld moves:-\n\n", end-moves);
      for(curr = moves; curr != end; curr++) {
        puts(StringMoveFull(*curr, PieceAt(&game->ChessSet, FROM(*curr)),
                        PieceAt(&game->ChessSet, TO(*curr)) != MissingPiece));
      }
      printf("\n");

      break;
    case CmdPerft:
      ticks = clock();
      count = QuickPerft(game, command.PerftDepth);
      ticks = clock() - ticks;
      elapsed = 1000*((double)ticks)/CLOCKS_PER_SEC;

      printf("Perft depth %d = %llu.\n", command.PerftDepth, count);
      printf("%fms elapsed, %f Mnps.\n", elapsed, 1E-3*count/elapsed);

      break;
    case CmdPerftFull:
      stats = Perft(game, command.PerftDepth);
      puts(StringPerft(&stats));

      break;
    case CmdPositionFen:
      *game = ParseFen(command.Fen);

      break;
    case CmdQuit:
      return;
    }
  }
}

static bool
isDrawnWon(Game *game)
{
  if(Stalemated(game)) {
    if(Checked(game)) {
      if(game->WhosTurn == Black) {
        puts("1-0");
      } else {
        puts("0-1");
      }
    } else {
      puts("0.5-0.5");
    }

    return true;
  }

  return false;
}
