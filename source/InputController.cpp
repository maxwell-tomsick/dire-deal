// //
// //  GLInputController.cpp
// //  Programming Lab
// //
// //  This class buffers in input from the devices and converts it into its
// //  semantic meaning. If your game had an option that allows the player to
// //  remap the control keys, you would store this information in this class.
// //  That way, the main game scene does not have to keep track of the current
// //  key mapping.
// //
// //  Author: Walker M. White
// //  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
// //  Version: 2/21/21
// //
// #include <cugl/cugl.h>
// #include "InputController.h"

// using namespace cugl;

// /** How far we must turn the tablet for the accelerometer to register */
// #define EVENT_ACCEL_THRESH  M_PI/10.0f
// /** How far we must swipe up for a jump gesture */
// #define SWIPE_LENGTH    50


// /**
//  * Initializes a new input controller for the specified player.
//  *
//  * The game supports two players working against each other in hot seat mode.
//  * We need a separate input controller for each player. In keyboard, this is
//  * WASD vs. Arrow keys.  Doing this on mobile requires you to get a little
//  * creative.
//  *
//  * @param id Player id number (0..1)
//  *
//  * @return true if the player was initialized correctly
//  */
// bool InputController::init(int id) {
// #ifdef CU_MOBILE
//     bool success = true;
//     _player = id;
//     success = Input::activate<Accelerometer>();
//     Touchscreen* touch = Input::get<Touchscreen>();
//     touch->addBeginListener(id,[=](const cugl::TouchEvent& event, bool focus) {
//         this->touchBeganCB(event,focus);
//     });
//     touch->addEndListener(id,[=](const cugl::TouchEvent& event, bool focus) {
//         this->touchEndedCB(event,focus);
//     });
//     touch->addMotionListener(id,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
//         this->touchesMovedCB(event, previous, focus);
//     });
//     return success;
// #else
//     return true;
// #endif
// }

// /**
//  * Reads the input for this player and converts the result into game logic.
//  *
//  * This is an example of polling input.  Instead of registering a listener,
//  * we ask the controller about its current state.  When the game is running,
//  * it is typically best to poll input instead of using listeners.  Listeners
//  * are more appropriate for menus and buttons (like the loading screen).
//  */
// void InputController::readInput() {
// #ifdef CU_MOBILE
//     // Left this from lab in case of reference - Rose
//     // YOU NEED TO PUT SOME CODE HERE
//     // _forward = _turning = 0;
//     // if (_player == 0) {
//     //     Vec3 acc = Input::get<Accelerometer>()->getAcceleration();
//     //     float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z));
//     //     bool left = (pitch > EVENT_ACCEL_THRESH);
//     //     bool right = (pitch < -EVENT_ACCEL_THRESH);
//     //     if (left){
//     //         _turning = 1;
//     //     } else if (right) {
//     //         _turning = -1;
//     //     }

// #else

//     // Left this from lab in case of reference - Rose
//     // Figure out, based on which player we are, which keys
//     // control our actions (depends on player).
//     // KeyCode up, left, right, down, shoot, jump;
//     // if (_player == 0) {
//     //     up    = KeyCode::ARROW_UP;
//     //     shoot = KeyCode::SPACE;
//     //     jump = KeyCode::COMMA;

//     // Left this from lab in case of reference - Rose
//        // Convert keyboard state into game commands
//     // _forward = _turning = 0;
//     // Keyboard* keys = Input::get<Keyboard>();
//     // if (keys->keyDown(up) && !keys->keyDown(down)) {
//     //     _forward = 1;
//     // } else if (keys->keyDown(down) && !keys->keyDown(up)) {
//     //     _forward = -1;
//     // }

// #endif
// }

// #pragma mark -
// #pragma mark Touch Callbacks
// /**
//  * Callback for the beginning of a touch event
//  *
//  * @param t     The touch information
//  * @param event The associated event
//  */
// // void InputController::touchBegan(
// //     std::map<int, Card>& cards,
// //     std::map<int, Response>& responses,
// //     std::shared_ptr<DeckNode>& deckNode,
// //     std::shared_ptr<cugl::scene2::Button>& burn, 
// //     std::vector<int>& currentDeck,
// //     std::shared_ptr<cugl::scene2::Button>& currCardButton,
// //     Vec2& pos,
// //     cugl::Size &dimen,
// //     std::shared_ptr<cugl::scene2::Button>& response1,
// //     std::shared_ptr<cugl::scene2::Button>& response2, 
// //     std::shared_ptr<cugl::scene2::Button>& response3,
// //     bool& display2,
// //     bool& display3,
// //     int& responseId1,
// //     int& responseId2,
// //     int& responseId3,
// //     std::shared_ptr<cugl::scene2::NinePatch> & _displayCard,
// //     std::shared_ptr<cugl::scene2::Label> &displayCardBurnText, 
// //     std::shared_ptr<cugl::AssetManager> &assets,
// //     std::shared_ptr<cugl::scene2::NinePatch> &displayCardBurnTexture){
// //     Card displayCard;
// //     if (currCardButton->containsScreen(pos)) {
// //         Vec2 posi = deckNode->screenToNodeCoords(pos);
// //         deckNode->setOffset(Vec2(dimen.width * WIDTH_SCALE - posi.x, dimen.height * (HEIGHT_SCALE + DECK_SCALE * currentDeck.size()) - posi.y));
// //         deckNode->setDrag(true);
// //         burn->setVisible(true);
// //         deckNode->setCurrCardPos(posi);
// //     }
// //     else if (response1->containsScreen(pos)) {
// //         displayCard = cards[responses[responseId1].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     }
// //     else if (response2->containsScreen(pos) & display2) {
// //         displayCard = cards[responses[responseId2].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     }
// //     else if (response3->containsScreen(pos) & display3) {
// //         displayCard = cards[responses[responseId3].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     } else {
// //         _displayCard->setVisible(false);
// //     }
// // }
// // void InputController::touchBeganCB(
// //     const cugl::TouchEvent& event, 
// //     bool focus,
// //     std::map<int, Card>& cards,
// //     std::map<int, Response>& responses,
// //     std::shared_ptr<DeckNode>& deckNode,
// //     std::shared_ptr<cugl::scene2::Button>& burn, 
// //     std::vector<int>& currentDeck,
// //     std::shared_ptr<cugl::scene2::Button>& currCardButton,
// //     Vec2& pos,
// //     cugl::Size &dimen,
// //     std::shared_ptr<cugl::scene2::Button>& response1,
// //     std::shared_ptr<cugl::scene2::Button>& response2, 
// //     std::shared_ptr<cugl::scene2::Button>& response3,
// //     bool& display2,
// //     bool& display3,
// //     int& responseId1,
// //     int& responseId2,
// //     int& responseId3,
// //     std::shared_ptr<cugl::scene2::NinePatch> & _displayCard,
// //     std::shared_ptr<cugl::scene2::Label> &displayCardBurnText, 
// //     std::shared_ptr<cugl::AssetManager> &assets,
// //     std::shared_ptr<cugl::scene2::NinePatch> &displayCardBurnTexture,
// //     int& movement,
// //     Vec2& prev,
// //     int& act) {
// //     if ((movement == 0) & !deckNode->getDrag() & act == 60){
// //           Card displayCard;
// //     if (currCardButton->containsScreen(pos)) {
// //         Vec2 posi = deckNode->screenToNodeCoords(pos);
// //         deckNode->setOffset(Vec2(dimen.width * WIDTH_SCALE - posi.x, dimen.height * (HEIGHT_SCALE + DECK_SCALE * currentDeck.size()) - posi.y));
// //         deckNode->setDrag(true);
// //         burn->setVisible(true);
// //         deckNode->setCurrCardPos(posi);
// //     }
// //     else if (response1->containsScreen(pos)) {
// //         displayCard = cards[responses[responseId1].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     }
// //     else if (response2->containsScreen(pos) & display2) {
// //         displayCard = cards[responses[responseId2].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     }
// //     else if (response3->containsScreen(pos) & display3) {
// //         displayCard = cards[responses[responseId3].getCards()[0]];
// //         _resourceController.setDisplayCardBurnText(displayCard, displayCardBurnText, assets, displayCardBurnTexture);
// //         _displayCard->setTexture(displayCard.getTexture());
// //         _displayCard->setVisible(true);
// //     } else {
// //         _displayCard->setVisible(false);
// //     }
// //     } else if (movement == 5 && currCardButton->containsScreen(event.position)){
// //         prev = event.position;
// //     }
// // }
// /**
//  * Callback for the end of a touch event
//  *
//  * @param t     The touch information
//  * @param event The associated event
//  */
// void InputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
//     // Gesture has ended.  Give it meaning.
//     //_forward = 0;
// }

// /**
//  * Callback for a touch moved event.
//  *
//  * @param event The associated event
//  * @param previous The previous position of the touch
//  * @param focus    Whether the listener currently has focus
//  */
// void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
//     Vec2 pos = event.position;
//     Size dimen = Application::get()->getDisplaySize();
//     // Only check for swipes in the main zone if there is more than one finger.
//     // if (!_swipeUp) {
//     //     if ((_rtouch.y-pos.y) > SWIPE_LENGTH && pos.x > dimen.width/3 && pos.x < 2 * dimen.width/3) {
//     //         _swipeUp = true;
//     //     }
//     // } else if (!_swipeDown){
//     //     if ((_rtouch.y-pos.y) < -SWIPE_LENGTH && pos.x > dimen.width/3 && pos.x < 2 * dimen.width/3) {
//     //         _swipeDown = true;
//     //     }
//     // }
// }
