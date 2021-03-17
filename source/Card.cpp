//
//  Card.cpp
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include <numeric>
#include "Card.h"

using namespace cugl;

Card::Card(){
    _text = "";
    _id = 0;
    _responses = {};
    _resources = {};
}

void Card::allocate(const string text, const int id, const string texture, const std::vector<int> responses, const std::vector<int> resources){
    _text = text;
    _id = id;
    _responses = responses;
    _resources = resources;
}

std::vector<int> Card::getThreeRandomResponses(){
    std::vector<unsigned int> indices(_responses.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    return {_responses[indices[0]],_responses[indices[1]],_responses[indices[2]]};
}
