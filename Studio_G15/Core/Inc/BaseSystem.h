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

typedef struct{
	uint32_t PositionZ;
	uint32_t PositionX;
	uint32_t Speed;
	uint32_t Acc;
	uint16_t Vacuum;
	uint16_t Gripper;
	uint16_t ShelveMode;
	int Shelve[5];
	uint16_t PointMode;
	uint16_t GoalPoint;
	uint16_t Pick;
	uint16_t Place;
}BaseStruct;
BaseStruct base = {0};



//------------ Function ------------//
void Heartbeat(){
	registerFrame[0x00].U16 = 22881;
}

void Routine(){
	if(registerFrame[0x00].U16 == 18537)
	{
		  registerFrame[0x04].U16 = 0b0000;
		  registerFrame[0x04].U16 = 0b0001;   //Gripper status 0b0010 = 0000 0000 0000 0010
		  registerFrame[0x10].U16 = 1;	//Z-axis status 0010 = 1
		  registerFrame[0x11].U16 = base.PositionZ	*10;	//Z-axis position
		  registerFrame[0x12].U16 = base.Speed		*10;		//Z-axis speed
		  registerFrame[0x13].U16 = base.Acc		*10;		//Z-axis acceleration
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
	if(registerFrame[0x01].U16 == 1){
		registerFrame[0x01].U16 = 0; 		//Z-axis reset BaseSystem status
		registerFrame[0x10].U16 = 1; 		//Z-axis update z-xis moving status to "Set Shelves"

		//Jogging for set shelve
		HAL_Delay(1500); 	//Delay for testing
		registerFrame[0x23].U16 = base.Shelve[0]*10; 	//0 = Position Shelve 1
		registerFrame[0x24].U16 = base.Shelve[1]*10;
		registerFrame[0x25].U16 = base.Shelve[2]*10;
		registerFrame[0x26].U16 = base.Shelve[3]*10;
		registerFrame[0x27].U16 = base.Shelve[4]*10;
//		if (base.ShelveMode == 0){
//			registerFrame[0x23].U16 = base.Shelve[0]*10; 	//0 = Position Shelve 1
//			registerFrame[0x24].U16 = base.Shelve[1]*10;
//			registerFrame[0x25].U16 = base.Shelve[2]*10;
//			registerFrame[0x26].U16 = base.Shelve[3]*10;
//			registerFrame[0x27].U16 = base.Shelve[4]*10;
//		}

		HAL_Delay(1500);	//Delay for testing

		// reset z-axis moving state after finish Set shelves
		//if (Finish Set shelves){
		registerFrame[0x10].U16 = 0;
		//}

	}
}

void SetGoalPoint(){
	if(registerFrame[0x01].U16 == 8){ //Point mode setup
		base.GoalPoint = (registerFrame[0x30].U16)/10 ; //Get Goal point from BaseSytem(Point Mode) that we pick/write After pressing Run Button
		 //Data form BaseSystem  = 10*Data in STM
	}
}

void RunPointMode(){
	if(registerFrame[0x01].U16 == 0b1000){ //Run point mode
		base.PointMode = 1;
		registerFrame[0x01].U16 = 0;		//0b0000 = 0 Reset Base System Status
		registerFrame[0x10].U16 = 16;		//0b0001 0000 = 16Update Z-axis Moving Status (Going to p
		HAL_Delay(3000); 					//Delay for testing
		registerFrame[0x10].U16 = 0;		//0b0000 = 0 Update Z-axis Moving Status (Finish)
	}
}

void SetHome(){
	if(registerFrame[0x01].U16 == 2){ //BaseSystem Status "Home"
		registerFrame[0x01].U16 = 0; //0b0000 = 0 Reset Base System Status
		registerFrame[0x10].U16 = 2; //0b0010 = 2 Update Z-axis moving status "Home"

		//Homing
		HAL_Delay(3000); //Delay for testing

		//if (Gripper at HomePoint){
		registerFrame[0x10].U16 = 0; // reset z-axis moving state after finish homing
		//}
		}
}

void SetPickPlace(){
	if(registerFrame[0x01].U16 == 4){ //0b0100 = 4 Pressing Run Jog mode
		base.Pick = registerFrame[0x21].U16 ; 	//Get Pick from BaseSystem
		base.Place = registerFrame[0x22].U16 ;	//Get Place from BaseSystem
		//ค่าที่ได้จะเรียงติดกัน ex.ถ้าเซ็ตค่าในUIชั้นแรกที่ต้อง Pick คือ ชั้น1-5 ตามลำดับ ค่าชั้นที่ต้องPick จะได้ 12345
	}
}

void RunJogMode(){
	if(registerFrame[0x01].U16 == 4){ //after pressing run button on Jogmode
		registerFrame[0x01].U16 = 0;

		//Loop{
		//Pick
		registerFrame[0x10].U16 = 4; // go pick state
		HAL_Delay(1500); //Delay for testing

		//Going to Pick from Shelve 5 round(Use PickOder to do task)
		//When finish Pick from round(i) shelve --> Go Place

		//Place
		registerFrame[0x10].U16 = 8; // go place state
		HAL_Delay(1500); //Delay for testing

		//Going to Place from Shelve 5 round(Use PlaceOder to do task)
		//When finish Place from round(i) shelve --> Return Pick
		//When Finish Place round 5 --> Out of Loop
		//}

		//if(All Pick&Place == Finish){
		registerFrame[0x10].U16 = 0; // after finish jogging
		//}
	}
}

#endif /* INC_BASESYSTEM_H_ */
