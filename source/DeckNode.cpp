//
//  DeckNode.cpp
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include "DeckNode.h"

using namespace cugl;

void DeckNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    Vec2 origin = _frontTexture->getSize()/2;
    Mat4 trans;
    trans.scale(0.4f);
    trans.translate(180, 450, 0);
    for (int i = 0; i < _size; i++){
        batch->draw(_backTexture,tint,origin,trans * transform);
        trans.translate(10,10,0);
    }
    if (_drawFront){
        Mat4 transf;
        transf.scale(0.5f);
        transf.translate(585, 350, 0);
        batch->draw(_frontTexture,tint,origin,transf * transform);
    }
} 
