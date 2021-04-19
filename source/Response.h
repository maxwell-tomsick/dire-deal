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
    string _outcome;
    std::vector<int> _resources;
    std::vector<int> _cards;
    bool _win;
    bool _lose;
    float _fontSize;
    int _animation;
    
public:
    Response();
    
    void allocate(
        const string text, 
        const string outcome, 
        const std::vector<int> resources, 
        const std::vector<int> cards, 
        const bool win, 
        const bool lose, 
        const float fontSize, 
        const int animation);
    
    string getText(){
        return _text;
    }

    string getOutcome() {
        return _outcome;
    }

    std::vector<int> getResources() {
        return _resources;
    }
    
    std::vector<int> getCards(){
        return _cards;
    }
    
    bool getWin(){
        return _win;
    }
    
    bool getLose(){
        return _lose;
    }
    
    void setFree(){
        _resources = {0,0,0,0};
    }
    
    float getFontSize(){
        return _fontSize;
    }

    int getAnimation(){
        return _animation;
    }
};

#endif /* Response_h */
