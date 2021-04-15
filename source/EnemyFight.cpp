//
//  Card.cpp
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include <numeric>
#include "EnemyFight.h"

using namespace cugl;

EnemyFight::EnemyFight(){
    _enemyName = ""; 
	_deck = {0};
    _enemyTexture = nullptr;
    // std::shared_ptr<cugl::scene2::SceneNode> background;
    _rows = 3;
    _cols = 4;
    _frames = 12;
}

void EnemyFight::allocate(
    string enemyName,
    std::vector<int> deck, 
    string enemyTexture, 
    int rows, 
    int cols, 
    int frames,
    float wscale,
    float hscale){
        
    _enemyName = enemyName;
	_deck = deck;
    _enemyTexture = enemyTexture;
    _rows = rows;
    _cols = cols;
    _frames = frames;
    _wscale = wscale;
    _hscale = hscale;
}
