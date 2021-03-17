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
    std::vector<int> _responses; // responses;
    std::vector<int> _resources;

public:
    Card();
    
    void allocate(const string text, const int id, const string texture, const std::vector<int> responses, const std::vector<int> resources);
    
    string getText(){
        return _text;
    }
    
    int getId(){
        return _id;
    }
    
    int getResponse(int i){
        return _responses[i];
    }
    
    int getResource(int i){
        return _resources[i];
    }
    
    std::vector<int> getThreeRandomResponses();
    

};
#endif
