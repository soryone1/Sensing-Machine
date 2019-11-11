#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // setup the dmx
    dmx.connect("tty.usbserial-ENY4T0J3"); // use the name
    
    kinect.init();
    kinect.open();
    kinect.setRegistration(true);
    
    // set the window size.
    ofSetWindowShape(640, 720);
    
    // Allocate the frame buffer.
    ofFboSettings settings;
    settings.width = 640;
    settings.height = 360;
    canvasFbo.allocate(settings);
    visionFbo.allocate(settings);
    
    // set up the parameter for Fbo.
    tintColor.set("Tint Color", ofColor(0, 255, 0));
    clearFbo.set("Clear Background", false);
    
    // Allocate the threshold image.
    thresholdImg.allocate(640, 360, OF_IMAGE_GRAYSCALE);
    
    // set up parameters for distance.
    nearThreshold.set("Near Threshold", 0.005f, 0.0f, 0.1f);
    farThreshold.set("Far Threshold", 0.02f, 0.0f, 0.1f);
    
    // find contour using the target color.
    contourFinder.setUseTargetColor(true);
    
    // set up parameters for contourfinder.
    colortarget.set("Color Target",ofColor(255));
    colorOffset.set("color Offset", 10, 0, 255);
    minArea.set("Min Area", 1000, 0, 5000);
    maxArea.set("Max Area", 70000, 0, 100000);
    blurAmount.set("Blur Amount",30,0,100);
    erodeIterations.set("Erode Iterations",0, 0, 10);
    persistance.set("Persistance", 15, 0 , 60);
    maxDistance.set("maxDistance", 64, 0, 640);
    
    // parameters for light.
    leftLightOn.set(false);
    rightLightOn.set(false);
    
    // set up the gui panel.
    guiPanel.setup("SkateTracking", "setting.json");
    guiPanel.add(nearThreshold);
    guiPanel.add(farThreshold);
    guiPanel.add(colorOffset);
    guiPanel.add(minArea);
    guiPanel.add(maxArea);
    guiPanel.add(blurAmount);
    guiPanel.add(erodeIterations);
    guiPanel.add(persistance);
    guiPanel.add(maxDistance);
    guiPanel.add(tintColor);
    guiPanel.add(clearFbo);
    
}

//--------------------------------------------------------------
void ofApp::update(){
        
        // update the kinect.
        kinect.update();
    
        // get the distance at the mouse position.
        float disAtMouse = kinect.getDistanceAt(ofGetMouseX(), ofGetMouseY());
        ofDrawBitmapStringHighlight(ofToString(disAtMouse,3), ofGetMouseX(),ofGetMouseY());
        
        // get the rawdepth value.
        ofFloatPixels rawDepthPix = kinect.getRawDepthPixels();
        
        // threshold the image.
        ofFloatPixels thresholdNear, thresholdFar, thresholdResult;
        ofxCv::threshold(rawDepthPix, thresholdNear, nearThreshold);
        ofxCv::threshold(rawDepthPix, thresholdFar, farThreshold, true);
        ofxCv::bitwise_and(thresholdNear, thresholdFar, thresholdResult);
        
        // get the threshold image.
        thresholdImg.setFromPixels(thresholdResult);
        
        // add blur to the threshold image.
       if(blurAmount > 0){
        ofxCv::medianBlur(thresholdImg, blurAmount);
       }
    
        // add erode iteration.
       if (erodeIterations > 0) {
        ofxCv::erode(thresholdImg, erodeIterations.get());
       }
        // update the threshold image.
        thresholdImg.update();
    
    // draw the threshold background.
    canvasFbo.begin();
    {
        if (clearFbo) {
            ofBackground(0);
            clearFbo = false;
        }
        if(ofGetMousePressed() && !guiPanel.getShape().inside(ofGetMouseX(), ofGetMouseY())){
        // draw a circle if the mouse is pressed and not over the gui panel.
        ofSetColor(255);
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
        }
    }
        canvasFbo.end();
        
    // download the FBO data as pixels.
    canvasFbo.readToPixels(canvasPiexls);
    
        // start the contourfinder
        contourFinder.setTargetColor(colortarget);
        contourFinder.setThreshold(colorOffset);
        contourFinder.setMinArea(minArea);
        contourFinder.setMaxArea(maxArea);
        contourFinder.getTracker().setPersistence(persistance);
        contourFinder.getTracker().setMaximumDistance(maxDistance);
        
    
        contourFinder.findContours(thresholdImg);
    
       for (int i =0 ; i < contourFinder.size(); i++){
        ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
        ofLog() << center.x;
           
           if(center.x > 0 && center.x< 320){
               leftLightOn = true;
               rightLightOn = false;
               
               // setup the light to red
               dmx.setLevel(1, 255);
               dmx.setLevel(2, 0);
               dmx.setLevel(3, 0);
               dmx.update();
           }
           
           if (center.x > 320 && center.x < 640) {
               leftLightOn = false;
               rightLightOn = true;
               
               // setup the light to blue
               dmx.setLevel(1, 0);
               dmx.setLevel(2, 0);
               dmx.setLevel(3, 255);
               dmx.update();
           }
               
       }
    
    }

//--------------------------------------------------------------

void ofApp::draw(){
    
    // draw the thresdold image
    thresholdImg.draw(0,0);
    contourFinder.draw();
    
    // draw the original image
    kinect.getDepthTexture().draw(0,360);
    
    // draw the canvas above with no tint.
    ofSetColor(255);
    canvasFbo.draw(0,0);
    
    // draw the canvas below with a tint color.
    ofSetColor(tintColor);
    visionFbo.draw(0,360);
    
//    if (leftLightOn) {
//        ofDrawCircle(30, 360, 20);
//    }
//
//    if (rightLightOn) {
//        ofDrawCircle(610, 360, 20);
//    }
    
    // draw the gui panel
    guiPanel.draw();
}

void ofApp::keyPressed(int key){
    if (key == ' ') {
        clearFbo = true;
    }
}
