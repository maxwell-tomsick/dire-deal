//
//  GLGameScene.cpp
//  Programming Lab
//
//  This is the primary class file for running the game.  You should study this file for
//  ideas on how to structure your own root class. This class is a reimagining of the
//  first game lab from 3152 in CUGL.
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "GLGameScene.h"
#include "GLCollisionController.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/** Number of rows in the ship image filmstrip */
#define SHIP_ROWS   4
/** Number of columns in this ship image filmstrip */
#define SHIP_COLS   5
/** Number of elements in this ship image filmstrip */
#define SHIP_SIZE   18
/** Maximum number of photons allowed on screen at a time. */
#define MAX_PHOTONS 512

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;
    //scene->setAnchor(Vec2::ANCHOR_CENTER);
     //scene->setAngle(M_PI_2);


    _blueSound = _assets->get<Sound>("laser");
    _redSound = _assets->get<Sound>("fusion");
    addChild(scene);
    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    auto root = getChild(0);
    if (_blueShip != nullptr && _blueShip->getShadowNode() != nullptr) {
        root->removeChild(_blueShip->getShadowNode());
    }
    if (_redShip != nullptr && _redShip->getShadowNode() != nullptr) {
        root->removeChild(_redShip->getShadowNode());
    }
    Size dimen = root->getContentSize();
    
    auto shipTexture = _assets->get<Texture>("ship");
    auto targTexture = _assets->get<Texture>("target");
     
     
    _blueShip = Ship::alloc(dimen.width*(2.0f / 3.0f), dimen.height*(1.0f / 2.0f), 90);
    _blueShip->setColor(Color4f(0.5f, 0.5f, 1.0f, 1.0f));   // Blue, but makes texture easier to see
     _blueShip->setBounds(getBounds());
    _blueShip->setTextures(shipTexture, targTexture);
    _blueShip->setSID(0);
    _blueController.init(0);

    _redShip = Ship::alloc(dimen.width*(1.0f / 3.0f), dimen.height*(1.0f / 2.0f), -90);
    _redShip->setColor(Color4f(1.0f, 0.25f, 0.25f, 1.0f));  // Red, but makes texture easier to see
     _redShip->setBounds(getBounds());
    _redShip->setTextures(shipTexture, targTexture);
    _redShip->setSID(1);
    _redController.init(1);

    _redShip->acquireTarget(_blueShip);
    _blueShip->acquireTarget(_redShip);
    //root->addChild(_redShip->getShadowNode());
    //root->addChild(_blueShip->getShadowNode());
     _photons = PhotonQueue::alloc(MAX_PHOTONS);
     _photons->setTexture(_assets->get<Texture>("photon"));
      root->addChild(_photons->getPhotonNode());
     root->addChild(_redShip->getSceneNode());
     root->addChild(_blueShip->getSceneNode());
     root->addChild(_redShip->getTargetNode());
     root->addChild(_blueShip->getTargetNode());
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    // Read the keyboard for each controller.
    _redController.readInput();
    _blueController.readInput();
     _redShip->setBounds(getBounds());
     _blueShip->setBounds(getBounds());
     if (_redController.didPressJump() && _redShip->getJumping() == 0) {
          _redShip->jump();
     }
     if (_blueController.didPressJump() && _blueShip->getJumping() == 0) {
          _blueShip->jump();
     }
    // Move the photons forward, and add new ones if necessary.
    if (_redController.didPressFire() && firePhoton(_redShip)) {
        // The last argument is force=true.  It makes sure only one instance plays.
        AudioEngine::get()->play("redfire", _redSound, false, 1.0f, true);
    }
    if (_blueController.didPressFire() && firePhoton(_blueShip)) {
        // The last argument is force=true.  It makes sure only one instance plays.
        AudioEngine::get()->play("bluefire", _blueSound, false, 1.0f, true);
    }

     _redShip->fall();
     if (_redShip->getJumping() == 0){
          _redShip->setScale(1);
     } else {
          float j = _redShip->getJumping();
          float scale = -(j * j)/640.0f + j/8.0f;
          _redShip->setScale(max(scale, 1.0f));
     }
     _blueShip->fall();
     if (_blueShip->getJumping() == 0){
          _blueShip->setScale(1);
     } else {
          float j = _blueShip->getJumping();
          float scale = -(j * j)/640.0f + j/8.0f;
          _blueShip->setScale(max(scale, 1.0f));
     }
     
    // Move the ships and photons forward (ignoring collisions)
    _redShip->move( _redController.getForward(),  _redController.getTurn());
    _blueShip->move(_blueController.getForward(), _blueController.getTurn());
    _photons->update();

    // Change the target position.
    _redShip->acquireTarget(_blueShip);
    _blueShip->acquireTarget(_redShip);

    // This call handles BOTH ships.
    collisions::checkForCollision(_blueShip, _redShip, getBounds());
    collisions::checkForCollision(_blueShip, _photons, getBounds());
    collisions::checkForCollision(_redShip,  _photons, getBounds());
    collisions::checkInBounds(_blueShip, getBounds());
    collisions::checkInBounds(_redShip, getBounds());
    collisions::checkInBounds(_photons, getBounds());

}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
    // Call SUPER to do standard rendering
    Scene2::render(batch);
    
     /*
    // Now do 3152-style rendering for the photons
    batch->begin(getCamera()->getCombined());
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    _photons->draw(batch);
    batch->end();
      */
}

/**
 * Fires a photon from the ship, adding it to the PhotonQueue.
 *
 * This is not inside either PhotonQueue or Ship because it is a relationship
 * between to objects.  As we will see in class, we do not want to code binary
 * relationships that way (because it increases dependencies).
 *
 * @param ship      Ship firing the photon
 * @param photons     PhotonQueue for allocation
 */
bool GameScene::firePhoton(const std::shared_ptr<Ship>& ship) {
    // Only process if enough time has passed since last.
    if (ship->canFireWeapon()) {
        _photons->addPhoton(ship);
        ship->reloadWeapon();
        return true;
    }
    return false;
}

