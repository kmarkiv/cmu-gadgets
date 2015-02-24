/*
  Star Wars Arduino

  Describe what it does in layman's terms.  Refer to the components
  attached to the various pins.

  The circuit:
  * Led Matrix
  * JoyStick

  Created day month year
  By Vikram Kamath
  Modified day month year
  By author's name

  http://url/of/online/tutorial.cc

*/
#include <avr/pgmspace.h>
#include <FrequencyTimer2.h>
#include "pitches.h"
// Contains an 8x8 font map for unicode points U+0000 - U+007F (basic latin)
const char PROGMEM font8x8_basic[128][8] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0000 (nul)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0001
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0002
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0003
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0004
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0005
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0006
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0007
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0008
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0009
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000A
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000B
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000C
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000D
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000E
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000F
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0010
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0011
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0012
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0013
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0014
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0015
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0016
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0017
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0018
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0019
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001A
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001B
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001C
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001D
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001E
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001F
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
  { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
  { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
  { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
  { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
  { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
  { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
  { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
  { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
  { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
  { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
  { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
  { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
  { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
  { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
  { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
  { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
  { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
  { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
  { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
  { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
  { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
  { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
  { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
  { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
  { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (//)
  { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
  { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
  { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
  { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
  { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
  { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
  { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
  { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
  { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
  { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
  { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
  { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
  { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
  { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
  { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
  { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
  { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
  { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
  { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
  { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
  { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
  { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
  { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
  { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
  { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
  { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
  { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
  { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
  { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
  { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
  { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
  { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
  { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
  { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
  { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
  { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
  { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
  { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
  { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
  { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
  { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
  { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
  { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
  { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
  { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
  { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
  { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
  { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
  { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
  { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
  { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
  { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
  { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
  { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
  { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
  { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
  { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
  { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
  { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
  { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
  { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
  { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
  { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // U+007F
};

PROGMEM int melody1[] = {
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_DS5, NOTE_AS4, NOTE_FS4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_G5, NOTE_G4, NOTE_G4, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_E5, 0, NOTE_GS4, NOTE_CS5, NOTE_C5, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_AS4, 0, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_FS4, NOTE_AS4, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_G5, NOTE_G4, NOTE_G4, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_E5, 0, NOTE_GS4, NOTE_CS5, NOTE_C5, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_AS4, 0, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_AS4, NOTE_G4, NOTE_DS4, NOTE_AS4, NOTE_G4
};
// note durations for Darth Vader's Theme Song: 4 = quarter note, 8 = eighth note, etc.:
PROGMEM int noteDurations1[] = {
  4, 4, 4, 6, 16, 4, 6, 16, 2, 4, 4, 4, 6, 16, 4, 6, 16, 2, 4, 6, 16, 4, 6, 16, 16, 16, 8, 8, 8, 4, 6, 16, 16, 16, 8, 8, 8, 4, 6, 16, 4, 6, 16, 2, 4, 6, 16, 4, 6, 16, 16, 16, 8, 8, 8, 4, 6, 16, 16, 16, 8, 8, 8, 4, 6, 16, 4, 6, 16, 2,
};
// Notes in the melody Cantina Band
int PROGMEM melody2[] = {
  NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, NOTE_D5, 0, NOTE_A4, 0, 0, NOTE_GS4, NOTE_A4, 0, NOTE_A4, NOTE_GS4, NOTE_A4, NOTE_G4, 0, NOTE_FS4, NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_D4, NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, NOTE_D5, 0, NOTE_A4, 0, 0, NOTE_GS4, NOTE_A4, 0, NOTE_G4, 0, NOTE_G4, NOTE_FS4, NOTE_G4, 0, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, 0, NOTE_D5, 0, NOTE_A4, NOTE_D5, 0, NOTE_A4, 0, 0, NOTE_GS4, NOTE_A4, 0, NOTE_C5, 0, 0, NOTE_C5, NOTE_A4, NOTE_G4, 0, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_DS5, NOTE_D5, NOTE_GS4, NOTE_A4, 0, NOTE_F4, 0
};
// note duration for Cantina Band
int PROGMEM noteDurations2[] = {
  4, 64, 4, 64, 4, 64, 4, 64, 8, 4, 64, 8, 64, 8, 8, 4, 64, 8, 8, 8, 8, 8, 8, 8, 8, 2.666, 1.99, 4, 64, 4, 64, 4, 64, 4, 64, 8, 4, 64, 8, 64, 8, 8, 4, 64, 8, 8, 2.666, 8, 4, 64, 8, 4, 4, 2.666, 4, 64, 4, 64, 4, 64, 4, 64, 8, 4, 64, 8, 64, 8, 8, 4, 64, 8, 64, 8, 2.666, 8, 4, 64, 2.666, 1.99, 2, 2, 2, 2, 4, 4, 8, 4, 64, 4, 2
};
// notes in melody May the Force be with You
int PROGMEM melody5[] = {
  NOTE_G3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_G3, NOTE_G3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_G3, NOTE_DS4, NOTE_C4, NOTE_G4, NOTE_F4, 0, NOTE_G3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_G3, NOTE_G4, NOTE_DS4, NOTE_C5, NOTE_C4, NOTE_DS4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_DS4, NOTE_C4, NOTE_G3, NOTE_G3, NOTE_G3, NOTE_C4, 0
};
// Note duration for May the Force Be with You
int PROGMEM noteDurations5[] = {
  2, 1, 1.333, 8, 8, 1, 1.333, 4, 1.333, 4, 4, 4, 3, 3, 3, 1, 2, 2, 1.333, 4, 1.333, 8, 1.333, 8, 1, 2, 3, 3, 3, 1.2, 3, 3, 2, 1.333, 8, 1, 2
};

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17] = { -1, 5, 4, 3, 2, 18, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};

// col[xx] of leds = pin yy on led matrix
int rows[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};

// row[xx] of leds = pin yy on led matrix
int cols[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};

int pattern = 0;

int brightness = 0;

const int  switchPin = A0;
int joyPin1 = A0;                 // slider variable connecetd to analog pin 0
int joyPin2 = A5;
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
int col = 0;
int melodyPins = 0;
int music  = 0;

const String start_intro = "Hi";
const String startGame_name = "..STARWARS...";
const String start_info = ".. Asteroids!";
const String end_string = " . score ";
const String points = 0;
const String end_ponts = " asteroids !!!";

int SABER[8][8] { \
  {0, 0, 0, 0, 0, 0, 1, 1},  \
  {0, 0, 0, 0, 0, 1, 1, 1}, \
  {0, 0, 0, 0, 1, 1, 1, 0}, \
  {0, 2, 0, 1, 1, 1, 0, 0}, \
  {0, 2, 2, 1, 1, 0, 0, 0}, \
  {0, 0, 2, 2, 0, 0, 0, 0}, \
  {0, 2, 0, 2, 2, 0, 0, 0}, \
  {2, 0, 0, 0, 0, 0, 0, 0} \
};

int MAUL[8][8] { \
  {0, 2, 0, 2, 2, 0, 2, 0},  \
  {0, 1, 1, 1, 1, 1, 1, 0}, \
  {1, 0, 1, 1, 1, 1, 0, 1}, \
  {1, 0, 0, 1, 1, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {0, 1, 1, 0, 0, 1, 1, 0}, \
  {0, 1, 0, 1, 1, 0, 1, 0}, \
  {0, 0, 1, 1, 1, 1, 0, 0} \
};

int YODA[8][8] { \
  {0, 0, 0, 0, 0, 0, 0, 0},  \
  {0, 0, 0, 1, 1, 0, 0, 0}, \
  {2, 0, 1, 1, 1, 1, 0, 2}, \
  {1, 1, 0, 1, 1, 0, 1, 1}, \
  {0, 1, 1, 0, 0, 1, 1, 0}, \
  {0, 0, 1, 1, 1, 1, 0, 0}, \
  {0, 1, 0, 0, 1, 1, 1, 0}, \
  {1, 1, 1, 0, 1, 1, 1, 1} \
};


typedef enum GameState {
  START,
  PLAY,
  STOP,
  TEST,
};

typedef struct Asteroid {
  int x;
  int y;
  int width;
  int speedx;
  int speedy;
  int type;
};

typedef struct Game {

  GameState state;

  int score;

  int oldScore;

  int asteroidCount;

  // y velocity
  int speed;

  // y position betw 0 and 1
  int falconY;

  int falconX;

  Asteroid asteroids[8];


  int framebuffer[8][8];
};


Game gGame;

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      gGame.framebuffer[i][j] = 0;
    }
  }
}

void displayText(String text, int k, int del)
{
  for (int l = 0; l < 8; l++) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {
        gGame.framebuffer[j][i] = gGame.framebuffer[j][i + 1];
      }
    }
    for (int j = 0; j < 8; j++) {
      gGame.framebuffer[j][7] = ((pgm_read_word(&(font8x8_basic[text[k]][j])) & (1 << l)) > 0) ? 1 : 0;
    }
    delay(del);
  }

}
void slideText(String text, int del) {

  for (int k = 0; k < text.length(); k++)
  { for (int l = 0; l < 8; l++) {
      for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 8; j++) {
          gGame.framebuffer[j][i] = gGame.framebuffer[j][i + 1];
        }
      }
      for (int j = 0; j < 8; j++) {
        gGame.framebuffer[j][7] = ((pgm_read_word(&(font8x8_basic[text[k]][j])) & (1 << l)) > 0) ? 1 : 0;
      }
      delay(del);
    }
  }
}

// Interrupt routine
void display() {
  brightness = ++brightness % 10;
  digitalWrite(cols[col], LOW);  // Turn whole previous column off
  col = ++col % 8;
  for (int row = 0; row < 8; row++) {
    //   brightness = ++brightness % 10;
    if (gGame.framebuffer[row][7 - col] > 0) {
      if (((gGame.framebuffer[row][7 - col]) == 1) || (((gGame.framebuffer[row][7 - col]) == 2) & ((brightness > 7))))
        digitalWrite(rows[row], LOW); // Turn on this led

    }
    else {
      digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)
}

Asteroid generateAsteroid()
{

  Asteroid ast;
  ast.y = random(0, 7);;
  ast.x = random(-gGame.asteroidCount, -1);
  ast.speedx = random(1, 2);
  ast.speedy = random(-1, 1);
  ast.width = 1;
  ast.type = 0;
  return ast;
}
void initAsteroids()
{

  for (int thisAsteroid = 0; thisAsteroid < gGame.asteroidCount; thisAsteroid++) {
    gGame.asteroids[thisAsteroid] = generateAsteroid();
  }
}

void clearScreen()
{
  pattern = 0;
  clearLeds();
}

void initializeGame()
{

  clearScreen();
  gGame.state = START;
  gGame.asteroidCount = 7;
  gGame.oldScore = (gGame.score > 10) ? gGame.score : 10;
  gGame.score = 1;
  gGame.falconX = 7;
  gGame.falconY = 4;
  gGame.speed = 400;
  initAsteroids();
}

void buzz(int targetPin, long frequency, long length) {
  //digitalWrite(12, HIGH);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }


}

void playSong(int melodyPin, int music, int total)
{



  // int size = sizeof(melody) / sizeof(int);
  for (int thisNote = 0; thisNote < total; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 0;
    if (music == 0)
    { noteDuration = 450 / (pgm_read_word_near(noteDurations1 + thisNote));
      buzz(melodyPin, pgm_read_word_near(melody1 + thisNote), noteDuration);
    }
    else
    {
      noteDuration = 300 / (pgm_read_word_near(noteDurations5 + thisNote));
      buzz(melodyPin, pgm_read_word_near(melody5 + thisNote), noteDuration);

    }

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(melodyPin, 0, noteDuration);

  }

  digitalWrite(melodyPin, 0);

}

void setup() {
  // sets the pins as output

  // initialize the I/O pins as outputs
  // iterate over the pins:
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    // initialize the output pins:
    pinMode(cols[thisPin], OUTPUT);
    pinMode(rows[thisPin], OUTPUT);
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off:
    digitalWrite(cols[thisPin], HIGH);
  }

  pinMode(melodyPins, OUTPUT);

  initializeGame();

  for (int i = 1; i <= 8; i++) {
    digitalWrite(cols[i - 1], LOW);
  }

  for (int i = 1; i <= 8; i++) {
    digitalWrite(rows[i - 1], LOW);
  }

  //playSong(melodyPins);

  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(500);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);

}



void playTone(int melodyPin, int number)
{
  //int size = sizeof(melody1) / sizeof(int);
  // for (int thisNote = 0; thisNote < 10; thisNote++) {

  int thisNote = number;

  // to calculate the note duration, take one second
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  int noteDuration = 400 / noteDurations1[thisNote];

  buzz(melodyPin, melody1[thisNote], noteDuration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  int pauseBetweenNotes = noteDuration * 1.3;
  delay(pauseBetweenNotes);

  // stop the tone playing:
  buzz(melodyPin, 0, noteDuration);

  //}

}




int treatValue(int data) {
  return  map(data, 0, 1023, 0, 100);
}

int treatValueFalconX(int data) {
  return  map(data, 0, 1023, -2, 2);
}

void normalizeFalconX() {
  gGame.falconX = (gGame.falconX > 7) ? 7 : (gGame.falconX < 1) ? 1 : gGame.falconX;
}

void normalizeFalconY() {
  gGame.falconY = (gGame.falconY > 6) ? 6 : (gGame.falconY < 1) ? 1 : gGame.falconY;
}

void updateInputFalconX(int input)
{
  gGame.falconX += input;
  normalizeFalconX();
}

void updateInputFalconY(int input)
{
  gGame.falconY -= input;
  normalizeFalconY();
}


int treatValueFalconY(int data) {
  return  map(data, 0, 1023, -2, 3);
}

void setPattern() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      gGame.framebuffer[i][j] = SABER[i][j];
    }
  }

}

void setPatternDelay(int *arr) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      gGame.framebuffer[i][j] = *((arr + i * 8) + j);
    }
  }

}

void setSad() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      gGame.framebuffer[i][j] = SABER[i][j];
    }
  }

}

void startGame()
{

  // slideText(start_intro, 60);
  // slideText(startGame_name, 90);



  if (pattern == 0)
  { setPattern();
    // delay(5000);
    playSong(melodyPins, 0, 9);
  }


  String text = start_intro + gGame.oldScore + startGame_name;
  displayText(text, ++pattern % text.length(), 85);
  // Diplay message
  // Falcon Animation
  // 3 2 1
}

void endGame()
{

  if (pattern == 12)
  { setPattern();
    // delay(5000);
    playSong(melodyPins, 1, 17);
  }

  String text =  "High " + gGame.score;

  if (gGame.score > gGame.oldScore)
    text = "High " + gGame.score;
  else
    text = "Almost " + gGame.score + gGame.oldScore;


  if (pattern == 0)
    if (gGame.score > gGame.oldScore)
    {
      setPatternDelay((int *)YODA);
      delay(3000);
    }
    else
    {
      setPatternDelay((int *)MAUL);
      delay(3000);

    }

  displayText(text, ++pattern % text.length(), 85);
}

void drawFalcon()
{

  gGame.framebuffer[gGame.falconX][gGame.falconY] = 1;
  gGame.framebuffer[gGame.falconX - 1][gGame.falconY - 1] = 1;
  gGame.framebuffer[gGame.falconX - 1][gGame.falconY + 1] = 1;

}

void readInput()
{

  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  // delay(30);
  // reads the value of the variable resistor
  value2 = treatValue(analogRead(joyPin2));

  // Serial.print('J');
  // Serial.println(treatValueFalconY(analogRead(joyPin1)));
  // Serial.println(gGame.falconY);

  switch (gGame.state) {
  case START:
  {

    if (value2 > 80)
    {
      clearScreen();
      gGame.state = PLAY;

    }
    //  gGame.state = STOP;
    // do something
  } break;
  case STOP:
  {
    if (value2 > 80)
    {
      clearScreen();
      gGame.state = TEST;
    }

  } // do something
  break;
  case PLAY:
  {
    // clearLeds();
    updateInputFalconY(treatValueFalconY(analogRead(joyPin1)));
    //gGame.speed +=  map(value2, 0, 1024, -1, 2);
    updateInputFalconX(treatValueFalconX(analogRead(joyPin2)));
    //   gGame.falconY = treatValueFalconY(analogRead(joyPin1));


  } // do something
  break;


    //default:
    // do something
  }
}

void testMatrix()
{

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      clearScreen();
      gGame.framebuffer[i][j] = 1;
      delay(50);

    }
  }
}

void clearAsteroids()
{
  gGame.asteroids = {};
}


void placeAsteroid(Asteroid ast)
{
  if ((((ast.x) > -1) && (ast.x < 8)) && ((((ast.y) > -1) && (ast.y < 8))))
    gGame.framebuffer[ast.x][ast.y] = 2;

}

void moveAsteroid(int thisAsteroid)
{

  if (gGame.asteroids[thisAsteroid].x > 7)
  {

    gGame.score += (8 - gGame.falconX);
    gGame.asteroids[thisAsteroid] = generateAsteroid();
    gGame.speed -= int(8 - gGame.falconX);

  }
  else
  {
    gGame.asteroids[thisAsteroid].x += gGame.asteroids[thisAsteroid].speedx;
    if (gGame.asteroids[thisAsteroid].x > 0)
      gGame.asteroids[thisAsteroid].y += gGame.asteroids[thisAsteroid].speedy;
  }
}

boolean checkAsteroid(int thisAsteroid)
{
  if ((gGame.asteroids[thisAsteroid].x == gGame.falconX) || (gGame.asteroids[thisAsteroid].x == (gGame.falconX - 1)))
  { if (gGame.asteroids[thisAsteroid].y == gGame.falconY)
    {
      gGame.state = STOP;
      return true;

    }
  }
  return false;
}

void drawAsteroids()
{

  for (int thisAsteroid = 0; thisAsteroid < gGame.asteroidCount; thisAsteroid++) {
    placeAsteroid(gGame.asteroids[thisAsteroid]);
    moveAsteroid(thisAsteroid);
    checkAsteroid(thisAsteroid);
  }


}


void loop() {


  readInput();
  switch (gGame.state) {
  case START:
  { startGame();
    //  gGame.state = STOP;
    // do something
  } break;
  case STOP:
  { endGame();

  } // do something
  break;
  //default:
  // do something
  case TEST:
  {
    testMatrix();
    clearLeds();
    //gGame.state = START;
    initializeGame();

  }
  break;
  case PLAY:
  {
    music = 0;
    clearLeds();
    drawFalcon();
    drawAsteroids();
    //playTone(melodyPins,music);
    //music+= ++music%15==0;

    delay(gGame.speed);


  }
  break;

  }

}