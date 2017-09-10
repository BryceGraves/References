#include "ofMain.h"

void drawManyCircles(int x) {
	if (x == 0) {
		return; 
	} else { 
		ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
		ofSetCircleResolution(1000);
		ofDrawCircle(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, x);
		x -= 1;
		return drawManyCircles(x);
	}
}