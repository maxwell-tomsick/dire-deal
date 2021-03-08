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
//using namespace std;

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
     std::shared_ptr<scene2::SceneNode> background = _assets->get<scene2::SceneNode>("background");
     addChild(background);
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;
    //scene->setAnchor(Vec2::ANCHOR_CENTER);
     //scene->setAngle(M_PI_2);
     Response firstResponse;
     Response secondResponse;
     Response thirdResponse;
     firstResponse.allocate("Roll Behind", "Shuffle in Enemy Exposed", {0,0,0,0}, {2}, false, false);
     secondResponse.allocate("Block", "No effect", {0,0,0,0}, {1}, false, false);
     thirdResponse.allocate("Take Hit", "Shuffle in Player Wounded", {0,0,0,0}, {1,5}, false, false);
     
     Card enemyAttacks1;
     enemyAttacks1.allocate("Enemy Attacks", 1, {firstResponse, secondResponse, thirdResponse});
     Card enemyAttacks2;
     enemyAttacks2.allocate("Enemy Attacks", 1, {firstResponse, secondResponse, thirdResponse});
     Card enemyAttacks3;
     enemyAttacks3.allocate("Enemy Attacks", 1, {firstResponse, secondResponse, thirdResponse});
     
     _currentDeck = Deck();
     _nextDeck = Deck();
     _pause = 0;
     _currentDeck.addCard(enemyAttacks1);
     _currentDeck.addCard(enemyAttacks2);
     _currentDeck.addCard(enemyAttacks3);
     
     _currentCard = _currentDeck.draw();
     //_currentDeck.printDeck();
     
    _blueSound = _assets->get<Sound>("laser");
    _redSound = _assets->get<Sound>("fusion");
     auto cardFrontTexture = _assets->get<Texture>("cardFront");
     auto cardBackTexture1 = _assets->get<Texture>("cardBack1");
     //auto cardBackTexture2 = _assets->get<Texture>("cardBack2");
     
     _deckNode = DeckNode::alloc();
     _deckNode->setSize(_currentDeck.getSize());
     _deckNode->setBackTexture(cardBackTexture1);
     _cardBack = 1;
     _deckNode->setFrontTexture(cardFrontTexture);
     _deckNode->setDrawFront(true);
     addChild(_deckNode);
    addChild(scene);
     
     
    //reset();

    //text field
    // Size dimen = Application::get()->getDisplaySize();
    // dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution

    // auto layer = assets->get<scene2::SceneNode>("textfield");
    // layer->setContentSize(dimen);
    // layer->doLayout(); // This rearranges the children to fit the screen
    // addChild(layer);

    // _field  = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("lab_action"));
    _currEvent = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_currEvent"));
    _response1 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response1"));
    _responseText1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_label"));
    _responseOutcome1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_outcome"));
    _response2 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response2"));
    _responseText2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_label"));
    _responseOutcome2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_outcome"));
    _response3 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response3"));
    _responseText3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_label"));
    _responseOutcome3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_outcome"));

     
    _response1->addListener([=](const std::string& name, bool down) {
        if (down) {
             buttonPress(0);
        }
        });
    _response2->addListener([=](const std::string& name, bool down) {
        if (down) {
             buttonPress(1);
        }
        });
    _response3->addListener([=](const std::string& name, bool down) {
        if (down) {
             buttonPress(2);
        }
        });

     //_response1->setText(_currentCard.getResponse(0).getText());
     
    if (_active) {
        _response1->activate();
        _response2->activate();
        _response3->activate();
    }
    // _field->addTypeListener([=](const std::string& name, const std::string& value) {
    //     CULog("Change to %s",value.c_str());
    // });
    // _field->addExitListener([=](const std::string& name, const std::string& value) {
    //     CULog("Finish to %s",value.c_str());
    //     _result->setText(strcat("Result: ","asdF"));
    // });
    _currEvent->setText(_currentCard.getText());
     _responseText1->setText(_currentCard.getResponse(0).getText());
     _responseOutcome1->setText(_currentCard.getResponse(0).getOutcome());
     _responseText2->setText(_currentCard.getResponse(1).getText());
     _responseOutcome2->setText(_currentCard.getResponse(1).getOutcome());
     _responseText3->setText(_currentCard.getResponse(2).getText());
     _responseOutcome3->setText(_currentCard.getResponse(2).getOutcome());
    // if (_active) {
    //     _field->activate();
    // }
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
        _assets = nullptr;
         _deckNode = nullptr;
        Scene2::dispose();
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
    flourish = 10;
     
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
     if (_pause > 1){
          _pause--;
     } else if (_pause == 1){
          _currEvent->setVisible(true);
          _response1->setVisible(true);
          _response2->setVisible(true);
          _response3->setVisible(true);
          _pause = 0;
          _currentCard = _currentDeck.draw();
          _currEvent->setText(_currentCard.getText());
          _responseText1->setText(_currentCard.getResponse(0).getText());
          _responseOutcome1->setText(_currentCard.getResponse(0).getOutcome());
          _responseText2->setText(_currentCard.getResponse(1).getText());
          _responseOutcome2->setText(_currentCard.getResponse(1).getOutcome());
          _responseText3->setText(_currentCard.getResponse(2).getText());
          _deckNode->setSize(_currentDeck.getSize());
          _responseOutcome3->setText(_currentCard.getResponse(2).getOutcome());
          _deckNode->setDrawFront(true);
     }
    // Read the keyboard for each controller.
    /*_redController.readInput();
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
     */
     


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

Card GameScene::getCard(const int id){
     Card newCard;
     Response firstResponse;
     Response secondResponse;
     Response thirdResponse;
     switch(id){
          case 1:
               firstResponse.allocate("Roll Behind", "Shuffle in Enemy Exposed", {0,0,0,0}, {2}, false, false);
               secondResponse.allocate("Block", "Shuffle in Enemy Attacks", {0,0,0,0}, {1}, false, false);
               thirdResponse.allocate("Take Hit", "Shuffle in Player Wounded and Enemy Attacks", {0,0,0,0}, {1,5}, false, false);
               newCard.allocate("Enemy Attacks", 1, {firstResponse, secondResponse, thirdResponse});
               break;
          case 2:
               firstResponse.allocate("Stab", "Shuffle in Enemy Attacks and Enemy Wounded", {0,0,0,0}, {1,3}, false, false);
               secondResponse.allocate("Heavy Slash", "Shuffle in Enemy Maimed and Enemy Enraged", {0,0,0,0}, {4,7}, false, false);
               thirdResponse.allocate("Maintain Spacing", "Shuiffle in Enemy Attacks", {0,0,0,0}, {1}, false, false);
               newCard.allocate("Enemy Exposed", 2, {firstResponse, secondResponse, thirdResponse});
               break;
          case 3:
               firstResponse.allocate("Maim", "Shuffle in Enemy Maimed", {0,0,0,0}, {4}, false, false);
               secondResponse.allocate("Slash", "Shuffle in Exposed and Enemy Wounded", {0,0,0,0}, {2,3}, false, false);
               thirdResponse.allocate("Tease", "Shuffle in 2 Enemy Wounded and Enemy Enraged", {0,0,0,0}, {3,3,7}, false, false);
               newCard.allocate("Enemy Wounded", 3, {firstResponse, secondResponse, thirdResponse});
               break;
          case 4:
               firstResponse.allocate("Execute", "Win", {0,0,0,0}, {}, true, false);
               secondResponse.allocate("Crush", "Win", {0,0,0,0}, {}, true, false);
               thirdResponse.allocate("Taunt", "Shuffle in 2 Enemy Maimed and Enemy Enraged", {0,0,0,0}, {4,4,7}, false, false);
               newCard.allocate("Enemy Maimed", 4, {firstResponse, secondResponse, thirdResponse});
               break;
          case 5:
               firstResponse.allocate("Disengage", "Shuffle in Enemy Attacks and Player Wounded", {0,0,0,0}, {1,5}, false, false);
               secondResponse.allocate("Save Strength", "Shuffle in Player Maimed", {0,0,0,0}, {6}, false, false);
               thirdResponse.allocate("Stand", "Shuffle in Enemy Attacks and Enemy Enranged", {0,0,0,0}, {2,7}, false, false);
               newCard.allocate("Player Wounded", 5, {firstResponse, secondResponse, thirdResponse});
               break;
          case 6:
               firstResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               secondResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               thirdResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               newCard.allocate("Player Maimed", 6, {firstResponse, secondResponse, thirdResponse});
               break;
          case 7:
               firstResponse.allocate("Almost Dodge", "Shuffle in Enemy Exposed and Player Wounded", {0,0,0,0}, {2,5}, false, false);
               secondResponse.allocate("Eye for an Eye", "Shuffle in Player Wounded and Enemy Wounded", {0,0,0,0}, {3,6}, false, false);
               thirdResponse.allocate("Double Down", "Shuffle in 2 Enemy Enranged", {0,0,0,0}, {7,7}, false, false);
               newCard.allocate("Enemy Enraged", 7, {firstResponse, secondResponse, thirdResponse});
               break;
     }
     return newCard;
}

void GameScene::buttonPress(const int r){
     Response response = _currentCard.getResponse(r);
     if (response.getWin()){
          _currEvent->setText("YOU WIN!");
          return;
     } else if (response.getLose()){
          _currEvent->setText("YOU DIED!");
          return;
     }
     //_currEvent->setText("Clicked " + std::to_string(r + 1));
     //_currentDeck.printDeck();
     std::vector<int> cards =response.getCards();
     for (int i = 0; i < cards.size(); i++){
          Card newCard = getCard(cards[i]);
          _nextDeck.addCard(newCard);
     }
     //CULog("Next Deck:");
     //_nextDeck.printDeck();
     //CULog("Current Deck:");
     //_currentDeck.printDeck();
     if (_currentDeck.getSize() == 0){
          _currEvent->setText("Shuffling Next Event Deck...");
          if (_cardBack == 1){
               auto cardBackTexture = _assets->get<Texture>("cardBack2");
               _deckNode->setBackTexture(cardBackTexture);
               _cardBack = 2;
          } else {
               auto cardBackTexture = _assets->get<Texture>("cardBack1");
               _deckNode->setBackTexture(cardBackTexture);
               _cardBack = 1;
          }
          if (_nextDeck.getSize() > 0){
               _currentDeck = _nextDeck;
               _nextDeck = Deck();
          } else {
               Response rollBehind;
               rollBehind.allocate("Roll Behind", "Shuffle in Enemy Exposed", {0,0,0,0}, {1}, false, false);
               Response block;
               block.allocate("Block", "No effect", {0,0,0,0}, {1}, false, false);
               Response saveStrength;
               saveStrength.allocate("Save Strength","Shuffle in Player Maimed", {0,0,0,0}, {1}, false, false);
               Card enemyAttacks1;
               enemyAttacks1.allocate("Enemy Attacks", 1, {rollBehind, block, saveStrength});
               Card enemyAttacks2;
               enemyAttacks2.allocate("Enemy Attacks", 1, {rollBehind, block, saveStrength});
               Card enemyAttacks3;
               enemyAttacks3.allocate("Enemy Attacks", 1, {rollBehind, block, saveStrength});
               _currentDeck = Deck();
               _nextDeck = Deck();
               _currentDeck.addCard(enemyAttacks1);
               _currentDeck.addCard(enemyAttacks2);
               _currentDeck.addCard(enemyAttacks3);
          }
     } else {
          _currEvent->setVisible(false);
     }
     //_response1->setDown(false);
     //_response1->deactivate();
     //_response2->deactivate();
     //_response3->deactivate();
     _response1->setVisible(false);
     _response2->setVisible(false);
     _response3->setVisible(false);
     _deckNode->setDrawFront(false);
     _pause = 40;
     /*
     _currentCard = _currentDeck.draw();
     _currEvent->setText(_currentCard.getText());
     _responseText1->setText(_currentCard.getResponse(0).getText());
     _responseText2->setText(_currentCard.getResponse(1).getText());
     _responseText3->setText(_currentCard.getResponse(2).getText());
      */
}

