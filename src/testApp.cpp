/*
 <streams>
 
 <stream url="http://148.61.142.228/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 **<stream url="http://82.79.176.85:8081/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://81.8.151.136:88/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://216.8.159.21/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 **<stream url="http://kassertheatercam.montclair.edu/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://213.77.33.2:8080/axis-cgi/mjpg/video.cgi?resolution=320x240"/>
 <stream url="http://81.20.148.158/anony/mjpg.cgi"/>
 <stream url="http://173.167.157.229/anony/mjpg.cgi"/>
 <stream url="http://67.181.87.150/anony/mjpg.cgi"/>
 <stream url="http://72.227.87.110/anony/mjpg.cgi"/>
 <stream url="http://69.205.126.54/anony/mjpg.cgi"/>
 <stream url="http://173.196.179.29/anony/mjpg.cgi"/>
 <stream url="http://208.105.17.62/anony/mjpg.cgi"/>
 <stream url="http://67.208.104.218/anony/mjpg.cgi"/>
 <stream url="http://212.42.63.190/anony/mjpg.cgi"/>
 <stream url="http://94.246.211.222/anony/mjpg.cgi"/>
 <stream url="http://213.251.201.196/anony/mjpg.cgi"/>
 <stream url="http://208.100.33.174/anony/mjpg.cgi"/>
 <stream url="http://85.186.35.67/anony/mjpg.cgi"/>
 <stream url="http://98.189.188.232/anony/mjpg.cgi"/>
 <stream url="http://209.119.5.4/anony/mjpg.cgi"/>
 <stream url="http://24.155.150.53/anony/mjpg.cgi"/>
 <stream url="http://98.235.174.112/anony/mjpg.cgi"/>
 <stream url="http://88.170.122.125/anony/mjpg.cgi"/>
 -->
 </streams>*/

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
     
}



//------------------------------------------------------------------------------
void testApp::update() {
	// update the cameras
    grabber->update();
    
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(100);
    background.setLearningTime(70);
    background.setThresholdValue(60);
    
    
    int erodeNum = 5;
    int dilateNum = 5;

    
    
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

