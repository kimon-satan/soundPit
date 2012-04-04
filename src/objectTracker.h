#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "trackingObject.h"
#include <algorithm>
#include "ofxVectorMath.h"


struct distListItem {
    int    np;
    int    fp;
    float  dist;


};


class objectTracker
{
    public:
        objectTracker();

        void draw();
        void drawMenus();
        void keyPressed(int key);
        void saveConfig(ofxXmlSettings XML, int tagNum);
        void openConfig(ofxXmlSettings XML);

        void feedObjects(vector<vector<trackingObject> >tempObjects);
        void cleanUp(vector<vector<trackingObject> >tempObjects);

        void updateDistanceList(int col);
        void updateFinalBlob(trackingObject* now, trackingObject* final);

        vector<vector<trackingObject> >getFinals();
        void reset();
        bool getClean();






    protected:
    private:

    vector<distListItem>distanceList;
    vector<vector<trackingObject> > finalObjects;
    vector<vector<trackingObject> > nowObjects;
    int maxDist;
    int ghostFrames;

    int selectedMenu;
    int selectedItem;
    int edgeBuf;


    int cleanCount;
    int cleanWait;
    bool clean;
    bool drawTracked;



};


#endif // OBJECTTRACKER_H
