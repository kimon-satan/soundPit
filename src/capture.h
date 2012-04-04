#ifndef CAPTURE_H
#define CAPTURE_H

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofxCvMyCv.h"
//#include "ofxVectorMath.h"
#include "ofxMyVec2f.h"
#include "trackingObject.h"
#include <iostream>
#include <cstdio>
#include <sys/stat.h>

struct distListItem {
    int    np;
    int    fp;
    float  dist;
};



class capture : public myUtilities
{
    public:
        capture();
        virtual ~capture();

	bool update(trackingObject t_objs[][10], int size);
	void draw(trackingObject t_objs[][10], int size);

	void drawMenus();
	void drawCap(int x, int y);
	void drawCap(int x, int y, int w, int h);
	void createZimg();
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);
	void saveHistograms(string fileName);
	void openHistograms(string fileName);

	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    myCvmask* getMasker();

    protected:
    private:

    ofVideoGrabber 		vidGrabber;
    bool bNewFrame;
    bool learnBg;
    bool learnImage;

	ofxCvColorImage		selectImg;
	ofxCvColorImage     zoomSelectImg;
	ofxCvColorImage     vidImg;

    bgSegmenter bgSeg;
    myCvmask masker;
    myColourChannel colChannels[4];
	ofxCvMyContourFinder contFinder[4];

	IplImage *selImage,*vidhsv,*vidImage;

	ofxCvColorImage segImg, maskedImg;

    int width,height;

    bool selectChanged;

	CvRect selection;

    int selectedItem;
    int selectedMenu;
    int selectedColour;
    int selectedView;

    int minBlob, maxBlob, db_thresh;

    int zoom, zx, zy, sample_view;
    bool zoomSel;
    bool newZbp;
    int learningBg;




};

#endif // CAPTURE_H
