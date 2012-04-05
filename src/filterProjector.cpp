#include "filterProjector.h"

filterProjector::filterProjector()
{
    glDepthMask(true);

    maskArray[0] = 0;  //translates
    maskArray[1] = 0;
    maskArray[2] = -100;
    maskArray[3] = -50;  // rotates
    maskArray[4] = 0;
    maskArray[5] = 0;
    maskArray[6] = 1.0; // proportion x/y
    maskArray[7] = 0; // viewport x
    maskArray[8] = 0; // viewport y

    projChanged = true;

    width = 320;
    height = 240;


}

void filterProjector::setTrPtrs(vector<patch> * pnt_ptr){

	transRects = pnt_ptr;

}

void filterProjector::feedRawBlobs(trackingObject t_objs[][10], int size){

	colBlobString.clear();


	glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);


    for(int col = 0; col < 4; col ++){

        for (int i = 0; i < 10; i++){

            if(t_objs[col][i].assigned){

                int y = t_objs[col][i].centroid.y;
                int x = t_objs[col][i].centroid.x;

                ofSetColor(0,255,0);
                ofRect(x,y,10,10);

                //y = y + (camHeight * scale);

                GLfloat winX, winY, winZ;

                double position[3];

                winX = (float)x;
                winY = ofGetHeight() - (float)y;

                glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

                if (winZ != 1.0f) {

                    gluUnProject( winX, winY, winZ, mvmatrix, projmatrix, viewport, &position[0], &position[1],&position[2]);


                    t_objs[col][i].projPos.set((float)position[0], (float)position[1]);

                    //fill the output string for chosen colNum

					if(position[0] >= 0 && position[1] >=0){
						t_objs[col][i].present = true;
					}else{
						t_objs[col][i].present = true;
					}

                    colBlobString = colBlobString + "\nprojected (x,y): " + ofToString(position[0],1) + "," + ofToString(position[1],1);
                }else{

                    t_objs[col][i].present = false;
                }


            }
        }

    }

	glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
}


void filterProjector::drawMenus(myCvmask *masker){

	masker->draw(width,0,1.0f);

	glPushMatrix();
	glTranslatef(width*2 + 25,20,0);

	if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
	ofDrawBitmapString("3D Mask Settings:",0,0);

	int maskMenuColors[9];

    for(int i = 0; i < 9; i++){

		if(selectedItem == i && selectedMenu == 0){
			maskMenuColors[i] = 0xff0000;
		}else{maskMenuColors[i] = 0xffffff;}

    }

	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[0]);
	ofDrawBitmapString("tran x:" + ofToString(maskArray[0],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[1]);
	ofDrawBitmapString("tran y:" + ofToString(maskArray[1],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[2]);
	ofDrawBitmapString("tran z:" + ofToString(maskArray[2],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[3]);
	ofDrawBitmapString("rot x:" + ofToString(maskArray[3],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[4]);
	ofDrawBitmapString("rot y:" + ofToString(maskArray[4],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[5]);
	ofDrawBitmapString("rot z:" + ofToString(maskArray[5],0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[6]);
	ofDrawBitmapString("prop (w*h):" + ofToString(maskArray[6],2),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[7]);
	ofDrawBitmapString("viewX:" + ofToString(maskArray[7],2),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[8]);
	ofDrawBitmapString("viewY:" + ofToString(maskArray[8],2),0,0);

	ofSetColor(255,255,255);

	glPopMatrix();


	glPushMatrix();

    glTranslatef(width*2 + 25,220,0);

    glTranslatef(0,15,0);
    if(selectedMenu == 1){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("2D Mask Settings:",0,0);

    int maskerMenuColors[4];

    for(int i = 0; i < 4; i++){

		if(selectedItem == i && selectedMenu == 1){
			maskerMenuColors[i] = 0xff0000;
		}else{maskerMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(maskerMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("pt0: " + ofToString(masker->getCoordinate(0).x, 0) + ","
                       + ofToString(masker->getCoordinate(0).y, 0),0,0);
    ofSetHexColor(maskerMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("pt1: " + ofToString(masker->getCoordinate(1).x, 0) + ","
                       + ofToString(masker->getCoordinate(1).y, 0),0,0);
    ofSetHexColor(maskerMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("pt2: " + ofToString(masker->getCoordinate(2).x, 0) + ","
                       + ofToString(masker->getCoordinate(2).y, 0),0,0);
    ofSetHexColor(maskerMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("pt3: " + ofToString(masker->getCoordinate(3).x, 0) + ","
                       + ofToString(masker->getCoordinate(3).y, 0),0,0);


    ofSetColor(255,255,255);

	glPopMatrix();





}

void filterProjector::draw(trackingObject t_objs[][10], int size){


    ofDrawBitmapString(colBlobString, 20,260);
    draw3DMask();
    reshape(width,ofGetHeight()-height,width,height);
    draw3DFrame();
    draw3DBlobs(t_objs, size);

}


void filterProjector::reshape(int x, int y, int w, int h)
{

	//glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);

	x += maskArray[7];
	y += maskArray[8];

	GLfloat prop = (GLfloat)w/(GLfloat)h;

	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport (x - (w*0.25), y -(h*0.25), (GLsizei)w * 1.5, (GLsizei)h * 1.5);
	if(projChanged == true)glGetIntegerv (GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (45.0, prop,1.0, 400.0);

	if(projChanged == true)glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

}

void filterProjector::reload(){

	glMatrixMode(GL_VIEWPORT);
	glViewport(0,0,ofGetWidth(),ofGetHeight());

}


void filterProjector::draw3DMask(){

    glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    reshape(0,ofGetHeight()- height,width,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	glTranslated(maskArray[0], maskArray[1], maskArray[2]);
	glRotated(maskArray[3], 1, 0, 0);
	glRotated(maskArray[4], 0, 1, 0);
	glRotated(maskArray[5], 0, 0, 1);

	// this will be the same modelview matrix that the object is drawn with
	// so vertices will be in the same frame of reference.

	glPushMatrix();
	ofFill();
	glTranslatef(-25,-25,0);
	ofRect(0,0,50,50*maskArray[6]);

	if(projChanged == true){glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);projChanged = false;}

	glPopMatrix();

	glPopMatrix();

	glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);

}

void filterProjector::draw3DFrame(){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslated(maskArray[0], maskArray[1], maskArray[2]);
	glRotated(maskArray[3], 1, 0, 0);
	glRotated(maskArray[4], 0, 1, 0);
	glRotated(maskArray[5], 0, 0, 1);
	// this will be the same modelview matrix that the object is drawn with
	// so vertices will be in the same frame of reference.

	glPushMatrix();
	ofNoFill();
	glTranslatef(-25,-25,0);
	ofSetColor(255,255,255);

	ofCircle(25,25,1);

	ofRect(0,0,50,50);
	ofFill();

	glPopMatrix();

	glPopMatrix();


}


void filterProjector::draw3DBlobs(trackingObject t_objs[][10], int size){

	glTranslated(maskArray[0], maskArray[1], maskArray[2]);
	glRotated(maskArray[3], 1, 0, 0);
	glRotated(maskArray[4], 0, 1, 0);
	glRotated(maskArray[5], 0, 0, 1);
	glTranslatef(-25,-25,0);

	ofSetColor(255,0,0);

	for(int col = 0; col < 4; col++){
		for(int i =0; i < 10; i++){

			float x = t_objs[col][i].projPos.x;
			float y = t_objs[col][i].projPos.y;

			if(t_objs[col][i].present)ofRect(x-1.5f, y-1.5f,3,3);

		}
	}

	ofSetColor(255,255,255);

}

float* filterProjector::getMaskArray()
{

    return &maskArray[0];

}


void filterProjector::keyPressed(int key, myCvmask *masker){

    switch (key){

        case ',':
			selectedMenu += 1; selectedMenu = selectedMenu%3;
			break;

        case '.':
			selectedItem += 1;
			if(selectedMenu == 0)selectedItem = selectedItem%9;
			if(selectedMenu == 1)selectedItem = selectedItem%4;
			if(selectedMenu == 2)selectedItem = selectedItem%5;
			break;

        case OF_KEY_UP:
            if(selectedMenu == 0){

				projChanged = true;
				maskArray[selectedItem] += 1;

            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.y -= 1;
                masker->setCoordinate(selectedItem, temp);

            }


			break;

        case OF_KEY_DOWN:
            if(selectedMenu == 0){

				projChanged = true;
				maskArray[selectedItem] -=1;

            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.y += 1;
                masker->setCoordinate(selectedItem, temp);
            }


			break;


        case OF_KEY_LEFT:
            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.x -= 1;
                masker->setCoordinate(selectedItem, temp);
            }

			break;

        case OF_KEY_RIGHT:
            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.x += 1;
                masker->setCoordinate(selectedItem, temp);
            }


			break;

    }
}


void filterProjector::openConfig(ofxXmlSettings XML){

	if( XML.pushTag("FILTERPROJECTOR", 0) ){

		if( XML.pushTag("MASK", 0) ){
			for(int i =0; i < 9; i++){
				maskArray[i] = XML.getValue("MASKARRAY",0, i);
			}
			XML.popTag();
		}
		XML.popTag();
	}
	projChanged = true;
}

void filterProjector::saveConfig(ofxXmlSettings XML, int tagNum){


	tagNum	= XML.addTag("FILTERPROJECTOR");
	if( XML.pushTag("FILTERPROJECTOR", tagNum) ){

		tagNum	= XML.addTag("MASK");
		if( XML.pushTag("MASK", tagNum) ){
			for(int i =0; i < 9; i++){
				XML.setValue("MASKARRAY",maskArray[i],tagNum+i);
			}
			XML.popTag();
		}
		XML.popTag();
	}
	projChanged = true;
}


void filterProjector::setProjChanged(bool temp){

	projChanged = true;

}
filterProjector::~filterProjector()
{
    //dtor
}
