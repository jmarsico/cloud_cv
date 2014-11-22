#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "IPVideoGrabber.h"
#include "ofxOpenCv.h"
#include "ofxHTTP.h"
#include "ofxOsc.h"

using ofx::Video::IPVideoGrabber;
using ofx::Video::SharedIPVideoGrabber;


// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
    
    //ofxCV declarations
	float threshold;
    
	ofxCv::ContourFinder contourFinder;
	bool bShowVideos;
    
    ofxCv::RunningBackground background;
    ofImage thresholded;
    ofImage edge;
    ofImage grabFrame;
    
    
    SharedIPVideoGrabber grabber;
    
    float* grabH;
    float* grabW;
    
    
    //variables for lights
    vector <int> brights;
    int numLEDs;
    int cellSize;
    
    ofx::HTTP::BasicIPVideoServer::SharedPtr server;
    
    ofxOscReceiver receiver;

    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];

    
};
