/*******************************************************************************
 * "flow" is a relaxed platforming game where the player navigates flowing 
 * particles into a sink. The particles bounce off two pebbles, which the player
 * arranges to carry the particles to the sink.
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "myLib.h"
#include "startbg.h"
#include "gamebg.h"
#include "sprites.h"
#include "menu.h"
#include "pause.h"
#include "win.h"
#include "loop.h"
#include "strike1.h"
#include "affine.h"
#include "fixed.h"
#include "trigLUT.h"


/*BEGIN MATH*/
#define SHIFTUP(i) (i<<8)
#define SHIFTDOWN(i) (i>>8)
#define TIMER 100
int timer;
/*END MATH*/


/* BEGIN GAME STATES*/
#define STARTSCREEN 0
#define GAMESCREEN 1
#define PAUSESCREEN 2
#define WINSCREEN 3
int state;
int cheat;
int points;
int maxPoints;
void start();
void game();
void pause();
void win();
/* END GAME STATES*/


/*BEGIN GRAPHICS*/
OBJ_ATTR shadowOAM[128];
Sprite pebble1;
Sprite pebble2;
Sprite spawner;
Sprite bucket;
#define NUMPARTICLES 120
Sprite particle[NUMPARTICLES];
float theta = 0.0;
float deltaTheta = -1.0;
unsigned short affinable;
/*END GRAPHICS*/


/*BEGIN SOUND*/
SOUND soundA;
SOUND soundB;
int vbCountA;
int vbCountB;
void setupSounds();
void playSoundB(const unsigned char* sound, int length, int frequency, int isLooping);
void playSoundA(const unsigned char* sound, int length, int frequency, int isLooping);
void setupInterrupts();
void interruptHandler();
/*END SOUND*/


/*BEGIN UTILS*/
unsigned int buttons;
unsigned int oldButtons;
void initialize();
void reset();
void spawnParticle(int i);
void updateParticle();
void updateOAM();
int boundCollide(Sprite *spr);
int collides(Sprite *spr1, Sprite *spr2);
/*END UTILS*/


/*BEGIN LEVELS*/
int spawnerRow, spawnerCol;
int bucketRow, bucketCol;
int pspawnRow, pspawnCol;
int angle;
void generateLevel();
/*END LEVELS*/


int main()
{
    reset();
    
    while(1){
        switch(state)
        {
            case STARTSCREEN:
                start();
                break;
            case GAMESCREEN:
                game();
                break;
            case PAUSESCREEN:
                pause();
                break;
            case WINSCREEN:
                win();
                break;
        }
    }
    return 0;
}

void start()
{
    if (BUTTON_HELD(BUTTON_START))
    {
        while(BUTTON_HELD(BUTTON_START));
        state = GAMESCREEN;
        initialize();
    }
}

void game()
{   
    if (BUTTON_HELD(BUTTON_A))
    {
        if(BUTTON_HELD(BUTTON_UP) && boundCollide(&pebble1)!=1)
        {
            pebble1.rvel = -1;
            --(pebble1.row);
        }
        if(BUTTON_HELD(BUTTON_RIGHT) && boundCollide(&pebble1)!=2)
        {
            pebble1.cvel = 1;
            ++(pebble1.col);
        }
        if(BUTTON_HELD(BUTTON_DOWN) && boundCollide(&pebble1)!=3)
        {
            pebble1.rvel = 1;
            ++(pebble1.row);
        }
        if(BUTTON_HELD(BUTTON_LEFT) && boundCollide(&pebble1)!=4)
        {
            pebble1.cvel = -1;
            --(pebble1.col); 
        }
    }
    
    if (BUTTON_HELD(BUTTON_B))
    {
        if(BUTTON_HELD(BUTTON_UP) && boundCollide(&pebble2)!=1)
        {
            pebble2.rvel = -1;
            --(pebble2.row);
        }
        if(BUTTON_HELD(BUTTON_RIGHT) && boundCollide(&pebble2)!=2)
        {
            pebble2.cvel = 1;
            ++(pebble2.col);
        }
        if(BUTTON_HELD(BUTTON_DOWN) && boundCollide(&pebble2)!=3)
        {
            pebble2.rvel = 1;
            ++(pebble2.row);
        }
        if(BUTTON_HELD(BUTTON_LEFT) && boundCollide(&pebble2)!=4)
        {
            pebble2.cvel = -1;
            --(pebble2.col); 
        }
    }
    
    if (BUTTON_HELD(BUTTON_START))
    {
        while(BUTTON_HELD(BUTTON_START));
        state = PAUSESCREEN;
        DMANow(3, (unsigned int*)pauseTiles, &CHARBLOCKBASE[3], pauseTilesLen/2);
        DMANow(3, (unsigned int*)pauseMap, &SCREENBLOCKBASE[30], pauseMapLen/2);
    }
    
    if (points >= maxPoints)
    {
        state=WINSCREEN;
        DMANow(3, (unsigned int*)winTiles, &CHARBLOCKBASE[3], winTilesLen/2);
        DMANow(3, (unsigned int*)winMap, &SCREENBLOCKBASE[30], winMapLen/2);
    }
    updateParticle();
    updateOAM();
}   

void pause()
{
    if (BUTTON_HELD(BUTTON_START))
    {
        while(BUTTON_HELD(BUTTON_START));
        state = GAMESCREEN;
        DMANow(3, 0, &CHARBLOCKBASE[3], menuTilesLen/2);
        DMANow(3, 0, &SCREENBLOCKBASE[30], menuMapLen/2);
    }
    
    if (BUTTON_HELD(BUTTON_SELECT))
    {
        while(BUTTON_HELD(BUTTON_SELECT));
        reset();
    }
    
    if (BUTTON_HELD(BUTTON_A))
    {
        while(BUTTON_HELD(BUTTON_A));
        cheat = !cheat;
        affinable = cheat ? 0 : ATTR0_AFFINE;
    }
}

void win()
{
    if (BUTTON_HELD(BUTTON_SELECT))
    {
        while(BUTTON_HELD(BUTTON_SELECT));
        reset();
    }
    if (BUTTON_HELD(BUTTON_START))
    {
        while(BUTTON_HELD(BUTTON_START));
        initialize();
        state = GAMESCREEN;
    }
}

void updateParticle()
{
    int i;
    for (i=0; i<NUMPARTICLES; ++i)
    {
        if (!particle[i].live && !particle[i].scored)
        {
            if (timer)
            {
                timer--;
            }
            else
            {
                spawnParticle(i);
                timer = TIMER;
            }
        }
        
        particle[i].rvel += particle[i].racc;
        particle[i].row += particle[i].rvel;
        particle[i].col += particle[i].cvel;

        if (!cheat && boundCollide(&particle[i])) particle[i].live = 0;

        if (cheat)
        {
            if (boundCollide(&particle[i])==1)
            {
                particle[i].row = particle[i].radius;
                particle[i].rvel = -particle[i].rvel;
            }
            if (boundCollide(&particle[i])==2)
            {
                particle[i].col = SHIFTUP(SCREENWIDTH) - particle[i].radius;
                particle[i].cvel = -particle[i].cvel;
            }
            if (boundCollide(&particle[i])==3)
            {
                particle[i].row = SHIFTUP(SCREENHEIGHT) - particle[i].radius;
                particle[i].rvel = -particle[i].rvel;
            }
            if (boundCollide(&particle[i])==4)
            {
                particle[i].col = particle[i].radius;
                particle[i].cvel = -particle[i].cvel;
            } 
        }

        if (collides(&particle[i], &pebble1))
        {
            particle[i].rvel -= particle[i].rvel<=-340 ? 0 : 60;
        }
        if (collides(&particle[i], &pebble2))
        {
            particle[i].rvel -= particle[i].rvel<=-340 ? 0 : 60;
        }
        
        if (collides(&particle[i], &bucket) && particle[i].live)
        {
            points++;
            particle[i].live = 0;
            particle[i].scored = 1;
            playSoundA(strike1, STRIKE1LEN, STRIKE1FREQ, 0);
        }
    }
}

void updateOAM()
{
    /*pebble1*/
    shadowOAM[0].attr0 = (pebble1.row - pebble1.width) | ATTR0_REGULAR | ATTR0_SQUARE | ATTR0_8BPP;
    shadowOAM[0].attr1 = (pebble1.col - pebble1.width) | ATTR1_SIZE16;
    shadowOAM[0].attr2 = SPRITEOFFSET16(0,0);
    
    /*pebble2*/
    shadowOAM[1].attr0 = (pebble2.row - pebble1.width) | ATTR0_REGULAR | ATTR0_SQUARE | ATTR0_8BPP;
    shadowOAM[1].attr1 = (pebble2.col - pebble1.width) | ATTR1_SIZE16;
    shadowOAM[1].attr2 = SPRITEOFFSET16(0,4);
    
    /*spawner*/
    shadowOAM[2].attr0 = (spawner.row) | ATTR0_REGULAR | ATTR0_SQUARE | ATTR0_8BPP;
    shadowOAM[2].attr1 = (spawner.col) | ATTR1_SIZE32;
    shadowOAM[2].attr2 = SPRITEOFFSET16(0,8);
    
    /*bucket*/
    shadowOAM[3].attr0 = (bucket.row - bucket.radius) | ATTR0_REGULAR | ATTR0_SQUARE | affinable | ATTR0_8BPP;
    shadowOAM[3].attr1 = (bucket.col - bucket.radius) | ATTR1_SIZE32 | ATTR1_AFFINEINDEX(1);
    shadowOAM[3].attr2 = SPRITEOFFSET16(0,24);
    
    /*particles*/
    int i;
    for (i=0; i<NUMPARTICLES; ++i)
    {
        if (particle[i].live)
        {
            shadowOAM[4+i].attr0 = (SHIFTDOWN(particle[i].row - particle[i].radius)) | ATTR0_REGULAR | ATTR0_SQUARE | ATTR0_8BPP;
            shadowOAM[4+i].attr1 = (SHIFTDOWN(particle[i].col - particle[i].radius)) | ATTR1_SIZE8;
            shadowOAM[4+i].attr2 = SPRITEOFFSET16(0,16);
        }
        if (!particle[i].live)
        {
            shadowOAM[4+i].attr0 = ATTR0_HIDE;
        }
    }
    
    /*BORROWED FROM AFFINESPRITES LAB THANKS :D*/
    theta += deltaTheta;
    if(theta > 360.0) theta -= 360.0;
    if(theta < 0.0) theta += 360.0;
    setAffineMatrix(1, theta, INT2FIX(1), INT2FIX(1));
    
    waitForVblank();
    DMANow(3, shadowOAM, OAM, 512 | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT);
}

int boundCollide(Sprite *spr)
{
    if (spr->row - spr->width <= 0) return 1;
    if (spr->col + spr->width >= (spr->isParticle ? SHIFTUP(SCREENWIDTH) : SCREENWIDTH)) return 2;
    if (spr->row + spr->width >= (spr->isParticle ? SHIFTUP(SCREENHEIGHT) : SCREENHEIGHT)) return 3;
    if (spr->col - spr->width <= 0) return 4;
    
    return 0;
}

int collides(Sprite *spr1, Sprite *spr2)
{
    if (spr1->isParticle)
    {
        int dx = SHIFTDOWN(spr1->row) - spr2->row;
        int dy = SHIFTDOWN(spr1->col) - spr2->col;
        int sqdist = (dx * dx) + (dy * dy);

        if (sqdist <= (SHIFTDOWN(spr1->radius) + spr2->radius)
                * (SHIFTDOWN(spr1->radius) + spr2->radius))
        {
            return 1;
        }
        
        return 0;
    }
    else 
    {
        int dx = spr1->row - spr2->row;
        int dy = spr1->col - spr2->col;
        int sqdist = (dx * dx) + (dy * dy);

        if (sqdist <= (spr1->radius + spr2->radius) * (spr1->radius + spr2->radius))
        {
            return 1;
        }
        return 0;
    }
}

void spawnParticle(int i)
{
    particle[i].isParticle = 1;
    particle[i].live = 1;
    particle[i].row = SHIFTUP(4);
    particle[i].col = SHIFTUP(4);
    particle[i].radius = SHIFTUP(3);
    particle[i].width = SHIFTUP(3);
    particle[i].rvel = 0;
    particle[i].cvel = (angle+90) + rand()%10;
    particle[i].racc = 4;
}

void initialize()
{        
    generateLevel();
    
    REG_SOUNDCNT_X = SND_ENABLED;
    cheat = 0;
    points = 0;
    maxPoints = NUMPARTICLES;
    loadPalette(gamebgPal);
    DMANow(3, (unsigned int*)gamebgTiles, &CHARBLOCKBASE[0], gamebgTilesLen/2);
    DMANow(3, (unsigned int*)gamebgMap, &SCREENBLOCKBASE[27], gamebgMapLen/2);
    DMANow(3, 0, &CHARBLOCKBASE[3], menuTilesLen/2);
    DMANow(3, 0, &SCREENBLOCKBASE[30], menuMapLen/2);
    
    pebble1.row = SCREENHEIGHT-20;
    pebble1.col = 20;
    pebble1.radius = 16;
    pebble1.rvel = pebble1.cvel = 0;
    pebble1.width = 12;
    pebble1.isPlatform = 1;
    
    pebble2.row = SCREENHEIGHT-20;
    pebble2.col = SCREENWIDTH-20;
    pebble2.radius = pebble1.radius;
    pebble2.rvel = pebble2.cvel = pebble1.rvel;
    pebble2.width = pebble1.width;
    pebble2.isPlatform = pebble1.isPlatform;
    
    spawner.row = spawnerRow;
    spawner.col = spawnerCol;
    
    bucket.row = bucketRow;
    bucket.col = bucketCol;
    
    int i;
    for (i=0; i<NUMPARTICLES; ++i)
    {
        particle[i].row = pspawnRow;
        particle[i].col = pspawnCol;
        particle[i].live = 1;
        particle[i].scored = 0;
    }
}

void generateLevel()
{
    angle = rand()%50;
    
    spawnerRow = 0;
    spawnerCol = 0;
    
    pspawnRow = SHIFTUP(4);
    pspawnCol = SHIFTUP(4);
    
    bucket.radius = 16;
    bucketRow = (SCREENHEIGHT-bucket.radius) - rand()%80;
    bucketCol = (SCREENWIDTH-bucket.radius) - rand()%60;
    affinable = (bucketRow > 108) ? ATTR0_AFFINE : 0;
}

void reset()
{
    REG_DISPCTL = MODE0 | SPRITE_ENABLE | BG0_ENABLE | BG1_ENABLE; 
    REG_BG1CNT = CBB(0) | SBB(27) | BG_SIZE0 | COLOR256 | BG_MOSAIC;
    REG_BG0CNT = CBB(3) | SBB(30) | BG_SIZE0 | COLOR256;
    
    //int ov, oh, bv, bh;
    //ov = oh = bv = bh = 0;
    //REG_MOSAIC = (ov<<12) | (oh<<8) | (bv << 4) | bh;
    
    /*BG1*/
    loadPalette(startbgPal);
    DMANow(3, (unsigned int*)startbgTiles, &CHARBLOCKBASE[0], startbgTilesLen/2);
    DMANow(3, (unsigned int*)startbgMap, &SCREENBLOCKBASE[27], startbgMapLen/2);
    
    /*BG0*/
    DMANow(3, (unsigned int*)menuTiles, &CHARBLOCKBASE[3], menuTilesLen/2);
    DMANow(3, (unsigned int*)menuMap, &SCREENBLOCKBASE[30], menuMapLen/2);
    
    /*SPRITE*/
    DMANow(3, spritesTiles, &CHARBLOCKBASE[4], spritesTilesLen/2);
    DMANow(3, spritesPal, SPRITE_PALETTE, 256);
    
    int i;
    for (i=0; i<128; ++i)
    {
        shadowOAM[i].attr0 = ATTR0_HIDE;
        OAM[i].attr0 = ATTR0_HIDE;
    }
    
    buttons = BUTTONS;
    state = STARTSCREEN;    
    cheat = 0;
    
    setupInterrupts();
    setupSounds();
    playSoundB(loop, LOOPLEN, LOOPFREQ, 1);
    REG_SOUNDCNT_X = 0;
}

void setupInterrupts()
{
    REG_IME = 0;
    REG_INTERRUPT = (unsigned int)interruptHandler;
    REG_IE |= INT_VBLANK;
    REG_DISPSTAT |= INT_VBLANK_ENABLE;

    REG_IME = 1;
}

void interruptHandler()
{
    REG_IME = 0;

    if(REG_IF & INT_VBLANK)
    {
        vbCountA++;
        if (vbCountA >= soundA.duration)
        {
            REG_TM0CNT = 0;
            dma[1].cnt = 0;
            if (soundA.loops)
            {
                playSoundA(soundA.data, soundA.length, soundA.frequency, soundA.loops);
            }
        }

        vbCountB++;
        if (vbCountB >= soundB.duration)
        {
            REG_TM1CNT = 0;
            dma[2].cnt = 0;

            if (soundB.loops)
            {
                playSoundB(soundB.data, soundB.length, soundB.frequency, soundA.loops);
            }
        }
        REG_IF = INT_VBLANK;
    }

    REG_IME = 1;
}

void playSoundA(const unsigned char* sound, int length, int frequency, int isLooping) 
{
    dma[1].cnt = 0;
    vbCountA = 0;
    int interval = 16777216/frequency;
    DMANow(1, sound, REG_FIFO_A, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);
    REG_TM0CNT = 0;
    REG_TM0D = -interval;
    REG_TM0CNT = TIMER_ON;

    soundA.data = sound;
    soundA.length = length;
    soundA.frequency = frequency;
    soundA.isPlaying = 1;
    soundA.loops = isLooping;

    soundA.duration = ((60*length)/frequency) - ((length/frequency)*3)-1;
}

void playSoundB(const unsigned char* sound, int length, int frequency, int isLooping) 
{
    dma[2].cnt = 0;
    vbCountB = 0;
    int interval = 16777216/frequency;
    DMANow(2, sound, REG_FIFO_B, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);
    REG_TM1CNT = 0;
    REG_TM1D = -interval;
    REG_TM1CNT = TIMER_ON;
    
    soundB.data = sound;
    soundB.length = length;
    soundB.frequency = frequency;
    soundB.isPlaying = 1;
    soundB.loops = isLooping;
    
    soundB.duration = ((60*length)/frequency) - ((length/frequency)*3)-1;       
}

void setupSounds()
{
    REG_SOUNDCNT_X = SND_ENABLED;

    REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 | 

        DSA_OUTPUT_RATIO_50 |
        DSA_OUTPUT_TO_BOTH |
        DSA_TIMER0 |
        DSA_FIFO_RESET |

        DSB_OUTPUT_RATIO_100 |
        DSB_OUTPUT_TO_BOTH |
        DSB_TIMER1 |
        DSB_FIFO_RESET;

    REG_SOUNDCNT_L = 0;
}