

#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
    ofLog() << "starting setup";
	ofSetLogLevel(OF_LOG_VERBOSE);
    //ofSetVerticalSync(true);
	//ofBackground(100);
    
    
    //initialize connection

    //grabber->setUseTexture(false);
    ofLog() << "grabber set usetext";
    grabber = IPVideoGrabber::makeShared();
    ofLog() << "makeShared";
    
    while(grabber->isConnected()==false)
    {
        ofLog() << "connecting...";
        grabber->setCameraName("cam1");
        grabber->setURI("http://216.8.159.21/axis-cgi/mjpg/video.cgi?resolution=320x240");
        grabber->setUsername("admin");
        grabber->setPassword("");
        grabber->connect();
        
        
                        //http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240"
    }
    
    ofLog() << "connection made!";
    ofLog() << "width: " << grabber->getWidth();
    ofLog() << "height: " << grabber->getHeight();
    
    thresholded.setUseTexture(false);
    thresholded.allocate(grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
	
    grabFrame.setUseTexture(false);
    grabFrame.allocate(grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
	
    contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(100);
	contourFinder.setThreshold(200);
    contourFinder.setAutoThreshold(false);
	 //wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(50);
	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(32);
    
    background.setLearningTime(50);
    background.setThresholdValue(200);

    
    
    ofx::HTTP::BasicIPVideoServerSettings settings;
    
    // Many other settings are available.
    settings.setPort(7890);
    server = ofx::HTTP::BasicIPVideoServer::makeShared(settings);
    
    // Start the server.
    server->start();

    //osc stuff
    receiver.setup(PORT);
    current_msg_string = 0;

    ofLog() << "ending setup";
    
    erodeNum = 5;
    dilateNum = 5;
    threshVal = 60;
    
}



//------------------------------------------------------------------------------
void testApp::update() {
	//check for new OSC messages
    // hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}
    
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        if(m.getAddress() == "/1/threshold")
        {
            ofLog() << "threshold: " << m.getArgAsFloat(0);
            threshVal = ofMap(m.getArgAsFloat(0), 0, 1, 0, 255);
        }
		else if(m.getAddress() == "/1/erode")
        {
            ofLog() << "erode: " << m.getArgAsFloat(0);
            erodeNum = ofMap(m.getArgAsFloat(0), 0, 1, 0, 255);
        }
        else if(m.getAddress() == "/1/dilate")
        {
            ofLog() << "dilate: " << m.getArgAsFloat(0);
            dilateNum = ofMap(m.getArgAsFloat(0), 0, 1, 0, 255);
        }
		
        else
        {
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}
        
	}

    
    
    
    
    // update the cameras
    grabber->update();
    
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(100);
    background.setLearningTime(70);
    background.setThresholdValue(60);
    
    


    
    
    grabFrame.setFromPixels(grabber->getPixels(), grabber->getWidth(), grabber->getHeight(), OF_IMAGE_COLOR);
    grabFrame.resize(384, 288);
    
    
    //get the thresholded frame from background function
    background.update(grabFrame, thresholded);
    thresholded.update();
    
    //manipulate thresholded image before contour finder
    dilate(thresholded, dilateNum);
    erode(thresholded, erodeNum);
    
    
    contourFinder.findContours(thresholded);
    ofLog() << "number contours: " << contourFinder.size();
    
    std::stringstream ss;
    
    ss << "Num clients connected: ";
    ss << server->getNumConnections();
    
    ofLog() << ofGetFrameRate();
    ofLog() << ss.str();
  
}


//------------------------------------------------------------------------------
void testApp::draw() {
    
    // If the frame is new, then send it to the server to be broadcast.
   
        // This can be any kind of pixels.
        server->send(thresholded.getPixelsRef());
    

    
    
}


//------------------------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == ' ')
    {
        bShowVideos = !bShowVideos;
    }
}


//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

