#ifndef DISPLAY_H
#define DISPLAY_H

#include "ofMain.h"
//#include "ofxVectorMath.h"
#include"trackingObject.h"
#include "interpreter.h"


class display
{
public:
	display();

	void draw(trackingObject t_objs[][10],vector<collision>* collisions, float maskArray[], vector<int>* colModes, bool disp);
	void drawMenus();
	void keyPressed(int key);
	virtual ~display();
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);
	void setTrPtrs(vector<patch> * pnt_ptr, vector<int> * md_ptr);

	ofRectangle displayRect;
	ofRectangle monitorRect;

protected:
private:

	void drawObj(trackingObject *t_obj, int col, int colMode);

    int radius, alpha;
    float lineWidth, x_overlap, y_overlap;


    int selectedItem;
    int selectedMenu;
	vector<patch> * transRects;
	vector<int>* colMode;
	ofImage buttons[4];


};

#endif // DISPLAY_H
