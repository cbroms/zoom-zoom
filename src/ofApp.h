#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "FaceProcessor.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);

		// number of vitual cameras (min 1, max 4)
		int NUM_WINDOWS = 2;

		// the current draw mode
		int drawMode = 1;
		
		// draw modes
		void drawMode1();
		void drawMode2();

		// get default window position
		pair<int, int> getDefaultPosition(int i);

		// the threads for processing each camera feed
		vector<unique_ptr<FaceProcessor>> processors;

		// information from processing 
		vector<unique_ptr<ofImage>> images;
		vector<unique_ptr<ofPoint>> points;
};
