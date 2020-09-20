#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // ofSetDataPathRoot("../Resources/data/");
    //Setup Kinect
    ofSetLogLevel(OF_LOG_VERBOSE);
    // ofSetFrameRate(30);
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    
    //Some sort of register but it makes things look worse
    //openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.start();
    
    //Load font
    descriptionFont.loadFont(ofToDataPath("RegencieLightAlt.ttf"), 14);
    
    //Set text names for birds
    loadBirdText();
    
    //Set array of background colours
    setupColorPalette();
    changeColor();
    
    //Set up image array for previous plays
    for (int i = 0; i < birdCatalogSize; i++) {
        //store previous images
        previousPlayBird.push_back( vector<ofImage>());
        }
    
    //Load bird images, sounds
    loadImages();
    loadSounds();
    loadNewBirdImage();
    
    //Setup the 12 grid for Display/Passive mode
    setupGrid();
    
    //Set scale, measurements for display
    imgScale = 0.50;
    gameBirdW = currentBirdImage.width;
    gameBirdH = currentBirdImage.height;
    birdRect = ofRectangle(0, 0, gameBirdW*imgScale, gameBirdH*imgScale); // This is the rectangle for the main screen
        
    state = Passive;
    
    //set initial depths
    openNIDevice.globalMinDepth = minDepthSetter;
    openNIDevice.globalMaxDepth = maxDepthSetter;
    
    //cursor issues
    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::setupGrid(){
    
    boxW = dimWidth/numCols;
    boxH = dimHeight/numRows;
    
    //Set up grid and objects
    int num = 0;
    int border = 0;
    
    //images
    ofImage imgShadow;
    ofImage imgColour;
    
    //Grid:
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {
            
            //Set colour
            colourBoxArray[num]= ofColor(ofRandom(100,254),ofRandom(50),ofRandom(50,154));
            
            //Set x and y
            int tX = boxW*j+border;
            int tY = boxH*i+border;
            
            //Retrieve images from file
            string pngName;
            
            int imgNum=0;
            if (num<12) {
                imgNum = num;
            } else {
                imgNum = ofRandom(0,5);
            }
            
            //dummy pix, are shown at start
            imgShadow = birdCatalog[num];
            imgColour = birdCatalog[num];
            imgShadow.setColor(0,0,0);
            imgShadow.resize(boxW,boxH);
            imgColour.resize(boxW,boxH);
            //Create Bird Box object
            BirdBox *b = new BirdBox(tX, tY, boxW, boxH, num, imgShadow, imgColour);
            
            birdBoxes.push_back(b);
            
            //next bird
            num++;
        }
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    openNIDevice.update();
    
    switch(state){
            
            /////////////////////////// Passive ///////////////////////////////
        case Passive:{
            
           int totalBoxes = numRows*numCols;
            
            //Update objects in grid
            for (int i=0; i<totalBoxes; i++) {
                birdBoxes[i]->update(flipStatus);
            }

        }break;
            
            /////////////////////////// Playing ///////////////////////////////
        case Playing:{
  
    }break;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    switch(state){
        
            /////////////////////////// Passive ///////////////////////////////
        case Passive:{
            if(saveImage){
                savePlayerImages();
                saveImage = false;
                gridOut = true; // Just put it here to make sure it works
            }
            displayFrozenImage();
                       // Timer
            triggerTimer =  ofGetElapsedTimeMillis() - triggerStartTime;
            

            //////////// Grid time
            if(triggerTimer >= 12000 && gridOut){ // Give players some time to view all of the grid
                if (currentBirdSound.getIsPlaying()) {
                    currentBirdSound.stop();
                }
                loadNewBirdImage();
                changeColor();

                ofSetColor(randomCol);
                ofRect(0,0, dimWidth, dimHeight); // The main space
                
                ofSetColor(1, 1, 1, 254); // DONT GO FULL BLACK IN CASE IMAGE USES IT!!!
                currentBirdImage.draw(0,0, dimWidth, dimHeight); // Silhouette
                ofSetColor(255);
                
                beforeDepthTrigger.grabScreen(0,0, dimWidth, dimHeight);
         
                /// Lets do it again!!!
                int num=0;
                //update the grid
                for (int i=0; i<numRows; i++) {
                    for (int j=0; j<numCols; j++) {
                                                //Calculate x and y
                        int tX = boxW*j;
                        int tY = boxH*i;
                        
                        // Chop up the current image on screen.
                        // Into 12 separate pieces so we can turn them over
                        ofImage choppedBird = greyOnScreenGrab;
                        
                        choppedBird.crop(tX,tY,boxW,boxH);
                       
                        ofImage shrunkBird = beforeDepthTrigger;
                        shrunkBird.crop(tX,tY,boxW,boxH);

                        birdBoxes[num]->setNewImages(shrunkBird, choppedBird);
                        
                        num++;
                    }
                }
                gridOut = false;
            }
            
                      ////////// TIME: Grid! Start flippin'
            if(triggerTimer >= 3000){
                drawGrid();
                if(triggerTimer >= 11500 && gridOut){
                    greyOnScreenGrab.grabScreen(0, 0, dimWidth, dimHeight); // This is the image of the 12 screens
                }
            }
            
            if (triggerTimer < 7500){
                int textLength = birdName[currentImageNumber].length();
                descriptionFont.drawString(birdName[currentImageNumber], dimWidth - (textLength*12+10), dimHeight-30);
            }
           
            //////////// TIME: Done! Switch to PLAY mode
            if(triggerTimer >= 21000){ // Give players some time to view all of the grid
                //Stop the sounds
             

                state = Playing;
                triggerStartTime = ofGetElapsedTimeMillis();
                playTrigger=false;
            }

        } break;
            
            ///////////////////////////Playing///////////////////////////////
        case Playing:{
            
            ofSetColor(randomCol);
            ofRect(birdRect); // The main space
            
            ofSetColor(1, 1, 1, 254); // DONT GO FULL BLACK IN CASE IMAGE USES IT!!!
            currentBirdImage.draw(birdRect); // Silhouette
            ofSetColor(255);
            
            beforeDepthTrigger.grabScreen(birdRect.x, birdRect.y, birdRect.width, birdRect.height);

            ofSetColor(randomColDepth);
            openNIDevice.drawDepth(-20, 0, birdRect.width + 20, birdRect.height);
            
            grabImg.grabScreen(birdRect.x, birdRect.y, birdRect.width, birdRect.height); // This is the image with depth

            Boolean setReadySwitch;
            setReadySwitch = colorComparison(beforeDepthTrigger, // before depth
                            currentBirdImage, // The bird image
                            birdRect.x,
                            birdRect.y,
                            birdRect.width,
                            birdRect.height);//We compare the two images together and see if they are the same
            
            if (!setReadySwitch) {
                //reset the ready to play timer...
                readyTimerElapsed=ofGetElapsedTimeMillis();
            }
            
            if (readyTimer < ofGetElapsedTimeMillis() - readyTimerElapsed) { 
                readyToPlay=true;
            }
            
            lastFrameReady = readyToPlay;
            
            /////////////////////////// THE ACTUAL IMAGE ///////////////////////////
            ofSetColor(50, 50, 50);
            ofRect(birdRect); // The main space
            ofSetColor(255, 255, 255);
            grabImg.draw(0, 0, dimWidth, dimHeight); // The after depth image
            
            // 1/3 top rectangle cover up (Aotea-specific roof fix hack)
            if (roofHax) {
                ofSetColor(randomCol);
                ofRect(0,0,dimWidth, roofRectHeight);
            }
          /*  Boolean isThereSomething = false;
            isThereSomething = colorComparison(beforeDepthTrigger, // before depth
                                               grabImg, // The bird image
                                               birdRect.x,
                                               birdRect.y,
                                               birdRect.width,
                                               birdRect.height);//We compare the two images together and see if they are the same
            

            if (debugMode) {
                displayDebugText();
            }*/
            
            // Wait until triggered
            if (!readyToPlay)  {
                triggerStartTime = ofGetElapsedTimeMillis();
            }
            
            // Timer
            triggerTimer =  absolutePlayTime - (ofGetElapsedTimeMillis() - triggerStartTime);
            
            //Indicate time left to play
            if(triggerTimer >= 0){ // Don't want dimmer to be there
            displayTimeIndicator();
                ofSetColor(255, 255, 255);
                savedShape.draw(0, 0, dimWidth/imgScale, dimHeight/imgScale); // The image created by the depth
                greyOnScreenGrab.grabScreen(0, 0, dimWidth, dimHeight); // This is the image with depth
            }
            else{
                ofSetColor(255, 255, 255);
                savedShape.draw(0, 0, dimWidth/imgScale, dimHeight/imgScale); // The image created by the depth
            }
                          // Time up!
                /////////////////////////// Capture images, switch to Passive/Display mode ///////////////////////////
                if(triggerTimer <= 0){
                    readyToPlay=false;
                    triggerStartTime = ofGetElapsedTimeMillis();
              //      cout << "BAM!" << endl;
                    Boolean isThereSomething = false;
                    isThereSomething = colorComparison(beforeDepthTrigger, // before depth
                                                       grabImg, // The bird image
                                                       birdRect.x,
                                                       birdRect.y,
                                                       birdRect.width,
                                                       birdRect.height);//We compare the two images together and see if they are the same
                    

                   // displayFrozenImage();
                                  //     cout << percentage << endl;
                    if(percentage < 16)
                    {
                        playTrigger=false;
                        if (currentBirdSound.getIsPlaying()) {
                            currentBirdSound.stop();
                            displayFrozenImage();
                        }
                    }
                    else{
                      //  cout << "TRUE? " << endl;

                        //////// I PUT THIS IN AGAIN TO RESET SAVED SHAPE ////////////
                        Boolean setReadySwitch;
                        setReadySwitch = colorComparison(beforeDepthTrigger, // before depth
                                                         currentBirdImage, // The bird image
                                                         birdRect.x,
                                                         birdRect.y,
                                                         birdRect.width,
                                                         birdRect.height);//We compare the two images together and see if they are the same
                        
                    state = Passive;
                   
                    //Display bird description over top
                    int textLength = birdName[currentImageNumber].length();
                    ofSetColor(0);
                    actualOnScreenGrab.grabScreen(0, 0, dimWidth, dimHeight); // This is the proper image
                    displayFrozenImage();
                    descriptionFont.drawString(birdName[currentImageNumber], dimWidth - (textLength*12+10), dimHeight-30);
                    saveImage = true; // This allows the saved images to load without freexing the program
                    }
                }

        } break;
    }

}

void ofApp:: displayFrozenImage() {
    
    //Display frozen image
    ofSetColor(255);
    greyOnScreenGrab.draw(0,0);
}

void ofApp::changeColor() {
    
    // Pick at random one of the colours in the array
    int rCol = ofRandom(0,colorPaletteSize);
    
    // Change bg color
    randomCol = colorPalette[rCol];
    // Change depth color
    randomColDepth = colorPaletteDepth[rCol];
}

void ofApp::drawGrid() {
    
    //Set border edge for each box in grid
    int num=0;
    
    //Draw grid
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {

            //Draw individual box
            birdBoxes[num]->display();
            
            num++;
        }
    }
}

Boolean ofApp::colorComparison(ofImage beforeDepth, ofImage birdImage, int grabX, int grabY, int grabWidth, int grabHeight){

    int centerX = beforeDepth.width*0.5;
    int centerY = beforeDepth.height*0.5;
    savedShape.allocate(gameBirdW, gameBirdH, OF_IMAGE_COLOR_ALPHA);
    
    totCount = 0;
    perCount = 0;
   // ofSetColor(30, 20, 255);
   // ofRect(centerX - 10, centerY - 10, 20, 20);
    ofSetColor(255,255, 255);
    int amountToCheck;
    int maxValueCheck;
    if(birdImage.height == grabImg.height){
        maxValueCheck = beforeDepth.height/2;
    }
    else{
        maxValueCheck = beforeDepth.height/3;
    }
    for(int x = 0; x < beforeDepth.width; x++){
        for(int y = maxValueCheck; y < beforeDepth.height; y++){// Took out top thrid because it is blocked by roof
            ofColor birdCol;
          
            if(birdImage.height == grabImg.height){ // This is a fix for end of round grabIMG check
             birdCol = birdImage.getColor(x, y);// This will be the after depth
            }
            else{
             birdCol = birdImage.getColor(x/imgScale, y/imgScale);// This will be the drawn silhouette to the screenadjustment size
            }
            savedShape.setColor(x, y, ofColor(255,255,255, 0));
            if(birdCol.a != 0){ // We only need to check the alpha dots
                ofColor orgCol = beforeDepth.getColor(x, y);
                float orgColR = orgCol.r;
                float orgColG = orgCol.g;
                float orgColB = orgCol.b;
                float orgColA = orgCol.a;
                
                ofColor transCol = ofColor(255, 255, 255, 0); // transparency
                savedShape.setColor(x, y, transCol);
                
                ofColor capCol = grabImg.getColor(x, y);
                float capColR = capCol.r;
                float capColG = capCol.g;
                float capColB = capCol.b;
                float capColA = capCol.a;
                
                if(orgColR != capColR || orgColG != capColG || orgColB != capColB || orgColA != capColA){
                    perCount ++;
                    savedShape.setColor(x, y, birdCol);

                }
                totCount ++;
            }
        }
    }
    savedShape.update();
    
    percentage = ((float)perCount / (float)totCount)*100;
    //trying out a different trigger for play
    // if we haven't already triggered the game, then look at the center point of the image and wait...
    if (!playTrigger ) {
        int area = 2;
        if (percentage >= 30) {
            //cout << "TRIGGERED PLAY! Let's go! " << x << " and " << y << endl;
            playTrigger=true;
            readyTimerElapsed=ofGetElapsedTimeMillis();
            currentBirdSound.play();
        }
    }
    return playTrigger;
}

void ofApp::savePlayerImages() {
    
    // remove the first image from the vector (decrease capacity, so we can put a new image in)
    // numImagesToKeep is set to 12?? at the moment, per type of bird
    
    if (previousPlayBird[currentImageNumber].size()>=numImagesToKeep) {
        previousPlayBird[currentImageNumber].erase(previousPlayBird[currentImageNumber].begin());
    }
    string time = ofGetTimestampString();
    // This is the cut out bird shape
    savedShape.saveImage("images/saved/cutbird_"+time+".png");
    previousPlayBird[currentImageNumber].push_back(actualOnScreenGrab);

    // This is the entire screen grab image
    actualOnScreenGrab.saveImage("images/saved/shape_"+time+".png");

    // Total count for all saved images, just goes up and up...
    imgCounter++;

    //To show previous player images on screen, count how many there are now
    totalPlayerImages = previousPlayBird[currentImageNumber].size();
    currentlyShowing  = totalPlayerImages;
    
    // shrink the previous played images...
    // ofImage shrunkDepth;
    ofImage shrunkBird;

    int num=0;
    //update the grid
    for (int i=0; i<numRows; i++) {
        for (int j=0; j<numCols; j++) {
            
            // display previously played image in this box
            if (num < totalPlayerImages) {
                shrunkBird = previousPlayBird[currentImageNumber][num];
            }
            // otherwise display the current played image in this box
            else {
                shrunkBird= beforeDepthTrigger;
 
            }
            
            shrunkBird.resize(boxW,boxH);
            
            //Calculate x and y
            int tX = boxW*j;
            int tY = boxH*i;
            
            // Chop up the current image on screen.
            // Into 12 separate pieces so we can turn them over
            ofImage choppedBird = greyOnScreenGrab;
            
            choppedBird.crop(tX,tY,boxW,boxH);
            
            birdBoxes[num]->setNewImages(shrunkBird, choppedBird);
            
            num++;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::loadImages() {
    
    birdCatalog[0].loadImage("images/fantail.png");
    birdCatalog[1].loadImage("images/dotterel.png");
    
    birdCatalog[2].loadImage("images/kingfisher.png");
    birdCatalog[3].loadImage("images/magpie.png");
    birdCatalog[4].loadImage("images/pigeon.png");
    
    birdCatalog[5].loadImage("images/quail.png");
    birdCatalog[6].loadImage("images/kakapo.png");
    
    birdCatalog[7].loadImage("images/penguin.png");
    birdCatalog[8].loadImage("images/kokako.png");
    birdCatalog[9].loadImage("images/oystercatcher.png");
    birdCatalog[10].loadImage("images/whio.png");
    birdCatalog[11].loadImage("images/seagull.png");
}

//--------------------------------------------------------------
void ofApp::loadSounds() {
    //Load Sounds * * * * * * * * * * * * * *
    
    //replace with for loop above
    birdSoundCatalog[0].loadSound("sounds/fantail.mp3");
    birdSoundCatalog[1].loadSound("sounds/dotterel.mp3");
    
    birdSoundCatalog[2].loadSound("sounds/kingfisher.mp3");
    birdSoundCatalog[3].loadSound("sounds/magpie.mp3");
    birdSoundCatalog[4].loadSound("sounds/pigeon.mp3");
    
    birdSoundCatalog[5].loadSound("sounds/quail.mp3");
    birdSoundCatalog[6].loadSound("sounds/kakapo.mp3");
    
    birdSoundCatalog[7].loadSound("sounds/penguin.mp3");
    birdSoundCatalog[8].loadSound("sounds/kokako.mp3");
    birdSoundCatalog[9].loadSound("sounds/oystercatcher.mp3");
    birdSoundCatalog[10].loadSound("sounds/whio.mp3");
    birdSoundCatalog[11].loadSound("sounds/seagull.mp3");
    
}

//--------------------------------------------------------------
void ofApp::loadNewBirdImage() {
    
    //rand = the type of bird we will generate.
    int rand = int(ofRandom(birdCatalogSize));
    
    currentImageNumber = rand;
    
    switch (currentImageNumber) {
            
        case 0: // Fantail
            currentBirdImage=birdCatalog[0];
            currentBirdSound=birdSoundCatalog[0];
            break;
        case 1: // Dotterel
            currentBirdImage=birdCatalog[1];
            currentBirdSound=birdSoundCatalog[1];
            break;
        case 2: // Kingfisher
            currentBirdImage=birdCatalog[2];
            currentBirdSound=birdSoundCatalog[2];
            break;
        case 3: // Magpie
            currentBirdImage=birdCatalog[3];
            currentBirdSound=birdSoundCatalog[3];
            break;
        case 4: // Pigeon
            currentBirdImage=birdCatalog[4];
            currentBirdSound=birdSoundCatalog[4];
            break;
        case 5: // Quail
            currentBirdImage=birdCatalog[5];
            currentBirdSound=birdSoundCatalog[5];
            break;
        case 6: // Kakapo
            currentBirdImage=birdCatalog[6];
            currentBirdSound=birdSoundCatalog[6];
            break;
        case 7: // Penguin
            currentBirdImage=birdCatalog[7];
            currentBirdSound=birdSoundCatalog[7];
            break;
        case 8: // Kokako
            currentBirdImage=birdCatalog[8];
            currentBirdSound=birdSoundCatalog[8];
            break;
        case 9: // Oystercatcher
            currentBirdImage=birdCatalog[9];
            currentBirdSound=birdSoundCatalog[9];
            break;
        case 10: // Whio
            currentBirdImage=birdCatalog[10];
            currentBirdSound=birdSoundCatalog[10];
            break;
        case 11: // Gull
            currentBirdImage=birdCatalog[11];
            currentBirdSound=birdSoundCatalog[11];
            break;
            
        default:
            currentBirdImage=birdCatalog[0];
            currentBirdSound=birdSoundCatalog[0];
    }
    
}

void ofApp::loadBirdText() {
    birdName[0] = "Piwakawaka / Fantail ";
    birdName[1] = "Tuturiwhatu / Dotterel";
    birdName[2] = "Kotare / Kingfisher";
    birdName[3] = "Australian Magpie";
    birdName[4] = "Kereru / Wood Pigeon";
    birdName[5] = "California Quail";
    birdName[6] = "Kakapo    ";
    
    birdName[7] = "Hoiho / Yellow-eyed Penguin";
    birdName[8] = "Kokako   ";
    birdName[9] = "Torea Pango / Oystercatcher";
    birdName[10] = "Whio / Blue Duck";
    birdName[11] = "Karoro / Black-backed gull";
}

void ofApp::displayTimeIndicator() {
    
    //sides of screen
     int timeWidth = dimWidth;
     int timeHeight = ofMap(triggerTimer,0,absolutePlayTime,0,dimHeight);
     ofSetColor(50, 50, 50, 80);
    
    //ofRect(0,0,timeWidth,dimHeight-timeHeight);
     ofRect(0, 0,timeWidth, dimHeight - timeHeight);
    ofSetColor(255, 255, 255, 255);

   //  ofSetColor(0);
     //ofRect(dimWidth-timeWidth,0,timeWidth,dimHeight-timeHeight);
   //  ofRect(dimWidth-timeWidth,0,timeWidth,timeHeight);
}

void ofApp::setupColorPalette() {
    
     //MmmMmManual Settings!
    
     //pink maroon
     colorPalette[0] = ofColor(152,44,84);
     colorPaletteDepth[0] = ofColor(224,126,186);
    
    // dark blue
     colorPalette[1] = ofColor(64,50,170);
     colorPaletteDepth[1] = ofColor(21,108,253);
    
    // blue green
     colorPalette[2] = ofColor(11,151,153);
     colorPaletteDepth[2] = ofColor(105,198,199);
    
    //green
     colorPalette[3] = ofColor(7,167,90);
     colorPaletteDepth[3] = ofColor(67,255,186);
    
    //yellow
    colorPalette[4] = ofColor(240,198,28);
    colorPaletteDepth[4] = ofColor(252,236,63);
    
    //orange
     colorPalette[5] = ofColor(244,103,0);
     colorPaletteDepth[5] = ofColor(216,180,152);
    
    //red
     colorPalette[6] = ofColor(255,47,47);
     colorPaletteDepth[6] = ofColor(252,167,167);
    
    //purple
     colorPalette[7] = ofColor(150,100,252);
    colorPaletteDepth[7] = ofColor(202,181,244);
    
     //blue
     colorPalette[8] = ofColor(49,82,110);
     colorPaletteDepth[8] = ofColor(49,186,237);
    
    // pinker
    colorPalette[9] = ofColor(255,34,133);
    colorPaletteDepth[9] = ofColor(244,168,255);
    
}

void ofApp::displayDebugText() {

 //DEBUG STATS
 ofDrawBitmapString("FRACTION MADE: " + ofToString(perCount) + " / " + ofToString(totCount), 10, 20);
 ofDrawBitmapString("PERCENT MADE: " + ofToString(percentage) + "%", 10, 500);
 ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 10, 60);
 ofDrawBitmapString("Mode: " + ofToString(state), 10, 80);
 ofDrawBitmapString("Trigger: " + ofToString(triggerTimer), 10, 100);
 
 ofDrawBitmapString("Min Depth: " + ofToString(minDepthSetter), 10, 120);
 ofDrawBitmapString("Max Depth: " + ofToString(maxDepthSetter), 10, 140);
 
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
   
  /*  if (key == OF_KEY_UP) {
        minDepthSetter+= 200;
        //cout << "Set min " << minDepthSetter<<endl;
        //accessing min max variables directly, whatever,
        openNIDevice.globalMinDepth = minDepthSetter;
        //coz the function is not doing it's job
        //openNIDevice.setMinDepth(minDepthSetter);
    }
    if (key == OF_KEY_DOWN) {
        minDepthSetter-= 200;
        //cout << "Set max " << maxDepthSetter<<endl;
        openNIDevice.globalMaxDepth = maxDepthSetter;
        //openNIDevice.setMinDepth(maxDepthSetter);
    }
    
    if (key == 'r') {
        flipStatus =!flipStatus;
    }
    if (key == 'z') {
        roofRectHeight += 5;
    }
    if (key == 'x') {
        roofRectHeight -= 5;
    }
*/
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(button == 2){randomColDepth = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(button == 0){state = Playing;}
    if(button == 1){state = Passive;}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
