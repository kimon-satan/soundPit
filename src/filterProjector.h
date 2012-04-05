#ifndef FILTERPROJECTOR_H
#define FILTERPROJECTOR_H

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "trackingObject.h"
#include "ofxXmlSettings.h"
#include "ofxCvMyCv.h"
#include "interpreter.h"
#include "myCamera.h"

class filterProjector
{
public:
	filterProjector();
	virtual ~filterProjector();

	void feedRawBlobs(trackingObject t_objs[][10], int size);


	void reshape(int x, int y, int w, int h);
	void reload();
	void drawFbo3DMask();
	void draw3DMask();
	void draw3DFrame();
	void drawNewFrame();
	void draw3DBlobs(trackingObject t_objs[][10], int size);
	void setProjChanged(bool temp);
	float* getMaskArray();
	void setTrPtrs(vector<patch> * pnt_ptr);

	void draw(trackingObject t_objs[][10], int size);
	void drawMenus(myCvmask *masker);
	void saveConfig(ofxXmlSettings XML, int tagNum);
	void openConfig(ofxXmlSettings XML);

	void keyPressed  (int key, myCvmask * masker);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

protected:

private:

    myCamera cam;
	int width, height;
	int selectedMenu;
	int selectedItem;
	bool projChanged;


	float maskArray[9];
	int projYdisplace;
	GLint viewport[4];
	GLdouble mvmatrix[16];
	GLdouble projmatrix[16];

	vector <vector<trackingObject> > blobs;

	string colBlobString;
	vector <patch>* transRects;

};

#endif // FILTERPROJECTOR_H
