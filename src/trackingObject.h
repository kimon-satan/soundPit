#ifndef TRACKINGOBJECT_H
#define TRACKINGOBJECT_H
#include "ofMain.h"
#include "ofxVectorMath.h"
#include "ofxCvMyCv.h"
#include <algorithm>

class trackingObject : public ofxCvMyBlob
{
    public:
        trackingObject();
        virtual ~trackingObject();

        ofxVec2f getAvPos();

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

        ofxVec2f projPos;
        ofxVec2f avPos;
        float direction, true_direction;
        bool collision;

        vector<ofxVec2f> history;
        vector<ofxVec2f> drawhistory;
        bool assigned, present, moving;
        int ghost, hist_size;
        float speed, lThresh, uThresh;


    protected:
    private:




};

#endif // TRACKINGOBJECT_H
