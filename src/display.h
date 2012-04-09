#ifndef DISPLAY_H
#define DISPLAY_H

#include "ofMain.h"
#include "myCamera.h"
#include"trackingObject.h"
#include "interpreter.h"


class display
{
public:
	display();

	void draw(trackingObject t_objs[][10],vector<collision>* collisions, myCamera t_cam, bool disp);
	void drawMenus();
	void keyPressed(int key);
	virtual ~display();
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);
	void setTrPtrs(vector<patch> * pnt_ptr);
	void setNumSampleGrps(int n);
	void loadButtonImages();

	ofRectangle displayRect;
	ofRectangle monitorRect;

protected:
private:

	void drawObj(trackingObject *t_obj, int col, int colMode);

    int radius, alpha;
    float lineWidth, x_overlap, y_overlap;

    int selectedItem;
    int selectedMenu;
    int numSampleGrps;
	vector<patch> * transInfo;
	vector<ofImage> buttons;
	vector<string> imageFilenames;


};

#endif // DISPLAY_H
