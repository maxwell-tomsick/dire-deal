//
//  GLGameScene.h
//  Programming Lab
//
//  This is the primary class file for running the game.  You should study this file for
//  ideas on how to structure your own root class. This class is a reimagining of the
//  first game lab from 3152 in CUGL.
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#ifndef __RESOURCE_CONTROLLER_H__
#define __RESOURCE_CONTROLLER_H__
#include <cugl/cugl.h>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "Card.h"
#include "Response.h"
using namespace cugl;

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class ResourceController {
    public:
    #pragma mark -
    #pragma mark Constructors
        /**
        * Creates a new game mode with the default values.
        *
        * This constructor does not allocate any objects or start the game.
        * This allows us to use the object without a heap pointer.
        */
        ResourceController() {}
        

        
    #pragma mark -
    #pragma mark Gameplay Handling
    void setResources(
        std::shared_ptr<cugl::scene2::Label> &bladeText, 
        std::shared_ptr<cugl::scene2::Label> &flourishText, 
        std::shared_ptr<cugl::scene2::Label> &lungeText,
        std::shared_ptr<cugl::scene2::Label> &brawnText,
        std::vector<int> &resources
        );  
        
    void setResponseResources(
        std::map<int, Response> &responses, 
        int responseId, 
        int response, 
        std::shared_ptr<cugl::AssetManager> &assets);

    void setBurnText(
        Card &currentCard, 
        std::shared_ptr<cugl::scene2::Label> &burnText, 
        std::shared_ptr<cugl::AssetManager> &assets,
        std::shared_ptr<cugl::scene2::NinePatch> &burnTexture);

    void setDisplayCardBurnText(
        Card &displayCard, 
        std::shared_ptr<cugl::scene2::Label> &displayCardBurnText, 
        std::shared_ptr<cugl::AssetManager> &assets,
        std::shared_ptr<cugl::scene2::NinePatch> &displayCardBurnTexture);
};

#endif /* __GAME_SCENE_H__ */
