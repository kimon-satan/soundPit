#ifndef TRACKINGOBJECT_H
#define TRACKINGOBJECT_H
#include "ofMain.h"
#include "ofxCvMyCv.h"
#include <algorithm>

class trackingObject : public ofxCvMyBlob
{
    public:
        trackingObject();
        virtual ~trackingObject();

        ofVec2f getAvPos();

        bool getAssigned();
        void setAssigned(bool temp);
        bool getPresent();
        void setPresent(bool temp);

        void ghostIncrement();
        void ghostReset();
        int getGhost();

        void reset();
        void updateHistory();
        void updateDrawHistory();
        void updateAvPos();
        void updateDirection();
        void updateSpeed();

        ofVec2f projPos;
        ofVec2f avPos;
        float direction, true_direction;
        bool collision;

        vector<ofVec2f> history;
        vector<ofVec2f> drawhistory;
        bool assigned, present, moving;
        int ghost, hist_size;
        float speed, lThresh, uThresh;


    protected:
    private:




};

#endif // TRACKINGOBJECT_H
