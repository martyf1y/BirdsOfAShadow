//
//  BirdBox.cpp
//  
//
//  Created by Jenna Gavin on 17/02/15.
//
//

#include "BirdBox.h"

BirdBox::BirdBox(int tx, int ty, int tW, int tH, int myID, ofImage myPic, ofImage myColourPic) {
    
    iID = myID;
    
    pic = myPic;
    picBird = myColourPic;
    
    x = tx;
    y = ty;
    
    w = tW;
    h = tH;
    
    myBGColour = ofColor(ofRandom(50,255),50,ofRandom(100,200));
    
    picSwitchTime = ofRandom(minTime,maxTime);
    
    flipboard.loadSound("sounds/page.mp3");
    flip = false;
    wave = 1;
}

void BirdBox::update(Boolean flipChange) {
    // cout << "FLIP " << flip <<  endl;
    if (flip ==false && picSwitchTime < ofGetElapsedTimeMillis() - picSwitchTimeElapsed ) {
       
        pictureSwitch=!pictureSwitch;
        
        flip = true;
       
        flipboard.play();

          //flip = !flip;

    }
    
    
    if (flipChange){
      //  flip = !flip;
       // flipboard.play();
        //cout << "flip "<< endl;
    }
}


void BirdBox::display() {
    
    //ofPushMatrix();
    //ofEnableAlphaBlending();

    //Draw Rect
    ofSetColor(myBGColour);
    int border = 2;
    //ofRect(x+border,y+border,w-border*2,h-border*2);
    
    //Draw Bird
    ofImage drawPic;
    
   // ofSetColor(255,255,255);
    if (pictureSwitch) {
        drawPic = pic;
    } else {
        drawPic = pic;
    }
    
    //drawPic.draw(x+(w*0.5)-((pic.width*picScale)*0.5),y+20,picBird.width*picScale, picBird.height*picScale);
    
    flipImage();
    
   // ofDisableAlphaBlending();
    //ofPopMatrix();
}

void BirdBox::setNewImages(ofImage birdShape, ofImage realBird) {
    pic = realBird;
    
    picBirdShape = birdShape;
    
    picSwitchTime = ofRandom(minTime,maxTime);
    picSwitchTimeElapsed = ofGetElapsedTimeMillis();
    flip = false;
}

void BirdBox::setTimer() {
    //flip = false;
}


void BirdBox::flipImage() {
    
 
    // float wave = sin(ofGetElapsedTimef());
    const float friction = 1.45;
    
    if(flip == true){
        wave *= friction;
        if(wave >= 181){
            wave = 181;
        }
    }
    if(flip == false){
        wave = 1;
        //wave /= friction;
        //if(wave <= 1){
         //   wave = 1;
        //}
    }
    
    
    // Well there it is... all the imagery in the boxes ... ....
    
    ofPushMatrix();
    ofTranslate(picBird.width/2, pic.height/2+pic.height/4);
    
    ofSetColor(0,0,0);
    ofRect(x-pic.width/2,y-pic.height/4, pic.width, pic.height/2);
    ofSetColor(255,255,255);
    
    pic.drawSubsection(x-pic.width/2,y-pic.height/4, pic.width, pic.height/2, 0 , pic.height/2); /// This is the bottom first
   
    
    
    //Background
     picBirdShape.drawSubsection(x-picBird.width/2,y-picBird.height/2-picBird.height/4, picBird.width, picBird.height/2, 0, 0);
    ofSetColor(255,255,255);
    

    ofPushMatrix();
    
    ofTranslate(x-picBird.width/2, y-picBird.height/4);
    ofRotate(181 - wave, 1.0, 0, 0);
    
    picBirdShape.drawSubsection(0, 0, picBird.width, picBird.height/2, 0, picBird.height/2); // bottom second
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(x-pic.width/2, y-pic.height/4);
    ofRotate(1 - wave, 1.0, 0, 0);
    
    ofSetColor(255, 255, 255);
    if(wave <= 90){ // This stops it being drawn after the halfway mark
            ofSetColor(255,255,255);
        pic.drawSubsection(0, -pic.height/2, pic.width, pic.height/2, 0, 0); // This is the top first
    }
    ofPopMatrix();
    
    ofPopMatrix();
}



/* original ...original
 ofSetColor(0,200,0);
 ofRect(x-pic.width/2,y-pic.height/4, pic.width, pic.height/2);
 ofSetColor(255,255,255);
 
 pic.drawSubsection(x-pic.width/2,y-pic.height/4, pic.width, pic.height/2, 0 , pic.height/2); /// This is the bottom first
 
 ofSetColor(200,0,0);
 ofRect(x-picBird.width/2,y-picBird.height/2-picBird.height/4, picBird.width, picBird.height/2);
 ofSetColor(255,255,255);
 picBird.drawSubsection(x-picBird.width/2,y-picBird.height/2-picBird.height/4, picBird.width, picBird.height/2, 0, 0); /// This is the top second
 
 ofPushMatrix();
 
 ofTranslate(x-picBird.width/2, y-picBird.height/4);
 ofRotate(181 - wave, 1.0, 0, 0);
 ofSetColor(200,0,0);
 ofRect(0, 0, picBird.width, picBird.height/2);
 ofSetColor(255,255,255);
 
 picBird.drawSubsection(0, 0, picBird.width, picBird.height/2, 0, picBird.height/2); // This is the bottom second
 ofPopMatrix();
 
 ofPushMatrix();
 ofTranslate(x-pic.width/2, y-pic.height/4);
 ofRotate(1 - wave, 1.0, 0, 0);
 
 ofSetColor(255, 255, 255);
 if(wave <= 90){ // This stops it being drawn after the halfway mark
 ofSetColor(0,200,0);
 ofRect(0, -pic.height/2, pic.width, pic.height/2);
 ofSetColor(255,255,255);
 pic.drawSubsection(0, -pic.height/2, pic.width, pic.height/2, 0, 0); // This is the top first
 }
 ofPopMatrix();
*/
