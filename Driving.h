/*
 * Driving.h
 *
 *  Created on: Nov 18, 2023
 *      Author: dell
 */

#ifndef DRIVING_H_
#define DRIVING_H_

extern int Driving(int mode, int speed, int direction);

extern int32_t drivingSpeedLeft, drivingSpeedRight, drivingFwdBackLeft, drivingFwdBackRight;
extern int32_t drivingMaxSpeed;
extern int32_t drivingSpeedAdjustment;
extern int32_t drivingTurnAdjustment;

#endif /* DRIVING_H_ */
