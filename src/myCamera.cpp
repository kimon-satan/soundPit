#include "myCamera.h"

myCamera::myCamera()
{
    //ctor
    resetTransform();
    setPosition(25,25,50);
    setFarClip(4000);
    setNearClip(20);

    ofVec3f c(0,0,0);
    //orbit(180,0, getPosition().z, c);
    //roll(180);
    cacheMatrices(true);

}

myCamera::~myCamera()
{
    //dtor
}

void myCamera::begin(ofRectangle viewport) {

glEnable(GL_DEPTH_TEST);
viewportRect = viewport;
ofCamera::begin(viewport);
ofPushMatrix();

glGetDoublev(GL_PROJECTION_MATRIX, this->matP);
glGetDoublev(GL_MODELVIEW_MATRIX, this->matM);
glGetIntegerv(GL_VIEWPORT, this->viewport);

}

//--------------------------
void myCamera::end() {

ofPopMatrix();
ofCamera::end();
glDisable(GL_DEPTH_TEST);

}

ofVec3f myCamera::getZPlaneProjection(ofVec2f screenPoint, ofRectangle viewport){

ofVec3f ray[2];

//define ray in screen space
ray[0] = ofVec3f(screenPoint.x, screenPoint.y, -1);
ray[1] = ofVec3f(screenPoint.x, screenPoint.y, 1);

//transform ray into world space
ray[0] = screenToWorld(ray[0], viewport);
ray[1] = screenToWorld(ray[1], viewport);

ofVec3f rayvec(ray[1]-ray[0]);
ofVec3f intersect;

float u = -1 * ray[0].z/rayvec.z; //for some reason it needs to be reversed
intersect.x = ray[0].x + rayvec.x * u;
intersect.y = ray[0].y + rayvec.y * u;
intersect.z = 0;

return intersect;
}
