//
//  BirdBox.h
//  alphabet
//
//  Created by Jenna Gavin on 17/02/15.
//
//

#ifndef __alphabet__BirdBox__
#define __alphabet__BirdBox__

#include <iostream>
#include "ofMain.h"

class BirdBox {
    
public:
    
    BirdBox(int tx, int ty, int tW, int tH, int myID, ofImage myPic, ofImage myColourPic);
    
    void update(Boolean flipChange);
    void display();
    int getX();
    int getY();
    
    void flipImage();
    void setNewImages(ofImage birdShape, ofImage realBird);
    void setTimer();
    
    //variables
    int x=0;
    int y=0;
    int w;
    int h;
    int iID;
    
    ofImage pic;
    ofImage picBird;
    ofImage picBirdShape;
 //   ofImage picBirdCutout;
    
 //   ofColor colBG;
 //   ofColor colDepth;
    
    ofColor myBGColour;
    
    Boolean pictureSwitch = false;
    
    //timer for switch between shadow and coloured pic
    float picSwitchTime = 5000;
    float picSwitchTimeElapsed = 0;
    int minTime=5000;
    int maxTime=8000;
    
    float picScale = 0.9;
    
    ofImage imgFold;
	float wave;
    bool flip;
    
    ofSoundPlayer flipboard;
    
};

#endif /* defined(__alphabet__BirdBox__) */
