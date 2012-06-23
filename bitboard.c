#include <strings.h>
#include "weak.h"

// Count the number of bits in the specified BitBoard.
int
PopCount(BitBoard x)
{
  // 'SWAR' population count.

 const BitBoard
   k1 = C64(0x5555555555555555),
   k2 = C64(0x3333333333333333),
   k4 = C64(0x0f0f0f0f0f0f0f0f),
   kf = C64(0x0101010101010101);

  // Put count of each 2 bits into those 2 bits.
  x = x - ((x >> 1) & k1);
  // Put count of each 4 bits into those 4 bits.
  x = (x & k2) + ((x >> 2) & k2);
  // Pu count of each 8 bits into those 8 bits.
  x = (x + (x >> 4)) & k4;
  // Returns 8 most significant bits of x + (x<<8) + (x<<16) + (x<<24) + ...
  x = (x * kf) >> 56;

  return (int)x;
}

// northwest    north   northeast
//  NoWe         Nort         NoEa
//          +7    +8    +9
//              \  |  /
//  West    -1 <-  0 -> +1    East
//              /  |  \
//          -9    -8    -7
//  SoWe         Sout         SoEa
//  southwest    south   southeast

BitBoard
SoutOne(BitBoard bitBoard)
{
  return bitBoard >> 8;
}

BitBoard
NortOne(BitBoard bitBoard)
{
  return bitBoard << 8;
}

BitBoard
WestOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileAMask) >> 1;
}

BitBoard
EastOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileHMask) << 1;
}

BitBoard
NoEaOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileHMask) << 9;
}

BitBoard
SoEaOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileHMask) >> 7;
}

BitBoard
SoWeOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileAMask) >> 9;
}

BitBoard
NoWeOne(BitBoard bitBoard)
{
  return (bitBoard & NotFileAMask) << 7;
}

// Flip bitboard vertically (about the horizontal axis).
BitBoard
FlipVertical(BitBoard bitBoard)
{
  // Flipping about the horizontal rank [4].

  return (bitBoard << 56) |
    ((bitBoard << 40) & Rank7Mask) |
    ((bitBoard << 24) & Rank6Mask) |
    ((bitBoard << 8) & Rank5Mask) |
    ((bitBoard >> 8) & Rank4Mask) |
    ((bitBoard >> 24) & Rank3Mask) |
    ((bitBoard >> 40) & Rank2Mask) |
    (bitBoard >> 56);
}

// Flip bitboard diagonally about the A1-H8 diagonal.
BitBoard
FlipDiagA1H8(BitBoard bitBoard)
{
  // Flipping across A1-H8 [5].

  BitBoard temp;
  const BitBoard
    k1 = C64(0x5500550055005500),
    k2 = C64(0x3333000033330000),
    k4 = C64(0x0f0f0f0f00000000);

  temp = k4 & (bitBoard ^ (bitBoard << 28));
  bitBoard ^= temp ^ (temp >> 28);
  temp = k2 & (bitBoard ^ (bitBoard << 14));
  bitBoard ^= temp ^ (temp >> 14);
  temp = k1 & (bitBoard ^ (bitBoard << 7));
  bitBoard ^= temp ^ (temp >> 7);
  return bitBoard;
}

// Rotate bitboard 90 degrees anticlockwise.
BitBoard
Rotate90AntiClockwise(BitBoard bitBoard)
{
  // Flip vertically, then across A1-H8 diagonal [6].

  bitBoard = FlipVertical(bitBoard);
  return FlipDiagA1H8(bitBoard);
}

// Rotate bitboard 90 degrees clockwise.
BitBoard
Rotate90Clockwise(BitBoard bitBoard)
{
  // Rotate 90 degrees clockwise [7].

  // Flip across A1-H8 diagonal, flip vertical.
  bitBoard = FlipDiagA1H8(bitBoard);
  return FlipVertical(bitBoard);
}

Positions
BoardPositions(BitBoard bitBoard)
{
  int i, n;
  Position pos;
  Positions ret;

  if(bitBoard == EmptyBoard) {
    ret.Vals = NULL;
    ret.Length = 0;
    return ret;
  }

  n = PopCount(bitBoard);
  ret.Length = n;
  ret.Vals = (Position*)allocate(n*sizeof(Position));

  for(pos = A1; pos <= H8; pos++) {
    if((bitBoard&POSBOARD(pos)) != 0) {
        ret.Vals[i] = pos;
        i++;
    }
  }

  return ret;
}
