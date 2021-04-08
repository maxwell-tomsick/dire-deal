//
//  PhotonQueue.cpp
//
//  This class implements a "particle system" that manages the photons fired
//  by either ship in the game.  When a ship fires a photon, it adds it to this
//  particle system.  The particle system is responsible for moving (and drawing)
//  the photon particle.  It also keeps track of the age of the photon.  Photons
//  that are too old are deleted, so that they are not bouncing about the game
//  forever.
//
//  The PhotonQueue is exactly what it sounds like: a queue. In this implementation
//  we use the circular array implementation of a queue (which you may have learned
//  in CS 2110). If you notice, all the Photon objects are declared and initialized
//  in the constructor; we just reassign the fields
//
//  Author: Walker M. White
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
//*** Saving this bc it looks useful
#include "GLPhotonQueue.h"

using namespace cugl;
/** Fixed velocity for a photon */
#define PHOTON_SPEED    5.0f
/** Number of animation frames a photon lives before deleted */
#define MAX_AGE         48

#pragma mark Photon Queue
/**
 * Creates a photon queue with the default values.
 *
 * To properly initialize the queue, you should call the init
 * method.
 */
PhotonQueue::PhotonQueue() :
_qhead(0),
_qtail(-1),
_qsize(0) {
}
    
/**
 * Disposes the photon queue, releasing all resources.
 */
void PhotonQueue::dispose() {
    _queue.clear();
    _photonNode = nullptr;
    _qhead = 0;
    _qtail = -1;
    _qsize = 0;
}
    
/**
 *  Initialies a new (empty) PhotonQueue
 *
 *  @param max  The maximum number of photons to support
 *
 *  @return true if initialization is successful
 */
bool PhotonQueue::init(size_t max) {
    _photonNode = PhotonNode::alloc();
    _photonNode->setQueue(_queue);
    _photonNode->setQSize(_qsize);
    _photonNode->setQHead(_qhead);
    _queue.resize(max);
    return true;
}

/**
 * Adds a photon to the active queue.
 *
 * When adding a photon, we assume that it is fired from the given ship.  We
 * factor in the position, velocity and angle of the ship.
 *
 * As all Photons are predeclared, this involves moving the head and the tail,
 * and reseting the values of the object in place.  This is a simple implementation
 * of a memory pool. It works because we delete objects in the same order that
 * we allocate them.
 *
 * @param ship  The ship that fired.
 */
void PhotonQueue::addPhoton(const std::shared_ptr<Ship>& ship) {
    // Determine direction and velocity of the photon.
    float rads = M_PI*ship->getAngle()/180.0f+M_PI_2;
    Vec2 dir(cosf(rads),sinf(rads));
    
    Vec2 fire = ship->getVelocity()+dir*PHOTON_SPEED;
    
    // Check if any room in queue.
    // If maximum is reached, remove the oldest photon.
    if (_qsize == _queue.size()) {
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Add a new photon at the end.
    // Already declared, so just initialize.
    _qtail = ((_qtail + 1) % _queue.size());
    _queue[_qtail].allocate(ship->getSID(),ship->getPosition(),fire);
    _qsize++;
}
    
/**
 * Moves all the photons in the active queue.
 *
 * Each photon is advanced according to its velocity. Photons which are too old
 * are deleted.  This method does not bounce off walls.  We moved all collisions
 * to the collision controller where they belong.
 */
void PhotonQueue::update() {
    // First, delete all old photons.
    // INVARIANT: Photons are in queue in decending age order.
    // That means we just remove the head until the photons are young enough.
    while (_qsize > 0 && _queue[_qhead].age > MAX_AGE) {
        // As photons are predeclared, all we have to do is move head forward.
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Now, step through each active photon in the queue.
    for (size_t ii = 0; ii < _qsize; ii++) {
        // Find the position of this photon.
        size_t idx = ((_qhead+ii) % _queue.size());

        // Move the photon according to velocity.
        _queue[idx].update();
    }
    _photonNode->setQueue(_queue);
    _photonNode->setQSize(_qsize);
    _photonNode->setQHead(_qhead);
}

/**
 * Returns the (reference to the) photon at the given position.
 *
 * If the position is not a valid photon, then the result is null.
 *
 * @param pos   The photon position in the queue
 *
 * @return the (reference to the) photon at the given position.
 */
Photon* PhotonQueue::get(size_t pos) {
    size_t idx = ((_qhead+pos) % _queue.size());
    if (_queue[idx].age < MAX_AGE) {
        return &_queue[idx];
    }
    return nullptr;
}



/**
 * Draws the photons to the drawing canvas.
 *
 * This method allows you to do old-style 3152 drawing instead.
 * This is the easiest way to set the blend mode.  Otherwise, if
 * you want to vary blend modes in a scene graph, you have to
 * define your own nodes.
 *
 * @param bath  The sprite batch
 */
void PhotonQueue::draw(const std::shared_ptr<SpriteBatch>& batch) {
    
    if (_texture == nullptr) {
        return;
    }
    
    // Get photon texture origin
    Vec2 origin = _texture->getSize()/2;
    
    // Step through each active photon in the queue.
    for (size_t ii = 0; ii < _qsize; ii++) {
        // Find the position of this photon.
        size_t idx = ((_qhead + ii) % _queue.size());

        // How big to make the photon.  Decreases with age.
        if (_queue[idx].age <= MAX_AGE) {
            Mat4 transform;
            transform.scale(_queue[idx].scale);
            transform.translate(_queue[idx].pos.x, _queue[idx].pos.y, 0);

            // Use this information to draw.
            batch->setTexture(_texture);
            batch->draw(_texture,origin,transform);
        }
    }
}


