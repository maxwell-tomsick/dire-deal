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
/** The key for the event handlers */
#define LISTENER_KEY        1

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
     background->setContentSize(dimen);
     background->doLayout();
     addChild(background);
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;
    //scene->setAnchor(Vec2::ANCHOR_CENTER);
     //scene->setAngle(M_PI_2);
     
     _cards = {};
     _responses = {};
     std::shared_ptr<JsonReader> jsonReaderCard = JsonReader::alloc("json/cards.json");
     std::shared_ptr<JsonValue> cardsJson = jsonReaderCard->readJson()->get("Cards");
     for (int i = 0; i < cardsJson->size(); i++){
          std::shared_ptr<JsonValue> jsonCard = cardsJson->get(i);
          int id = jsonCard->get("id")->asInt();
          string name = jsonCard->get("name")->asString();
          string tex = jsonCard->get("texture")->asString();
          auto texture = _assets->get<Texture>(tex);
          std::vector<int> responses = jsonCard->get("reactions")->asIntArray();
          std::vector<int> resources = jsonCard->get("resources")->asIntArray();
          int level = jsonCard->get("level")->asInt();
          Card card;
          card.allocate(name, id, texture, responses, resources, level);
          _cards[id] = card;
     }
     std::shared_ptr<JsonReader> jsonReaderResponse = JsonReader::alloc("json/responses.json");
     std::shared_ptr<JsonValue> responsesJson = jsonReaderResponse->readJson()->get("Responses");
     for (int i = 0; i < responsesJson->size(); i++){
          std::shared_ptr<JsonValue> jsonResponse = responsesJson->get(i);
          int id = jsonResponse->get("id")->asInt();
          string name = jsonResponse->get("name")->asString();
          string description = jsonResponse->get("description")->asString();
          std::vector<int> cost = jsonResponse->get("cost")->asIntArray();
          std::vector<int> addToDeck = jsonResponse->get("addToDeck")->asIntArray();
          bool win = jsonResponse->get("win")->asBool();
          bool lose = jsonResponse->get("lose")->asBool();
          Response response;
          response.allocate(name, description, cost, addToDeck, win, lose);
          _responses[id] = response;
     }
     //cout << cardsArray->asString();
     
     _resources = { 10, 12, 12, 10 };
     _currentDeck = {};
     _nextDeck = {};
     _keepCards = false;
     _pause = 0;

     _currentDeck.push_back(0);
     _currentDeck.push_back(0);
     _currentDeck.push_back(1);
     _currentDeck.push_back(2);
     _currentDeck.push_back(3);
     std::random_device rd;
     std::mt19937 g(rd());
      std::shuffle(_currentDeck.begin(), _currentDeck.end(), g);
     _currentCard = _cards[_currentDeck.back()];
     _currentDeck.pop_back();
     //_currentDeck.printDeck();
     
    _blueSound = _assets->get<Sound>("laser");
    _redSound = _assets->get<Sound>("fusion");
     auto cardBackTexture1 = _assets->get<Texture>("cardBack1");
     auto cardBackTexture2 = _assets->get<Texture>("cardBack2");
     
     _deckNode = DeckNode::alloc();
     _deckNode->setSize(int(_currentDeck.size()));
     _deckNode->setNextSize(0);
     _deckNode->setBackTexture(cardBackTexture1);
     _deckNode->setNextBackTexture(cardBackTexture2);
     _deckNode->setDimen(dimen);
     _deckNode->setFrontTexture(_currentCard.getTexture());
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
    //_resourceCount = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_resourceCount"));
    _response1 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response1"));
    _responseText1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_label"));
    _responseCost1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_costs"));//
    //_responseOutcome1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_outcome"));
    _response2 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response2"));
    _responseText2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_label"));
    //_responseCost2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_costs"));
    //_responseOutcome2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_outcome"));
    _response3 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response3"));
    _responseText3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_label"));
    //_responseCost3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_costs"));
    //_responseOutcome3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_outcome"));
     _responseTexture1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up"));
     _responseCard1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up_card"));
     _responseCard2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up_card"));
     _responseCard3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up_card"));
     _responseGlow1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up_glow"));
     _responseTexture2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up"));
     _responseGlow2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up_glow"));
     _responseTexture3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up"));
     _responseGlow3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up_glow"));
     _burn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_burn"));
     _burnTexture =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_burn_up"));
     _burnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_burn_up_amount"));
     _bladeText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_blade_amount"));
     _brawnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_brawn_amount"));
     _flourishText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_flourish_amount"));
     _lungeText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_lunge_amount"));
     _displayCard =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_displayCard"));
     bool success = true;
#ifndef CU_TOUCH_SCREEN
     success = Input::activate<Keyboard>();
     success = success && Input::activate<Mouse>();
     _mouse = Input::get<Mouse>();
     _mouse->setPointerAwareness(cugl::Mouse::PointerAwareness::ALWAYS);
     _response1->addListener([=](const std::string& name, bool down) {
          if ( (!(_pause > 1)) & down) {
              buttonPress(0);
          }
         });
     _response2->addListener([=](const std::string& name, bool down) {
         if ( (!(_pause > 1)) & down) {
              buttonPress(1);
         }
         });
     _response3->addListener([=](const std::string& name, bool down) {
         if ( (!(_pause > 1)) & down) {
              buttonPress(2);
         }
         });
      _burn->addListener([=](const std::string& name, bool down) {
          if ( (!(_pause > 1)) & down) {
               buttonPress(-1);
          }
          });
     if (_active) {
         _response1->activate();
         _response2->activate();
         _response3->activate();
          _burn->activate();
     }
#else
     Touchscreen* touch = Input::get<Touchscreen>();
     touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
         this->touchBeganCB(event,focus);
     });
     touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
         this->touchEndedCB(event,focus);
     });
     touch->addMotionListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus) {
       this->touchesMovedCB(event, previous, focus);
     });
#endif
     /*
    _response1->addListener([=](const std::string& name, bool down) {
         if ( (!(_pause > 1)) & down) {
              _response1->setPosition(0, 0);
         } else if ( (!(_pause > 1)) & !down & _response1->containsScreen(mouse->pointerPosition())) {
             buttonPress(0);
         } else {
              _response1->setVisible(true);
         }
        });
    _response2->addListener([=](const std::string& name, bool down) {
        if ( (!(_pause > 1)) & !down & _response2->containsScreen(mouse->pointerPosition())) {
             buttonPress(1);
        }
        });
    _response3->addListener([=](const std::string& name, bool down) {
        if ( (!(_pause > 1)) & !down & _response3->containsScreen(mouse->pointerPosition())) {
             buttonPress(2);
        }
        });
     _burn->addListener([=](const std::string& name, bool down) {
         if ( (!(_pause > 1)) & !down & _burn->containsScreen(mouse->pointerPosition())) {
              buttonPress(-1);
         }
         });
*/
     //_response1->setText(_currentCard.getResponse(0).getText());
     
    if (_active) {
         _burn->activate();
    }
    // _field->addTypeListener([=](const std::string& name, const std::string& value) {
    //     CULog("Change to %s",value.c_str());
    // });
    // _field->addExitListener([=](const std::string& name, const std::string& value) {
    //     CULog("Finish to %s",value.c_str());
    //     _result->setText(strcat("Result: ","asdF"));
    // });
     _currEvent->setVisible(false);
    _currEvent->setText(_currentCard.getText());
     setBurnText();
     //_burnText->setText(resourceString({_currentCard.getResource(0),_currentCard.getResource(1),_currentCard.getResource(2),_currentCard.getResource(3)}));
    //_resourceCount->setText(resourceString(_resources));
    std::vector<int> threeResponses = _currentCard.getThreeRandomResponses();
    _responseId1=threeResponses[0];
    _responseId2=threeResponses[1];
     _responseId3=threeResponses[2];
    _responseText1->setText(_responses[_responseId1].getText());
    //_responseCost1->setText(resourceString(_responses[_responseId1].getResources()));
    //_responseOutcome1->setText(_responses[_responseId1].getOutcome());
    _responseText2->setText(_responses[_responseId2].getText());
    //_responseCost2->setText(resourceString(_responses[_responseId2].getResources()));
    //_responseOutcome2->setText(_responses[_responseId2].getOutcome());
    _responseText3->setText(_responses[_responseId3].getText());
    //_responseCost3->setText(resourceString(_responses[_responseId3].getResources()));
    //_responseOutcome3->setText(_responses[_responseId3].getOutcome());
    _responseId1=threeResponses[0];
    _responseId2=threeResponses[1];
     _responseId3=threeResponses[2];
     responseUpdate(_responseId1, 1);
     responseUpdate(_responseId2, 2);
     responseUpdate(_responseId3, 3);
     _responseCard1->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
     _responseCard2->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
     _responseCard3->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
     setResources();
    // if (_active) {
    //     _field->activate();
    // }
    return success;
}

void GameScene::responseUpdate(const int responseId, const int response) {
     int id = _responses[responseId].getCards()[0];
     int level = _cards[id].getLevel();
     string responseAddress = "response" + std::to_string(level);
     string glowAddress = "glow" + std::to_string(level);
     auto responseTexture = _assets->get<Texture>(responseAddress);
     auto responseGlow = _assets->get<Texture>(glowAddress);
     switch (response){
          case 1:
               _responseTexture1->setTexture(responseTexture);
               _responseGlow1->setTexture(responseGlow);
          case 2:
               _responseTexture2->setTexture(responseTexture);
               _responseGlow2->setTexture(responseGlow);
          case 3:
               _responseTexture3->setTexture(responseTexture);
               _responseGlow3->setTexture(responseGlow);
     }
     setResponseResources(1);
     setResponseResources(2);
     setResponseResources(3);
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
          _currEvent->setColor(Color4::WHITE);
          //_resourceCount->setVisible(true);
          _response1->setColor(Color4::WHITE);
          _response1->setVisible(true);
          _response2->setColor(Color4::WHITE);
          _response2->setVisible(true);
          _response3->setColor(Color4::WHITE);
          _response3->setVisible(true);
          _pause = 0;
          _currentCard = _cards[_currentDeck.back()];
          _currentDeck.pop_back();
          _currEvent->setText(_currentCard.getText());
          _deckNode->setFrontTexture(_currentCard.getTexture());
          setBurnText();
          //_burnText->setText(resourceString({_currentCard.getResource(0),_currentCard.getResource(1),_currentCard.getResource(2),_currentCard.getResource(3)}));
          //_resourceCount->setText(resourceString(_resources));
          if (!_keepCards) {
              std::vector<int> threeResponses = _currentCard.getThreeRandomResponses();
              _responseId1 = threeResponses[0];
              _responseId2 = threeResponses[1];
               _responseId3 = threeResponses[2];
          }
          _responseText1->setText(_responses[_responseId1].getText());
          //_responseCost1->setText(resourceString(_responses[_responseId1].getResources()));
          //_responseOutcome1->setText(_responses[_responseId1].getOutcome());
          _responseText2->setText(_responses[_responseId2].getText());
          //_responseCost2->setText(resourceString(_responses[_responseId2].getResources()));
          //_responseOutcome2->setText(_responses[_responseId2].getOutcome());
          _responseText3->setText(_responses[_responseId3].getText());
          //_responseCost3->setText(resourceString(_responses[_responseId3].getResources()));
          //_responseOutcome3->setText(_responses[_responseId3].getOutcome());
          responseUpdate(_responseId1, 1);
          responseUpdate(_responseId2, 2);
          responseUpdate(_responseId3, 3);
          _responseCard1->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
          _responseCard2->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
          _responseCard3->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
          // _responseId1=twoResponses[0];
          // _responseId2=twoResponses[1];
          // _responseId3=_currentCard.getGuaranteed();
          _deckNode->setSize(int(_currentDeck.size()));
          _deckNode->setNextSize(int(_nextDeck.size()));
          _deckNode->setDrawFront(true);
     }
#ifndef CU_TOUCH_SCREEN
     if (!(_pause > 1)) {
          if (_response1->containsScreen(_mouse->pointerPosition())) {
               _displayCard->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response2->containsScreen(_mouse->pointerPosition())) {
               _displayCard->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response3->containsScreen(_mouse->pointerPosition())) {
               _displayCard->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
               _displayCard->setVisible(true);
          } else {
               _displayCard->setVisible(false);
          }
     }
#endif
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

/*
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
               newCard.allocate("Enemy Attacks", 1, "",{0, 0, 0});
               break;
          case 2:
               firstResponse.allocate("Stab", "Shuffle in Enemy Attacks and Enemy Wounded", {0,0,0,0}, {1,3}, false, false);
               secondResponse.allocate("Heavy Slash", "Shuffle in Enemy Maimed and Enemy Enraged", {0,0,0,0}, {4,7}, false, false);
               thirdResponse.allocate("Maintain Spacing", "Shuiffle in Enemy Attacks", {0,0,0,0}, {1}, false, false);
               newCard.allocate("Enemy Exposed", 2, "",{0, 0, 0});
               break;
          case 3:
               firstResponse.allocate("Maim", "Shuffle in Enemy Maimed", {0,0,0,0}, {4}, false, false);
               secondResponse.allocate("Slash", "Shuffle in Exposed and Enemy Wounded", {0,0,0,0}, {2,3}, false, false);
               thirdResponse.allocate("Tease", "Shuffle in 2 Enemy Wounded and Enemy Enraged", {0,0,0,0}, {3,3,7}, false, false);
               newCard.allocate("Enemy Wounded", 3, "",{firstResponse, secondResponse, thirdResponse});
               break;
          case 4:
               firstResponse.allocate("Execute", "Win", {0,0,0,0}, {}, true, false);
               secondResponse.allocate("Crush", "Win", {0,0,0,0}, {}, true, false);
               thirdResponse.allocate("Taunt", "Shuffle in 2 Enemy Maimed and Enemy Enraged", {0,0,0,0}, {4,4,7}, false, false);
               newCard.allocate("Enemy Maimed", 4, "",{firstResponse, secondResponse, thirdResponse});
               break;
          case 5:
               firstResponse.allocate("Disengage", "Shuffle in Enemy Attacks and Player Wounded", {0,0,0,0}, {1,5}, false, false);
               secondResponse.allocate("Save Strength", "Shuffle in Player Maimed", {0,0,0,0}, {6}, false, false);
               thirdResponse.allocate("Stand", "Shuffle in Enemy Attacks and Enemy Enranged", {0,0,0,0}, {2,7}, false, false);
               newCard.allocate("Player Wounded", 5, "",{firstResponse, secondResponse, thirdResponse});
               break;
          case 6:
               firstResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               secondResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               thirdResponse.allocate("Concede", "Lose", {0,0,0,0}, {}, false, true);
               newCard.allocate("Player Maimed", 6, "",{firstResponse, secondResponse, thirdResponse});
               break;
          case 7:
               firstResponse.allocate("Almost Dodge", "Shuffle in Enemy Exposed and Player Wounded", {0,0,0,0}, {2,5}, false, false);
               secondResponse.allocate("Eye for an Eye", "Shuffle in Player Wounded and Enemy Wounded", {0,0,0,0}, {3,6}, false, false);
               thirdResponse.allocate("Double Down", "Shuffle in 2 Enemy Enranged", {0,0,0,0}, {7,7}, false, false);
               newCard.allocate("Enemy Enraged", 7, "",{firstResponse, secondResponse, thirdResponse});
               break;
     }
     return newCard;
}
*/

string GameScene::resourceString(std::vector<int> resources) {
    std::stringstream s;
    s << "Bl: " << resources[0] << ", ";
    s << "F: " << resources[1] << ", ";
    s << "L: " << resources[2] << ", ";
    s << "Br: " << resources[3];

    return s.str();
}

void GameScene::buttonPress(const int r){
     if (r == -1){
          for (int i = 0; i < _resources.size(); i++) {
               _resources[i] += _currentCard.getResource(i);
               setResources();
          }
     } else {
          Response response;
          if (r == 0){
               response=_responses[_responseId1];
          } else if (r == 1){
               response=_responses[_responseId2];
          } else {
               response=_responses[_responseId3];
          }
          std::vector<int> cost = response.getResources();
          for (int i = 0; i < cost.size(); i++) {
              if (_resources[i] < cost[i]) {
                  if (r == 0) {
                      _responseText1->setText("Need Resources");
                       _responseText1->setForeground(Color4::WHITE);
                      _response1->setColor(Color4::GRAY);
                  } else if (r == 1) {
                      _responseText2->setText("Need Resources");
                       _responseText2->setForeground(Color4::WHITE);
                      _response2->setColor(Color4::GRAY);
                  }
                  else if (r == 2) {
                      _responseText3->setText("Need Resources");
                       _responseText3->setForeground(Color4::WHITE);
                      _response3->setColor(Color4::GRAY);
                  }
                  //_currentDeck.addCardFront(_currentCard);
                  //_keepCards = true;
                  //_pause = 40;
                  return;
              }
          }
          for (int i = 0; i < cost.size(); i++) {
              _resources[i] -= cost[i];
               setResources();
          }
          if (response.getWin()){
               _currEvent->setText("YOU WIN!");
               _currEvent->setVisible(true);
               return;
          } else if (response.getLose()){
               _currEvent->setText("YOU DIED!");
               _currEvent->setVisible(true);
               return;
          }
          //_currEvent->setText("Clicked " + std::to_string(r + 1));
          //_currentDeck.printDeck();
          std::vector<int> cards =response.getCards();
          for (int i = 0; i < cards.size(); i++){
               int newCard = cards[i];
               _nextDeck.push_back(newCard);
          }
     }
     //CULog("Next Deck:");
     //_nextDeck.printDeck();
     //CULog("Current Deck:");
     //_currentDeck.printDeck();
     if (_currentDeck.size() == 0){
          _currEvent->setText("Shuffling Next Event Deck...");
          _currEvent->setColor(Color4::BLACK);
          _deckNode->swapTextures();
          _deckNode->setNextSize(0);
          if (_nextDeck.size() > 0){
               _currentDeck = _nextDeck;
               std::random_device rd;
               std::mt19937 g(rd());
                std::shuffle(_currentDeck.begin(), _currentDeck.end(), g);
               _nextDeck = {};
          } else {
               _deckNode->setVisible(false);
               _currEvent->setText("YOU DIED!");
               _currEvent->setVisible(true);
               return;
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
     _keepCards = false;
     _pause = 40;
     /*
     _currentCard = _currentDeck.draw();
     _currEvent->setText(_currentCard.getText());
     _responseText1->setText(_currentCard.getResponse(0).getText());
     _responseText2->setText(_currentCard.getResponse(1).getText());
     _responseText3->setText(_currentCard.getResponse(2).getText());
      */
}

void GameScene::setResources(){
     _bladeText->setText(to_string(_resources[0]));
     _flourishText->setText(to_string(_resources[1]));
     _lungeText->setText(to_string(_resources[2]));
     _brawnText->setText(to_string(_resources[3]));
}

void GameScene::setResponseResources(const int response){
     std::vector<int> cost = _responses[_responseId3].getResources();
     if (response == 1){
          cost = _responses[_responseId1].getResources();
     } else if (response ==2){
          cost = _responses[_responseId2].getResources();
     }
     int acc = 1;
     std::shared_ptr<cugl::scene2::NinePatch> responseResourcePointer;
     std::shared_ptr<cugl::scene2::Label> responseResourceAmountPointer;
     for (int i = 0; i < cost.size(); i++){
          if (cost[i] > 0){
               string path = "lab_response" + to_string(response) + "_up_resource" + to_string(acc);
               string amountPath = "lab_response" + to_string(response) + "_up_resource" + to_string(acc) + "_amount";
               responseResourcePointer = std::dynamic_pointer_cast<scene2::NinePatch>(_assets->get<scene2::SceneNode>(path));
               responseResourceAmountPointer = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>(amountPath));
               responseResourcePointer->setVisible(true);
               responseResourceAmountPointer->setVisible(true);
               acc += 1;
               string resource = "brawn";
               if (i == 0){
                    resource = "blade";
               } else if (i == 1){
                    resource = "flourish";
               } else if (i == 2){
                    resource = "lunge";
               }
               responseResourcePointer->setTexture(_assets->get<Texture>(resource));
               responseResourceAmountPointer->setText(to_string(cost[i]));
          }
     }
     for (int j = acc; j <= 4; j++){
          string path = "lab_response" + to_string(response) + "_up_resource" + to_string(j);
          string amountPath = "lab_response" + to_string(response) + "_up_resource" + to_string(j) + "_amount";
          responseResourcePointer = std::dynamic_pointer_cast<scene2::NinePatch>(_assets->get<scene2::SceneNode>(path));
          responseResourceAmountPointer = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>(amountPath));
          responseResourcePointer->setVisible(false);
          responseResourceAmountPointer->setVisible(false);
     }
     responseResourcePointer = nullptr;
     responseResourceAmountPointer = nullptr;
}

void GameScene::setBurnText(){
     for (int i = 0; i < 4; i ++){
          if (_currentCard.getResource(i) > 0){
               _burnText->setText(to_string(_currentCard.getResource(i)));
               string resource = "brawn";
               if (i == 0){
                    resource = "blade";
               } else if (i == 1){
                    resource = "flourish";
               } else if (i == 2){
                    resource = "lunge";
               }
               _burnTexture->setTexture(_assets->get<Texture>(resource));
          }
     }
}

void GameScene::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
     if (!(_pause > 1)){
          touchBegan(event.position);
     }
}

void GameScene::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
     if (!(_pause > 1)){
          touchEnded(event.position);
     }
}

void GameScene::touchBegan(const cugl::Vec2& pos) {
     if (_response1->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response2->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response3->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     } else {
          _displayCard->setVisible(false);
     }
}

void GameScene::touchEnded(const cugl::Vec2& pos) {
     _displayCard->setVisible(false);
     if (_response1->containsScreen(pos)) {
          buttonPress(0);
     }
     else if (_response2->containsScreen(pos)) {
          buttonPress(1);
     }
     else if (_response3->containsScreen(pos)) {
          buttonPress(2);
     }
     else if (_burn->containsScreen(pos)) {
          buttonPress(-1);
     }
}

void GameScene::touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus){
     if (!(_pause > 1)){
          touchMoved(event.position);
     }
}

void GameScene::touchMoved(const cugl::Vec2& pos){
     if (_response1->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response2->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response3->containsScreen(pos)) {
          _displayCard->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
          _displayCard->setVisible(true);
     } else {
          _displayCard->setVisible(false);
     }
}
