#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	

	// create threads
	for (int i = 0; i < NUM_WINDOWS; i++) {
		processors.push_back(move(unique_ptr<FaceProcessor>(new FaceProcessor)));
		processors.back()->setup(i);
		processors.back()->startThread(true);

		images.push_back(move(unique_ptr<ofImage>(new ofImage)));
		points.push_back(move(unique_ptr<ofPoint>(new ofPoint)));
	}
}

//--------------------------------------------------------------
void ofApp::update(){

	for (int i = 0; i < processors.size(); i++) {

		processors[i]->lock();

		// set the image from the grabber frame
		images[i]->setFromPixels(processors[i]->grabber.getPixels());

		// set the center point 
		points[i]->x = processors[i]->center.x / 2;
		points[i]->y = processors[i]->center.y / 2;

		// done
		processors[i]->unlock();
		
	}

}

//--------------------------------------------------------------
pair<int, int> ofApp::getDefaultPosition(int i) {
	int x = 10;
	int y = 10;

	if (i >= 2) {
		y = 560;
	}

	if (i == 1 || i == 3) {
		x = 980;
	}
	else if (i == 2) {
		x = 500;
	}

	return make_pair(x, y);
}

//--------------------------------------------------------------
void ofApp::drawMode1() {

	for (int i = 0; i < processors.size(); i++) {

		pair<int, int> pos = getDefaultPosition(i);

		images[i]->draw(pos.first, pos.second, 960, 540);
		ofDrawCircle(points[i]->x + pos.first, points[i]->y + pos.second, 10);
	}
}

//--------------------------------------------------------------
void ofApp::drawMode2() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	switch (drawMode) {
	case 1:
		drawMode1();
		break;
	case 2:
		drawMode2();
		break;
	}


	////image.draw(0, 0);
	//for (int i = 0; i < processors.size(); i++) {

	//	//processors[i]->lock();
	//	/*ofImage temp;
	//	temp.getPixelsRef() = processors[i]->grabber.getPixels();
	//	temp.update();

	//	temp.draw(0, 0);*/

	//	
	//	images[i]->draw(x, y, 960, 540);
	//	ofDrawCircle(points[i]->x + x, points[i]->y + y, 10);

	////	processors[i]->tracker.drawDebug();

	////	processors[i]->unlock();
	//	//ofDrawBitmapStringHighlight("Tracker fps: " + ofToString(processors[i]->tracker.getThreadFps()), 10, 20);
	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key) {
	case '1':
		drawMode = 1;
		break;
	case '2':
		drawMode = 2;
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
