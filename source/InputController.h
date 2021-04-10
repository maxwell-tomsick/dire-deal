//
//  GLInputController.h
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
#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__
#include <cugl/cugl.h>

/**
 * Device-independent input manager.
 *
 * This class supports both a keyboard and an X-Box controller.  Each player is
 * assigned an ID.  When the class is created, we check to see if there is a
 * controller for that ID.  If so, we use the controller.  Otherwise, we default
 * the the keyboard.
 */
class InputController {
private:
    /** Player id, to identify which keys map to this player */
    int _player;
    
    /** Did we press the fire button? */
    bool _didFire;
    bool _upperLeft;
    bool _upperRight;
    bool _lowerLeft;
    bool _lowerRight;
    bool _lowerRightRight;
    bool _lowerRightLeft;
    bool _middleLeft;
    bool _didJump;
    bool _swipeDown;
    bool _swipeUp;
    cugl::Vec2 _rtouch;

public:
     // Left this from lab in case of reference - Rose
    // /**
    //  * Returns the player for this controller
    //  *
    //  * @return the player for this controller
    //  */
    // int getPlayer() const {
    //     return _player;
    // }
    /**
     * Creates a new input controller with the default settings
     *
     * To use this controller, you will need to initialize it first
     */
    InputController();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~InputController() {}
    
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
    bool init(int id);

    /**
     * Reads the input for this player and converts the result into game logic.
     *
     * This is an example of polling input.  Instead of registering a listener,
     * we ask the controller about its current state.  When the game is running,
     * it is typically best to poll input instead of using listeners.  Listeners
     * are more appropriate for menus and buttons (like the loading screen).
     */
    void readInput();
    
#pragma mark -
#pragma mark Touch Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus    Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
};

#endif /* __INPUT_CONTROLLER_H__ */
