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
    int _guaranteed;
    std::vector<int> _responses; // responses;

public:
    Card();
    
    void allocate(const string text, const int id, const string texture, const std::vector<int> responses, const int guaranteed);
    
    string getText(){
        return _text;
    }
    
    int getId(){
        return _id;
    }
    
    int getResponse(int i){
        return _responses[i];
    }
    
    std::vector<int> getTwoRandomResponses();
    
    int getGuaranteed(){
        return _guaranteed;
    }
    

};
#endif
