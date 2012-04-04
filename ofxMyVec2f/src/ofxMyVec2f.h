#ifndef OFXMYVEC2F_H
#define OFXMYVEC2F_H
#include "ofMain.h"
#include "ofxVectorMath.h"

class ofxMyVec2f : public ofxVec2f
{
    public:
        ofxMyVec2f();

        int pointPolyTest(vector <ofPoint>* polygon);
        int pointPolyTest(vector <ofxVec2f>* polygon);
        virtual ~ofxMyVec2f();
    protected:
    private:
};

#endif // OFXMYVEC2F_H
