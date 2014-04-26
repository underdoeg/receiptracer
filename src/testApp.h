#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxJoypad.h"
#define LINE_WIDTH 42
#define WAIT_ROUNDS 50
static int LINE_STEPS = 20;
static float DRAW_Y = 25.f;
static float DRAW_X = 10.f;

static char WALL = (char)2482;
static char OBSTACLE = (char)9947;
static char FREE = ' ';
static int UPDATE_RATE = 5;


class Car: public ofRectangle{
public:
	void setup(){
		img.loadImage("raceCar.png");
        update();
        x=DRAW_X*LINE_WIDTH*.5;
	} 
    
    void update(){
		scale=DRAW_X/(float)img.width;
		width=DRAW_X;
		height=DRAW_Y;
	}
	
	void draw(){
		ofPushMatrix();
		ofTranslate(x, y);
		ofScale(scale, scale);
		img.draw(0, 0);
		ofPopMatrix();
	}
	float scale;
	ofImage img;
};

class testApp : public ofBaseApp, public ofxJoypadListener{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    void onStart();
    void onCrash();
	
	void togglePause();
	
    void buttonReleased(int pad, int btn);
    
	std::deque<unsigned char*> lines;
	std::map<unsigned char, ofColor> colors;
	
	ofxJoypad joypad;
	Car car;
	
	float noise_off;
	float street_width;
	float street_width_off;
	
	bool checkCollision();
	
	bool hasCrashed;
	
	
	int level;
    
    ofSerial serial;
    int sendCommand(stringstream s);
    int sendCommand(string s);
    
    int score;
	
	int roundCounter;
    
    bool showTrack;
    
    bool onPause;
    
    ofTrueTypeFont font;
    ofTrueTypeFont font_s;
	
	ofPoint drawOffset;
    float pointSize;
    int elapsed;
};

#endif
