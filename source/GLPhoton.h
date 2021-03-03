//
//  GLPhoton.h
//  GameLab
//
//  Created by Maxwell Tomsick on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef GLPhoton_h
#define GLPhoton_h
#include <cugl/cugl.h>

/**
 * An inner class that represents a single Photon.
 *
 * To count down on memory references, the photon is "flattened" so that
 * it contains no other objects.
 */
class Photon {
public:
    /** Photon position */
    cugl::Vec2 pos;
    /** Photon velocity */
    cugl::Vec2 vel;
    /** The ship (SID) that fired this photon */
    int ship;
    /** Age for the photon in frames (for decay) */
    int age;
    /** The drawing scale of the photon (to vary the size) */
    float scale;
    
    /**
     * Creates a new empty photon with age -1.
     *
     * Photons created this way "do not exist".  This constructor is
     * solely for preallocation.  To actually use a photon, use the
     * allocate() method.
     */
    Photon();
    
    /**
     * Allocates a photon by setting its position and velocity.
     *
     * A newly allocated photon starts with age 0.
     *
     * @param id The ship id
     * @param p  The position
     * @param v  The velocity
     */
    void allocate(const int id, const cugl::Vec2 p, const cugl::Vec2 v);
    
    /**
     * Moves the photon one animation frame
     *
     * This method also advances the age of the photon. This method does
     * not bounce off walls. We moved all collisions to the collision
     * controller where they belong.
     */
    void update();

    /**
     * Flags the photon for deletion.
     *
     * This just sets the age of the photon to be the maximum age.
     * That way it is removed soon after during the collection phase.
     */
    void destroy();
};

#endif /* GLPhoton_h */
