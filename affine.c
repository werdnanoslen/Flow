#include "affine.h"
#include "trigLUT.h"
#include "fixed.h"

void setAffineMatrix(int index, float theta, signed short sx, signed short sy){
    AFFINE_MEMORY[index].pa = fixedDivide(cosine(theta), sx);
    AFFINE_MEMORY[index].pb = fixedDivide(-sine(theta), sx);
    AFFINE_MEMORY[index].pc = fixedDivide(sine(theta), sy);
    AFFINE_MEMORY[index].pd = fixedDivide(cosine(theta), sy);
}