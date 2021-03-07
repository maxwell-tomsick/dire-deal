//
//  GLPhotonNode.h
//  GameLab
//
//  Created by Maxwell Tomsick on 2/28/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef GLPhotonNode_h
#define GLPhotonNode_h
#include <cugl/cugl.h>
#include "GLPhoton.h"

class PhotonNode : public cugl::scene2::SceneNode {
private:
    /** Graphic asset representing a single photon. */
    std::shared_ptr<cugl::Texture> _texture;
    // QUEUE DATA STRUCTURES
    /** Vector implementation of a circular queue. */
    std::vector<Photon> _queue;
    /** Index of head element in the queue */
    int _qhead;
    /** Number of elements currently in the queue */
    int _qsize;
    
public:
    PhotonNode() : SceneNode() {}

    ~PhotonNode() { dispose(); }
    
    static std::shared_ptr<PhotonNode> alloc() {
        std::shared_ptr<PhotonNode> result = std::make_shared<PhotonNode>();
        return (result->init() ? result : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    void setTexture(std::shared_ptr<cugl::Texture> texture){
        _texture = texture;
    }
    void setQueue(std::vector<Photon> queue){
        _queue = queue;
    }
    void setQHead(int qhead){
        _qhead = qhead;
    }
    void setQSize(int qsize){
        _qsize = qsize;
    }
};


#endif /* GLPhotonNode_h */
