#include "IMU.h"
#include "math.h"

#define RtA 		57.324841f
#define AtR    		0.0174533f
#define Acc_G 		0.0011963f
#define Gyro_G 		0.0010351f
#define Gyro_Gr		0.0010653f
#define FILTER_NUM 	20

#define Kp 1.6f                        /* proportional gain governs rate of convergence to accelerometer/magnetometer */
#define Ki 0.001f                          /* integral gain governs rate of convergence of gyroscope biases */
#define halfT 0.001f

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    /* quaternion elements representing the estimated orientation */
float exInt = 0, eyInt = 0, ezInt = 0;    /* scaled integral error */

void Prepare_Data(int16_t *acc_in, int16_t *acc_out)
{
    static uint8_t 	filter_cnt = 0;
    static int16_t	ACC_X_BUF[FILTER_NUM], ACC_Y_BUF[FILTER_NUM], ACC_Z_BUF[FILTER_NUM];
    int32_t temp1 = 0, temp2 = 0, temp3 = 0;
    uint8_t i;

    ACC_X_BUF[filter_cnt] = acc_in[0];
    ACC_Y_BUF[filter_cnt] = acc_in[1];
    ACC_Z_BUF[filter_cnt] = acc_in[2];
    for(i = 0; i < FILTER_NUM; i++)
    {
        temp1 += ACC_X_BUF[i];
        temp2 += ACC_Y_BUF[i];
        temp3 += ACC_Z_BUF[i];
    }
    acc_out[0] = temp1 / FILTER_NUM;
    acc_out[1] = temp2 / FILTER_NUM;
    acc_out[2] = temp3 / FILTER_NUM;
    filter_cnt++;
    if(filter_cnt == FILTER_NUM)	filter_cnt = 0;
}

/*

angle[2] = yaw;
angle[0] = rol;
angle[1] = pit;

*/

void IMUupdate(float *gyr, int16_t *acc, float *angle)
{
    float ax = acc[0], ay = acc[1], az = acc[2];
    float gx = gyr[0], gy = gyr[1], gz = gyr[2];
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q1q1 = q1 * q1;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;

    if(ax * ay * az == 0)
        return;

    gx *= Gyro_Gr;
    gy *= Gyro_Gr;
    gz *= Gyro_Gr;

    norm = sqrt(ax * ax + ay * ay + az * az); 
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // estimated direction of gravity and flux (v and w)              
    vx = 2 * (q1q3 - q0q2);											
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors
    ex = (ay * vz - az * vy) ;                           					
    ey = (az * vx - ax * vz) ;
    ez = (ax * vy - ay * vx) ;

    exInt = exInt + ex * Ki;								 
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // adjusted gyroscope measurements
    gx = gx + Kp * ex + exInt;					   						
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;				   							

    // integrate quaternion rate and normalise						   
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    // normalise quaternion
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);

    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    angle[2] += gyr[2] * Gyro_G * 0.002f;

    angle[0] = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3; // pitch
    angle[1] = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3; // roll
}
