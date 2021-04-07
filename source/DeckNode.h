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
    int _drawFront;
    cugl::Size _dimen;
    bool _drag;
    cugl::Vec2 _currCardPos;
    cugl::Vec2 _offset;
    float _hoffset;
    float _voffset;
    float _scaler;
    cugl::Vec2 _offset2;
    
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
    
    void setDrawFront(int drawFront){
        _drawFront = drawFront;
    }
    
    void setDrag(bool drag){
        _drag = drag;
    }
    
    bool getDrag(){
        return _drag;
    }
    
    void setCurrCardPos(cugl::Vec2 pos){
        _currCardPos = pos;
    }
    
    void setOffset(cugl::Vec2 pos){
        _offset = pos;
    }
    
    cugl::Vec2 getOffset(){
        return _offset;
    }
    
    void setHOffset(float h){
        _hoffset = h;
    }
    
    float getHOffset(){
        return _hoffset;
    }
    
    void setScaler(float s){
        _scaler = s;
    }
    
    float getScaler(){
        return _scaler;
    }
    
    void setVOffset(float v){
        _voffset = v;
    }
    
    float getVOffset(){
        return _voffset;
    }
    
    void setOffsetVector(cugl::Vec2 v){
        _offset2 = v;
    }
    
    cugl::Vec2 getOffsetVector(){
        return _offset2;
    }
    
    void reset(){
        _hoffset = 0.0125f;
        _voffset = 0;
        _scaler = 0;
        _offset2 = cugl::Vec2(0,0);
    }

};

#endif /* DeckNode_h */
