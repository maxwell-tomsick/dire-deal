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
    trans.scale(0.41f);
    int width = _dimen.width;
    int height = _dimen.height;
    trans.translate(width * 0.517f, height * 0.45f, 0);
    for (int i = 0; i < _size; i++){
        batch->draw(_backTexture,tint,origin,trans * transform);
        trans.translate(0,height * 0.01f,0);
    }
    Mat4 transf;
    transf.scale(0.135f + _scaler);
    transf.translate(width * 0.45f+ _offset2.x, height * 0.875f+ _offset2.y, 0);
    for (int i = 0; i < _nextSize; i++){
        batch->draw(_backTexture,tint,origin,transf * transform);
        transf.translate(width * _hoffset,height * _voffset ,0);
    }
    if (_drag){
        Mat4 tr;
        tr.scale(0.41f);
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
