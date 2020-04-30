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
		int NUM_WINDOWS = 3;
		bool DEBUG_SINGLE_CAM = false;

		// the current draw mode
		int drawMode = 0;
		bool debugMode = false;
		
		// draw modes
		void drawMode0(); // debug
		void drawMode1(); // more movement = more video
		void drawMode2(); // more movement = less video
		void drawMode3(); // you talk, you're gone
		void drawMode4(); // toggle video on with your head
		void drawMode5(); // face circles
		void drawMode6(); // face circle tag


		// get default window position
		pair<int, int> getDefaultPosition(int i, int n);

		// the threads for processing each camera feed
		vector<unique_ptr<FaceProcessor>> processors;

		// information from processing 
		vector<ofImage> images;
		vector<ofPoint> points;
		vector<pair<ofPoint, ofPoint>> edges;
		vector<pair<ofPoint, ofPoint>> mouths;
		

		// draw mode stuff
		// mode 1
		vector<ofPoint> prevPoints;
		vector<int> currentAlpha;

		// mode 3
		vector<int> numIterations;

		// mode 4
		vector<bool> isOn;
		vector<bool> outOfTarget;

		//mode 5
		vector<float> prevDist;

		// mode 6
		vector<pair<ofPoint, float>> circles;
		int selected = 0;
		bool isIn = false;
		vector<int> allIn;
		bool won = false;
};
