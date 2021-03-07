//
//  Card.cpp
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include "Card.h"

using namespace cugl;

Card::Card(){
    _text = "";
    _id = 0;
    _responses = {};
}

void Card::allocate(const string text, const int id, const std::vector<Response> responses){
    _text = text;
    _id = id;
    _responses = responses;
}
