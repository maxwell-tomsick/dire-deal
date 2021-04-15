#ifndef __ENEMYFIGHT_H__
#define __ENEMYFIGHT_H__

#include <cugl/cugl.h>

/** 
 * Model class representing a level/fight
 */
class EnemyFight {
private:
	string _enemyName; 
	std::vector<int> _deck;
    string _enemyTexture;
    // std::shared_ptr<cugl::scene2::SceneNode> background;
    int _rows;
    int _cols;
    int _frames;
    float _wscale;
    float _hscale;
    int _id;

public:
    EnemyFight();
    
    void allocate(
        string enemyName,
        std::vector<int> deck, 
        string enemyTexture, 
        int rows, 
        int cols, 
        int frames,
        float wscale,
        float hscale,
        int id);

    string getEnemyName(){
        return _enemyName;
    }
    std::vector<int> getDeck() {
        return _deck;
    }
    string getEnemyTexture(){
        return _enemyTexture;
    }
    int getRows(){
        return _rows;
    }
    int getCols(){
        return _cols;
    }
    int getFrames() {
        return _frames;
    }
    float getWscale() {
        return _wscale;
    }
    float getHscale() {
        return _hscale;
    }
    int getId() {
        return _id;
    }
};
#endif /* EnemyFight_h */
