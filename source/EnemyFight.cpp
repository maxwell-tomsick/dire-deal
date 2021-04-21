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
    _nextDeck ={};
    _enemyTexture = "";
    // std::shared_ptr<cugl::scene2::SceneNode> background;
    _rows = 3;
    _cols = 4;
    _frames = 12;
    _wscale = 1.0f;
    _hscale = 1.0f;
    _id = 0;
}

void EnemyFight::allocate(
    string enemyName,
    std::vector<int> deck, 
    std::vector<int> nextDeck,
    string enemyTexture, 
    int rows, 
    int cols, 
    int frames,
    float wscale,
    float hscale,
    int id,
    float scale){
        
    _enemyName = enemyName;
	_deck = deck;
    _nextDeck = nextDeck;
    _enemyTexture = enemyTexture;
    _rows = rows;
    _cols = cols;
    _frames = frames;
    _wscale = wscale;
    _hscale = hscale;
    _scale = scale;
    _id = id;
}
