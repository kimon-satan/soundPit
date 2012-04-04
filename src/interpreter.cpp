#include "interpreter.h"

interpreter::interpreter()
{

	sender.setup( HOST, PORT );

	for(int col = 0; col < 4; col ++){
		vector<bool> temp;

		for(int i = 0; i < 10; i ++){
			movingArray[col][i] = false;
			bool val = false;
			temp.push_back(val);
		}
		nowCollisions.push_back(temp);

		colTrs[col] = 0;
	}

	collDist = 3.0f;
	collSpeedThresh = 5.0f;
	selectedItem = 0;
	selectedMenu = 0;
	maxCollisions = 10;
	maxSynths = 15;
	synthsRunning = 0;
	speed_scale = 15;



}

void interpreter::sendInitialise(){

	ofxOscMessage m;
	m.setAddress( "/init" );
	sender.sendMessage(m);
	m.clear();

}

vector<int>* interpreter::getColModes(){

	return colMode;

}


void interpreter::setTrPtrs(vector<patch> * pnt_ptr, vector<int>* md_ptr){

	transRects = pnt_ptr;
	colMode = md_ptr;

}

void interpreter::feedObjects(trackingObject t_objs[][10], int size, bool newFrame){





	bool isStill = true;

	for(int col = 0; col < 4; col ++)
	{

		for(int i = 0; i < 10; i ++)if(t_objs[col][i].present && t_objs[col][i].moving)isStill = false;

	}

	if(!isStill)for(int col = 0; col < 4; col ++)isTransforming[col] = false; // meaning only one transformation per col per still moment

	for(int tr = 0; tr < 4; tr++){
		transRects->at(tr).isFilled = false;

	}



	for(int col = 0; col < 4; col ++)
	{

		for(int i = 0; i < 10; i ++)
		{

			nowCollisions[col][i]=false;

			if(t_objs[col][i].present)
			{

				if(newFrame)t_objs[col][i].updateHistory();
				t_objs[col][i].updateDrawHistory();

				if(!t_objs[col][i].moving && isStill)
				{

					ofPoint avPos = ofPoint(t_objs[col][i].avPos.x, t_objs[col][i].avPos.y);

					for(int tr = 0; tr < 4; tr++)
					{

						if(
						   (avPos.x > transRects->at(tr).rect.x - (transRects->at(tr).rect.width/2)
							&& avPos.x < transRects->at(tr).rect.x + (transRects->at(tr).rect.width/2))&&
						   (avPos.y > transRects->at(tr).rect.y  - (transRects->at(tr).rect.height/2) &&
							avPos.y < transRects->at(tr).rect.y + (transRects->at(tr).rect.height/2))
						   ){

							transRects->at(tr).isFilled = true;

							if(transRects->at(tr).isLive == 45 && colTrs[col] != tr && !isTransforming[col]){

								colTrs[col] = tr;
								isTransforming[col] = true;
								transRects->at(tr).isLive = 46;
								colMode->at(col) = tr;
								transRects->at(tr).count = 90;
								transRects->at(tr).col = col;

								//changes sound set + plays transforming sound
								ofxOscMessage m;
								m.setAddress( "/transformOn" );
								m.addIntArg(col);
								m.addIntArg(tr);
								sender.sendMessage(m);
								m.clear();

								//turnoff any current moving synths for that colour
								for(int st = 0; st < 10; st ++){

									if(movingArray[col][st]){

										movingArray[col][st] = false;
										synthsRunning -= 1;

										ofxOscMessage m;
										m.setAddress( "/still" );
										m.addIntArg(col);
										m.addIntArg(st);

										sender.sendMessage(m);
										m.clear();

									}
								}//st



							}else if(transRects->at(tr).isLive < 45 && colTrs[col] != tr && !isTransforming[col]){

								transRects->at(tr).isLive = transRects->at(tr).isLive + 1;

							}

						}//if inside
					}//tr
				} //moving
			} //present
		} //i
	}	//col



	if(newFrame){

		for(int col = 0; col < 4; col ++){

			if(!isTransforming[col])
			{

				for(int i = 0; i < 10; i ++){

					if(!movingArray[col][i] &&
					   t_objs[col][i].moving &&
					   t_objs[col][i].present &&
					   synthsRunning < maxSynths){


						movingArray[col][i] = true;
						synthsRunning += 1;
						ofPoint index;
						index.set(col,i);
						expiredCollisions.push_back(index);

						ofVec2f pos = t_objs[col][i].avPos;
						ofPoint mid;
						mid.set(25,25);

						float offCent  = pos.distance(mid)/36;
						offCent = min(offCent, 1.0f);
						offCent = max(offCent, 0.01f);
						float speed = t_objs[col][i].speed/speed_scale;
						speed = min(speed, 1.0f);
						speed = max(speed, 0.01f);
						float angle = t_objs[col][i].direction/180;
						angle = min(angle, 1.0f);
						angle = max(angle, 0.01f);

						ofxOscMessage m;
						m.setAddress( "/move" );
						m.addIntArg(col);
						m.addIntArg(i);
						m.addFloatArg(speed); //speed
						m.addFloatArg(angle); //angle
						m.addFloatArg(offCent); //position
						sender.sendMessage(m);
						m.clear();



					}else if((movingArray[col][i] && !t_objs[col][i].moving) ||
							 (movingArray[col][i] && !t_objs[col][i].present)){

						movingArray[col][i] = false;
						synthsRunning -= 1;

						ofxOscMessage m;
						m.setAddress( "/still" );
						m.addIntArg(col);
						m.addIntArg(i);

						m.addIntArg(t_objs[col][i].moving);
						m.addIntArg(t_objs[col][i].present);
						sender.sendMessage(m);
						m.clear();


					}else if(movingArray[col][i] == true && t_objs[col][i].moving){

						ofVec2f pos = t_objs[col][i].avPos;
						ofPoint mid;
						mid.set(25,25);

						float offCent  = pos.distance(mid)/36;
						offCent = min(offCent, 1.0f);
						offCent = max(offCent, 0.01f);
						float speed = t_objs[col][i].speed/speed_scale;
						speed = min(speed, 1.0f);
						speed = max(speed, 0.01f);
						float angle = t_objs[col][i].direction/180;
						angle = min(angle, 1.0f);
						angle = max(angle, 0.01f);

						ofxOscMessage m;
						m.setAddress( "/update" );
						m.addIntArg(col);
						m.addIntArg(i);
						m.addFloatArg(speed); //speed
						m.addFloatArg(angle); //angle
						m.addFloatArg(offCent);

						sender.sendMessage(m);
						m.clear();

					} //if series

					trackingObject *selectObj;

					if(t_objs[col][i].moving && t_objs[col][i].present && colTrs[col] == 3){

						if(t_objs[col][i].speed > collSpeedThresh){
							selectObj = &t_objs[col][i];
							detectCollision(selectObj,col,i, t_objs);
						}
					}

				}//i

			} //isTrans
		}//col


		//delete old collisions here...

		for(int i =0; i < pastCollisions.size(); i++){

			if(pastCollisions[i].time > 40){
				pastCollisions.erase(pastCollisions.begin()+i);
			}else{ pastCollisions[i].time +=1;}

		}

		for(int tr = 0; tr < 4; tr++){

			if(transRects->at(tr).count > 0){
				transRects->at(tr).count -= 1;
			}

			if(!transRects->at(tr).isFilled)transRects->at(tr).isLive = 0;
		}


	}//newFrame

}

void interpreter::detectCollision(trackingObject* selectObj, int o_col, int o_i, trackingObject t_objs[][10]){

	int collisionCount = 0;

	for(int col = 0; col < 4; col ++){
		for(int i = 0; i < 10; i ++){

			ofVec2f temp1 = selectObj->avPos;


			if((col != o_col || i != o_i) && (t_objs[col][i].present) && (!t_objs[col][i].moving)){

				float dist;

				ofVec2f temp2 = t_objs[col][i].avPos;
				ofPoint temp3;
				temp3.set(temp2.x,temp2.y);
				dist = temp1.distance(temp3);
				ofPoint location = temp1.getMiddle(temp2);

				if(dist < collDist){

					bool past = false;

					for(int thisC = 0; thisC < pastCollisions.size(); thisC ++){

						if(
						   (pastCollisions[thisC].obj1[0] == col &&
							pastCollisions[thisC].obj1[1] == i &&
							pastCollisions[thisC].obj2[0] == o_col &&
							pastCollisions[thisC].obj2[1] == o_i)

						   ||

						   (pastCollisions[thisC].obj1[0] == o_col &&
							pastCollisions[thisC].obj1[1] == o_i &&
							pastCollisions[thisC].obj2[0] == col &&
							pastCollisions[thisC].obj2[1] == i)

						   ){

							past = true;

						}

					}

					if(!past){

						if(collisionCount < 2 && pastCollisions.size() < maxCollisions){

							collisionCount +=1;

							ofPoint mid;
							mid.set(25,25);
							float offCent = temp1.distance(mid)/36;
							offCent = min(offCent, 1.0f);
							offCent = max(offCent, 0.01f);
							float speed = selectObj->speed/speed_scale;
							speed = min(speed, 1.0f);
							speed = max(speed, 0.01f);

							collision tempCollision;
							tempCollision.obj1[0] = col; tempCollision.obj1[1] = i;
							tempCollision.obj2[0] = o_col; tempCollision.obj2[1] = o_i;
							tempCollision.repeat = true;
							tempCollision.time = 0;
							tempCollision.power = selectObj->speed;
							tempCollision.location = location;


							pastCollisions.push_back(tempCollision);
							nowCollisions[col][i] = true;
							nowCollisions[o_col][o_i]=true;

							ofxOscMessage m;
							m.setAddress( "/collision" );
							m.addIntArg(o_col); //this is the relevant obj
							m.addIntArg(col);
							m.addFloatArg(offCent);
							m.addFloatArg(speed);
							sender.sendMessage(m);
							m.clear();
						}

					}

				}
			}

		}
	}

}




void interpreter::draw(trackingObject t_objs[][10], int size){

	ofSetColor(255,255,255);

	string trackedString;

	glPushMatrix();
	glTranslatef(340,0,0);


	for(int col = 0; col < 4; col++){

		if(transRects->at(col).count > 0){

			ofEnableAlphaBlending();

			float alpha = transRects->at(col).count * 2;
			if(transRects->at(col).col == 0)ofSetColor(238,59,59,alpha); // red
			if(transRects->at(col).col == 1)ofSetColor(30,144,255,alpha); // blue
			if(transRects->at(col).col == 2)ofSetColor(255,255,0,alpha); //yellow
			if(transRects->at(col).col == 3)ofSetColor(34,139,34,alpha); // green


			ofSetRectMode(OF_RECTMODE_CENTER);
			ofRect(transRects->at(col).rect.x * 6, (50 - transRects->at(col).rect.y) * 6,
				   transRects->at(col).rect.width * 6 ,transRects->at(col).rect.height * 6);
			ofSetRectMode(OF_RECTMODE_CORNER);

			ofDisableAlphaBlending();

		}

		trackedString = trackedString + "\n\n" + "colour: " + ofToString(col);

		for(int i = 0; i < 10; i ++){


			trackedString = trackedString + "\nobject: " + ofToString(i,0);

			if(t_objs[col][i].present){
				trackedString += "   present: " + ofToString(t_objs[col][i].present) +
				"  projX: " + ofToString(t_objs[col][i].projPos.x,1) +
				"   projY:" + ofToString(t_objs[col][i].projPos.y,1)+
				"   speed:" + ofToString(t_objs[col][i].speed,1) +
				"   angle:" + ofToString(t_objs[col][i].direction,1);}else{

					trackedString += "   present: 0.0   projX: 0.0   projY: 0.0   speed: 0.0   angle: 0.0";
				}


			if(t_objs[col][i].present){

				ofVec2f pos;
				pos = t_objs[col][i].avPos;
				//pos.x = t_objs[col][i].getProjCentX();
				//pos.y = t_objs[col][i].getProjCentY();

				float x = pos.x * 6;
				float y = pos.y * 6;


				y = 300 -y;


				if(col == 0)ofSetColor(238,59,59); // red
				if(col == 1)ofSetColor(30,144,255); // blue
				if(col == 2)ofSetColor(255,255,0); //yellow
				if(col == 3)ofSetColor(34,139,34); // green


				ofCircle(x,y,10);

				ofSetColor(255,255,255);
				ofDrawBitmapString(ofToString(i),x-5,y-5);

			}
		}



	}

	ofSetColor(255,255,255);

	ofNoFill();
	ofRect(0,0,300,300);

	ofSetRectMode(OF_RECTMODE_CENTER);
	for(int r = 0; r < 4; r ++){
		ofRect(transRects->at(r).rect.x * 6, (50 - transRects->at(r).rect.y) * 6,
			   transRects->at(r).rect.width * 6, transRects->at(r).rect.height * 6);
	}
	ofSetRectMode(OF_RECTMODE_CORNER);


	ofFill();


	glPopMatrix();

	ofDrawBitmapString(trackedString, 660,0);


}

void interpreter::saveConfig(ofxXmlSettings XML, int tagNum){

	tagNum	= XML.addTag("INTERPRETER");

	if( XML.pushTag("INTERPRETER", tagNum) ){

		XML.setValue("COLLDIST", collDist, tagNum);
		XML.setValue("MAXSYNTHS", maxSynths, tagNum);
		XML.setValue("MAXCOLL", maxCollisions, tagNum);
		XML.setValue("SPEEDSCALE", speed_scale, tagNum);
		XML.setValue("COLLSPEEDTHRESH", collSpeedThresh, tagNum);

		XML.popTag();

	}

}

void interpreter::openConfig(ofxXmlSettings XML){


	if( XML.pushTag("INTERPRETER", 0) ){

		collDist  = XML.getValue("COLLDIST", 0);
		maxSynths = XML.getValue("MAXSYNTHS", 0);
		maxCollisions = XML.getValue("MAXCOLLISIONS",0);
		speed_scale = XML.getValue("SPEEDSCALE", 10.0f);
		collSpeedThresh = XML.getValue("COLLSPEEDTHRESH",5.0f);

		XML.popTag();

	}

}

void interpreter::drawMenus(){

	glPushMatrix();
	glTranslatef(30,300,0);

	if(selectedMenu == 0){ofSetColor(0x00ffff);}else{ofSetColor(0xffffff);}
	ofDrawBitmapString("Interpreter Settings:",0,0);

	int maskMenuColors[5];

	for(int i = 0; i < 5; i++){

		if(selectedItem == i && selectedMenu == 0){
			maskMenuColors[i] = 0xff0000;
		}else{maskMenuColors[i] = 0xffffff;}

	}

	glTranslatef(0,15,0);
	ofSetColor(maskMenuColors[0]);
	ofDrawBitmapString("Collision Distance:" + ofToString(collDist,2),0,0);
	glTranslatef(0,15,0);
	ofSetColor(maskMenuColors[1]);
	ofDrawBitmapString("MaxSynths:" + ofToString(maxSynths,0),0,0);
	glTranslatef(0,15,0);
	ofSetColor(maskMenuColors[2]);
	ofDrawBitmapString("MaxCollisions:" + ofToString(maxCollisions,0),0,0);
	glTranslatef(0,15,0);
	ofSetColor(maskMenuColors[3]);
	ofDrawBitmapString("CollSpeedThresh:" + ofToString(collSpeedThresh,2),0,0);
	glTranslatef(0,15,0);
	ofSetColor(maskMenuColors[4]);
	ofDrawBitmapString("SpeedScale:" + ofToString(speed_scale,2),0,0);
	glPopMatrix();

	ofSetColor(255,255,255);

}

void interpreter::keyPressed(int key){

	switch (key){

		case ',':
			//selectedMenu += 1; selectedMenu = selectedMenu%1;
			break;

		case '.':
			selectedItem += 1;
			selectedItem = selectedItem%5;
			break;

		case OF_KEY_UP:
			if(selectedMenu == 0){
				if(selectedItem == 0)collDist += 0.1;
				if(selectedItem == 1)maxSynths += 1;
				if(selectedItem == 2)maxCollisions +=1;
				if(selectedItem == 3)collSpeedThresh += 0.1;
				if(selectedItem == 4)speed_scale += 1;
			}

			break;

		case OF_KEY_DOWN:
			if(selectedMenu == 0){
				if(selectedItem == 0)collDist -= 0.1;
				if(selectedItem == 1)maxSynths -= 1;
				if(selectedItem == 2)maxCollisions -=1;
				if(selectedItem == 3)collSpeedThresh -= 0.1;
				if(selectedItem == 4)speed_scale -= 1;

			}
			break;

	}

}

vector<collision>* interpreter::getCollisions(){

	return &pastCollisions;

}

interpreter::~interpreter()
{
	//dtor
}
