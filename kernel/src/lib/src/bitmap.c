//
//  bitmap.c
//      part of the H-Os project
//

#pragma once

#ifndef H_OS_LIB_BITMAP_CPP
	#define H_OS_LIB_BITMAP_CPP

void font_init() {

	u8 i = 0;
	u8 ii = 0;

	font.h = 8;
	font.w = 8;

	//  null_t
	for (; ii < 8; ii++) {    //  just set all to 0
		font.table[i][ii] = 0;
	}
	//  space
	i = 1;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  !
	i = 2;
	for (ii = 0; ii <= 5; ii++) {    //  first 4 lines
		font.table[i][ii] = 0b00100000;
	}
	font.table[i][6] = 0;
	font.table[i][7] = 0b00100000;

	//  "
	i = 3;
	for (ii = 0; ii <= 2; ii++) {
		font.table[i][ii] = 0b01010000;
	}
	for (;ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  #
	i = 4;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0b00100100;
	}
	font.table[i][2] = MAX_U8;
	font.table[i][5] = MAX_U8;


	//  $
	i = 5;
	font.table[i][0] = 0b00111100;
	font.table[i][1] = 0b01011010;
	font.table[i][2] = 0b01011000;
	font.table[i][3] = 0b00111100;
	font.table[i][4] = 0b00011010;
	font.table[i][5] = 0b00011010;
	font.table[i][6] = 0b01011010;
	font.table[i][7] = 0b00111100;


	//  %
	i = 6;
	font.table[i][0] = 0b00000100;
	font.table[i][1] = 0b10001000;
	font.table[i][2] = 0b00001000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00100000;
	font.table[i][6] = 0b00100010;
	font.table[i][7] = 0b01000000;

	//  &
	i = 7;
	font.table[i][0] = 0b00011100;
	font.table[i][1] = 0b00110110;
	font.table[i][2] = 0b00011100;
	font.table[i][3] = 0b00111011;
	font.table[i][4] = 0b01110110;
	font.table[i][5] = 0b11011100;
	font.table[i][6] = 0b11001100;
	font.table[i][7] = 0b01110110;


	//  '
	i = 8;
	for (ii = 0; ii <= 2; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	for (; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  (
	i = 9;
	font.table[i][0] = 0b00010000;
	font.table[i][1] = 0b00100000;
	for (ii = 2; ii <= 5; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][6] = 0b00100000;
	font.table[i][7] = 0b00010000;

	//  )
	i = 10;
	font.table[i][0] = 0b00001000;
	font.table[i][1] = 0b00000100;
	for (ii = 2; ii <= 5; ii++) {
		font.table[i][ii] = 0b00000010;
	}
	font.table[i][6] = 0b00000100;
	font.table[i][7] = 0b00001000;

	//  *
	i = 11;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00101000;
	font.table[i][2] = 0b00010000;
	font.table[i][3] = 0b00101000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  +
	i = 12;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00010000;
	font.table[i][2] = 0b00010000;
	font.table[i][3] = 0b01111100;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0;
	font.table[i][7] = 0;


	//  ,
	i = 13;
	for (ii = 0; ii < 6; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b01000000;

	//  -
	i = 14;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][4] = 0xff;

	//  .
	i = 15;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][7] = 0b01000000;

	//  /
	i = 16;
	font.table[i][0] = 0b00000100;
	font.table[i][1] = 0b00001000;
	font.table[i][2] = 0b00001000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00100000;
	font.table[i][6] = 0b00100000;
	font.table[i][7] = 0b01000000;

	//  0
	i = 17;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01010100;
	font.table[i][4] = 0b01010100;
	font.table[i][5] = 0b01000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  1
	i = 18;
	font.table[i][0] = 0b00010000;
	font.table[i][1] = 0b00110000;
	font.table[i][2] = 0b01010000;
	for (ii = 3; ii < 7; ii++) {
		font.table[i][ii] = 0b00010000;
	}
	font.table[i][7] = 0b01111100;

	//  2
	i = 19;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b00000100;
	font.table[i][3] = 0b00000100;
	font.table[i][4] = 0b00001000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0b00100000;
	font.table[i][7] = 0b01111100;

	//  3
	i = 20;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b00000100;
	font.table[i][3] = 0b00011000;
	font.table[i][4] = 0b00000100;
	font.table[i][5] = 0b00000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  4
	i = 21;
	font.table[i][0] = 0b00000100;
	font.table[i][1] = 0b00001000;
	font.table[i][2] = 0b00010000;
	font.table[i][3] = 0b00100000;
	font.table[i][4] = 0b01111100;
	font.table[i][5] = 0b00000100;
	font.table[i][6] = 0b00000100;
	font.table[i][7] = 0b00000100;

	//  5
	i = 22;
	font.table[i][0] = 0b01111110;
	font.table[i][1] = 0b01000000;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b01111100;
	font.table[i][4] = 0b00000010;
	font.table[i][5] = 0b00000010;
	font.table[i][6] = 0b01000010;
	font.table[i][7] = 0b00111100;

	//  6
	i = 23;
	font.table[i][0] = 0b00011110;
	font.table[i][1] = 0b00100000;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b01111100;
	font.table[i][4] = 0b01000010;
	font.table[i][4] = 0b01000010;
	font.table[i][5] = 0b01000010;
	font.table[i][7] = 0b00111100;

	//  7
	i = 24;
	font.table[i][0] = 0b01111110;
	font.table[i][1] = 0b00000010;
	font.table[i][2] = 0b00000100;
	font.table[i][3] = 0b00001000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00100000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b01000000;

	//  8
	i = 25;
	font.table[i][0] = 0b00111100;
	font.table[i][1] = 0b01000010;
	font.table[i][2] = 0b01000010;
	font.table[i][3] = 0b00111100;
	for (ii = 4; ii < 7; ii++) {
		font.table[i][ii] = 0b01000010;
	}
	font.table[i][7] = 0b00111110;

	//  9
	i = 26;
	font.table[i][0] = 0b00111100;
	font.table[i][1] = 0b01000010;
	font.table[i][2] = 0b01000010;
	font.table[i][3] = 0b00111110;
	font.table[i][4] = 0b00000010;
	font.table[i][5] = 0b00000010;
	font.table[i][6] = 0b01000010;
	font.table[i][7] = 0b00111100;

	//  :
	i = 27;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][2] = 0b00100000;
	font.table[i][6] = 0b00100000;

	//  ;
	i = 28;
	font.table[i][0] = 0;
	font.table[i][1] = 0;
	font.table[i][2] = 0b00100000;
	font.table[i][3] = 0;
	font.table[i][4] = 0b00100000;
	font.table[i][5] = 0b00100000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0;

	//  <
	i = 29;
	font.table[i][0] = 0;
	font.table[i][1] = 0;
	font.table[i][2] = 0b00001000;
	font.table[i][3] = 0b00110000;
	font.table[i][4] = 0b01000000;
	font.table[i][5] = 0b00110000;
	font.table[i][6] = 0b00001000;
	font.table[i][7] = 0;

	//  =
	i = 30;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][2] = 0b01111100;
	font.table[i][4] = 0b01111100;

	//  >       //  dodelat
	i = 31;
	font.table[i][0] = 0;
	font.table[i][1] = 0;
	font.table[i][2] = 0b00010000;
	font.table[i][3] = 0b00001100;
	font.table[i][4] = 0b00000010;
	font.table[i][5] = 0b00001100;
	font.table[i][6] = 0b00010000;
	font.table[i][7] = 0;

	//  ?
	i = 32;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b00000100;
	font.table[i][3] = 0b00001000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0;
	font.table[i][7] = 0b00010000;

	//  @
	i = 33;
	font.table[i][0] = 0b00111100;
	font.table[i][1] = 0b01000010;
	font.table[i][2] = 0b01001110;
	font.table[i][3] = 0b01010010;
	font.table[i][4] = 0b01010010;
	font.table[i][5] = 0b01001100;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b00111110;


	//  A
	i = 34;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01111100;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii]= 0b01000100;
	}

	//  B
	i = 35;
	font.table[i][0] = 0b01111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01001000;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b01111000;

	//  C
	i = 36;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	for (ii = 2; ii < 6; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  D
	i = 37;

	font.table[i][0] = 0b01111000;
	font.table[i][7] = 0b01111000;
	for (ii = 1; ii < 7; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  E
	i = 38;
	font.table[i][0] = 0b01111000;
	font.table[i][1] = 0b01000000;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b01110000;
	font.table[i][4] = 0b01000000;
	font.table[i][5] = 0b01000000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b01111000;

	//  F
	i = 39;
	font.table[i][0] = 0b01111000;
	font.table[i][1] = 0b01000000;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b01110000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000000;
	}

	//  G
	i = 40;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b01011100;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  H
	i = 41;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0b01000010;
	}
	font.table[i][3] = 0b01111110;

	//  I
	i = 42;
	for (ii = 1; ii < 7; ii++) {
		font.table[i][ii] = 0b00010000;
	}
	font.table[i][0] = 0b00111000;
	font.table[i][7] = 0b00111000;

	//  J
	i = 43;
	font.table[i][0] = 0b00111100;
	for (ii = 1; ii < 6; ii++) {
		font.table[i][ii] = 0b00000100;
	}
	font.table[i][6] = 0b00100100;
	font.table[i][7] = 0b00011100;

	//  K
	i = 44;
	font.table[i][0] = 0b01000100;
	font.table[i][1] = 0b01001000;
	font.table[i][2] = 0b01010000;
	font.table[i][3] = 0b01100000;
	font.table[i][4] = 0b01010000;
	font.table[i][5] = 0b01001000;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b01000100;

	//  L
	i = 45;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][7] = 0b01111000;


	//  M
	i = 46;
	font.table[i][0] = 0b01000100;
	font.table[i][1] = 0b01101100;
	font.table[i][2] = 0b01010100;
	for (ii = 3; ii < 8; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  N
	i = 47;
	font.table[i][0] = 0b01000100;
	font.table[i][1] = 0b01100100;
	font.table[i][2] = 0b01010100;
	font.table[i][3] = 0b01001100;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  O
	i = 48;
	font.table[i][0] = 0b00111000;
	for (ii = 1; ii < 7; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][7] = 0b0011000;

	//  P
	i = 49;
	font.table[i][0] = 0b01111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000000;
	}

	//  Q
	i = 50;
	font.table[i][0] = 0b00111000;
	for (ii = 1; ii < 4; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][4] = 0b01010100;
	font.table[i][5] = 0b01001100;
	font.table[i][6] = 0b00111100;
	font.table[i][7] = 0b00000110;

	//  R
	i = 51;
	font.table[i][0] = 0b01111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  S
	i = 52;
	font.table[i][0] = 0b00111000;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b01000000;
	font.table[i][3] = 0b00111000;
	font.table[i][4] = 0b00000100;
	font.table[i][5] = 0b00000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  T
	i = 53;
	font.table[i][0] = 0b01111100;
	for (ii = 1; ii < 8; ii++) {
		font.table[i][ii] = 0b00010000;
	}

	//  U
	i = 54;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][7] = 0b00111000;

	//  V
	i = 55;
	for (ii = 0; ii < 5; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][5] = 0b00101000;
	font.table[i][6] = 0b00101000;
	font.table[i][7] = 0b00010000;

	//  W
	i = 56;
	for (ii = 0; ii < 5; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][5] = 0b01010100;
	font.table[i][6] = 0b01101100;
	font.table[i][7] = 0b01000100;

	//  X
	i = 57;
	font.table[i][0] = 0b01000100;
	font.table[i][1] = 0b01000100;
	font.table[i][2] = 0b00101000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00101000;
	font.table[i][5] = 0b00101000;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b01000100;

	//  Y
	i = 58;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][3] = 0b00111100;
	font.table[i][4] = 0b00000100;
	font.table[i][5] = 0b00000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;


	//  Z
	i = 59;
	font.table[i][0] = 0b01111100;
	font.table[i][1] = 0b00001000;
	font.table[i][2] = 0b00001000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00100000;
	font.table[i][6] = 0b00100000;
	font.table[i][7] = 0b01111100;

	//  [
	i = 60;
	font.table[i][0] = 0b00111000;
	for (ii = 1; i < 7; ii++) {
		font.table[i][ii] = 0b00100000;
	}
	font.table[i][7] = 0b00111000;

	//  backslash
	i = 61;
	font.table[i][0] = 0b01000000;
	font.table[i][1] = 0b00100000;
	font.table[i][2] = 0b00100000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00001000;
	font.table[i][6] = 0b00001000;
	font.table[i][7] = 0b00000100;

	//  ]
	i = 62;
	font.table[i][0] = 0b00011100;
	for (ii = 1; ii < 7; ii++) {
		font.table[i][ii] = 0b00000100;
	}
	font.table[i][7] = 0b00011100;

	//  ^
	i = 63;
	font.table[i][0] = 0b00010000;
	font.table[i][1] = 0b00101000;
	for (ii = 2; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  _
	i = 64;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][7] = 0b01111100;

	//  `
	i = 65;
	font.table[i][0] = 0b00100000;
	font.table[i][1] = 0b00010000;
	for (ii = 2; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}

	//  a
	i = 66;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b00111000;
	for (ii = 4; ii < 7; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][7] = 0b00111100;

	//  b
	i = 67;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][3] = 0b01111000;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  c
	i = 68;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b00111000;
	font.table[i][4] = 0b01000000;
	font.table[i][5] = 0b01000000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b00111000;

	//  d
	i = 69;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0b00000010;
	}
	font.table[i][3] = 0b00011110;
	font.table[i][4] = 0b00100010;
	font.table[i][5] = 0b00100010;
	font.table[i][6] = 0b00100010;
	font.table[i][7] = 0b00011110;

	//  e
	i = 70;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b00111000;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01111000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b00111000;

	//  f
	i = 71;
	font.table[i][0] = 0;
	font.table[i][1] = 0;
	font.table[i][2] = 0b00111000;
	font.table[i][3] = 0b01000000;
	font.table[i][4] = 0b01110000;
	font.table[i][5] = 0b01000000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b01000000;

	//  g
	i = 72;
	font.table[i][0] = 0;
	font.table[i][1] = 0;
	font.table[i][2] = 0b00111100;
	font.table[i][3] = 0b01000100;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b00111100;
	font.table[i][6] = 0b00000100;
	font.table[i][7] = 0b00111000;

	//  h
	i = 73;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  i
	i = 74;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00100000;
	font.table[i][2] = 0;
	for (ii = 3; ii < 8; ii++) {
		font.table[i][ii] = 0b00100000;
	}

	//  j
	i = 75;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00000100;
	font.table[i][2] = 0;
	for (ii = 3; ii < 6; ii++) {
		font.table[i][ii] = 0b00000100;
	}
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  k
	i = 76;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][3] = 0b01001000;
	font.table[i][4] = 0b01110000;
	for (ii = 5; ii < 8; ii++) {
		font.table[i][ii] = 0b01001000;
	}

	//  l
	i = 77;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0b00100000;
	}
	font.table[i][7] = 0b00010000;

	//  m
	i = 78;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01111000;
	font.table[i][4] = 0b01010100;
	font.table[i][5] = 0b01010100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b01000100;

	//  n
	i = 79;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		font.table[i][ii] = 0b01000100;
	}

	//  output
	i = 80;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b00111000;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;


	//  p
	i = 81;
	font.table[i][0] = 0;
	font.table[i][1] = 0b01111000;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01000100;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01111000;
	font.table[i][6] = 0b01000000;
	font.table[i][7] = 0b01000000;

	//  q
	i = 82;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00111000;
	font.table[i][2] = 0b01000100;
	font.table[i][3] = 0b01000100;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b00111100;
	font.table[i][6] = 0b00000101;
	font.table[i][7] = 0b00000110;

	//  r
	i = 83;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01111000;
	font.table[i][4] = 0b01000100;
	for (ii = 5; ii < 8; ii++) {
		font.table[i][ii] = 0b01000000;
	}

	//  s
	i = 84;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b00111100;
	font.table[i][4] = 0b01000000;
	font.table[i][5] = 0b00111000;
	font.table[i][6] = 0b00000100;
	font.table[i][7] = 0b01111000;

	//  t
	i = 85;
	for (ii = 0; ii < 7; ii++) {
		font.table[i][ii] = 0b01000000;
	}
	font.table[i][3] = 0b01111000;
	font.table[i][7] = 0b00111000;

	//  u
	i = 86;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	for (; ii < 7; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][7] = 0b00111100;

	//  v
	i = 87;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	for (; ii < 6; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][6] = 0b00101000;
	font.table[i][7] = 0b00010000;

	//  w
	i = 88;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01000100;
	font.table[i][4] = 0b01000100;
	font.table[i][5] = 0b01010100;
	font.table[i][6] = 0b01010100;
	font.table[i][7] = 0b00111000;

	//  x
	i = 89;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01000100;
	font.table[i][4] = 0b00101000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0b00101000;
	font.table[i][7] = 0b01000100;

	//  y
	i = 90;
	font.table[i][0] = 0;
	for (ii = 1; ii < 4; ii++) {
		font.table[i][ii] = 0b01000100;
	}
	font.table[i][4] = 0b00111100;
	font.table[i][5] = 0b00000100;
	font.table[i][6] = 0b01000100;
	font.table[i][7] = 0b00111000;

	//  z
	i = 91;
	for (ii = 0; ii < 3; ii++) {
		font.table[i][ii] = 0;
	}
	font.table[i][3] = 0b01111100;
	font.table[i][4] = 0b00001000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0b00100000;
	font.table[i][7] = 0b01111100;

	//  {
	i = 92;
	font.table[i][0] = 0b00000100;
	font.table[i][1] = 0b00001000;
	font.table[i][2] = 0b00001000;
	font.table[i][3] = 0b00010000;
	font.table[i][4] = 0b00001000;
	font.table[i][5] = 0b00001000;
	font.table[i][6] = 0b00001000;
	font.table[i][7] = 0b00000100;

	//  |
	i = 93;
	for (ii = 0; ii < 8; ii++) {
		font.table[i][ii] = 0b00010000;
	}

	//  }
	i = 94;
	font.table[i][0] = 0b00100000;
	font.table[i][1] = 0b00010000;
	font.table[i][2] = 0b00010000;
	font.table[i][3] = 0b00001000;
	font.table[i][4] = 0b00010000;
	font.table[i][5] = 0b00010000;
	font.table[i][6] = 0b00010000;
	font.table[i][7] = 0b00100000;

	//  ~
	i = 95;
	font.table[i][0] = 0;
	font.table[i][1] = 0b00110100;
	font.table[i][2] = 0b01011000;
	for (ii = 3; ii < 8; ii++) {
		font.table[i][ii] = 0;
	}
}

#endif
//	#warning bitmap.cpp already included
//#endif