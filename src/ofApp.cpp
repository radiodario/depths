#include "ofApp.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  // this number describes how many bins are used
  // on my machine, 2 is the ideal number (2^2 = 4x4 pixel bins)
  // if this number is too high, binning is not effective
  // because the screen is not subdivided enough. if
  // it's too low, the bins take up so much memory as to
  // become inefficient.
  int binPower = 5;

  padding = 256;
  particleSystem.setup(ofGetWidth() + padding * 2, ofGetHeight() + padding * 2, binPower);

  kBinnedParticles = 3200;
  for(int i = 0; i < kBinnedParticles; i++) {
    float x = ofRandom(0, ofGetWidth()) + padding;
    float y = ofRandom(0, ofGetHeight()) + padding;
    BinnedParticle particle(x, y, 0, 0);
    particleSystem.add(particle);
  }

  isMousePressed = false;
  slowMotion = true;
  gui.setup();
  gui.add(timeStep.set("Time Step", 100, 1, 1000));
  gui.add(particleNeighborhood.set("P. Neighborhood", 32, 1, 256));
  gui.add(particleRepulsion.set("P. Repulsion", 0.2, 0.0, 1.0));
  gui.add(centerAttraction.set("Center Attraction", 0.01, 0.0, 1.0));
  gui.add(minAlpha.set("min alpha", 5, 0, 255));
  gui.add(maxAlpha.set("max alpha", 155, 0, 255));

  // zoom pass
  gui.add(zoomCenterX.set("z Center X", 0.5, 0, 1));
  gui.add(zoomCenterY.set("z Center Y", 0.5, 0, 1));
  gui.add(zoomExposure.set("z Exposure", 0.48, 0, 1));
  gui.add(zoomDecay.set("z Decay", 0.9, 0, 1));
  gui.add(zoomDensity.set("z Density", 0.25, 0, 1));
  gui.add(zoomWeight.set("z Weight", 0.24, 0, 1));
  gui.add(zoomClamp.set("z Clamp", 1, 0, 1));

  // dof pass
  gui.add(dofFocus.set("dof Focus", 0.985, 0, 1));
  gui.add(dofAperture.set("dof Aperture", 0.8, 0, 1));
  gui.add(dofMaxBlur.set("dof Max Blur", 0.6, 0, 1));

  // colours
  gui.add(red.set("red", 255, 0, 255));
  gui.add(green.set("green", 250, 0, 255));
  gui.add(blue.set("blue", 255, 0, 255));
  drawBalls = false;

  post.init(ofGetWidth(), ofGetHeight());

  dfpass = post.createPass<DofPass>();
  zbpass = post.createPass<ZoomBlurPass>();
  grpass = post.createPass<GodRaysPass>();
  post.createPass<FxaaPass>();
}

void ofApp::update(){
  zbpass->setCenterX(zoomCenterX);
  zbpass->setCenterY(zoomCenterY);
  zbpass->setExposure(zoomExposure);
  zbpass->setDecay(zoomDecay);
  zbpass->setDensity(zoomDensity);
  zbpass->setWeight(zoomWeight);
  zbpass->setClamp(zoomClamp);

  dfpass->setFocus(dofFocus);
  dfpass->setAperture(dofAperture);
  dfpass->setMaxBlur(dofMaxBlur);

}

void ofApp::draw(){
  post.begin();
  ofBackground(0);

  particleSystem.setTimeStep(timeStep);


  // do this once per frame
  particleSystem.setupForces();

  ofPushMatrix();
  ofTranslate(-padding, -padding);

  // apply per-particle forces
  if(!drawBalls) {
    ofSetLineWidth(0.1);
    glBegin(GL_LINES); // need GL_LINES if you want to draw inter-particle forces
  }
  for(int i = 0; i < particleSystem.size(); i++) {
    BinnedParticle& cur = particleSystem[i];
    float alpha = ofMap(cur.xv + cur.yv, 0, 20, minAlpha, maxAlpha);
    //float alpha = ofMap(cur.xf + cur.yf, 0, 20, minAlpha, maxAlpha);
    ofSetColor(red, green, blue, alpha);
    // global force on other particles
    particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
    // forces on this particle
    cur.bounceOffWalls(0, 0, particleSystem.getWidth(), particleSystem.getHeight());
    cur.addDampingForce();
  }
  if(!drawBalls) {
    glEnd();
  }

  // single-pass global forces
  particleSystem.addAttractionForce(particleSystem.getWidth() / 2, particleSystem.getHeight() / 2, particleSystem.getWidth() * 100, centerAttraction);
  if(isMousePressed) {
    particleSystem.addRepulsionForce(mouseX + padding, mouseY + padding, 200, 1);
  }
  particleSystem.update(ofGetLastFrameTime());

  // draw all the particles
  if(drawBalls) {
    for(int i = 0; i < particleSystem.size(); i++) {
      ofCircle(particleSystem[i].x, particleSystem[i].y, particleNeighborhood * .3);
    }
  }

  ofPopMatrix();
  post.end();
  ofSetColor(255);
  ofDrawBitmapString(ofToString(kBinnedParticles) + " particles", 32, 32);
  ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
  gui.draw();
}

void ofApp::keyPressed(int key){
  if(key == 'p') {
    ofSaveScreen(ofToString(ofGetMinutes()) + "_" + ofToString(ofGetFrameNum()) + ".png");
  }
  if(key == 's') {
    slowMotion = !slowMotion;
    if(slowMotion)
      timeStep = 10;
    else
      timeStep = 100;
  }
  if(key == 'b') {
    drawBalls = !drawBalls;
  }
}

void ofApp::mousePressed(int x, int y, int button){
  isMousePressed = true;
}

void ofApp::mouseReleased(int x, int y, int button){
  isMousePressed = false;
}
