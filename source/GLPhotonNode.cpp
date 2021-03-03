//
//  GLPhotonNode.cpp
//  GameLab (Mac)
//
//  Created by Maxwell Tomsick on 2/28/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "GLPhotonNode.h"
/** Number of animation frames a photon lives before deleted */
#define MAX_AGE         100

using namespace cugl;

void PhotonNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
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
            Mat4 trans;
            trans.scale(_queue[idx].scale);
            trans.translate(_queue[idx].pos.x, _queue[idx].pos.y, 0);

            // Use this information to draw.
            batch->setBlendFunc(GL_ONE, GL_ONE);
            batch->draw(_texture,tint,origin,trans * transform);
        }
    }
}
