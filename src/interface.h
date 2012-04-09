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
#include "myCamera.h"


class interface : public ofBaseApp{

public:

    interface(string t_filename);
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

	void exit();





private:

    int viewMode, fileOp, numSampleGrps;
    capture thisCapture;
    filterProjector thisFP;
    interpreter thisInterp;
    display thisDisplay;
	myTextInput fileNameInput;
	string filename;

    bool displayOn;
    trackingObject t_objs[4][10];
	vector <patch> transRects;
	vector <int> colMode;

	float transSize;


};

#endif
