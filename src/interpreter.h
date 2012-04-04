#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ofMain.h"
#include "ofxOsc.h"
//#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"
#include"trackingObject.h"

#define HOST "localhost"
#define PORT 57120

struct collision{

	int obj1[2];
	int obj2[2];
	float power;
	ofPoint location;
	bool repeat;
	int time;

};

struct patch{

	ofRectangle rect;
	int count;
	int col;
	int isLive;
	bool isFilled;

};


class interpreter
{
public:

	interpreter();
	void setTrPtrs(vector<patch> * pnt_ptr, vector<int> * md_ptr);
	void feedObjects(trackingObject t_objs[][10], int size, bool newFrame);
	void detectCollision(trackingObject* selectObj, int o_col, int o_i, trackingObject t_objs[][10]);
	void sendInitialise();
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);
	void drawMenus();
	void draw(trackingObject t_objs[][10], int size);
	void keyPressed(int key);
	vector<collision>* getCollisions();
	vector<int>* getColModes();


	virtual ~interpreter();
protected:
private:

    ofxOscSender sender;

    float collDist, collSpeedThresh;
    int synthsRunning;
    int maxSynths;
    int speed_scale;
    vector<int> collisionsRunning;
    int maxCollisions;
    bool movingArray[4][10];
	bool isTransforming[4];
	int colTrs[4];
    vector<ofPoint> expiredCollisions;
    vector<collision> pastCollisions;
    vector< vector <bool> > nowCollisions;

	vector<patch> * transRects;
	vector<int>* colMode;

    int selectedItem;
    int selectedMenu;


};

#endif // INTERPRETER_H
