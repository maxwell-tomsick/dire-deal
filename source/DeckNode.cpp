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
    trans.scale(0.43f);
    int width = _dimen.width;
    int height = _dimen.height;
    trans.translate(width * 0.52f, height * 0.33f, 0);
    for (int i = 0; i < _size; i++){
        batch->draw(_backTexture,tint,origin,trans * transform);
        trans.translate(0,height * 0.0125f,0);
    }
    Mat4 transf;
    transf.scale(0.25f);
    transf.translate(width * 0.48f, height * 0.775f, 0);
    for (int i = 0; i < _nextSize; i++){
        batch->draw(_backTexture,tint,origin,transf * transform);
        transf.translate(width * 0.0125f,0,0);
    }
    if (_drag){
        Mat4 tr;
        tr.scale(0.43f);
        Vec2 pos = Vec2(_currCardPos.x, _currCardPos.y) + _offset;
        tr.translate(pos.x,pos.y,0);
        batch->draw(_frontTexture,tint,origin,tr * transform);
    } else {
        if (_drawFront == 0){
            batch->draw(_frontTexture,tint,origin,trans * transform);
        } else if (_drawFront == 1) {
            batch->draw(_backTexture,tint,origin,trans * transform);
        }
    }
} 
