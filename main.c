#include <stdio.h>
#include <sys/time.h>
#include "weak.h"

//#define GET_FULL_PERFT

// Initialise lookup tables, etc.
static void
init()
{
  GetMoveTargets[Pawn] = &PawnPushTargets;
  GetMoveTargets[Knight] = &KnightMoveTargets;
  GetMoveTargets[Bishop] = &BishopMoveTargets;
  GetMoveTargets[Rook] = &RookMoveTargets;
  GetMoveTargets[Queen] = &QueenMoveTargets;
  GetMoveTargets[King] = &KingMoveTargets;

  GetCaptureTargets[Pawn] = &PawnCaptureTargets;
  GetCaptureTargets[Knight] = &KnightCaptureTargets;
  GetCaptureTargets[Bishop] = &BishopCaptureTargets;
  GetCaptureTargets[Rook] = &RookCaptureTargets;
  GetCaptureTargets[Queen] = &QueenCaptureTargets;
  GetCaptureTargets[King] = &KingCaptureTargets;

  InitKnight();
  InitRays();

  // Relies on above.
  InitMagics();
}

int
main(int argc, char **argv)
{
  double elapsed;
  Game game;
  int plies;
#if defined(GET_FULL_PERFT)
  PerftStats stats;
#else
  uint64_t count;
#endif
  struct timeval start, end;

  // Use unbuffered output.
  setbuf(stdout, NULL);

  if(argc <= 1) {
    printf("Usage: %s [perft plies]\n", argv[0]);
    return 1;
  }

  plies = atoi(argv[1]);

  if(plies < 1) {
    printf("Invalid integer %s.\n", argv[1]);
  }

  puts("WeakC v0.0.dev.\n");

  printf("Initialising... ");
  init();
  puts("done.\n");

  printf("%d plies.\n", plies);

  game = NewGame(false, White);

  gettimeofday(&start, NULL);
#if defined(GET_FULL_PERFT)
  stats = Perft(&game, plies);
#else
  count = QuickPerft(&game, plies);
#endif
  gettimeofday(&end, NULL);

  elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
  elapsed += (end.tv_usec - start.tv_usec) / 1000.0;

#if defined(GET_FULL_PERFT)
  puts(StringPerft(&stats));
#else
  printf("%llu moves.\n", count);
  printf("%llu nps.\n\n", (uint64_t)(1000*count/elapsed));
#endif

  printf("%f ms elapsed.\n", elapsed);

  return 0;
}
