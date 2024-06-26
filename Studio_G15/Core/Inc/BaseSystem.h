/*
 * Basesystem.h
 *
 *  Created on: Apr 30, 2024
 *      Author: porpo
 */

#ifndef INC_BASESYSTEM_H_
#define INC_BASESYSTEM_H_

#include <ModBusRTU.h>

//------------ PV ------------//

ModbusHandleTypedef hmodbus;
u16u8_t registerFrame[200];
int caseF;
typedef struct{
	uint16_t bStatus;
	uint32_t PositionZ;
	uint32_t PositionX;
	uint32_t Speed;
	uint32_t Acc;
	uint16_t Vacuum;
	uint16_t Gripper;
	uint16_t ShelveMode; 	//for ps2
	int Shelve[5];
//	uint16_t PointMode;
	uint16_t GoalPoint;
	uint16_t Pick;
	uint16_t Place;
	uint16_t bS;
	uint16_t vS;
	uint16_t gmS;
	uint16_t gmaS;
	uint16_t zmS;
// for testing
	uint16_t sw;
	uint16_t swp;
	uint16_t sh;
	uint16_t sp;

}BaseStruct;
BaseStruct base = {0};



//------------ Function ------------//
void Reset(){
	registerFrame[0x01].U16 = 0;
	registerFrame[0x10].U16 = 0;
}
void easyCase(){
	base.bS = registerFrame[0x01].U16;
	base.vS = registerFrame[0x02].U16;
	base.gmS = registerFrame[0x03].U16;
//	base.gmaS = registerFrame[0x04].U16;
	base.zmS = registerFrame[0x10].U16;
}
void Heartbeat(){
	registerFrame[0x00].U16 = 22881;
}

void Routine(){
	if(registerFrame[0x00].U16 == 18537)
	{
		//Gripper 0x04 not sure!?!?
//		  registerFrame[0x04].U16 = 0b0000;
//		  registerFrame[0x04].U16 = 0b0001;   //Gripper status 0b0010 = 0000 0000 0000 0010
		registerFrame[0x10].U16 = base.bStatus;	//Z-axis status 0010 = 1
		registerFrame[0x11].U16 = base.PositionZ	*10;	//Z-axis position
		registerFrame[0x12].U16 = base.Speed		*10;		//Z-axis speed
		registerFrame[0x13].U16 = base.Acc			*10;		//Z-axis acceleration
		registerFrame[0x40].U16 = base.PositionX	*10;	//X-axis position
	}
}

void Vacuum(){
	if(registerFrame[0x02].U16 == 0b0000){
		base.Vacuum = 0;			//Vacuum status: Off
	}
	else if(registerFrame[0x02].U16 == 0b0001){
		base.Vacuum = 1;			//Vacuum status: On
	}
}

void GripperMovement(){
	if(registerFrame[0x03].U16 == 0b0000){
		base.Gripper = 0;			//Gripper Movement: Backward
	}
	else if(registerFrame[0x03].U16 == 0b0001){
		base.Gripper = 1;			//Gripper Movement: Forward
	}
}

void SetShelves(){
	registerFrame[0x10].U16 = 1; 		//Z-axis update z-xis moving status to "Set Shelves"
	// save Position = floor[0] for moving to that position
	// set shelves 1 = 100 mm shelves 2 = 250 mm
	registerFrame[0x23].U16 = 10 *10; 	//0 = Position Shelve 1
	registerFrame[0x24].U16 = 20 *10;
	registerFrame[0x25].U16 = 30 *10;
	registerFrame[0x26].U16 = 40 *10;
	registerFrame[0x27].U16 = 50 *10;
	//finish
	if (base.sw == 1){
		base.bStatus = 0;
		registerFrame[0x01].U16 = base.bStatus;
		registerFrame[0x10].U16 = 0;
		base.sw = 0;
	}
}

void RunPoint(){
	base.GoalPoint = (registerFrame[0x30].U16)/10; //Get Goal point from BaseSytem(Point Mode) that we pick/write After pressing Run Button
	registerFrame[0x10].U16 = 16;
	//finish
	if(base.swp == 1){
		base.bStatus = 0;
		registerFrame[0x01].U16 = base.bStatus;
		registerFrame[0x10].U16 = 0;
		base.swp = 0;
	}
}

void SetHome(){
	registerFrame[0x10].U16 = 2;
	if(base.sh == 1){
		base.bStatus = 0;
		registerFrame[0x01].U16 = base.bStatus;
		registerFrame[0x10].U16 = 0;
		base.sh = 0;
	}
}

void RunJog(){
	base.Pick = registerFrame[0x21].U16 ; 	//Get Pick from BaseSystem
	base.Place = registerFrame[0x22].U16 ;	//Get Place from BaseSystem
	registerFrame[0x10].U16 = 4;
	registerFrame[0x10].U16 = 8;
	//pick place 5 time
	if(base.sp == 1){
		base.bStatus = 0;
		registerFrame[0x01].U16 = base.bStatus;
		registerFrame[0x10].U16 = 0;
		base.sp = 0;
	}
}

#endif /* INC_BASESYSTEM_H_ */
