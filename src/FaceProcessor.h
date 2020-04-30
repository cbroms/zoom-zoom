#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"

class FaceProcessor : public ofThread {

public:
	ofVideoGrabber grabber; // the cam
	ofxFaceTracker2 tracker; // the tracker

	// the current image frame
	ofImage image;

	// current points for face landmarks
	ofPoint center;
	ofPoint left;
	ofPoint right;
	ofPoint topMouth;
	ofPoint bottomMouth;
		
	void setup(int grabberId) {

		// ids range from 0-4 and reference the video inputs (4 virtual cams max)
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

					// grab and store the relevant image points
					center = landmarks.getImagePoint(29);
					left = landmarks.getImagePoint(0); // left side of head
					right = landmarks.getImagePoint(16); // right side of head
					topMouth = landmarks.getImagePoint(62);
					bottomMouth = landmarks.getImagePoint(66);
				}
				unlock();
			}
		}
		// done
	}
};
