#ifndef __CARD_H__
#define __CARD_H__

#include <cugl/cugl.h>
#include "Response.h"

/** 
 * Model class representing an individual card.
 */
class Card {
private:
	string _text; // do we want this to be a string?
	int _id;
    std::vector<Response> _responses; // responses;

public:
    Card();
    
    void allocate(const string text, const int id, const std::vector<Response> responses);
    
    string getText(){
        return _text;
    }
    
    int getId(){
        return _id;
    }
    
    Response getResponse(int i){
        return _responses[i];
    }

};
#endif
