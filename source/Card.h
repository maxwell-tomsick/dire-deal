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
    int _level;
    std::vector<int> _responses; // responses;
    std::vector<int> _resources;
    std::shared_ptr<cugl::Texture> _texture;

public:
    Card();
    
    void allocate(const string text, const int id, const std::shared_ptr<cugl::Texture> texture, const std::vector<int> responses, const std::vector<int> resources, const int level);
    
    string getText(){
        return _text;
    }
    
    int getId(){
        return _id;
    }
    
    int getLevel(){
        return _level;
    }
    
    int getResponse(int i){
        return _responses[i];
    }
    
    int getResponses(){
        return _responses.size();
    }
    
    int getResource(int i){
        return _resources[i];
    }
    
    std::shared_ptr<cugl::Texture> getTexture(){
        return _texture;
    }
    
    std::vector<int> getRandomResponses();
    
    void setResources(int i, int r){
        _resources[i] = r;
    }

};
#endif
