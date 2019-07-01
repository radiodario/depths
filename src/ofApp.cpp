#include "ofApp.h"

void ofApp::setup(){
  receive.setup(5002);
  send.setup("localhost", 5003);
  ofSetVerticalSync(true);
  // this number describes how many bins are used
  // on my machine, 2 is the ideal number (2^2 = 4x4 pixel bins)
  // if this number is too high, binning is not effective
  // because the screen is not subdivided enough. if
  // it's too low, the bins take up so much memory as to
  // become inefficient.
  int binPower = 4;

  padding = 64;
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
  drawGui = false;
  gui.setup();
  ofParameterGroup group_simulation;
  group_simulation.setName("simulation");
  group_simulation.add(timeStep.set("Time Step", 100, 1, 1000));
  group_simulation.add(particleNeighborhood.set("P. Neighborhood", 64, 1, 256));
  group_simulation.add(particleRepulsion.set("P. Repulsion", 0.5, 0.0, 1.0));
  group_simulation.add(centerAttraction.set("Center Attraction", 0.01, 0.0, 1.0));
  group_simulation.add(dampingForce.set("Damping Force", 0.01, 0.0, 1.0));
  group_simulation.add(attractorCenterX.set("Attractor X", 0.5, 0.0, 1.0));
  group_simulation.add(attractorCenterY.set("Attractor Y", 0.5, 0.0, 1.0));
  gui.add(group_simulation);

  // zoom pass
  ofParameterGroup group_zoom;
  group_zoom.setName("Zoom Blur");
  group_zoom.add(zoomEnabled.set("Enabled", true));
  group_zoom.add(zoomCenterX.set("z Center X", 0.5, 0, 1));
  group_zoom.add(zoomCenterY.set("z Center Y", 0.5, 0, 1));
  group_zoom.add(zoomExposure.set("z Exposure", 0.48, 0, 1));
  group_zoom.add(zoomDecay.set("z Decay", 0.9, 0, 1));
  group_zoom.add(zoomDensity.set("z Density", 0.25, 0, 1));
  group_zoom.add(zoomWeight.set("z Weight", 0.24, 0, 1));
  group_zoom.add(zoomClamp.set("z Clamp", 1, 0, 1));
  gui.add(group_zoom);

  // dof pass
  ofParameterGroup group_dof;
  group_dof.setName("Depth of Field");
  group_dof.add(dofEnabled.set("dof", true));
  group_dof.add(dofFocus.set("dof Focus", 0.985, 0, 1));
  group_dof.add(dofAperture.set("dof Aperture", 0.8, 0, 1));
  group_dof.add(dofMaxBlur.set("dof Max Blur", 0.6, 0, 1));
  gui.add(group_dof);

  ofParameterGroup group_post;
  group_post.setName("Postprocessing");
  group_post.add(grEnabled.set("God Rays", true));
  group_post.add(fxaaEnabled.set("fxaa", true));
  gui.add(group_post);

  // colours
  ofParameterGroup group_colour;
  group_colour.setName("Colour");
  group_colour.add(absoluteValues.set("Absolute Values", true));
  group_colour.add(minAlpha.set("min alpha", 5, 0, 255));
  group_colour.add(maxAlpha.set("max alpha", 155, 0, 255));
  group_colour.add(red.set("red", 255, 0, 255));
  group_colour.add(green.set("green", 250, 0, 255));
  group_colour.add(blue.set("blue", 255, 0, 255));
  gui.add(group_colour);
  drawBalls = false;

  post.init(ofGetWidth(), ofGetHeight());

  dfpass = post.createPass<DofPass>();
  zbpass = post.createPass<ZoomBlurPass>();
  grpass = post.createPass<GodRaysPass>();
  fxpass = post.createPass<FxaaPass>();
}

void ofApp::update(){
  handleOSCMessages();
  zbpass->setEnabled(zoomEnabled);
  zbpass->setCenterX(zoomCenterX);
  zbpass->setCenterY(zoomCenterY);
  zbpass->setExposure(zoomExposure);
  zbpass->setDecay(zoomDecay);
  zbpass->setDensity(zoomDensity);
  zbpass->setWeight(zoomWeight);
  zbpass->setClamp(zoomClamp);

  dfpass->setEnabled(dofEnabled);
  dfpass->setFocus(dofFocus);
  dfpass->setAperture(dofAperture);
  dfpass->setMaxBlur(dofMaxBlur);

  grpass->setEnabled(grEnabled);
  fxpass->setEnabled(fxaaEnabled);
}

void ofApp::handleOSCMessages() {
  while (receive.hasWaitingMessages()) {
    ofxOscMessage m;
    receive.getNextMessage(&m);
    string msgAddress = m.getAddress();

    if (msgAddress == "/timeStep") {
      timeStep = m.getArgAsFloat(0);
    }
    if (msgAddress == "/particleNeighborhood") {
      particleNeighborhood = m.getArgAsFloat(0);
    }
    if (msgAddress == "/particleRepulsion") {
      particleRepulsion = m.getArgAsFloat(0);
    }
    if (msgAddress == "/centerAttraction") {
      centerAttraction = m.getArgAsFloat(0);
    }
    if (msgAddress == "/dampingForce") {
      dampingForce = m.getArgAsFloat(0);
    }
    if (msgAddress == "/minAlpha") {
      minAlpha = m.getArgAsFloat(0);
    }
    if (msgAddress == "/maxAlpha") {
      maxAlpha = m.getArgAsFloat(0);
    }
    if (msgAddress == "/zoomEnabled") {
      zoomEnabled = m.getArgAsBool(0);
    }
    if (msgAddress == "/zoomCenter") {
      zoomCenterX = m.getArgAsFloat(0);
      zoomCenterY = m.getArgAsFloat(1);
    }
    if (msgAddress == "/attractorCenter") {
      attractorCenterX = m.getArgAsFloat(0);
      attractorCenterY = 1 - m.getArgAsFloat(1);
    }
    if (msgAddress == "/zoomExposure") {
      zoomExposure = m.getArgAsFloat(0);
    }
    if (msgAddress == "/zoomDecay") {
      zoomDecay = m.getArgAsFloat(0);
    }
    if (msgAddress == "/zoomDensity") {
      zoomDensity = m.getArgAsFloat(0);
    }
    if (msgAddress == "/zoomWeight") {
      zoomWeight = m.getArgAsFloat(0);
    }
    if (msgAddress == "/zoomClamp") {
      zoomClamp = m.getArgAsFloat(0);
    }
    if (msgAddress == "/dofEnabled") {
      dofEnabled = m.getArgAsBool(0);
    }
    if (msgAddress == "/dofFocus") {
      dofFocus = m.getArgAsFloat(0);
    }
    if (msgAddress == "/dofAperture") {
      dofAperture = m.getArgAsFloat(0);
    }
    if (msgAddress == "/dofMaxBlur") {
      dofMaxBlur = m.getArgAsFloat(0);
    }
    if (msgAddress == "/grEnabled") {
      grEnabled = m.getArgAsBool(0);
    }
    if (msgAddress == "/fxaaEnabled") {
      fxaaEnabled = m.getArgAsBool(0);
    }
    if (msgAddress == "/red") {
      red = m.getArgAsInt(0);
    }
    if (msgAddress == "/green") {
      green = m.getArgAsInt(0);
    }
    if (msgAddress == "/blue") {
      blue = m.getArgAsInt(0);
    }
    if (msgAddress == "/absoluteValues") {
      absoluteValues = m.getArgAsBool(0);
    }
  }
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
    float alphav, alphaf;
    if (absoluteValues) {
       alphav = ofMap(abs(cur.xv) + abs(cur.yv), 0, 20, minAlpha, maxAlpha);
       alphaf = ofMap(abs(cur.xf) + abs(cur.yf), 0, 20, minAlpha, maxAlpha);
    } else {
       alphav = ofMap((cur.xv) + (cur.yv), 0, 20, minAlpha, maxAlpha);
       alphaf = ofMap((cur.xf) + (cur.yf), 0, 20, minAlpha, maxAlpha);
    }
    float alpha = alphav * 0.5 + alphaf * 0.5;
    ofSetColor(red, green, blue, alpha);
    // global force on other particles
    particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
    // forces on this particle
    cur.bounceOffWalls(0, 0, particleSystem.getWidth(), particleSystem.getHeight());
    cur.addDampingForce(dampingForce);
  }
  if(!drawBalls) {
    glEnd();
  }

  // single-pass global forces
  particleSystem.addAttractionForce(
      particleSystem.getWidth() * attractorCenterX,
      particleSystem.getHeight() * attractorCenterY,
      particleSystem.getWidth() * 100,
      centerAttraction
  );
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
  if (drawGui) gui.draw();
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
  if (key == 'h') {
    drawGui = !drawGui;
  }
}

void ofApp::mousePressed(int x, int y, int button){
  isMousePressed = true;
}

void ofApp::mouseReleased(int x, int y, int button){
  isMousePressed = false;
}
