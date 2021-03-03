//
//  GLShadowNode.h
//  GameLab (Mac)
//
//  Created by Maxwell Tomsick on 2/26/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef GLShadowNode_h
#define GLShadowNode_h
#include <cugl/cugl.h>

class ShadowNode : public cugl::scene2::AnimationNode {
public:
    ShadowNode() : AnimationNode() {}

    ~ShadowNode() { dispose(); }
    
    static std::shared_ptr<ShadowNode> alloc(const std::shared_ptr<cugl::Texture>& texture,
                                                int rows, int cols, int size) {
        std::shared_ptr<ShadowNode> node = std::make_shared<ShadowNode>();
        return (node->initWithFilmstrip(texture,rows,cols,size) ? node : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
};

#endif /* GLShadowNode_h */
