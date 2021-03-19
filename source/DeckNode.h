//
//  DeckNode.h
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef DeckNode_h
#define DeckNode_h
#include <cugl/cugl.h>

class DeckNode : public cugl::scene2::SceneNode {
private:
    int _size;
    int _nextSize;
    std::shared_ptr<cugl::Texture> _frontTexture;
    std::shared_ptr<cugl::Texture> _backTexture;
    std::shared_ptr<cugl::Texture> _backTexture2;
    bool _drawFront;
    cugl::Size _dimen;
    
public:
    DeckNode() : SceneNode() {}

    ~DeckNode() { dispose(); }
    
    static std::shared_ptr<DeckNode> alloc() {
        std::shared_ptr<DeckNode> result = std::make_shared<DeckNode>();
        return (result->init() ? result : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    void setBackTexture(std::shared_ptr<cugl::Texture> texture){
        _backTexture = texture;
    }
    void setNextBackTexture(std::shared_ptr<cugl::Texture> texture){
        _backTexture2 = texture;
    }
    
    void swapTextures(){
        std::shared_ptr<cugl::Texture> temp = _backTexture;
        _backTexture = _backTexture2;
        _backTexture2 = temp;
    }
    
    void setFrontTexture(std::shared_ptr<cugl::Texture> texture){
        _frontTexture = texture;
    }
    void setDimen(cugl::Size dimen){
        _dimen = dimen;
    }
    
    void setSize(int size){
        _size = size;
    }
    
    void setNextSize(int size){
        _nextSize = size;
    }
    
    void setDrawFront(bool drawFront){
        _drawFront = drawFront;
    }
};

#endif /* DeckNode_h */
