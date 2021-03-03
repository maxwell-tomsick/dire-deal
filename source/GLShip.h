//
//  GLShip.h
//  Programming Lab
//
//  This class tracks all of the state (position, velocity, rotation) of a
//  single ship. In order to obey the separation of the model-view-controller
//  pattern, controller specific code (such as reading the keyboard) is not
//  present in this class.
//
//  Looking through this code you will notice certain optimizations. We want
//  to eliminate as many "new" statements as possible in the draw loop. In
//  game programming, it is considered bad form to have "new" statements in
//  an update or a graphics loop if you can easily avoid it.  Each "new" is
//  a potentially  expensive memory allocation.
//
//  To get around this, we have predeclared some Vector2 objects.  These are
//  used by the draw method to position the objects on the screen. As we know
//  we will need that memory animation frame, it is better to have them
//  declared ahead of time (even though we are not taking state across frame
//  boundaries).
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#ifndef __GL_SHIP_H__
#define __GL_SHIP_H__
#include <cugl/cugl.h>
#include "GLShadowNode.h"
#include <vector>

/** The number of frames until we can fire again */
#define RELOAD_RATE 3

/**
 * Model class representing an alien ship.
 *
 * Note that the graphics resources in this class are static.  That
 * is because all ships share the same image file, and it would waste
 * memory to load the same image file for each ship.
 */
class Ship {
private:
    /** Position of the ship */
    cugl::Vec2 _pos;
    /** Velocity of the ship */
    cugl::Vec2 _vel;
    /** Color to tint this ship (red or blue) */
    cugl::Color4 _tint;
    /** Color of the ships shadow (cached) */
    cugl::Color4 _stint;
    /** The ship identifier */
    int _id;
    
    /** Radius of the ship in pixels */
    float _radius;
    /** Mass/weight of the ship. Used in collisions. */
    float _mass;
    
    // The following are protected, because they have no accessors
    /** Offset of the ships target */
    cugl::Vec2 _tofs;
    /** Current angle of the ship */
    float _ang;
    /** Accumulator variable to turn faster as key is held down */
    float _dang;
    /** Countdown to limit refire rate */
    int _refire;
    
    // Asset references.  These should be set by GameMode
    /** Reference to ship's sprite and ship's shadow's sprite for drawing */
    std::shared_ptr<ShadowNode> _shadowNode;
    /** Reference to the target reticule node */
    std::shared_ptr<cugl::scene2::PolygonNode>   _targetNode;
    /** Reference to the ships texture */
    std::shared_ptr<cugl::Texture> _shipTexture;
    /** Texture for the target reticule */
    std::shared_ptr<cugl::Texture> _targetTexture;
    std::shared_ptr<cugl::scene2::SceneNode>   _sceneGraphNode;
    std::vector<std::shared_ptr<ShadowNode>> _sNodes;
    cugl::Rect _bounds;
    int _jumping;
public:
#pragma mark Properties
    
    /**
     * Returns the id of the ship.
     *
     * @return the id of the ship.
     */
    int getSID() const {
        return _id;
    }

    /**
     * Sets the id of the ship.
     *
     * @param id    The id of the ship.
     */
    void setSID(int id) {
        _id = id;
    }
    
    void setBounds(cugl::Rect bounds) {
        _bounds = bounds;
    }
    
    void jump() {
        _jumping = 80;
    }
    
    void fall() {
        //CULog("%d",_jumping);
        if (_jumping > 0){
            _jumping -= 1;
        }
    }
    
    int getJumping() {
        return _jumping;
    }

    /**
     * Returns the position of this ship.
     *
     * This is location of the center pixel of the ship on the screen.
     *
     * @return the position of this ship
     */
    const cugl::Vec2& getPosition() const {
        return _pos;
    }
    
    /**
     * Sets the position of this ship.
     *
     * This is location of the center pixel of the ship on the screen.
     *
     * @param value the position of this ship
     */
    void setPosition(cugl::Vec2 value);

    /**
     * Returns the velocity of this ship.
     *
     * This value is necessary to control momementum in ship movement.
     *
     * @return the velocity of this ship
     */
    const cugl::Vec2& getVelocity() const {
        return _vel;
    }

    /**
     * Sets the velocity of this ship.
     *
     * This value is necessary to control momementum in ship movement.
     *
     * @param value the velocity of this ship
     */
    void setVelocity(cugl::Vec2 value) {
        _vel = value;
    }
    
    /**
     * Returns the angle that this ship is facing.
     *
     * The angle is specified in degrees, not radians.
     * IMPORTANT: This is different from how scene graphs work.
     *
     * @return the angle of the ship
     */
    float getAngle() const {
        return _ang;
    }
    
    /**
     * Sets the angle that this ship is facing.
     *
     * The angle is specified in degrees, not radians.
     *
     * @param value the angle of the ship
     */
    void setAngle(float value);

    void setScale(float value);
    
    /**
     * Returns the tint color for this ship.
     *
     * We can change how an image looks without loading a new image by
     * tinting it differently.
     *
     * @return the tint color
     */
    cugl::Color4f getColor() const {
        return _tint;
    }
    
    /**
     * Sets the tint color for this ship.
     *
     * We can change how an image looks without loading a new image by
     * tinting it differently.
     *
     * @param value the tint color
     */
    void setColor(cugl::Color4f value);
    
    /**
     * Returns true if the ship can fire its weapon
     *
     * Weapon fire is subjected to a cooldown.  You can modify this
     * to see what happens if RELOAD_RATE is faster or slower.
     *
     * @return true if the ship can fire
     */
    bool canFireWeapon() const {
        return (_refire > RELOAD_RATE && _jumping == 0);
    }
    
    /**
     * Resets the reload counter so the ship cannot fire again immediately.
     *
     * The ship must wait RELOAD_RATE steps before it can fire.
     */
    void reloadWeapon() {
        _refire = 0;
    }

    /**
     * Returns the mass of the ship.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the ship mass
     */
    float getMass() const {
        return _mass;
    }

    /**
     * Returns the radius of the ship.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the ship radius
     */
    float getRadius() {
        return _radius;
    }
    
#pragma mark Graphics
    /**
     * Returns the scene graph node for this ship
     *
     * The scene graph node contains the ship, its shadow, and the
     * target. It is centered on the ship itself. This method
     * returns the null pointer if the textures have not yet
     * been set.
     *
     * @return the image texture for this ship
     */
    const std::shared_ptr<cugl::scene2::AnimationNode> getShadowNode() const {
        return _shadowNode;
    }
    
    /**
     * Returns the target node for this ship
     *
     * @return the target node for this ship
     */
    const std::shared_ptr<cugl::scene2::PolygonNode> getTargetNode() const {
        return _targetNode;
    }
    
    const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() const {
        return _sceneGraphNode;
    }
    
    /**
     * Sets the textures for this ship.
     *
     * The two textures are the ship texture and the target texture. The
     * scene graph node associated with this ship is nullptr until these
     * values are set.
     *
     * @param ship      The texture for the ship filmstrip
     * @param target    The texture for the ship target
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& ship,
                     const std::shared_ptr<cugl::Texture>& target);
    
    /**
     * Returns the image texture for the target reticule
     *
     * @return the image texture for the target reticule
     */
    const std::shared_ptr<cugl::Texture> getTargetTexture() const {
        return _targetTexture;
    }

    /**
     * Returns the image texture for the ship filmstrip
     *
     * @return the image texture for the ship filmstrip
     */
    const std::shared_ptr<cugl::Texture> getShipTexture() const {
        return _shipTexture;
    }
    
#pragma mark Constructors
    /**
     * Creates a ship with the default values.
     *
     * To properly initialize the ship, you should call the init
     * method.
     */
    Ship();
    
    /**
     * Disposes the ship, releasing all resources.
     */
    ~Ship() { dispose(); }

    /**
     * Disposes the ship, releasing all resources.
     */
    void dispose();
    
    /**
     * Initializes a new ship at the given location with the given facing.
     *
     * This method does NOT create a scene graph node for this ship.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param ang The initial angle of rotation
     *
     * @return true if the initialization was successful
     */
    bool init(float x, float y, float ang);

    /**
     * Returns a newly allocated ship at the given location with the given facing.
     *
     * This method does NOT create a scene graph node for this ship.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param ang The initial angle of rotation
     *
     * @return a newly allocated ship at the given location with the given facing.
     */
    static std::shared_ptr<Ship> alloc(float x, float y, float ang) {
        std::shared_ptr<Ship> result = std::make_shared<Ship>();
        return (result->init(x,y,ang) ? result : nullptr);
    }

#pragma mark Movement
    /**
     * Moves the ship by the specified amount.
     *
     * Forward is the amount to move forward, while turn is the angle to turn the ship
     * (used for the "banking" animation. This method performs no collision detection.
     * Collisions are resolved afterwards.
     *
     * @param forward    Amount to move forward
     * @param turn        Amount to turn the ship
     */
    void move(float forward, float turn);
    
    /**
     * Update the animation of the ship to process a turn
     *
     * Turning changes the frame of the filmstrip, as we change from a level ship to
     * a hard bank.  This method also updates the field dang cumulatively.
     *
     * @param turn Amount to turn the ship
     */
    void processTurn(float turn);

    /**
     * Aim the target reticule at the opponent
     *
     * The target reticule always shows the location of our opponent.  In order
     * to place it we need to know where our opponent is.  This method is
     * called by the game engine to let us know the location of our
     * opponent.
     *
     * @param other The opponent
     */
    void acquireTarget(const std::shared_ptr<Ship>& other);

};

#endif /* __GL_SHIP_H__ */

