//
//  font.c
//      part of the H-Os project
//

#pragma once

void font_init() {

	u8 i = 0;
	u8 ii = 0;

	default_font.size = 8;

	//  null_t
	for (; ii < 8; ii++) {    //  just set all to 0
		default_font.table[i][ii] = 0;
	}
	//  space
	i = 1;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  !
	i = 2;
	for (ii = 0; ii <= 5; ii++) {    //  first 4 lines
		default_font.table[i][ii] = 0b00100000;
	}
	default_font.table[i][6] = 0;
	default_font.table[i][7] = 0b00100000;

	//  "
	i = 3;
	for (ii = 0; ii <= 2; ii++) {
		default_font.table[i][ii] = 0b01010000;
	}
	for (;ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  #
	i = 4;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0b00100100;
	}
	default_font.table[i][2] = MAX_U8;
	default_font.table[i][5] = MAX_U8;


	//  $
	i = 5;
	default_font.table[i][0] = 0b00111100;
	default_font.table[i][1] = 0b01011010;
	default_font.table[i][2] = 0b01011000;
	default_font.table[i][3] = 0b00111100;
	default_font.table[i][4] = 0b00011010;
	default_font.table[i][5] = 0b00011010;
	default_font.table[i][6] = 0b01011010;
	default_font.table[i][7] = 0b00111100;


	//  %
	i = 6;
	default_font.table[i][0] = 0b00000100;
	default_font.table[i][1] = 0b10001000;
	default_font.table[i][2] = 0b00001000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00100000;
	default_font.table[i][6] = 0b00100010;
	default_font.table[i][7] = 0b01000000;

	//  &
	i = 7;
	default_font.table[i][0] = 0b00011100;
	default_font.table[i][1] = 0b00110110;
	default_font.table[i][2] = 0b00011100;
	default_font.table[i][3] = 0b00111011;
	default_font.table[i][4] = 0b01110110;
	default_font.table[i][5] = 0b11011100;
	default_font.table[i][6] = 0b11001100;
	default_font.table[i][7] = 0b01110110;


	//  '
	i = 8;
	for (ii = 0; ii <= 2; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	for (; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  (
	i = 9;
	default_font.table[i][0] = 0b00010000;
	default_font.table[i][1] = 0b00100000;
	for (ii = 2; ii <= 5; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][6] = 0b00100000;
	default_font.table[i][7] = 0b00010000;

	//  )
	i = 10;
	default_font.table[i][0] = 0b00001000;
	default_font.table[i][1] = 0b00000100;
	for (ii = 2; ii <= 5; ii++) {
		default_font.table[i][ii] = 0b00000010;
	}
	default_font.table[i][6] = 0b00000100;
	default_font.table[i][7] = 0b00001000;

	//  *
	i = 11;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00101000;
	default_font.table[i][2] = 0b00010000;
	default_font.table[i][3] = 0b00101000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  +
	i = 12;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00010000;
	default_font.table[i][2] = 0b00010000;
	default_font.table[i][3] = 0b01111100;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0;
	default_font.table[i][7] = 0;


	//  ,
	i = 13;
	for (ii = 0; ii < 6; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b01000000;

	//  -
	i = 14;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][4] = 0xff;

	//  .
	i = 15;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][7] = 0b01000000;

	//  /
	i = 16;
	default_font.table[i][0] = 0b00000100;
	default_font.table[i][1] = 0b00001000;
	default_font.table[i][2] = 0b00001000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00100000;
	default_font.table[i][6] = 0b00100000;
	default_font.table[i][7] = 0b01000000;

	//  0
	i = 17;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01010100;
	default_font.table[i][4] = 0b01010100;
	default_font.table[i][5] = 0b01000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  1
	i = 18;
	default_font.table[i][0] = 0b00010000;
	default_font.table[i][1] = 0b00110000;
	default_font.table[i][2] = 0b01010000;
	for (ii = 3; ii < 7; ii++) {
		default_font.table[i][ii] = 0b00010000;
	}
	default_font.table[i][7] = 0b01111100;

	//  2
	i = 19;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b00000100;
	default_font.table[i][3] = 0b00000100;
	default_font.table[i][4] = 0b00001000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0b00100000;
	default_font.table[i][7] = 0b01111100;

	//  3
	i = 20;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b00000100;
	default_font.table[i][3] = 0b00011000;
	default_font.table[i][4] = 0b00000100;
	default_font.table[i][5] = 0b00000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  4
	i = 21;
	default_font.table[i][0] = 0b00000100;
	default_font.table[i][1] = 0b00001000;
	default_font.table[i][2] = 0b00010000;
	default_font.table[i][3] = 0b00100000;
	default_font.table[i][4] = 0b01111100;
	default_font.table[i][5] = 0b00000100;
	default_font.table[i][6] = 0b00000100;
	default_font.table[i][7] = 0b00000100;

	//  5
	i = 22;
	default_font.table[i][0] = 0b01111110;
	default_font.table[i][1] = 0b01000000;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b01111100;
	default_font.table[i][4] = 0b00000010;
	default_font.table[i][5] = 0b00000010;
	default_font.table[i][6] = 0b01000010;
	default_font.table[i][7] = 0b00111100;

	//  6
	i = 23;
	default_font.table[i][0] = 0b00011110;
	default_font.table[i][1] = 0b00100000;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b01111100;
	default_font.table[i][4] = 0b01000010;
	default_font.table[i][4] = 0b01000010;
	default_font.table[i][5] = 0b01000010;
	default_font.table[i][7] = 0b00111100;

	//  7
	i = 24;
	default_font.table[i][0] = 0b01111110;
	default_font.table[i][1] = 0b00000010;
	default_font.table[i][2] = 0b00000100;
	default_font.table[i][3] = 0b00001000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00100000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b01000000;

	//  8
	i = 25;
	default_font.table[i][0] = 0b00111100;
	default_font.table[i][1] = 0b01000010;
	default_font.table[i][2] = 0b01000010;
	default_font.table[i][3] = 0b00111100;
	for (ii = 4; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000010;
	}
	default_font.table[i][7] = 0b00111110;

	//  9
	i = 26;
	default_font.table[i][0] = 0b00111100;
	default_font.table[i][1] = 0b01000010;
	default_font.table[i][2] = 0b01000010;
	default_font.table[i][3] = 0b00111110;
	default_font.table[i][4] = 0b00000010;
	default_font.table[i][5] = 0b00000010;
	default_font.table[i][6] = 0b01000010;
	default_font.table[i][7] = 0b00111100;

	//  :
	i = 27;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][2] = 0b00100000;
	default_font.table[i][6] = 0b00100000;

	//  ;
	i = 28;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0;
	default_font.table[i][2] = 0b00100000;
	default_font.table[i][3] = 0;
	default_font.table[i][4] = 0b00100000;
	default_font.table[i][5] = 0b00100000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0;

	//  <
	i = 29;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0;
	default_font.table[i][2] = 0b00001000;
	default_font.table[i][3] = 0b00110000;
	default_font.table[i][4] = 0b01000000;
	default_font.table[i][5] = 0b00110000;
	default_font.table[i][6] = 0b00001000;
	default_font.table[i][7] = 0;

	//  =
	i = 30;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][2] = 0b01111100;
	default_font.table[i][4] = 0b01111100;

	//  >       //  dodelat
	i = 31;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0;
	default_font.table[i][2] = 0b00010000;
	default_font.table[i][3] = 0b00001100;
	default_font.table[i][4] = 0b00000010;
	default_font.table[i][5] = 0b00001100;
	default_font.table[i][6] = 0b00010000;
	default_font.table[i][7] = 0;

	//  ?
	i = 32;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b00000100;
	default_font.table[i][3] = 0b00001000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0;
	default_font.table[i][7] = 0b00010000;

	//  @
	i = 33;
	default_font.table[i][0] = 0b00111100;
	default_font.table[i][1] = 0b01000010;
	default_font.table[i][2] = 0b01001110;
	default_font.table[i][3] = 0b01010010;
	default_font.table[i][4] = 0b01010010;
	default_font.table[i][5] = 0b01001100;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b00111110;


	//  A
	i = 34;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01111100;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii]= 0b01000100;
	}

	//  B
	i = 35;
	default_font.table[i][0] = 0b01111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01001000;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b01111000;

	//  C
	i = 36;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	for (ii = 2; ii < 6; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  D
	i = 37;

	default_font.table[i][0] = 0b01111000;
	default_font.table[i][7] = 0b01111000;
	for (ii = 1; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  E
	i = 38;
	default_font.table[i][0] = 0b01111000;
	default_font.table[i][1] = 0b01000000;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b01110000;
	default_font.table[i][4] = 0b01000000;
	default_font.table[i][5] = 0b01000000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b01111000;

	//  F
	i = 39;
	default_font.table[i][0] = 0b01111000;
	default_font.table[i][1] = 0b01000000;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b01110000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}

	//  G
	i = 40;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b01011100;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  H
	i = 41;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000010;
	}
	default_font.table[i][3] = 0b01111110;

	//  I
	i = 42;
	for (ii = 1; ii < 7; ii++) {
		default_font.table[i][ii] = 0b00010000;
	}
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][7] = 0b00111000;

	//  J
	i = 43;
	default_font.table[i][0] = 0b00111100;
	for (ii = 1; ii < 6; ii++) {
		default_font.table[i][ii] = 0b00000100;
	}
	default_font.table[i][6] = 0b00100100;
	default_font.table[i][7] = 0b00011100;

	//  K
	i = 44;
	default_font.table[i][0] = 0b01000100;
	default_font.table[i][1] = 0b01001000;
	default_font.table[i][2] = 0b01010000;
	default_font.table[i][3] = 0b01100000;
	default_font.table[i][4] = 0b01010000;
	default_font.table[i][5] = 0b01001000;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b01000100;

	//  L
	i = 45;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][7] = 0b01111000;


	//  M
	i = 46;
	default_font.table[i][0] = 0b01000100;
	default_font.table[i][1] = 0b01101100;
	default_font.table[i][2] = 0b01010100;
	for (ii = 3; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  N
	i = 47;
	default_font.table[i][0] = 0b01000100;
	default_font.table[i][1] = 0b01100100;
	default_font.table[i][2] = 0b01010100;
	default_font.table[i][3] = 0b01001100;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  O
	i = 48;
	default_font.table[i][0] = 0b00111000;
	for (ii = 1; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][7] = 0b0011000;

	//  P
	i = 49;
	default_font.table[i][0] = 0b01111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}

	//  Q
	i = 50;
	default_font.table[i][0] = 0b00111000;
	for (ii = 1; ii < 4; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][4] = 0b01010100;
	default_font.table[i][5] = 0b01001100;
	default_font.table[i][6] = 0b00111100;
	default_font.table[i][7] = 0b00000110;

	//  R
	i = 51;
	default_font.table[i][0] = 0b01111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  S
	i = 52;
	default_font.table[i][0] = 0b00111000;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b01000000;
	default_font.table[i][3] = 0b00111000;
	default_font.table[i][4] = 0b00000100;
	default_font.table[i][5] = 0b00000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  T
	i = 53;
	default_font.table[i][0] = 0b01111100;
	for (ii = 1; ii < 8; ii++) {
		default_font.table[i][ii] = 0b00010000;
	}

	//  U
	i = 54;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][7] = 0b00111000;

	//  V
	i = 55;
	for (ii = 0; ii < 5; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][5] = 0b00101000;
	default_font.table[i][6] = 0b00101000;
	default_font.table[i][7] = 0b00010000;

	//  W
	i = 56;
	for (ii = 0; ii < 5; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][5] = 0b01010100;
	default_font.table[i][6] = 0b01101100;
	default_font.table[i][7] = 0b01000100;

	//  X
	i = 57;
	default_font.table[i][0] = 0b01000100;
	default_font.table[i][1] = 0b01000100;
	default_font.table[i][2] = 0b00101000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00101000;
	default_font.table[i][5] = 0b00101000;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b01000100;

	//  Y
	i = 58;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][3] = 0b00111100;
	default_font.table[i][4] = 0b00000100;
	default_font.table[i][5] = 0b00000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;


	//  Z
	i = 59;
	default_font.table[i][0] = 0b01111100;
	default_font.table[i][1] = 0b00001000;
	default_font.table[i][2] = 0b00001000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00100000;
	default_font.table[i][6] = 0b00100000;
	default_font.table[i][7] = 0b01111100;

	//  [
	i = 60;
	default_font.table[i][0] = 0b00111000;
	for (ii = 1; i < 7; ii++) {
		default_font.table[i][ii] = 0b00100000;
	}
	default_font.table[i][7] = 0b00111000;

	//  backslash
	i = 61;
	default_font.table[i][0] = 0b01000000;
	default_font.table[i][1] = 0b00100000;
	default_font.table[i][2] = 0b00100000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00001000;
	default_font.table[i][6] = 0b00001000;
	default_font.table[i][7] = 0b00000100;

	//  ]
	i = 62;
	default_font.table[i][0] = 0b00011100;
	for (ii = 1; ii < 7; ii++) {
		default_font.table[i][ii] = 0b00000100;
	}
	default_font.table[i][7] = 0b00011100;

	//  ^
	i = 63;
	default_font.table[i][0] = 0b00010000;
	default_font.table[i][1] = 0b00101000;
	for (ii = 2; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  _
	i = 64;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][7] = 0b01111100;

	//  `
	i = 65;
	default_font.table[i][0] = 0b00100000;
	default_font.table[i][1] = 0b00010000;
	for (ii = 2; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	//  a
	i = 66;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b00111000;
	for (ii = 4; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][7] = 0b00111100;

	//  b
	i = 67;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][3] = 0b01111000;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  c
	i = 68;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b00111000;
	default_font.table[i][4] = 0b01000000;
	default_font.table[i][5] = 0b01000000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b00111000;

	//  d
	i = 69;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0b00000010;
	}
	default_font.table[i][3] = 0b00011110;
	default_font.table[i][4] = 0b00100010;
	default_font.table[i][5] = 0b00100010;
	default_font.table[i][6] = 0b00100010;
	default_font.table[i][7] = 0b00011110;

	//  e
	i = 70;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b00111000;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01111000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b00111000;

	//  f
	i = 71;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0;
	default_font.table[i][2] = 0b00111000;
	default_font.table[i][3] = 0b01000000;
	default_font.table[i][4] = 0b01110000;
	default_font.table[i][5] = 0b01000000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b01000000;

	//  g
	i = 72;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0;
	default_font.table[i][2] = 0b00111100;
	default_font.table[i][3] = 0b01000100;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b00111100;
	default_font.table[i][6] = 0b00000100;
	default_font.table[i][7] = 0b00111000;

	//  h
	i = 73;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  i
	i = 74;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00100000;
	default_font.table[i][2] = 0;
	for (ii = 3; ii < 8; ii++) {
		default_font.table[i][ii] = 0b00100000;
	}

	//  j
	i = 75;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00000100;
	default_font.table[i][2] = 0;
	for (ii = 3; ii < 6; ii++) {
		default_font.table[i][ii] = 0b00000100;
	}
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  k
	i = 76;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][3] = 0b01001000;
	default_font.table[i][4] = 0b01110000;
	for (ii = 5; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01001000;
	}

	//  l
	i = 77;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0b00100000;
	}
	default_font.table[i][7] = 0b00010000;

	//  m
	i = 78;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01111000;
	default_font.table[i][4] = 0b01010100;
	default_font.table[i][5] = 0b01010100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b01000100;

	//  n
	i = 79;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01111000;
	for (ii = 4; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}

	//  output
	i = 80;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b00111000;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;


	//  p
	i = 81;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b01111000;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01000100;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01111000;
	default_font.table[i][6] = 0b01000000;
	default_font.table[i][7] = 0b01000000;

	//  q
	i = 82;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00111000;
	default_font.table[i][2] = 0b01000100;
	default_font.table[i][3] = 0b01000100;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b00111100;
	default_font.table[i][6] = 0b00000101;
	default_font.table[i][7] = 0b00000110;

	//  r
	i = 83;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01111000;
	default_font.table[i][4] = 0b01000100;
	for (ii = 5; ii < 8; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}

	//  s
	i = 84;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b00111100;
	default_font.table[i][4] = 0b01000000;
	default_font.table[i][5] = 0b00111000;
	default_font.table[i][6] = 0b00000100;
	default_font.table[i][7] = 0b01111000;

	//  t
	i = 85;
	for (ii = 0; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000000;
	}
	default_font.table[i][3] = 0b01111000;
	default_font.table[i][7] = 0b00111000;

	//  u
	i = 86;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	for (; ii < 7; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][7] = 0b00111100;

	//  v
	i = 87;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	for (; ii < 6; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][6] = 0b00101000;
	default_font.table[i][7] = 0b00010000;

	//  w
	i = 88;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01000100;
	default_font.table[i][4] = 0b01000100;
	default_font.table[i][5] = 0b01010100;
	default_font.table[i][6] = 0b01010100;
	default_font.table[i][7] = 0b00111000;

	//  x
	i = 89;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01000100;
	default_font.table[i][4] = 0b00101000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0b00101000;
	default_font.table[i][7] = 0b01000100;

	//  y
	i = 90;
	default_font.table[i][0] = 0;
	for (ii = 1; ii < 4; ii++) {
		default_font.table[i][ii] = 0b01000100;
	}
	default_font.table[i][4] = 0b00111100;
	default_font.table[i][5] = 0b00000100;
	default_font.table[i][6] = 0b01000100;
	default_font.table[i][7] = 0b00111000;

	//  z
	i = 91;
	for (ii = 0; ii < 3; ii++) {
		default_font.table[i][ii] = 0;
	}
	default_font.table[i][3] = 0b01111100;
	default_font.table[i][4] = 0b00001000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0b00100000;
	default_font.table[i][7] = 0b01111100;

	//  {
	i = 92;
	default_font.table[i][0] = 0b00000100;
	default_font.table[i][1] = 0b00001000;
	default_font.table[i][2] = 0b00001000;
	default_font.table[i][3] = 0b00010000;
	default_font.table[i][4] = 0b00001000;
	default_font.table[i][5] = 0b00001000;
	default_font.table[i][6] = 0b00001000;
	default_font.table[i][7] = 0b00000100;

	//  |
	i = 93;
	for (ii = 0; ii < 8; ii++) {
		default_font.table[i][ii] = 0b00010000;
	}

	//  }
	i = 94;
	default_font.table[i][0] = 0b00100000;
	default_font.table[i][1] = 0b00010000;
	default_font.table[i][2] = 0b00010000;
	default_font.table[i][3] = 0b00001000;
	default_font.table[i][4] = 0b00010000;
	default_font.table[i][5] = 0b00010000;
	default_font.table[i][6] = 0b00010000;
	default_font.table[i][7] = 0b00100000;

	//  ~
	i = 95;
	default_font.table[i][0] = 0;
	default_font.table[i][1] = 0b00110100;
	default_font.table[i][2] = 0b01011000;
	for (ii = 3; ii < 8; ii++) {
		default_font.table[i][ii] = 0;
	}

	font.size = default_font.size;

	for (i = 0; i < 97; i++) {
		font.table[i] = &default_font.table[i];
	}

}