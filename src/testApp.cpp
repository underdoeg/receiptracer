#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofBackground(0, 0, 0);
	colors[FREE]=ofColor(0, 0, 0);
	colors[WALL]=ofColor(110,110,110);
	colors[OBSTACLE]=ofColor(150,150,150);
	ofSetFrameRate(60);

	gui.addTitle("SETTINGS");
	gui.addSlider("LINE FEED LENGTH", LINE_STEPS, 1, 100);
	gui.addSlider2d("OFFSET", drawOffset, 0, ofGetWidth(), 0, ofGetHeight());
	gui.addSlider("DRAW SIZE X", DRAW_X, 10, 50);
	gui.addToggle("RENDER TRACK", showTrack);
	gui.addSlider("DRAW SIZE Y", DRAW_Y, 1, 50);

	gui.loadFromXML();

	joypad.setup();
	joypad.addListener(this);
	car.setup();

	level = 10;
	score = 0;

	hasCrashed = false;

	noise_off=0;
	street_width_off=0;

	//
	ofSetLogLevel(OF_LOG_NOTICE);
	serial.enumerateDevices();
	serial.setup("/dev/ttyUSB0", 9600);

	onPause=true;

	onStart();

	font.loadFont("Andale Mono.ttf", 35);
	font_s.loadFont("Andale Mono.ttf", 15);

	//ofToggleFullscreen();

	sendCommand("\x1b\x40"); //Clear data in buffer and reset modes

	sendCommand("\x1b");//ESC
	sendCommand("3");
	serial.writeByte(24);

	elapsed = 0;
}

//--------------------------------------------------------------
void testApp::update() {

	//HIT TEST
	std::vector<ofPoint> carPos;


	//UPDATE CAR POSITION
	car.update();
	if(joypad.getNumPads()>0) {
		//car.x = ofxJoypad::getAxisNormalizedA(-joypad.getAxisValue(PS3_STICK_L_X))*LINE_WIDTH*DRAW_X;
		car.x+=ofxJoypad::getAxisNormalized(-joypad.getAxisValue(PS3_STICK_L_X))*5;
	}

	//NO FRAMELY UPDATES BEHIND THIS LINE
	if(ofGetFrameNum()%UPDATE_RATE!=0 || gui.isOn() || onPause)
		return;


	noise_off+=0.05;
	street_width_off+=0.01;
	street_width=ofNoise(street_width_off)*25;

	int wallL=ofNoise(noise_off)*30;
	int wallR=street_width+wallL;

	unsigned char* line = new unsigned char[LINE_WIDTH];
	for(int i=0; i<LINE_WIDTH; i++) {
		if(i<wallL) {
			line[i]=WALL;
		} else if(i>wallR) {
			line[i]=WALL;
		} else {
			line[i]=FREE;
		}
	}

	if( (int) ofRandom(level) < 2 || (wallR - wallL > 12 && (int) ofRandom(5) < 1) ) {
		cout << " obstacle " << endl;
		line[ (int) ofRandom(wallL, wallR) ] = OBSTACLE;
	}


	if(roundCounter>WAIT_ROUNDS) {
		UPDATE_RATE=5;
		if(checkCollision()) {
			cout << " crash! " << endl;
			onCrash();
			hasCrashed = true;
		}
	} else {
		UPDATE_RATE=5;
	}

	roundCounter++;

	lines.push_back(line);

	while(lines.size()>LINE_STEPS) {
		lines.pop_front();
	}
	//int elapsed = ofGetElapsedTimeMillis();

	if( ofGetElapsedTimeMillis() - elapsed > 3000) {
		elapsed = ofGetElapsedTimeMillis();
		level--;
	}

	score++;
	//if(serial.available()){
	serial.writeBytes(line, LINE_WIDTH);
	sendCommand("\n");
	//}
}

void testApp::buttonReleased(int pad, int btn) {
	if(btn==PS3_BTN_CROSS)
		togglePause();
	if(btn==PS3_BTN_TRIANGLE) {
		sendCommand("\n\n\n\n\n\n\n\n\n\n\n\x1d\x56\x01");
	}
}

int testApp::sendCommand(stringstream s) {
	return sendCommand(s.str());
}

int testApp::sendCommand(string s) {
	serial.writeBytes((unsigned char*)s.c_str(),s.length());
}

void testApp::onStart() {
	string boom = "\
    _____ _____ ___  ______ _____ \n\
    /  ___|_   _/ _ \\ | ___ \\_   _|\n\
    \\ `--.  | |/ /_\\ \\| |_/ / | |  \n\
    `--. \\ | ||  _  ||    /  | |  \n\
    /\\__/ / | || | | || |\\ \\  | |  \n\
    \\____/  \\_/\\_| |_/\\_| \\_| \\_/\n\
    ";
	sendCommand(boom);
	roundCounter=0;
	//onPause=false;
	hasCrashed=false;
	level=30;
	score=0;
	pointSize = 0;
}

void testApp::onCrash() {

	string boom = "\
    ______  _____  ________  ___  _ _ \n\
    | ___ \\|  _  ||  _  |  \\/  | | | |\n\
    | |_/ /| | | || | | | .  . | | | |\n\
    | ___ \\| | | || | | | |\\/| | | | |\n\
    | |_/ /\\ \\_/ /\\ \\_/ / |  | | |_|_|\n\
    \\____/  \\___/  \\___/\\_|  |_/ (_|_)\n\
    ";

	sendCommand(boom);
	sendCommand("TIME PLAYED: "+ofToString(ofGetElapsedTimef())+"\n");
	sendCommand("SCORE: "+ofToString(score)+"\n");
	sendCommand("LEVEL: "+ofToString(level)+"\n\n\n\n");
	sendCommand("\n\n\n\n");
	sendCommand("\n\n\n\n");
	//sendCommand("\x1d\x56\x01");//Full cut paper
	onPause=true;
}

//--------------------------------------------------------------
void testApp::draw() {
	ofEnableAlphaBlending();
	ofColor col;
	ofPushMatrix();
	ofTranslate(drawOffset);
	ofRotate(180);
	if(showTrack) {
		for(int i=0; i<lines.size(); i++) {
			for(int j=0; j<LINE_WIDTH; j++) {
				char l=lines[i][j];
				ofSetColor(colors[l]);
				ofRect(j*DRAW_X, i*DRAW_Y, DRAW_X, DRAW_Y);
			}
		}
	}
	ofSetColor(255, 255, 255);
	font_s.drawString("LEVEL: "+ofToString(level), 10, -10);
	font_s.drawString("SCORE: "+ofToString(score), 10, -30);
	if(!hasCrashed) {
		car.draw();
	} else {

		glEnable(GL_POINT_SMOOTH);
		if(pointSize < 24 && ofRandom(2) < 1) {
			pointSize++;
		}

		ofEnableAlphaBlending();
		ofSetColor(255, 0, 0, 255 - (pointSize/24 * 255));
		glPointSize(pointSize);
		glBegin(GL_POINTS);

		for( int i = 0; i<150; i++) {
			glVertex3f(car.x + (cos(i) * ofRandom(pointSize * 40)), car.y + (sin(i) * ofRandom(pointSize * 40)), 1);
		}
		glEnd();
		glDisable(GL_POINT_SMOOTH);
		ofDisableAlphaBlending();
		font.drawString("BOOM!!!", car.x, car.y);
	}
	if(roundCounter<WAIT_ROUNDS) { //WATING FOR THE GAME TO ACTUALLY START
		//ofRect(car.x, car.y, 5, 5);
		font.drawString("WAIT...", car.x, car.y);
	}
	ofPopMatrix();
	gui.draw();
}

bool testApp::checkCollision() {
	if(lines.size() > 0) {
		int lObs = 0;
		while(lines[0][lObs] != FREE) lObs++;

		int rObs = 42;
		while(lines[0][rObs] != FREE) rObs--;

		int loc = (int)(car.x+(DRAW_X/2))/DRAW_X;

		if(loc < lObs || loc > rObs || lines[0][loc] != FREE) {
			//cout << (car.x / DRAW_X) << " " << lObs << " " << rObs << endl;
			return true;
		}
	}

	return false;

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key==OF_KEY_LEFT)
		car.x+=DRAW_X;
	if(key==OF_KEY_RIGHT)
		car.x-=DRAW_X;

	if(key == OF_KEY_BACKSPACE) {
		sendCommand("\x1d\x56\x01");
		onCrash();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
	if(key==' ')
		gui.toggleDraw();
	if(key=='p')
		togglePause();
	if(key=='f')
		ofToggleFullscreen();
}

void testApp::togglePause() {
	onPause=!onPause;
	if(hasCrashed)
		onStart();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}
