#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"

class FaceProcessor : public ofThread {

public:
	ofVideoGrabber grabber; // the cam
	ofxFaceTracker2 tracker; // the tracker
	ofImage image;
	ofPoint center;

		
	void setup(int grabberId) {

		grabber.setDeviceID(grabberId);
		grabber.setup(1920, 1080);
		tracker.setup();
	}


		// the thread function
	void FaceProcessor::threadedFunction() {

		// start

		while (isThreadRunning()) {

			grabber.update();

			if (grabber.isFrameNew()) {

				lock();

				// update the tracker
				tracker.update(grabber);

		
				// set the center point
				for (auto face : tracker.getInstances()) {

					face.loadPoseMatrix();
					ofxFaceTracker2Landmarks landmarks = face.getLandmarks();
					center = landmarks.getImagePoint(22);
				}
				unlock();
			
			}
		}

		// done
	}
};
