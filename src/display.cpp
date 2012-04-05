
#include "display.h"

display::display(){

	radius  = 3;
	lineWidth = 1;
	alpha = 100;
	x_overlap = 3.0f;
	y_overlap = 3.0f;
	displayRect.set(1300,0,3.0f,3.0f);
	monitorRect.set(320,0,2.0f,2.0f);

	buttons[0].loadImage("images/xenakis3.png");
	buttons[1].loadImage("images/stock4.png");
	buttons[2].loadImage("images/varese2.png");
	buttons[3].loadImage("images/exp1.png");
}

void display::setTrPtrs(vector<patch> * pnt_ptr, vector<int>* md_ptr){

	transRects = pnt_ptr;
	colMode = md_ptr;

}

void display::draw(trackingObject t_objs[][10],vector<collision>* collisions, float maskArray[], vector<int>*colModes, bool disp){

    ofPushStyle();
    ofNoFill();
    ofRectangle dimensions;

    if(disp){dimensions = displayRect;} else{ dimensions = monitorRect;}


    GLfloat prop = (GLfloat)(dimensions.width *320.0f)/(GLfloat)(dimensions.height * 240.0f);

    float x = dimensions.x + (maskArray[7] * dimensions.width) - (80 * dimensions.width);
    float y = ofGetHeight() - (dimensions.y + (240 * dimensions.height));
    y += (maskArray[8] * dimensions.height);
    y -= 60 * dimensions.height;

    glMatrixMode(GL_VIEWPORT);
    glLoadIdentity();
    glViewport( x,y, 320 * dimensions.width *1.5, 240 * dimensions.height * 1.5);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (45.0, prop,1.0, 400.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    glTranslated(maskArray[0], maskArray[1], maskArray[2]);
    glRotated(maskArray[3], 1, 0, 0);
    glRotated(maskArray[4], 0, 1, 0);
    glRotated(maskArray[5], 0, 0, 1);

    glTranslatef(-25,-25,0);
    ofSetColor(255,255,255);
    ofRect(0,0,50,50);

	bool isTransforming[4] = {false,false,false,false};

	for(int tr = 0; tr < 4; tr++){

		if(transRects->at(tr).count > 15){

			isTransforming[tr] = true;

			for(int i = 0; i <10; i++){
				if(t_objs[transRects->at(tr).col][i].present){

					drawObj(&t_objs[transRects->at(tr).col][i],transRects->at(tr).col, tr);

					/*ofSetHexColor(0xFF3E96);
					ofLine(transRects->at(tr).rect.x, transRects->at(tr).rect.y,
						   t_objs[transRects->at(tr).col][i].avPos.x , t_objs[transRects->at(tr).col][i].avPos.y);*/

				}
			}
		}
	}


	ofSetColor(255, 255, 255);
	ofNoFill();



    for(int col = 0; col < 4; col ++){

		if(!isTransforming[col]){

			for(int i = 0; i <10; i++){

				if(t_objs[col][i].present && t_objs[col][i].moving){

					drawObj(&t_objs[col][i], col, colModes->at(col));

				}
			}
		}
    }


	//draw collisions
    ofNoFill();
    ofSetHexColor(0xFF3E96);
    for(int i = 0; i < collisions->size(); i++){

		ofCircle(collisions->at(i).location.x,
				 collisions->at(i).location.y,
				 (((float)collisions->at(i).time + 1.0f)/40) * 25.0f);

    }


    glPopMatrix();
    ofPopStyle();

    ofSetColor(255,255,255);


}


void display::drawObj(trackingObject * t_obj, int col, int colMode){

	ofEnableAlphaBlending();


	float x = t_obj->avPos.x;
	float y = t_obj->avPos.y;


	ofFill();
	ofSetColor(255,255,255,255);

	glPushMatrix();
	glTranslatef(x,y,0);
	buttons[colMode].draw(-radius,-radius,radius*2,radius*2);
	glPopMatrix();

	if(col == 0)ofSetColor(238,59,59,alpha); // red
	if(col == 1)ofSetColor(30,144,255,alpha); // blue
	if(col == 2)ofSetColor(255,255,0,alpha); //yellow
	if(col == 3)ofSetColor(34,139,34,alpha); //green

	ofCircle(x,y,radius);
	ofNoFill();

	ofDisableAlphaBlending();

	ofSetLineWidth(lineWidth);
	ofSetHexColor(0x00ffff);
	ofCircle(x,y,radius);
	glPushMatrix();
	glTranslatef(x,y,0);
	glRotatef(t_obj->true_direction, 0,0,1);
	ofLine(0 - radius * x_overlap,0,0 + radius * x_overlap,0);
	ofLine(0, 0 - radius * y_overlap, 0, 0 + radius * y_overlap);
	glPopMatrix();

}

void display::drawMenus(){


	glPushMatrix();
	glTranslatef(20,300,0);

	if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
	ofDrawBitmapString("Display Settings:",0,0);

	int dispMenuColors[5];

    for(int i = 0; i < 5; i++){

		if(selectedItem == i && selectedMenu == 0){
			dispMenuColors[i] = 0xff0000;
		}else{dispMenuColors[i] = 0xffffff;}

    }

	glTranslatef(0,15,0);
	ofSetHexColor(dispMenuColors[0]);
	ofDrawBitmapString("radius:" + ofToString(radius,0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispMenuColors[1]);
	ofDrawBitmapString("linewidth:" + ofToString(lineWidth,2),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispMenuColors[2]);
	ofDrawBitmapString("alpha:" + ofToString(alpha,0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispMenuColors[3]);
	ofDrawBitmapString("x_overlap:" + ofToString(x_overlap,2),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispMenuColors[4]);
	ofDrawBitmapString("y_overlap:" + ofToString(y_overlap,2),0,0);

	ofSetColor(255,255,255);

	glPopMatrix();

	glPushMatrix();
	glTranslatef(20,500,0);

	if(selectedMenu == 1){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
	ofDrawBitmapString("Screen Dimensions:",0,0);

	int dispRectMenuColors[4];

    for(int i = 0; i < 4; i++){

		if(selectedItem == i && selectedMenu == 1){
			dispRectMenuColors[i] = 0xff0000;
		}else{dispRectMenuColors[i] = 0xffffff;}

    }

	glTranslatef(0,15,0);
	ofSetHexColor(dispRectMenuColors[0]);
	ofDrawBitmapString("disp_x:" + ofToString(displayRect.x,0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispRectMenuColors[1]);
	ofDrawBitmapString("disp_y:" + ofToString(displayRect.y,0),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispRectMenuColors[2]);
	ofDrawBitmapString("disp_w:" + ofToString(displayRect.width,2),0,0);
	glTranslatef(0,15,0);
	ofSetHexColor(dispRectMenuColors[3]);
	ofDrawBitmapString("disp_h:" + ofToString(displayRect.height,2),0,0);

	ofSetColor(255,255,255);

	glPopMatrix();


}

void display::keyPressed(int key){

    switch (key){

        case ',':
			selectedMenu += 1; selectedMenu = selectedMenu%2;
			selectedItem = 0;
			break;

        case '.':
			selectedItem += 1;
			if(selectedMenu == 0)selectedItem = selectedItem%5;
			if(selectedMenu == 1)selectedItem = selectedItem%4;
			break;

        case OF_KEY_UP:
			if(selectedMenu == 0){

				if(selectedItem == 0)radius += 1;
				if(selectedItem == 1)lineWidth += 0.5;
				if(selectedItem == 2)alpha += 1;
				if(selectedItem == 3)x_overlap += 0.5;
				if(selectedItem == 4)y_overlap += 0.5;

			}

			if(selectedMenu == 1){

				if(selectedItem == 0)displayRect.x += 1;
				if(selectedItem == 1)displayRect.y += 1;
				if(selectedItem == 2)displayRect.width += 0.01;
				if(selectedItem == 3)displayRect.height += 0.01;

			}

			break;

        case OF_KEY_DOWN:
			if(selectedMenu == 0){

				if(selectedItem == 0)radius -= 1;
				if(selectedItem == 1)lineWidth -= 0.5;
				if(selectedItem == 2)alpha -= 1;
				if(selectedItem == 3)x_overlap -= 0.5;
				if(selectedItem == 4)y_overlap -= 0.5;

			}

			if(selectedMenu == 1){

				if(selectedItem == 0)displayRect.x -= 1;
				if(selectedItem == 1)displayRect.y -= 1;
				if(selectedItem == 2)displayRect.width -= 0.01;
				if(selectedItem == 3)displayRect.height -= 0.01;

			}
			break;

    }


}


void display::saveConfig(ofxXmlSettings XML, int tagNum){

	tagNum	= XML.addTag("DISPLAY");

    if( XML.pushTag("DISPLAY", tagNum) ){

		XML.setValue("RADIUS", radius, tagNum);
		XML.setValue("LINEWIDTH", lineWidth, tagNum);
		XML.setValue("ALPHA", alpha, tagNum);
		XML.setValue("X_OVERLAP", x_overlap, tagNum);
		XML.setValue("Y_OVERLAP", y_overlap, tagNum);

		XML.setValue("DRX", displayRect.x, tagNum);
		XML.setValue("DRY", displayRect.y, tagNum);
		XML.setValue("DRW", displayRect.width, tagNum);
		XML.setValue("DRH", displayRect.height, tagNum);

		XML.popTag();

    }

}

void display::openConfig(ofxXmlSettings XML){


    if( XML.pushTag("DISPLAY", 0) ){

		radius = XML.getValue("RADIUS", 0);
		lineWidth =  XML.getValue("LINEWIDTH", 0);
		alpha = XML.getValue("ALPHA", 0);
		x_overlap = XML.getValue("X_OVERLAP", 0);
		y_overlap = XML.getValue("Y_OVERLAP", 0);

		displayRect.x = XML.getValue("DRX", 0);
		displayRect.y = XML.getValue("DRY", 0);
		displayRect.width = XML.getValue("DRW", 0.0);
		displayRect.height = XML.getValue("DRH", 0.0);


		XML.popTag();

    }

}

display::~display(){


}
