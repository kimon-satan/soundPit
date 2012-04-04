#ifndef _INTERFACE
#define _INTERFACE

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "myTextInput.h"
#include "capture.h"
#include "filterProjector.h"
#include "trackingObject.h"
#include "interpreter.h"
#include "display.h"


class interface : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void saveConfig(string fileName);
	void openConfig(string fileName);

	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);





private:

    int viewMode, fileOp;
    capture thisCapture;
    filterProjector thisFP;
    interpreter thisInterp;
    display thisDisplay;
	myTextInput fileNameInput;

    bool displayOn;
    trackingObject t_objs[4][10];
	vector <patch> transRects;
	vector <int> colMode;

	float transSize;


};

#endif
