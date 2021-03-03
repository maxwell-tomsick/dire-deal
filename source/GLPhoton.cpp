//
//  GLPhoton.cpp
//  GameLab (Mac)
//
//  Created by Maxwell Tomsick on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include <cugl/cugl.h>
#include "GLPhoton.h"
/** Number of animation frames a photon lives before deleted */
#define MAX_AGE         100

using namespace cugl;

/**
 * Creates a new empty photon with age -1.
 *
 * Photons created this way "do not exist".  This constructor is
 * solely for preallocation.  To actually use a photon, use the
 * allocate() method.
 */
Photon::Photon() {
    pos = Vec2::ZERO;
    vel = Vec2::ZERO;
    age = -1;
    ship = -1;
}
        
/**
 * Allocates a photon by setting its position and velocity.
 *
 * A newly allocated photon starts with age 0.
 *
 * @param id The ship id
 * @param p  The position
 * @param v  The velocity
 */
void Photon::allocate(const int id, const Vec2 p, const Vec2 v) {
    pos = p;
    vel = v;
    age = 0;
    ship = id;
    scale = 1.25f;
}
        
/**
 * Moves the photon one animation frame
 *
 * This method also advances the age of the photon. This method does
 * not bounce off walls. We moved all collisions to the collision
 * controller where they belong.
 */
void Photon::update() {
    pos += vel;
    age++;
    scale =  1.25f - age * 0.5f / MAX_AGE;
}

/**
 * Flags the photon for deletion.
 *
 * This just sets the age of the photon to be the maximum age.
 * That way it is removed soon after during the collection phase.
 */
void Photon::destroy() {
    age = MAX_AGE+1;
}
