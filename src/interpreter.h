#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ofMain.h"
#include "ofxOsc.h"
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

	int count;
	int sampleGroup;
	bool isActive;

};


class interpreter
{
public:

	interpreter();
	void setTrPtrs(vector<patch> * pnt_ptr);
	void feedObjects(trackingObject t_objs[][10], int size, bool newFrame);
	void detectCollision(trackingObject* selectObj, int o_col, int o_i, trackingObject t_objs[][10]);
	void sendInitialise();
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);
	void checkForInit();
	void checkForHandshake();
	void drawMenus();
	void draw(trackingObject t_objs[][10], int size);
	void keyPressed(int key);
	vector<collision>* getCollisions();
	vector<int>* getColModes();
    void setNumSampleGrps(int n);

	virtual ~interpreter();
protected:
private:

    ofxOscSender sender;
    ofxOscReceiver rec;
    bool isHandshake;
    float initTimeStamp;
    float collDist, collSpeedThresh;
    int synthsRunning;
    int numSampleGroups;
    int maxSynths;
    int speed_scale;
    int stillCount, stillTarget;
    vector<int> collisionsRunning;
    int maxCollisions;
    bool movingArray[4][10];
    vector<ofPoint> expiredCollisions;
    vector<collision> pastCollisions;
    vector< vector <bool> > nowCollisions;

	vector<patch> * transInfo;

    int numSampleGrps;
    int selectedItem;
    int selectedMenu;
    int port;


};

#endif // INTERPRETER_H
