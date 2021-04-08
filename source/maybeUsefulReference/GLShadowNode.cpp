//
//  GLShadowNode.cpp
//  GameLab (Mac)
//
//  Created by Maxwell Tomsick on 2/26/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "GLShadowNode.h"

/** The amount to offset the shadow image by */
#define SHADOW_OFFSET   10.0f

using namespace cugl;

void ShadowNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    AnimationNode::draw(batch,transform * Mat4::createTranslation(SHADOW_OFFSET,SHADOW_OFFSET,0), Color4f(0,0,0,0.5));
    AnimationNode::draw(batch,transform,tint);
}
