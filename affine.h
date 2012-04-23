#ifndef AFFINE_H
#define AFFINE_H
typedef struct{
    unsigned short fill0[3];
    signed short pa;
    unsigned short fill1[3];
    signed short pb;
    unsigned short fill2[3];
    signed short pc;
    unsigned short fill3[3];
    signed short pd;
}OBJ_AFF;

#define AFFINE_MEMORY ((OBJ_AFF*)(0x7000000))

void setAffineMatrix(int index, float theta, signed short sx, signed short sy);
#endif