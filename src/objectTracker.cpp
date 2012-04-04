#include "objectTracker.h"

bool comparebyDistance (const distListItem a, const distListItem b) {
        return (a.dist < b.dist);
 }

objectTracker::objectTracker(){

    drawTracked = false;
    maxDist = 10;
    ghostFrames = 15;
    cleanWait = 20;
    edgeBuf = 3;

    selectedMenu = 0;
    selectedItem = 0;

    cleanCount = 0;

    //intialise finalObjects

    reset();

}


void objectTracker::reset(){

finalObjects.clear();

for(int col = 0; col < 4; col ++){

    vector<trackingObject> colObjects;

    for(int i = 0; i < 10; i ++){

    trackingObject temp;


    colObjects.push_back(temp);

    }

    finalObjects.push_back(colObjects);
}

}


void objectTracker::feedObjects(vector<vector<trackingObject> >tempObjects){

int nowCount = 0;
nowObjects.clear();
nowObjects = tempObjects;

        for(int col = 0; col < finalObjects.size(); col++){

            nowCount += nowObjects[col].size();

            for(int i = 0; i < finalObjects[col].size(); i++){

            finalObjects[col][i].setAssigned(false);


            }

        }

if(nowCount == 0 && !clean){
                    cleanCount += 1;
                    //printf("%i \n", cleanCount);
                    }else if(nowCount > 0){
                    //printf("not clean %i \n", nowCount);
                    clean = false;
                    cleanCount = 0;
                    }


for(int col = 0; col < finalObjects.size(); col++ ){
//create the distance list

updateDistanceList(col);

//assign to present or ghosted blobs

          for(int i = 0; i < distanceList.size(); i++){


              if(nowObjects[col][distanceList[i].np].getAssigned() == false &&
                 finalObjects[col].at(distanceList.at(i).fp).getAssigned() == false){

                    updateFinalBlob(&nowObjects[col][distanceList[i].np], &finalObjects[col][distanceList[i].fp]);

                 }
          }



//if nowPerformer still not allocated give empty assignment


    for(int i = 0; i < nowObjects[col].size(); i++){

            for(int j = 0; j < finalObjects[col].size(); j++){

                if(!nowObjects[col][i].getAssigned()){

                    if(!finalObjects[col][j].getAssigned() && finalObjects[col][j].getGhost() == 0 && !finalObjects[col][j].getPresent()){

                     // printf("assg %i %i %i\n ",col, j,ofGetFrameNum());
                        nowObjects[col][i].setAssigned(true);
                        finalObjects[col].at(j).setAssigned(true);
                        //finalObjects[col][j].setPresent(true);
                        finalObjects[col][j].ghostIncrement();
                        //finalObjects[col][j].setMoving(true);
                        finalObjects[col].at(j).setProjCentX(nowObjects[col][i].getProjCentX());
                        finalObjects[col].at(j).setProjCentY(nowObjects[col][i].getProjCentY());

                        finalObjects[col].at(j).updateHistory();

                        if(finalObjects[col][j].getProjCentX() > 50 - edgeBuf || finalObjects[col][j].getProjCentX() < edgeBuf
                       ||finalObjects[col][j].getProjCentY() > 50 - edgeBuf || finalObjects[col][j].getProjCentY() < edgeBuf
                       ){ finalObjects[col][j].setEdge(true);}else{finalObjects[col][j].setEdge(false);}

                    }
                }
            }

    }



 //now reset unallocated final performers

    for(int j = 0; j < finalObjects[col].size(); j++){

                if(!finalObjects[col].at(j).getAssigned() && finalObjects[col].at(j).getPresent()){


                if(finalObjects[col][j].getEdge()){
                    if(finalObjects[col].at(j).getGhost() > ghostFrames){
                        finalObjects[col].at(j).reset();
                        finalObjects[col].at(j).setAssigned(true);
                       // printf("reset %i %i %i\n ",col, j, ofGetFrameNum());


                    }else{
                        finalObjects[col].at(j).ghostIncrement();
                        //printf("incr %i %i %i\n ",col , j,ofGetFrameNum());
                    }
                }else{

                finalObjects[col][j].setAssigned(true);
                finalObjects[col][j].setMoving(false);

                }

                }

                if(!finalObjects[col].at(j).getAssigned() && !finalObjects[col].at(j).getPresent() && finalObjects[col][j].getGhost() > 0){

                    if(finalObjects[col].at(j).getGhost() > ghostFrames){
                        finalObjects[col].at(j).reset();
                        finalObjects[col].at(j).setAssigned(true);
                    //    printf("reset %i %i %i\n ",col, j, ofGetFrameNum());
                    }else{
                        finalObjects[col].at(j).ghostIncrement();
                      //   printf("incr %i %i %i\n ",col , j,ofGetFrameNum());
                    }

                }



            }


    }

}

void objectTracker::cleanUp(vector<vector<trackingObject> >tempObjects){

printf("clean \n");
    //int nowcount = 0;
    nowObjects.clear();
    nowObjects = tempObjects;

        for(int col = 0; col < finalObjects.size(); col++){

         //   nowcount += nowObjects[col].size();

            for(int i = 0; i < finalObjects[col].size(); i++){

            finalObjects[col][i].setAssigned(false);


            }

        }

//printf(" nowcount: %i \n", nowcount);

for(int col = 0; col < finalObjects.size(); col++ ){

    updateDistanceList(col);

//go through final objects and assign for related nows - ignore edge nows and ghosted finals(not yet implemented)

          for(int i = 0; i < distanceList.size(); i++){


              if(nowObjects[col][distanceList[i].np].getAssigned() == false &&
                 finalObjects[col].at(distanceList.at(i).fp).getAssigned() == false){

                    updateFinalBlob(&nowObjects[col][distanceList[i].np], &finalObjects[col][distanceList[i].fp]);

                 }
          }

//assign new finals for any unassigned nows - no ghosting

    for(int i = 0; i < nowObjects[col].size(); i++){

            for(int j = 0; j < finalObjects[col].size(); j++){

                if(!nowObjects[col][i].getAssigned()){

                    if(!finalObjects[col][j].getAssigned() && finalObjects[col][j].getGhost() == 0 && !finalObjects[col][j].getPresent()){

                     // printf("assg %i %i %i\n ",col, j,ofGetFrameNum());
                        nowObjects[col][i].setAssigned(true);
                        finalObjects[col].at(j).setAssigned(true);

                        finalObjects[col].at(j).setProjCentX(nowObjects[col][i].getProjCentX());
                        finalObjects[col].at(j).setProjCentY(nowObjects[col][i].getProjCentY());
                        finalObjects[col][j].setPresent(true);

                        finalObjects[col].at(j).updateHistory();
                        finalObjects[col][j].setMoving(false);

                        if(finalObjects[col][j].getProjCentX() > 50 - edgeBuf || finalObjects[col][j].getProjCentX() < edgeBuf
                       ||finalObjects[col][j].getProjCentY() > 50 - edgeBuf || finalObjects[col][j].getProjCentY() < edgeBuf
                       ){ finalObjects[col][j].setEdge(true);}else{finalObjects[col][j].setEdge(false);}

                    }
                }
            }

    }

//delete any old finals that don't have corresponding nows - no ghosting

    for(int j = 0; j < finalObjects[col].size(); j++){

                if(!finalObjects[col].at(j).getAssigned() && finalObjects[col].at(j).getPresent()){

                   //     printf("reset %i %i %i\n ",col, j, ofGetFrameNum());
                        finalObjects[col].at(j).reset();
                        finalObjects[col].at(j).setAssigned(true);

                            }
            }

    }


}

void objectTracker::updateDistanceList(int col){

    distanceList.clear();

        for(int i = 0; i < nowObjects[col].size(); i++){

            for(int j = 0; j < finalObjects[col].size(); j++){


            if(finalObjects[col].at(j).getPresent()|| finalObjects[col][j].getGhost()> 0){


                distListItem *tempItem = new distListItem;


                float distX = nowObjects[col].at(i).getProjCentX() - finalObjects[col][j].getProjCentX();
                float distY = nowObjects[col].at(i).getProjCentY() - finalObjects[col][j].getProjCentY();

               tempItem->dist = sqrt((distX * distX) + (distY * distY));

                tempItem->np = i;

                tempItem->fp = j;


                if(tempItem->dist < maxDist) {

                //give priority to moving fo's
                //if (!finalObjects[col][j].getMoving())tempItem->dist +=100;
                distanceList.push_back(*tempItem);
               // printf("pass, %i, %i, %f \n", col, j,tempItem->dist);

                }//else{printf("fail,%i, %i, %f \n", col, j, tempItem->dist);}

                delete tempItem;

                }


            }

        }


      if(distanceList.size() > 1){

      sort(distanceList.begin(),distanceList.end(), comparebyDistance);

      }



}

void objectTracker::updateFinalBlob(trackingObject *now, trackingObject *final){


                 final->setAssigned(true);
                 now->setAssigned(true);

                    final->setProjCentX(now->getProjCentX());
                    final->setProjCentY(now->getProjCentY());
                    final->updateHistory();

                    //printf("%f, %f \n",final->getAvPos().x, final->getAvPos().y);

                if(!final->getPresent()){

                   if(final->getGhost() > ghostFrames){
                    final->setPresent(true);
                    printf("set present \n");
                    final->ghostReset();

                    }else{

                    final->ghostIncrement();

                    }

                }else{

                     final->ghostReset();

                }


                    if(final->getProjCentX() > 50 - edgeBuf || final->getProjCentX() < edgeBuf
                       ||final->getProjCentY() > 50 - edgeBuf || final->getProjCentY() < edgeBuf
                       ){ final->setEdge(true);}else{final->setEdge(false);}

}



bool objectTracker::getClean(){

if(cleanCount > cleanWait){clean = true; cleanCount = 0; return clean; }else{

return false;
}

}


void objectTracker::draw(){

drawMenus();

ofSetColor(255,255,255);

string trackedString;

glPushMatrix();
glTranslatef(340,0,0);

    ofNoFill();
    ofRect(0,0,300,300);

    int offset = edgeBuf * 6;
    ofSetColor(0,0,255);
    ofRect(0 + offset/2, 0 + offset/2, 300 - offset, 300 - offset);
    ofSetColor(255,255,255);

    ofFill();
    for(int col = 0; col < 4; col++){

        trackedString = trackedString + "\n\n" + "colour: " + ofToString(col);

        for(int i = 0; i < finalObjects[col].size(); i ++){


        trackedString = trackedString + "\nobject: " + ofToString(i,0);

        if(finalObjects[col][i].getPresent() || finalObjects[col][i].getGhost() > 0){
            trackedString += "   present: " + ofToString(finalObjects[col][i].getPresent()) +
                  "  projX: " + ofToString(finalObjects[col][i].getProjCentX(),1) +
                   "   projY:" + ofToString(finalObjects[col][i].getProjCentY(),1)+
                   "   speed:" + ofToString(finalObjects[col][i].getSpeed(),1) +
                    "   angle:" + ofToString(finalObjects[col][i].getDirection(),1);}else{

                    trackedString += "   present: 0.0   projX: 0.0   projY: 0.0   speed: 0.0   angle: 0.0";
                    }


            if(finalObjects[col][i].getPresent()){

                ofxVec2f pos;
                pos = finalObjects[col][i].getAvPos();
                //pos.x = finalObjects[col][i].getProjCentX();
                //pos.y = finalObjects[col][i].getProjCentY();

                float x = pos.x * 6;
                float y = pos.y * 6;


                y = 300 -y;

                if(col == 0)ofSetColor(238,59,59); // red
                if(col == 1)ofSetColor(30,144,255); // blue
                if(col == 2)ofSetColor(255,255,0); //yellow
                if(col == 3)ofSetColor(34,139,34); // green


                ofCircle(x,y,10);

                ofSetColor(255,255,255);
                if(finalObjects[col][i].getEdge())ofSetColor(120,120,120);
                ofDrawBitmapString(ofToString(i),x-5,y-5);

                }
        }



    }

glPopMatrix();

    ofSetColor(255,255,255);

    if(drawTracked){
    ofDrawBitmapString(trackedString, 700,0);
    }

}

void objectTracker::drawMenus(){

glPushMatrix();
glTranslatef(30,400,0);

if(selectedMenu == 0){ofSetColor(0x00ffff);}else{ofSetColor(0xffffff);}
ofDrawBitmapString("Tracking Settings:",0,0);

  int maskMenuColors[5];

    for(int i = 0; i < 5; i++){

    if(selectedItem == i && selectedMenu == 0){
    maskMenuColors[i] = 0xff0000;
    }else{maskMenuColors[i] = 0xffffff;}

    }

glTranslatef(0,15,0);
ofSetColor(maskMenuColors[0]);
ofDrawBitmapString("ghostFrames:" + ofToString(ghostFrames,0),0,0);
glTranslatef(0,15,0);
ofSetColor(maskMenuColors[1]);
ofDrawBitmapString("maxDist:" + ofToString(maxDist,0),0,0);
glTranslatef(0,15,0);
ofSetColor(maskMenuColors[2]);
ofDrawBitmapString("edgeBuf:" + ofToString(edgeBuf,0),0,0);
glTranslatef(0,15,0);
ofSetColor(maskMenuColors[3]);
ofDrawBitmapString("cleanWait:" + ofToString(cleanWait,0),0,0);
glTranslatef(0,15,0);
ofSetColor(maskMenuColors[4]);
ofDrawBitmapString("drawTracked:" + ofToString(drawTracked,0),0,0);
ofSetColor(255,255,255);

glPopMatrix();

}

void objectTracker::saveConfig(ofxXmlSettings XML, int tagNum){

 tagNum	= XML.addTag("OBJECTTRACKER");

    if( XML.pushTag("OBJECTTRACKER", tagNum) ){

    XML.setValue("MAXDIST", maxDist, tagNum);
    XML.setValue("GHOSTFRAMES", ghostFrames, tagNum);
    XML.setValue("EDGEBUF", edgeBuf, tagNum);
    XML.setValue("CLEANWAIT", cleanWait, tagNum);

    XML.popTag();

    }

}

void objectTracker::openConfig(ofxXmlSettings XML){

if( XML.pushTag("OBJECTTRACKER", 0) ){

    maxDist = XML.getValue("MAXDIST", 0);
    ghostFrames = XML.getValue("GHOSTFRAMES", 0);
    edgeBuf = XML.getValue("EDGEBUF", 0);
    cleanWait = XML.getValue("CLEANWAIT", 0);

    XML.popTag();

    }

}

void objectTracker::keyPressed(int key){

switch (key){

    case' ':
    reset();
    break;

    case OF_KEY_PAGE_UP:
    selectedMenu += 1; selectedMenu = selectedMenu%1;
    break;

    case OF_KEY_PAGE_DOWN:
    selectedItem += 1;
    if(selectedMenu == 0)selectedItem = selectedItem%5;
    break;

    case OF_KEY_UP:
    if(selectedMenu == 0){
        if(selectedItem == 0)ghostFrames += 1;
        if(selectedItem == 1)maxDist += 1;
        if(selectedItem == 2)edgeBuf += 1;
        if(selectedItem == 3)cleanWait += 1;
        if(selectedItem == 4)drawTracked = true;


    }

    break;

    case OF_KEY_DOWN:
    if(selectedMenu == 0){
        if(selectedItem == 0)ghostFrames -= 1;
        if(selectedItem == 1)maxDist -= 1;
        if(selectedItem == 2)edgeBuf -= 1;
        if(selectedItem == 3)cleanWait -= 1;
         if(selectedItem == 4)drawTracked = false;

    }
    break;

	}

}



vector<vector<trackingObject> > objectTracker::getFinals(){

 return finalObjects;

}


