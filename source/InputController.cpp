//
//  GLInputController.cpp
//  Programming Lab
//
//  This class buffers in input from the devices and converts it into its
//  semantic meaning. If your game had an option that allows the player to
//  remap the control keys, you would store this information in this class.
//  That way, the main game scene does not have to keep track of the current
//  key mapping.
//
//  Author: Walker M. White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#include <cugl/cugl.h>
#include "InputController.h"

using namespace cugl;

/** How far we must turn the tablet for the accelerometer to register */
#define EVENT_ACCEL_THRESH  M_PI/10.0f
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50

/**
 * Creates a new input controller with the default settings
 *
 * To use this controller, you will need to initialize it first
 */
InputController::InputController() :
_player(0),
_upperLeft(false),
_upperRight(false),
_lowerLeft(false),
_lowerRight(false),
_lowerRightRight(false),
_lowerRightLeft(false),
_middleLeft(false),
_swipeDown(false),
_swipeUp(false){
}

/**
 * Initializes a new input controller for the specified player.
 *
 * The game supports two players working against each other in hot seat mode.
 * We need a separate input controller for each player. In keyboard, this is
 * WASD vs. Arrow keys.  Doing this on mobile requires you to get a little
 * creative.
 *
 * @param id Player id number (0..1)
 *
 * @return true if the player was initialized correctly
 */
bool InputController::init(int id) {
#ifdef CU_MOBILE
    bool success = true;
    _player = id;
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(id,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(id,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(id,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
    return success;
#else
    _player = id;
    return true;
#endif
}

/**
 * Reads the input for this player and converts the result into game logic.
 *
 * This is an example of polling input.  Instead of registering a listener,
 * we ask the controller about its current state.  When the game is running,
 * it is typically best to poll input instead of using listeners.  Listeners
 * are more appropriate for menus and buttons (like the loading screen).
 */
void InputController::readInput() {
#ifdef CU_MOBILE
    // Left this from lab in case of reference - Rose
    // YOU NEED TO PUT SOME CODE HERE
    // _forward = _turning = 0;
    // if (_player == 0) {
    //     Vec3 acc = Input::get<Accelerometer>()->getAcceleration();
    //     float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z));
    //     bool left = (pitch > EVENT_ACCEL_THRESH);
    //     bool right = (pitch < -EVENT_ACCEL_THRESH);
    //     if (left){
    //         _turning = 1;
    //     } else if (right) {
    //         _turning = -1;
    //     }

#else

    // Left this from lab in case of reference - Rose
    // Figure out, based on which player we are, which keys
    // control our actions (depends on player).
    // KeyCode up, left, right, down, shoot, jump;
    // if (_player == 0) {
    //     up    = KeyCode::ARROW_UP;
    //     shoot = KeyCode::SPACE;
    //     jump = KeyCode::COMMA;

    // Left this from lab in case of reference - Rose
       // Convert keyboard state into game commands
    // _forward = _turning = 0;
    // Keyboard* keys = Input::get<Keyboard>();
    // if (keys->keyDown(up) && !keys->keyDown(down)) {
    //     _forward = 1;
    // } else if (keys->keyDown(down) && !keys->keyDown(up)) {
    //     _forward = -1;
    // }

#endif
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    Size dimen = Application::get()->getDisplaySize();
    if (event.position.x > 2 * dimen.width/3 && event.position.y <= dimen.height/2 && !_lowerRight){
        _upperRight = true;
    } else if (event.position.x > 2 * dimen.width/3 && event.position.y > dimen.height/2 && !_upperRight){
        _lowerRight = true;
        if (event.position.x > 5 * dimen.width/6 && !_lowerRightLeft){
            _lowerRightRight = true;
        } else if (!_lowerRightRight){
            _lowerRightLeft = true;
        }
    } else if (event.position.x <= dimen.width/3 && event.position.y < dimen.height/3 && !_lowerLeft && !_middleLeft){
        _upperLeft = true;
    } else if (event.position.x <= dimen.width/3 && event.position.y < 2 * dimen.height/3 && !_upperLeft && !_lowerLeft){
        _middleLeft = true;
    } else if (event.position.x <= dimen.width/3 && !_upperLeft && !_middleLeft){
        _lowerLeft = true;
    } else if (event.position.x > dimen.width/3){
        _rtouch = event.position;
    }
}
 
/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    // Gesture has ended.  Give it meaning.
    //_forward = 0;
    Size dimen = Application::get()->getDisplaySize();
    if (event.position.x > dimen.width/2) {
        _upperRight = false;
        _lowerRight = false;
        _lowerRightRight = false;
        _lowerRightLeft = false;
    }
    if (event.position.x <= dimen.width/2) {
        _upperLeft = false;
        _lowerLeft = false;
        _middleLeft = false;
    }
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    Size dimen = Application::get()->getDisplaySize();
    // Only check for swipes in the main zone if there is more than one finger.
    if (!_swipeUp) {
        if ((_rtouch.y-pos.y) > SWIPE_LENGTH && pos.x > dimen.width/3 && pos.x < 2 * dimen.width/3) {
            _swipeUp = true;
        }
    } else if (!_swipeDown){
        if ((_rtouch.y-pos.y) < -SWIPE_LENGTH && pos.x > dimen.width/3 && pos.x < 2 * dimen.width/3) {
            _swipeDown = true;
        }
    }
}
