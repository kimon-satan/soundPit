#include "ofxMyVec2f.h"

ofxMyVec2f::ofxMyVec2f()
{
    //ctor
}

int ofxMyVec2f::pointPolyTest(vector <ofVec2f>* polygon){
int counter = 0;
  int i;
  int N = polygon->size();
  float xinters;
  ofPoint p1,p2;

  p1 = polygon->at(0);
  for (i=1;i<=N;i++) {

    p2 = polygon->at(i % N);
    if (y > min(p1.y,p2.y)) {
      if (y <= max(p1.y,p2.y)) {
        if (x <= max(p1.x,p2.x)) {
          if (p1.y != p2.y) {
            xinters = (y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;

            if (p1.x == p2.x || x <= xinters)
              counter++;
          }
        }
      }
    }
    p1 = p2;
  }

  if (counter % 2 == 0)
    return(0);
  else
    return(1);

}

int ofxMyVec2f::pointPolyTest(vector <ofPoint>* polygon){

int counter = 0;
  int i;
  int N = polygon->size();
  float xinters;
  ofPoint p1,p2;

  p1 = polygon->at(0);
  for (i=1;i<=N;i++) {

    p2 = polygon->at(i % N);
    if (y > min(p1.y,p2.y)) {
      if (y <= max(p1.y,p2.y)) {
        if (x <= max(p1.x,p2.x)) {
          if (p1.y != p2.y) {
            xinters = (y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;

            if (p1.x == p2.x || x <= xinters)
              counter++;
          }
        }
      }
    }
    p1 = p2;
  }

  if (counter % 2 == 0)
    return(0);
  else
    return(1);



}

ofxMyVec2f::~ofxMyVec2f()
{
    //dtor
}
