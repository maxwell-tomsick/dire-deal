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
    _level = 1;
}

void Card::allocate(const string text, const int id, const std::shared_ptr<cugl::Texture> texture, const std::vector<int> responses, const std::vector<int> resources, const int level){
    _text = text;
    _id = id;
    _responses = responses;
    _resources = resources;
    _texture = texture;
    _level = level;
}

std::vector<int> Card::getRandomResponses(){
    if (_responses.size() >= 3) {
        std::vector<unsigned int> indices(_responses.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);
        return {_responses[indices[0]],_responses[indices[1]],_responses[indices[2]]};
    } else {
        std::vector<unsigned int> indices(_responses.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);
        std::vector<int> returnedResponses;
        for (int i = 0; i < _responses.size(); i++){
            returnedResponses.push_back(_responses[indices[i]]);
        }

        return returnedResponses;
    }
}
