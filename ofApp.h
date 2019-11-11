#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxDmx.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void keyPressed(int key);
    
    // set up the kinect
    ofxKinect kinect;
    
    // setup the dmx
    ofxDmx dmx;
    
    ofParameter<int> chan1;
    ofParameter<int> chan2;
    ofParameter<int> chan3;
    
    // setup the contourFinder
    ofImage  thresholdImg;
    ofParameter<float> nearThreshold;
    ofParameter<float> farThreshold;
    
    ofxCv::ContourFinder contourFinder;
    
    ofParameter<ofColor> colortarget;
    ofParameter<int> colorOffset;
    
    ofParameter<int> minArea;
    ofParameter<int> maxArea;
    
    ofParameter<int> blurAmount;
    ofParameter<int> erodeIterations;
    
    ofParameter<int> persistance;
    ofParameter<float> maxDistance;
    
    // setup the Fbo
    ofFbo canvasFbo;
    ofFbo visionFbo;
    ofPixels canvasPiexls;
    ofParameter<ofColor> tintColor;
    ofParameter<bool> clearFbo;
    
    // setup the visual output
    ofParameter<bool> leftLightOn;
    ofParameter<bool> rightLightOn;
    
    // setup the gui
    ofxPanel guiPanel;
		
};
