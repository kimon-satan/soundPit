#include "filterProjector.h"

filterProjector::filterProjector()
{


    width = 320;
    height = 240;


}


void filterProjector::feedRawBlobs(trackingObject t_objs[][10], int size){

	colBlobString.clear();

    for(int col = 0; col < 4; col ++){

        for (int i = 0; i < 10; i++){

            if(t_objs[col][i].assigned){

                ofVec3f tp = cam.getZPlaneProjection(t_objs[col][i].centroid, ofRectangle(0,0,viewPort.width,viewPort.height));
                ofRectangle frame(0,0,50,50);
                if(frame.inside(ofVec2f(tp.x,tp.y))){
                    colBlobString = colBlobString + "\nprojected (x,y): " + ofToString(tp.x,1) + "," + ofToString(tp.y,1);
                    t_objs[col][i].projPos.set(tp.x, tp.y);
                    t_objs[col][i].present = true;
                }else{
                    t_objs[col][i].present = false;
                }

            }
        }

    }

}



void filterProjector::draw(trackingObject t_objs[][10], int size){


    ofDrawBitmapString(colBlobString, 20,260);
    viewPort.set(320,0,320,240);
    ofNoFill();
    ofRect(viewPort);
    cam.begin(viewPort);
        ofPushStyle();
            ofNoFill();
            ofSetColor(255);
            ofRect(0,0,50,50);
            ofCircle(25,25,1);
        ofPopStyle();

        draw3DBlobs(t_objs, size);
    cam.end();


}






void filterProjector::draw3DBlobs(trackingObject t_objs[][10], int size){

    ofPushStyle();
	ofSetColor(255,0,0);
	ofFill();

	for(int col = 0; col < 4; col++){
		for(int i =0; i < 10; i++){

			float x = t_objs[col][i].projPos.x;
			float y = t_objs[col][i].projPos.y;

			if(t_objs[col][i].present)ofRect(x-1.5f, y-1.5f,3,3);


		}
	}

	ofPopStyle();

}


void filterProjector::openConfig(ofxXmlSettings XML){

	if( XML.pushTag("FILTERPROJECTOR", 0) ){

		if( XML.pushTag("MASK", 0) ){
			for(int i =0; i < 9; i++){
//				maskArray[i] = XML.getValue("MASKARRAY",0, i);
			}
			XML.popTag();
		}
		XML.popTag();
	}

}

void filterProjector::saveConfig(ofxXmlSettings XML, int tagNum){


	tagNum	= XML.addTag("FILTERPROJECTOR");
	if( XML.pushTag("FILTERPROJECTOR", tagNum) ){

		tagNum	= XML.addTag("MASK");
		if( XML.pushTag("MASK", tagNum) ){
			for(int i =0; i < 9; i++){
				//XML.setValue("MASKARRAY",maskArray[i],tagNum+i);
			}
			XML.popTag();
		}
		XML.popTag();
	}

}


void filterProjector::keyPressed(int key, myCvmask *masker){

    switch (key){

        case ',':
			selectedMenu += 1; selectedMenu = selectedMenu%2;
			break;

        case '.':
			selectedItem += 1;
			if(selectedMenu == 0)selectedItem = selectedItem%2;
			if(selectedMenu == 1)selectedItem = selectedItem%4;
			break;

        case OF_KEY_UP:
            if(selectedMenu == 0){
                if(selectedItem == 1)cam.tilt(1);
                if(selectedItem == 0)cam.boom(-1);
            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.y -= 1;
                masker->setCoordinate(selectedItem, temp);

            }


			break;

        case OF_KEY_DOWN:
            if(selectedMenu == 0){

				if(selectedItem == 1)cam.tilt(-1);
                if(selectedItem == 0)cam.boom(1);

            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.y += 1;
                masker->setCoordinate(selectedItem, temp);
            }


			break;


        case OF_KEY_LEFT:

            if(selectedMenu == 0){
                if(selectedItem == 1)cam.pan(-1);
                if(selectedItem == 0)cam.truck(1);
            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.x -= 1;
                masker->setCoordinate(selectedItem, temp);
            }

			break;

        case OF_KEY_RIGHT:

            if(selectedMenu == 0){
                if(selectedItem == 1)cam.pan(1);
                if(selectedItem == 0)cam.truck(-1);
            }

            if(selectedMenu == 1){

                CvPoint temp = masker->getCoordinate(selectedItem);
                temp.x += 1;
                masker->setCoordinate(selectedItem, temp);
            }


			break;

			case OF_KEY_PAGE_DOWN:
			if(selectedMenu == 0){
                if(selectedItem == 1)cam.roll(-1);
                if(selectedItem == 0)cam.dolly(-1);
			}
			break;

			case OF_KEY_PAGE_UP:
			if(selectedMenu == 0){
                if(selectedItem == 0)cam.dolly(1);
                if(selectedItem == 1)cam.roll(1);
			}
			break;

    }
}



void filterProjector::drawMenus(myCvmask *masker){

	masker->draw(width,0,1.0f);

	glPushMatrix();
	glTranslatef(width*2 + 25,20,0);

	if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
	ofDrawBitmapString("3D Mask Settings:",0,0);

	int maskMenuColors[2];

    for(int i = 0; i < 2; i++){

		if(selectedItem == i && selectedMenu == 0){
			maskMenuColors[i] = 0xff0000;
		}else{maskMenuColors[i] = 0xffffff;}

    }

	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[0]);
	ofDrawBitmapString("move:",0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(maskMenuColors[1]);
	ofDrawBitmapString("angle:",0,0);


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


filterProjector::~filterProjector()
{
    //dtor
}
