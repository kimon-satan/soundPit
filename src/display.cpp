
#include "display.h"

display::display(){

	radius  = 3;
	lineWidth = 1;
	alpha = 100;
	x_overlap = 3.0f;
	y_overlap = 3.0f;
	displayRect.set(0,0,640.0f,480.0f);
	monitorRect.set(320,0,640.0f,480.0f);

	buttons[0].loadImage("images/xenakis3.png");
	buttons[1].loadImage("images/stock4.png");
	buttons[2].loadImage("images/varese2.png");
	buttons[3].loadImage("images/exp1.png");
}

void display::setTrPtrs(vector<patch> * pnt_ptr, vector<int>* md_ptr){

	transRects = pnt_ptr;
	colMode = md_ptr;

}

void display::draw(trackingObject t_objs[][10],vector<collision>* collisions, myCamera t_cam, vector<int>*colModes, bool disp){


    ofRectangle dimensions;

    if(disp){dimensions = displayRect;} else{ dimensions = monitorRect;}

    t_cam.begin(dimensions);

        ofPushStyle();
            ofNoFill();
            ofSetColor(255);
            ofRect(0,0,50,50);
            ofCircle(25,25,1);
        ofPopStyle();

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


    t_cam.end();


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

	ofCircle(x,y,0.5,radius);
	ofNoFill();

	ofDisableAlphaBlending();
    ofPushStyle();
	ofSetLineWidth(lineWidth);
	ofSetHexColor(0x00ffff);
	ofCircle(x,y,radius);
	glPushMatrix();
	glTranslatef(x,y,0);
	glRotatef(t_obj->true_direction, 0,0,1);
	ofLine(0 - radius * x_overlap,0,0 + radius * x_overlap,0);
	ofLine(0, 0 - radius * y_overlap, 0, 0 + radius * y_overlap);
	ofPopStyle();
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
				if(selectedItem >= 2){
				    displayRect.width *= 1.01;
				    displayRect.height *=1.01;
				}


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
				if(selectedItem >= 2){
				    displayRect.width *= 0.99;
				    displayRect.height *= 0.99;
				}


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
