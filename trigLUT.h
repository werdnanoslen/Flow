#ifndef trigLUT_H
#define trigLUT_H

extern const signed short cosLUT[512];
extern const signed short sinLUT[512];

/*
Expects a float number between 0 and 360.
returns an 8.8 fixed point number
*/
short cosine(float theta);
short sine(float theta);
#endif
