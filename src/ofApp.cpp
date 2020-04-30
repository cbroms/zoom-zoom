#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	

	// create threads
	for (int i = 0; i < NUM_WINDOWS; i++) {

		processors.push_back(move(unique_ptr<FaceProcessor>(new FaceProcessor)));

		// if debugging with one camera, set up multiple instances of the same
		if (DEBUG_SINGLE_CAM) processors.back()->setup(0);
		else processors.back()->setup(i);

		processors.back()->startThread(true);

		images.push_back(ofImage());
		points.push_back(ofPoint(0, 0));
		edges.push_back(make_pair(ofPoint(0, 0), ofPoint(0, 0)));
		mouths.push_back(make_pair(ofPoint(0, 0), ofPoint(0, 0)));

		circles.push_back(make_pair(ofPoint(200, 200), 40));

		isOn.push_back(true);
		outOfTarget.push_back(true);
		numIterations.push_back(0);
		prevPoints.push_back(ofPoint(0, 0));
		prevDist.push_back(0);
		currentAlpha.push_back(0);
	}

	if (DEBUG_SINGLE_CAM) {
		circles.push_back(make_pair(ofPoint(200, 200), 40));
		allIn.push_back(1);
	}

	ofSetFrameRate(20);
}

//--------------------------------------------------------------
void ofApp::update(){

	for (int i = 0; i < processors.size(); i++) {

		processors[i]->lock();

		// set the image from the grabber frame
		images[i].setFromPixels(processors[i]->grabber.getPixels());


		// set the center point 
		points[i].x = processors[i]->center.x / 2;
		points[i].y = processors[i]->center.y / 2;

		// set the edge points 
		edges[i] = make_pair(processors[i]->left / 2, processors[i]->right / 2);

		// set the mouth points 
		mouths[i] = make_pair(processors[i]->topMouth / 2, processors[i]->bottomMouth / 2);

		// done
		processors[i]->unlock();
	}
}

//--------------------------------------------------------------
pair<int, int> ofApp::getDefaultPosition(int i, int n) {
	int x = 10;
	int y = 10;

	if (i >= 2) {
		y = 560;
	}

	if (i == 1 || i == 3) {
		x = 980;
	}
	else if (i == 2 && n == 3) {
		x = 500;
	}

	return make_pair(x, y);
}

//--------------------------------------------------------------
// basic debug mode, nothing special happening
void ofApp::drawMode0() {

	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i, processors.size());
		images[i].draw(pos.first, pos.second, 960, 540);

		//processors[i]->lock();
		//processors[i]->tracker.drawDebug();
		//processors[i]->unlock();

		ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
		ofDrawLine(edges[i].first, edges[i].second);
		ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), pos.first + 5, pos.second + 15);
	}

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 5, ofGetHeight() - 5);
}

//--------------------------------------------------------------
// in this mode, each frame begins to fade when there's not enough 
// movement; shaking makes the video become visible again
void ofApp::drawMode1() {

	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i, processors.size());

		// get the previous point and compare distance
		if (prevPoints[i] != points[i]) {
			ofPoint prevP = prevPoints[i];
			ofPoint newP = points[i];
			float dist = sqrt(pow(prevP.x - newP.x, 2) + pow(prevP.y - newP.y, 2));

			// if distance is great enough, make color more transparent 
			if (dist > 25 && currentAlpha[i] >= 5) {
				currentAlpha[i] = currentAlpha[i] - 5;
			}
		}
		else {
			// make color less transparent
			if (currentAlpha[i] <= 250) currentAlpha[i] = currentAlpha[i] + 5;
		}

		prevPoints[i] = points[i];

		images[i].draw(pos.first, pos.second, 960, 540);

		// faking image transparency by drawing over the frame 
		ofEnableAlphaBlending();
		ofSetColor(255, currentAlpha[i]);
		ofDrawRectangle(pos.first, pos.second, 960, 540);
		ofDisableAlphaBlending();

		if (debugMode) {
			ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), pos.first + 5, pos.second + 15);
			ofDrawBitmapStringHighlight("Alpha: " +  ofToString(currentAlpha[i]), pos.first + 5, pos.second + 35);
		}
	}
}

//--------------------------------------------------------------
// this is the opposite of mode 1; a lack of movement means full 
// video, lots means none
void ofApp::drawMode2() {

	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i, processors.size());

		// get the previous point and compare distance
		if (prevPoints[i] != points[i]) {
			ofPoint prevP = prevPoints[i];
			ofPoint newP = points[i];
			float dist = sqrt(pow(prevP.x - newP.x, 2) + pow(prevP.y - newP.y, 2));

			// if distance is great enough, make color less transparent 
			if (dist > 25 && currentAlpha[i] + 10 <= 255) {
				currentAlpha[i] = currentAlpha[i] + 10;
			}
		}
		else {
			// make color more transparent
			if (currentAlpha[i] >= 5) currentAlpha[i] = currentAlpha[i] - 5;
		}

		prevPoints[i] = points[i];

		images[i].draw(pos.first, pos.second, 960, 540);

		// faking image transparency by drawing over the frame 
		ofEnableAlphaBlending();
		ofSetColor(255, currentAlpha[i]);
		ofDrawRectangle(pos.first, pos.second, 960, 540);
		ofDisableAlphaBlending();

		if (debugMode) {
			ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), pos.first + 5, pos.second + 15);
			ofDrawBitmapStringHighlight("Alpha: " + ofToString(currentAlpha[i]), pos.first + 5, pos.second + 35);
		}
	}
}

//--------------------------------------------------------------
// remove the video of the person talking 
void ofApp::drawMode3() {
	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i, processors.size());

		float alpha = 0;
		float dist = sqrt(pow(mouths[i].second.x - mouths[i].first.x, 2) + pow(mouths[i].second.y - mouths[i].first.y, 2));

		// check if the mouth is open or moving and start incrementing the counter
		// when the counter gets above the interval, block out the video
		// do the same thing to establish when its time to bring the video back
		if (dist > 5 || fabs(dist - prevDist[i]) > 1) {
			
			if (currentAlpha[i] != 255 && numIterations[i] > 6) {
				alpha = 255;
				numIterations[i] = 0;
			}
			else if (currentAlpha[i] == 255) {
				alpha = 255;
				numIterations[i] = 0;
			}
			else {
				numIterations[i] += 1;
			}
		}
		else if (currentAlpha[i] == 255) {
			if (numIterations[i] > 4) {
				numIterations[i] = 0;
			}
			else {
				numIterations[i] += 1;
				alpha = 255;
			}
		}
		else {
			numIterations[i] = 0;
		}
		currentAlpha[i] = alpha;
		prevDist[i] = dist;


		images[i].draw(pos.first, pos.second, 960, 540);

		// faking image transparency by drawing over the frame 
		ofEnableAlphaBlending();
		ofSetColor(255, alpha);
		ofDrawRectangle(pos.first, pos.second, 960, 540);
		ofDisableAlphaBlending();

		if (debugMode) {
			ofDrawLine(mouths[i].first, mouths[i].second);
			ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), pos.first + 5, pos.second + 15);
			ofDrawBitmapStringHighlight("Alpha: " + ofToString(currentAlpha[i]), pos.first + 5, pos.second + 35);
			ofDrawBitmapStringHighlight("Num Iterations: " + ofToString(numIterations[i]), pos.first + 5, pos.second + 55);
		}
	}
}

//--------------------------------------------------------------
// hit head against the top top toggle the video on/off
void ofApp::drawMode4() {
	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i, processors.size());

		if (points[i].y < 100) {
			if (outOfTarget[i]) {
				isOn[i] = !isOn[i];
			}
			outOfTarget[i] = false;
		}
		else {
			outOfTarget[i] = true;
		}

		images[i].draw(pos.first, pos.second, 960, 540);

		int alpha = 255;
		if (isOn[i]) alpha = 0;

		// faking image transparency by drawing over the frame 
		ofEnableAlphaBlending();
		ofSetColor(255, alpha);
		ofDrawRectangle(pos.first, pos.second, 960, 540);
		ofDisableAlphaBlending();

		if (debugMode) {
			ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), pos.first + 5, pos.second + 15);
			ofDrawBitmapStringHighlight("Out of target: " + ofToString(outOfTarget[i]), pos.first + 5, pos.second + 35);
			ofDrawBitmapStringHighlight("Visible: " + ofToString(isOn[i]), pos.first + 5, pos.second + 55);
			ofDrawBitmapStringHighlight("Position: " + ofToString(points[i].y), pos.first + 5, pos.second + 75);
		}
	}
}

//--------------------------------------------------------------
// circles
void ofApp::drawMode5() {

	for (int i = 0; i < processors.size(); i++) {

		float dist = sqrt(pow(edges[i].second.x - edges[i].first.x, 2) + pow(edges[i].second.y - edges[i].first.y, 2));
		float newDist = (dist + prevDist[i]) / 2;

		ofPoint newPos = (points[i] + prevPoints[i]) / 2;

		prevPoints[i] = newPos;
		prevDist[i] = newDist;

		// mask out the images
		ofFbo fbo;
		fbo.allocate(images[i].getWidth(), images[i].getHeight(), GL_RGBA);
		fbo.begin();
		{
			ofClear(0, 0, 0, 0);
		}
		fbo.end();
		fbo.begin();
		{
			ofClear(0, 0, 0, 0);
			//ofBackground(255,255,255);
			ofSetColor(255);
			// mult by two because the points have already been scaled down to fit 
			ofDrawEllipse(newPos.x * 2, newPos.y * 2, newDist * 2, newDist * 2);
		}
		fbo.end();

		ofImage temp;
		temp.setFromPixels(images[i].getPixels());
		temp.getTexture().setAlphaMask(fbo.getTexture());
		temp.draw(0, 0, 960, 540);

		if (debugMode) {
			//	ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawLine(edges[i].first, edges[i].second);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), 15, 25 + i * 20);
		}
	}
}

//--------------------------------------------------------------
// circles tag
void ofApp::drawMode6() {

	if (DEBUG_SINGLE_CAM) {

		if (selected == 1) {
			ofSetColor(255, 0, 0);
			ofDrawEllipse(circles[1].first, circles[1].second * 2 + 10, circles[1].second * 2 + 10);
			ofSetColor(255);
		}

		ofSetColor(0);
		ofDrawEllipse(circles[1].first, circles[1].second * 2, circles[1].second * 2);
		ofSetColor(255);
	}

	for (int k = 0; k < allIn.size(); k++) {

		int i = allIn[k];

		// distance from ear to ear
		float dist = sqrt(pow(edges[i].second.x - edges[i].first.x, 2) + pow(edges[i].second.y - edges[i].first.y, 2));

		// average with previous dist
		float newDist = (dist + prevDist[i]) / 2;
		// average center point with previous point
		ofPoint newPos = (points[i] + prevPoints[i]) / 2;

		prevPoints[i] = newPos;
		prevDist[i] = newDist;

		// mask out the images
		ofFbo fbo;
		fbo.allocate(images[i].getWidth(), images[i].getHeight(), GL_RGBA);
		fbo.begin();
		{
			ofClear(0, 0, 0, 0);
		}
		fbo.end();
		fbo.begin();
		{
			ofClear(0, 0, 0, 0);
			//ofBackground(255,255,255);
			ofSetColor(255, 255, 255, currentAlpha[i]);
			// mult by two because the points have already been scaled down to fit 
			ofDrawEllipse(newPos.x * 2, newPos.y * 2, newDist * 2, newDist * 2);
		}
		fbo.end();

		pair<ofPoint, float> newCirc = make_pair(newPos, newDist);

		bool touching;
		dist = 0;

		if (!won) {
			// check that the new circle is not intersecting any other
			for (int j = 0; j < circles.size(); j++) {

				if (i != j) {
					// distance btwn the points
					dist = sqrt(pow(circles[j].first.x - newCirc.first.x, 2) + pow(circles[j].first.y - newCirc.first.y, 2)) * 2;

					// the two circles are intersecting 
					if (dist < circles[j].second + newCirc.second) {
						// if they weren't in before
						if (!isIn) {
							// switch the selected circle
							if (selected == i) selected = j;
							else selected = i;
						}
						ofDrawLine(circles[j].first, newCirc.first);
						touching = true;
					}
				}
			}
		}
		
		isIn = touching;
		circles[i] = newCirc;

		// highlight behind selected one 
		if (!won && i == selected) {
			//ofSetColor(255, 0, 0);
			ofPath path;
			path.setStrokeColor(ofColor(255, 0, 0));
			path.setStrokeWidth(5);
			path.ellipse(newPos, newDist + 5, newDist + 5);
			path.draw();
			/*ofDrawEllipse(newPos, newDist + 10, newDist + 10);
			ofSetColor(255);*/

			if (currentAlpha[i] >= 41) {
				currentAlpha[i] -= 1;
			}
			else {

				// remove the current video from being rendered
				for (int p = 0; p < allIn.size(); p++) {
					if (allIn[p] == i) {
						allIn.erase(allIn.begin() + p);
					}
				}

				if (allIn.size() > 1) {
					selected = allIn[0];
				}
				else {
					won = true;
					currentAlpha[allIn[0]] = 255;
					cout << "winner: " << allIn[0];
				}
			}
		}
		// draw the image
		ofImage temp;
		temp.setFromPixels(images[i].getPixels());
		temp.getTexture().setAlphaMask(fbo.getTexture());
		temp.draw(0, 0, 960, 540);

		//// faking image transparency by drawing over the frame 
		//ofEnableAlphaBlending();
		//ofSetColor(255, currentAlpha[i]);
		//ofDrawRectangle(10, 10, 960, 540);
		//ofDisableAlphaBlending();

		


		if (debugMode) {
			//	ofDrawCircle(points[i].x + pos.first, points[i].y + pos.second, 10);
			ofDrawLine(edges[i].first, edges[i].second);
			ofDrawBitmapStringHighlight("Tracker FPS: " + ofToString(processors[i]->tracker.getThreadFps()), 15, 25);
			ofDrawBitmapStringHighlight("Touching: " + ofToString(touching), 15, 45);
			ofDrawBitmapStringHighlight("Dist: " + ofToString(dist), 15, 65);
		}
	}
}





//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(255);

	switch (drawMode) {
	case 0:
		drawMode0();
		break;
	case 1:
		drawMode1();
		break;
	case 2:
		drawMode2();
		break;
	case 3:
		drawMode3();
		break;
	case 4:
		drawMode4();
		break;
	case 5:
		drawMode5();
		break;
	case 6:
		drawMode6();
		break;
	default:
		drawMode0();
		break;
	}

	if (debugMode) ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 5, ofGetHeight() - 5);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key) {
	case '0':
		drawMode = 0;
		break;
	case '1':
		drawMode = 1;
		break;
	case '2':
		drawMode = 2;
		break;
	case '3':
		drawMode = 3;
		break;
	case '4':
		drawMode = 4;
		break;
	case '5':
		drawMode = 5;
		break;
	case '6':
		allIn.clear();
		won = false;
		selected = 0;
		for (int i = 0; i < currentAlpha.size(); i++) {
			currentAlpha[i] = 255;
			allIn.push_back(i);
		}
		drawMode = 6;
		break;
	case 'd':
		debugMode = !debugMode;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::exit() {

	// kill threads
	for (int i = 0; i < processors.size(); i++) {
		processors[i]->stopThread();
	}
}
