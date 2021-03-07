//
//  Response.h
//  GameLab
//
//  Created by Maxwell Tomsick on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef Response_h
#define Response_h

#include <vector>

class Response {
private:
    string _text;
    std::vector<int> _resources;
    std::vector<int> _cards;
    bool _win;
    bool _lose;
    
public:
    Response();
    
    void allocate(const string text, const std::vector<int> resources, const std::vector<int> cards, const bool win, const bool lose);
    
    string getText(){
        return _text;
    }
};

#endif /* Response_h */
