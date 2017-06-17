#ifndef __MATH_IMU_H
#define __MATH_IMU_H

#include "stm32l4xx.h"

void Prepare_Data(int16_t *acc_in,int16_t *acc_out);
void IMUupdate(float *gyr, int16_t *acc, float *angle);

#endif
