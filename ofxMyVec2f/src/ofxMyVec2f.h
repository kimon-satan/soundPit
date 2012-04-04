#ifndef OFXMYVEC2F_H
#define OFXMYVEC2F_H
#include "ofMain.h"


class ofxMyVec2f : public ofVec2f
{
    public:
        ofxMyVec2f();

        int pointPolyTest(vector <ofPoint>* polygon);
        int pointPolyTest(vector <ofVec2f>* polygon);
        virtual ~ofxMyVec2f();
    protected:
    private:
};

#endif // OFXMYVEC2F_H
