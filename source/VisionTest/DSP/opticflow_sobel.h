#ifndef _OPTICFLOW_SOBEL_H_
#define _OPTICFLOW_SOBEL_H_
void OFCreateBuffs(unsigned int startAddress);
void ApplySobelFilter(struct img_struct* imgIn, unsigned int *profileX, unsigned int *profileY, unsigned int *threshold);
signed int CalcOF3(unsigned int *act, unsigned int *prev, unsigned int length);
#endif
