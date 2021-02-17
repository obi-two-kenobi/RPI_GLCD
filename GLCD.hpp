#pragma once
#include "NewPin.hpp"
#include <string>

#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_DISPLAYOFF  0x08
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_GFXMODE     0x36
#define LCD_TXTMODE     0x34
#define LCD_STANDBY     0x01
#define LCD_SCROLL      0x03
#define LCD_SCROLLADDR  0x40
#define LCD_ADDR        0x80
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98


class GLCD
{
private:
static uint16_t vram[16][32];
static PIN CS;
static void write_inst(byte inst);
static void write_data(byte data);

public:
static void init();
static void fill();
static void clear();
static void updateVram();

static void pixel(byte x, byte y, bool vru=true);
static void unpixel(byte x, byte y, bool vru=true);

static void line(byte x0, byte y0, byte x1, byte y1, bool q = true);
static void unline(byte x0, byte y0, byte x1, byte y1, bool q = true);

static void rec(byte x,byte y,byte h,byte w, bool q = true);
static void unrec(byte x,byte y,byte h,byte w, bool q = true);

static void circle(byte x,byte y,byte r,bool q=true);
static void uncircle(byte x,byte y,byte r,bool q=true);



static void char_white(byte col, byte row, bool uvr=true);
static void char_black(byte col, byte row, bool uvr=true);

static void A(byte col, byte row, bool uvr = true);
static void B(byte col, byte row, bool uvr = true);
static void C(byte col, byte row, bool uvr = true);
static void D(byte col, byte row, bool uvr = true);
static void E(byte col, byte row, bool uvr = true);
static void lF(byte col, byte row, bool uvr = true);
static void G(byte col, byte row, bool uvr = true);
static void H(byte col, byte row, bool uvr = true);
static void I(byte col, byte row, bool uvr = true);
static void J(byte col, byte row, bool uvr = true);
static void K(byte col, byte row, bool uvr = true);
static void L(byte col, byte row, bool uvr = true);
static void M(byte col, byte row, bool uvr = true);
static void N(byte col, byte row, bool uvr = true);
static void O(byte col, byte row, bool uvr = true);
static void P(byte col, byte row, bool uvr = true);
static void Q(byte col, byte row, bool uvr = true);
static void R(byte col, byte row, bool uvr = true);
static void S(byte col, byte row, bool uvr = true);
static void T(byte col, byte row, bool uvr = true);
static void U(byte col, byte row, bool uvr = true);
static void V(byte col, byte row, bool uvr = true);
static void W(byte col, byte row, bool uvr = true);
static void X(byte col, byte row, bool uvr = true);
static void Y(byte col, byte row, bool uvr = true);
static void Z(byte col, byte row, bool uvr = true);
static void writeText(string data,byte col=0, byte row=0, bool uvr = true);

static void floating_A(byte c, byte r);
static void floating_B(byte c, byte r);
static void floating_C(byte c, byte r);
static void floating_D(byte c, byte r);
static void floating_E(byte c, byte r);
static void floating_lF(byte c, byte r);
static void floating_G(byte c, byte r);
static void floating_H(byte c, byte r);
static void floating_I(byte c, byte r);
static void floating_J(byte c, byte r);
static void floating_K(byte c, byte r);
static void floating_L(byte c, byte r);
static void floating_M(byte c, byte r);
static void floating_N(byte c, byte r);
static void floating_O(byte c, byte r);
static void floating_P(byte c, byte r);
static void floating_Q(byte c, byte r);
static void floating_R(byte c, byte r);
static void floating_S(byte c, byte r);
static void floating_T(byte c, byte r);
static void floating_U(byte c, byte r);
static void floating_V(byte c, byte r);
static void floating_W(byte c, byte r);
static void floating_X(byte c, byte r);
static void floating_Y(byte c, byte r);
static void floating_Z(byte c, byte r);
};