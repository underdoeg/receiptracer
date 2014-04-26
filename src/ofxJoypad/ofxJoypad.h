/*
 *  ofxJoypad.h
 *  flipperScreenControllable
 *
 *  Created by Philip Whitfield on 5/11/10.
 *  Copyright 2010 undef.ch. All rights reserved.
 *
 */

#ifndef _CLASS_ofxJoypad_
#define _CLASS_ofxJoypad_

#include "ofMain.h"
#include "OIS.h"

enum PS3 {
	PS3_STICK_L_X = 0,
	PS3_STICK_L_Y = 1,
	PS3_STICK_R_X = 2,
	PS3_STICK_R_Y = 3,
	PS3_BTN_CROSS = 14,
	PS3_BTN_CIRCLE = 13,
	PS3_BTN_SQUARE = 15,
	PS3_BTN_TRIANGLE = 12,
	PS3_BTN_R1 = 11,
	PS3_BTN_R2 = 9,
	PS3_BTN_L1 = 10,
	PS3_BTN_L2 = 8,
	PS3_BTN_SELECT = 0,
	PS3_BTN_START = 3,
	PS3_BTN_STICK_R = 2,
	PS3_BTN_STICK_L = 1,
	PS3_DIR_RIGHT = 5,
	PS3_DIR_UP = 4,
	PS3_DIR_LEFT = 7,
	PS3_DIR_DOWN = 6
};

#define XBOX_NAME "Microsoft Wireless 360 Controller"

enum XBOX{
	XB_STICK_L_X = 2,
	XB_STICK_L_Y = 3,
	XB_STICK_R_X = 4,
	XB_STICK_R_Y = 5,
	XB_STICK_LT = 0,
	XB_STICK_RT = 1,
	XB_BTN_A = 11,
	XB_BTN_B = 12,
	XB_BTN_X = 13,
	XB_BTN_Y = 14,
	XB_BTN_BACK = 5,
	XB_BTN_START = 4,
	XB_BTN_LOGO = 10,
	XB_BTN_LB = 8,
	XB_BTN_RB = 9,
	XB_BTN_STICK_L = 6,
	XB_BTN_STICK_R = 7,
	XB_DIR_UP = 0,
	XB_DIR_DOWN = 1,
	XB_DIR_LEFT = 2,
	XB_DIR_RIGHT = 3
};

using namespace OIS;

class ofxJoypadListener{
public:
	virtual void buttonPressed(int pad, int btn){};
	virtual void buttonReleased(int pad, int btn){};
	virtual void axisMoved(int pad, int axis, int valAbs, int valRel){};
	virtual void povMoved(int pad, int pov, int value){};
};

struct ofxPad{
	JoyStick* joystick;
	std::vector<int> axisValAbs;
	//std::vector<int> axisValRel;
	std::vector<bool> btnsVal;
};

class ofxJoypad : public  JoyStickListener{
public:
	ofxJoypad();
	~ofxJoypad();
	void setup();
	void addListener(ofxJoypadListener* listener);
	int getNumPads();
	string getPadName(int num);
	JoyStick* getOISPad(int num);
	JoyStick* getOISPad(string name);
	
	void update(ofEventArgs &arg);
	
	static float getAxisNormalized(int val); //values between -1,1
	static float getAxisNormalizedA(int val); //values between 0,1
	
	bool getButtonPressed(int btn, int pad=0); //does currently not work, I'm not sure why
	int getAxisValue(int axis, int pad=0); //					"
	
	bool buttonPressed( const JoyStickEvent &arg, int button );
	bool buttonReleased( const JoyStickEvent &arg, int button );
	bool axisMoved( const JoyStickEvent &arg, int axis );
	bool povMoved( const JoyStickEvent &arg, int pov );
	bool vector3Moved( const JoyStickEvent &arg, int index);
	
	static int axisDeadZone;
	static int axisMaxVal;
	static int axisMinVal;
protected:
private:
	int idOfPad(const Object* obj);
	
	std::vector<ofxPad> joysticks;
	std::vector<ofxJoypadListener*> listeners;
	InputManager* inputManager;
};

#endif