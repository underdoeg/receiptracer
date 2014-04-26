/*
 *  ofxJoypad.cpp
 *  flipperScreenControllable
 *
 *  Created by Philip Whitfield on 5/11/10.
 *  Copyright 2010 undef.ch. All rights reserved.
 *
 */

#include "ofxJoypad.h"

int ofxJoypad::axisDeadZone = 2800;
int ofxJoypad::axisMaxVal = 32768;
int ofxJoypad::axisMinVal = -32766;


ofxJoypad::ofxJoypad(){
}

void ofxJoypad::setup(){
	ofAddListener(ofEvents().update, this, &ofxJoypad::update);
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), ""));

	try
	{
		//This never returns null.. it will raise an exception on errors
		inputManager = InputManager::createInputSystem(pl);

		inputManager->enableAddOnFactory(InputManager::AddOn_All);

		int numSticks = inputManager->getNumberOfDevices(OISJoyStick);
		for( int i = 0; i < numSticks; ++i )
		{
			JoyStick* joy= (JoyStick*)inputManager->createInputObject( OISJoyStick, true );
			joy->setEventCallback( this );
			stringstream msg;
			msg << "\n\nCreating Joystick " << i
			<< "\n\tName: " << joy->vendor()
			<< "\n\tAxes: " << joy->getNumberOfComponents(OIS_Axis)
			<< "\n\tSliders: " << joy->getNumberOfComponents(OIS_Slider)
			<< "\n\tPOV/HATs: " << joy->getNumberOfComponents(OIS_POV)
			<< "\n\tButtons: " << joy->getNumberOfComponents(OIS_Button)
			<< "\n\tVector3: " << joy->getNumberOfComponents(OIS_Vector3);
			ofLog(OF_LOG_NOTICE, msg.str());
			ofxPad pad;
			for(int i=0;i<joy->getNumberOfComponents(OIS_Axis);i++){
				pad.axisValAbs.push_back(0);
			}
			for(int i=0;i<joy->getNumberOfComponents(OIS_Button);i++){
				pad.btnsVal.push_back(false);
			}
			pad.joystick = joy;
			joysticks.push_back(pad);
		}
		if(numSticks==0){
			stringstream msg;
			msg << "NO JOYSTICKS CONNECTED" << std::endl;
			ofLog(OF_LOG_ERROR, msg.str());
		}
	}
	catch(OIS::Exception &ex)
	{
		stringstream msg;
		msg << "\nException raised on joystick creation: " << ex.eText << std::endl;
		ofLog(OF_LOG_ERROR, msg.str());
	}
};

ofxJoypad::~ofxJoypad(){

};

void ofxJoypad::update(ofEventArgs &args){
	for(int i=0;i<joysticks.size();i++){
		joysticks[i].joystick->capture();
	}
}

float ofxJoypad::getAxisNormalized(int val){
	return ofMap(val, axisMinVal, axisMaxVal, -1, 1);
}

float ofxJoypad::getAxisNormalizedA(int val){
	return ofMap(val, axisMinVal, axisMaxVal, 0, 1);
}

bool ofxJoypad::getButtonPressed(int btn, int pad){
	return joysticks[pad].btnsVal[btn];
}
int ofxJoypad::getAxisValue(int axis, int pad){
	return joysticks[pad].axisValAbs[axis];
}

int ofxJoypad::getNumPads(){
	return joysticks.size();
}

void ofxJoypad::addListener(ofxJoypadListener* listener){
	listeners.push_back(listener);
}

JoyStick* ofxJoypad::getOISPad(int num){
	return joysticks[num].joystick;
}

string ofxJoypad::getPadName(int num){
	if(getNumPads()<=num)
		return "";
	return joysticks[num].joystick->vendor();
}

JoyStick* ofxJoypad::getOISPad(string name){
	for(int i=0;i<joysticks.size();i++){
		ofxPad pad = joysticks[i];
		if(name.compare(pad.joystick->vendor())==0){
			return pad.joystick;
		}
	}
	return 0;
}

int ofxJoypad::idOfPad(const Object* obj){
	for(int i=0;i<joysticks.size();i++){
		if(joysticks[i].joystick->getID()== obj->getID())
			return i;
	}
	return -1;
}

bool ofxJoypad::buttonPressed( const JoyStickEvent &arg, int button ){
	int id = idOfPad(arg.device);
	joysticks[id].btnsVal[button] = false;
	for(int i=0;i<listeners.size();i++){
		listeners[i]->buttonReleased(id, button);
	}
	return true;
};
bool ofxJoypad::buttonReleased( const JoyStickEvent &arg, int button ){
	int id = idOfPad(arg.device);
	joysticks[id].btnsVal[button] = true;
	for(int i=0;i<listeners.size();i++){
		listeners[i]->buttonPressed(id, button);
	}
	return true;
};
bool ofxJoypad::axisMoved( const JoyStickEvent &arg, int axis ){
	int val = arg.state.mAxes[axis].abs;
	if( arg.state.mAxes[axis].abs < axisDeadZone && arg.state.mAxes[axis].abs > -axisDeadZone)
		val =  0;
	int id = idOfPad(arg.device);
	joysticks[id].axisValAbs[axis] = val;
	for(int i=0;i<listeners.size();i++){
		listeners[i]->axisMoved(id, axis, val , arg.state.mAxes[axis].rel);
	}
	return true;
};
bool ofxJoypad::povMoved( const JoyStickEvent &arg, int pov ){
	return true;
};
bool ofxJoypad::vector3Moved( const JoyStickEvent &arg, int index){
	return true;
};