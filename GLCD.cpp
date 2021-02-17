#include "GLCD.hpp"

PIN GLCD::CS(26,OUTPUT);
uint16_t GLCD::vram[16][32];

void GLCD::write_inst(byte inst)
{
    byte block[]={0xf8, inst&0xf0, inst<<4};
    CS.Write(HIGH);
    SPIDataRW(1,block, 3);
    CS.Write(LOW);
    usleep(5);
    return;
}

void GLCD::write_data(byte data)
{
    byte block[]={0xfa, data&0xf0, data<<4};
    CS.Write(HIGH);
    SPIDataRW(1,block, 3);
    CS.Write(LOW);
    usleep(5);
    return;
}

void GLCD::init()
{
    SPISetupMode (1, 10000000, SPI_MODE_3);
    write_inst(LCD_BASIC);
    write_inst(LCD_BASIC);
    write_inst(LCD_CLS); usleep(2000);
    write_inst(LCD_ADDRINC);
    write_inst(LCD_DISPLAYON);
    write_inst(LCD_EXTEND);
    write_inst(LCD_GFXMODE);
}

void GLCD::pixel(byte x, byte y, bool vru){
    byte xx, yy;
    byte shift;
    if(x >=0 & x <=15)        {xx = 0;shift = x;}
    else if(x >=16 & x <=31)  {xx = 1;shift = x-16;}
    else if(x >=32 & x <=47)  {xx = 2;shift = x-32;}
    else if(x >=48 & x <=63)  {xx = 3;shift = x-48;}
    else if(x >=64 & x <=79)  {xx = 4;shift = x-64;}
    else if(x >=80 & x <=95)  {xx = 5;shift = x-80;}
    else if(x >=96 & x <=111) {xx = 6;shift = x-96;}
    else if(x >=112 & x <=127){xx = 7;shift = x-112;}

    if(y>=32){xx+=8;yy=y-32;}
    else yy=y;

    vram[xx][yy]=vram[xx][yy] | 1<<(15-shift);
    if(vru) updateVram();
    return;
}

void GLCD::unpixel(byte x, byte y, bool vru)
{
    byte xx, yy;
    byte shift;
    if(x >=0 & x <=15)        {xx = 0;shift = x;}
    else if(x >=16 & x <=31)  {xx = 1;shift = x-16;}
    else if(x >=32 & x <=47)  {xx = 2;shift = x-32;}
    else if(x >=48 & x <=63)  {xx = 3;shift = x-48;}
    else if(x >=64 & x <=79)  {xx = 4;shift = x-64;}
    else if(x >=80 & x <=95)  {xx = 5;shift = x-80;}
    else if(x >=96 & x <=111) {xx = 6;shift = x-96;}
    else if(x >=112 & x <=127){xx = 7;shift = x-112;}

    if(y>=32){xx+=8;yy=y-32;}
    else yy=y;

    vram[xx][yy]=vram[xx][yy] & ~(1<<(15-shift));
    if(vru) updateVram();
    return;
}

void GLCD::fill()
{
    for(int x = 0; x<16; x++)
    {
        for(int y = 0; y<32; y++)
        {
        vram[x][y] = 0xFFFF;
        }
    }
    updateVram();
    return;
}

void GLCD::clear()
{
    for(int x = 0; x<16; x++)
    {
        for(int y = 0; y<32; y++)
        {
        vram[x][y] = 0x0000;
        }
    }
    updateVram();
    return;
}

void GLCD::updateVram()
{
    for(int x=0; x<16; x++)
    {
        for(int y=0; y<32; y++)
        {
        write_inst(0x80|y);
        write_inst(0x80|x);
        write_data(highByte(GLCD::vram[x][y]));
        write_data(lowByte(GLCD::vram[x][y]));
        }
    }
    return;
}

void GLCD::line(byte x0, byte y0, byte x1, byte y1, bool q)
{
    if (x0 > x1){byte tmp = x0; x0 = x1; x1 = tmp;}
    if (y0 > y1){byte tmp = y0; y0 = y1; y1 = tmp;}

    if (y0 == y1)
    {
        while(x0<=x1)
        {
        GLCD::pixel(x0,y0,0);
        x0++;  
        }
    }
    else if (x0 == x1)
    {
        while(y0<=y1)
        {
        GLCD::pixel(x0,y0,0);
        y0++;
        }
    }
    else
    {
        while(!(x0==x1 & y0 == y1))
        {
        GLCD::pixel(x0,y0,0);
        if(x0<x1) x0++;
        if(y0<y1) y0++;

        }
    }
    if(q) GLCD::updateVram();
    return;
}
void GLCD::unline(byte x0, byte y0, byte x1, byte y1, bool q)
{
    if (x0 > x1){byte tmp = x0; x0 = x1; x1 = tmp;}
    if (y0 > y1){byte tmp = y0; y0 = y1; y1 = tmp;}

    if (y0 == y1)
    {
        while(x0<=x1)
        {
        GLCD::unpixel(x0,y0,0);
        x0++;  
        }
    }
    else if (x0 == x1)
    {
        while(y0<=y1)
        {
        GLCD::unpixel(x0,y0,0);
        y0++;
        }
    }
    else
    {
        while(!(x0==x1 & y0 == y1))
        {
        GLCD::unpixel(x0,y0,0);
        if(x0<x1) x0++;
        if(y0<y1) y0++;

        }
    }
    if(q) GLCD::updateVram();
    return;
}

void GLCD::rec(byte x,byte y,byte h,byte w, bool q)
{
    GLCD::line(x,y,x+h,y,0);
    GLCD::line(x+h,y,x+h,y+w,0);
    GLCD::line(x+h,y+w,x,y+w,0);
    GLCD::line(x,y+w,x,y,0);
    if(q) updateVram();
}
void GLCD::unrec(byte x,byte y,byte h,byte w, bool q)
{
    GLCD::unline(x,y,x+h,y,0);
    GLCD::unline(x+h,y,x+h,y+w,0);
    GLCD::unline(x+h,y+w,x,y+w,0);
    GLCD::unline(x,y+w,x,y,0);
    if(q) updateVram();
}

void GLCD::circle(byte x,byte y,byte r,bool q)
{
    for(int t = 0; t <360; t++)
    {
        pixel(x+round((float)r*cos(t)), y+round((float)r*sin(t)),0);
    }
    if(q) updateVram();
}
void GLCD::uncircle(byte x,byte y,byte r,bool q)
{
    for(int t = 0; t <360; t++)
    {
        unpixel(x+round((float)r*cos(t)), y+round((float)r*sin(t)),0);
    }
    if(q) updateVram();
}

//-----------------------------------------------------------
//text related stuff: 21(0-20) cols and 10(0-9) lines 
void GLCD::char_black(byte col, byte row, bool uvr){
  byte c = col*6;
  byte r = row*6;
  byte lc= c+5;
  byte lr= r+5;
  GLCD::unline(lc,r,lc,lr,0);
  GLCD::unline(c,lr,lc,lr,0);
  for (r=r;r<=lr-1;r++)
  {
    GLCD::line(c,r,lc-1,r,0);
  }
  if(uvr) GLCD::updateVram();
}

void GLCD::char_white(byte col, byte row, bool uvr){
  byte c = col*6;
  byte r = row*6;
  byte lc= c+5;
  byte lr= r+5;
  GLCD::unline(lc,r,lc,lr,0);
  GLCD::unline(c,lr,lc,lr,0);
  for (r=r;r<=lr-1;r++)
  {
    GLCD::unline(c,r,lc-1,r,0);
  }

  if(uvr) GLCD::updateVram();

}

void GLCD::A(byte col, byte row, bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+1,r+1,0);GLCD::pixel(c+3,r+1,0);
  GLCD::line(c+0,r+2,c+4,r+2,0);
  GLCD::pixel(c+0,r+3,0);GLCD::pixel(c+4,r+3,0);
  GLCD::pixel(c+0,r+4,0);GLCD::pixel(c+4,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::B(byte col, byte row, bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+3,r+0,0);
  GLCD::line(c+0,r+2,c+3,r+2,0);
  GLCD::line(c+0,r+4,c+3,r+4,0);
  GLCD::pixel(c+4,r+1,0);
  GLCD::pixel(c+3,r+2,0);
  GLCD::pixel(c+4,r+3,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::C(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+0,r+1,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+4,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::D(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::rec(c+0,r+0,4,4,0);
  GLCD::unpixel(c+4,r+0,0);GLCD::unpixel(c+4,r+4,0);
  if(uvr) GLCD::updateVram();

}
void GLCD::E(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  GLCD::line(c+0,r+4,c+4,r+4,0);
 if(uvr) GLCD::updateVram();
}
void GLCD::lF(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::G(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+0,r+1,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+4,r+4,0);
  GLCD::pixel(c+4,r+3,0);
  GLCD::line(c+2,r+2,c+4,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::H(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::line(c+0,r+2,c+4,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::I(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+2,r+0,c+2,r+4,0);
  GLCD::line(c+1,r+0,c+3,r+0,0);
  GLCD::line(c+1,r+4,c+3,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::J(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+3,r+1,c+3,r+3,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+3,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::K(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  pixel(c+1,r+2,0);pixel(c+2,r+1,0);pixel(c+3,r+0,0);
  pixel(c+2,r+3,0);pixel(c+3,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::L(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+1,r+0,c+1,r+4,0);
  GLCD::line(c+1,r+4,c+4,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::M(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+3,r+1,0);
  pixel(c+2,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::N(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+3,r+3,0);
  if(uvr) GLCD::updateVram();
}

void GLCD::O(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::rec(c+0,r+0,4,4,0);
  GLCD::unpixel(c+0,r+0,0);
  GLCD::unpixel(c+0,r+4,0);
  GLCD::unpixel(c+4,r+4,0);
  GLCD::unpixel(c+4,r+0,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::P(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::pixel(c+1,r+0,0);
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+2,0);
  if(uvr) GLCD::updateVram();
}

void GLCD::Q(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::rec(c+0,r+0,3,4,0);
  GLCD::unpixel(c+0,r+0,0);
  GLCD::unpixel(c+3,r+0,0);
  GLCD::unpixel(c+0,r+4,0);
  GLCD::unpixel(c+3,r+4,0);
  GLCD::pixel(c+4,r+4,0);
  GLCD::pixel(c+2,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::R(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::pixel(c+1,r+0,0);
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+2,0);
  GLCD::pixel(c+3,r+4,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::S(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+1,r+0,c+3,r+0,0);
  GLCD::pixel(c+0,r+1,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  GLCD::pixel(c+4,r+3,0);
  GLCD::line(c+1,r+4,c+3,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::T(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+2,r+0,c+2,r+4,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::U(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+2,r+4,0);
  GLCD::line(c+3,r+0,c+3,r+3,0);
  GLCD::pixel(c+4,r+4,0); 
  if(uvr) GLCD::updateVram();
}
void GLCD::V(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+2,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+3,r+3,0);
  GLCD::line(c+4,r+0,c+4,r+2,0);
 if(uvr) GLCD::updateVram();
}
void GLCD::W(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+3,0); 
  GLCD::pixel(c+3,r+3,0);
  pixel(c+2,r+2,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::X(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+4,r+4,0);
  GLCD::pixel(c+4,r+4,0);
  GLCD::pixel(c+0,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+4,r+0,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::Y(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+2,r+2,c+2,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+0,r+0,0);
  GLCD::pixel(c+4,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  if(uvr) GLCD::updateVram();
}
void GLCD::Z(byte col, byte row,bool uvr){
  GLCD::char_white(col, row,0);
  byte c = col*6;
  byte r = row*6;
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::line(c+0,r+4,c+4,r+4,0);
  if(uvr) GLCD::updateVram();
}

void GLCD::writeText(string data, byte col, byte row,bool uvr)
{
    byte X = col*6;
    byte Y = row*6;
    for(byte i = 0; i < data.length(); i++){  
    switch (data.at(i))
    {
    case 'A' | 'a':
        GLCD::A(X,Y,0);
        break;
    case 'B'|'b':
        GLCD::B(X,Y,0);
        break;
    case 'C'|'c':
        GLCD::C(X,Y,0);
        break;
    case 'D'|'d':
        GLCD::D(X,Y,0);
        break;
    case 'E'|'e':
        GLCD::E(X,Y,0);
        break;
    case 'F'|'f':
        GLCD::lF(X,Y,0);
        break;
    case 'G'|'g':
        GLCD::G(X,Y,0);
        break;
    case 'H'|'h':
        GLCD::H(X,Y,0);
        break;
    case 'I'|'i':
        GLCD::I(X,Y,0);
        break;
    case 'J'|'j':
        GLCD::J(X,Y,0);
        break;
    case 'K'|'k':
        GLCD::K(X,Y,0);
        break;
    case 'L'|'l':
        GLCD::L(X,Y,0);
        break;
    case 'M'|'m':
        GLCD::M(X,Y,0);
        break;
    case 'N'|'n':
        GLCD::N(X,Y,0);
        break;
    case 'O'|'o':
        GLCD::O(X,Y,0);
        break;
    case 'P'|'p':
        GLCD::P(X,Y,0);
        break;
    case 'Q'|'q':
        GLCD::Q(X,Y,0);
        break;
    case 'R'|'r':
        GLCD::R(X,Y,0);
        break;
    case 'S'|'s':
        GLCD::S(X,Y,0);
        break;
    case 'T'|'t':
        GLCD::T(X,Y,0);
        break;
    case 'U'|'u':
        GLCD::U(X,Y,0);
        break;
    case 'V'|'v':
        GLCD::V(X,Y,0);
        break;
    case 'W'|'w':
        GLCD::W(X,Y,0);
        break;
    case 'X'|'x':
        GLCD::X(X,Y,0);
        break;
    case 'Y'|'y':
        GLCD::Y(X,Y,0);
        break;
    case 'Z'|'z':
        GLCD::Z(X,Y,0);
        break;
    case ' ':
        GLCD::char_white(X,Y,0);
        break;
    case '\n':
        X=0; Y++;
        break;
    
    default:
        break;
    }
    X++;
    if(X>20){X = 0; Y++;}
    if(Y>9){Y=0;X=0;}
    }
    if(uvr) GLCD::updateVram();
return;
}
//---------------------------------------------------------------
//----------------floating charachters---------------------------
void GLCD::floating_A(byte c, byte r){
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+1,r+1,0);GLCD::pixel(c+3,r+1,0);
  GLCD::line(c+0,r+2,c+4,r+2);
  GLCD::pixel(c+0,r+3,0);GLCD::pixel(c+4,r+3,0);
  GLCD::pixel(c+0,r+4,0);GLCD::pixel(c+4,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_B(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+3,r+0,0);
  GLCD::line(c+0,r+2,c+3,r+2,0);
  GLCD::line(c+0,r+4,c+3,r+4,0);
  GLCD::pixel(c+4,r+1,0);
  GLCD::pixel(c+3,r+2,0);
  GLCD::pixel(c+4,r+3,0);
  GLCD::updateVram();
}
void GLCD::floating_C(byte c, byte r){
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+0,r+1,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+4,r+4);
  GLCD::updateVram();
}
void GLCD::floating_D(byte c, byte r){
  GLCD::rec(c+0,r+0,4,4,0);
  GLCD::unpixel(c+4,r+0,0);GLCD::unpixel(c+4,r+4,0);
  GLCD::updateVram();

}
void GLCD::floating_E(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  GLCD::line(c+0,r+4,c+4,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_lF(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_G(byte c, byte r){
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+0,r+1,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+4,r+4,0);
  GLCD::pixel(c+4,r+3,0);
  GLCD::line(c+2,r+2,c+4,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_H(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::line(c+0,r+2,c+4,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_I(byte c, byte r){
  GLCD::line(c+2,r+0,c+2,r+4,0);
  GLCD::line(c+1,r+0,c+3,r+0,0);
  GLCD::line(c+1,r+4,c+3,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_J(byte c, byte r){
  GLCD::line(c+1,r+0,c+4,r+0,0);
  GLCD::line(c+3,r+1,c+3,r+3,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+3,r+4,0);
  GLCD::updateVram();  
}
void GLCD::floating_K(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  pixel(c+1,r+2,0);pixel(c+2,r+1,0);pixel(c+3,r+0,0);
  pixel(c+2,r+3,0);pixel(c+3,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_L(byte c, byte r){
  GLCD::line(c+1,r+0,c+1,r+4,0);
  GLCD::line(c+1,r+4,c+4,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_M(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+3,r+1,0);
  pixel(c+2,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_N(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+3,r+3,0);
  
  GLCD::updateVram();
}
void GLCD::floating_O(byte c, byte r){
  GLCD::rec(c+0,r+0,4,4);
  GLCD::unpixel(c+0,r+0,0);
  GLCD::unpixel(c+0,r+4,0);
  GLCD::unpixel(c+4,r+4,0);
  GLCD::unpixel(c+4,r+0,0);
  GLCD::updateVram();
}
void GLCD::floating_P(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4);
  GLCD::pixel(c+1,r+0,0);
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_Q(byte c, byte r){
  GLCD::rec(c+0,r+0,3,4,0);
  GLCD::unpixel(c+0,r+0,0);
  GLCD::unpixel(c+3,r+0,0);
  GLCD::unpixel(c+0,r+4,0);
  GLCD::unpixel(c+3,r+4,0);
  GLCD::pixel(c+4,r+4,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_R(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4);
  GLCD::pixel(c+1,r+0,0);
  GLCD::pixel(c+2,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+2,0);
  GLCD::pixel(c+3,r+4,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::updateVram();
}
void GLCD::floating_S(byte c, byte r){
  GLCD::line(c+1,r+0,c+3,r+0,0);
  GLCD::pixel(c+0,r+1,0);
  GLCD::line(c+1,r+2,c+3,r+2,0);
  GLCD::pixel(c+4,r+3,0);
  GLCD::line(c+1,r+4,c+3,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_T(byte c, byte r){
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::line(c+2,r+0,c+2,r+4,0);
  GLCD::updateVram();
}
void GLCD::floating_U(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+3,0);
  GLCD::line(c+1,r+4,c+2,r+4,0);
  GLCD::line(c+3,r+0,c+3,r+3,0);
  GLCD::pixel(c+4,r+4,0); 
  GLCD::updateVram();
}
void GLCD::floating_V(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+2,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+2,r+4,0);
  GLCD::pixel(c+3,r+3,0);
  GLCD::line(c+4,r+0,c+4,r+2,0);
  GLCD::updateVram(); 
}
void GLCD::floating_W(byte c, byte r){
  GLCD::line(c+0,r+0,c+0,r+4,0);
  GLCD::line(c+4,r+0,c+4,r+4,0);
  GLCD::pixel(c+1,r+3,0); 
  GLCD::pixel(c+3,r+3,0);
  pixel(c+2,r+2,0);
  GLCD::updateVram();
}
void GLCD::floating_X(byte c, byte r){
  GLCD::line(c+0,r+0,c+4,r+4,0);
  GLCD::pixel(c+4,r+4,0);
  GLCD::pixel(c+0,r+4,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+4,r+0,0);
  GLCD::updateVram();
}
void GLCD::floating_Y(byte c, byte r){
  GLCD::line(c+2,r+2,c+2,r+4,0);
  GLCD::pixel(c+1,r+1,0);
  GLCD::pixel(c+0,r+0,0);
  GLCD::pixel(c+4,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::updateVram();
}
void GLCD::floating_Z(byte c, byte r){
  GLCD::line(c+0,r+0,c+4,r+0,0);
  GLCD::pixel(c+3,r+1,0);
  GLCD::pixel(c+2,r+2,0);
  GLCD::pixel(c+1,r+3,0);
  GLCD::line(c+0,r+4,c+4,r+4,0);
  GLCD::updateVram();
}