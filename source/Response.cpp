//
//  Response.cpp
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include "Response.h"

using namespace cugl;

Response::Response() {
    _text = "";
    _outcome = "";
    _resources = {0,0,0,0};
    _cards = {};
    _win = false;
    _lose = false;
}

void Response::allocate(const string text, const string outcome, const std::vector<int> resources, const std::vector<int> cards, const bool win, const bool lose){
    _text = text;
    _outcome = outcome;
    _resources = resources;
    _cards = cards;
    _win = win;
    _lose = lose;
}
