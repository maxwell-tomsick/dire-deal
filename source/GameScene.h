//
//  GLGameScene.h
//  Programming Lab
//
//  This is the primary class file for running the game.  You should study this file for
//  ideas on how to structure your own root class. This class is a reimagining of the
//  first game lab from 3152 in CUGL.
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <cugl/audio/CUAudioEngine.h>
#include "InputController.h"
#include "ResourceController.h"
#include "JsonLoader.h"
#include "EnemyFight.h"
#include "Deck.h"
#include "Card.h"
#include "Response.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /**
     * Activates the UI elements to make them interactive
     *
     * The elements do not actually do anything.  They just visually respond
     * to input.
     */
    void activateUI(const std::shared_ptr<cugl::scene2::SceneNode>& scene);
    
    // Attach input controllers directly to the scene (no pointers)
    /** Controller for the blue player */
    // InputController _inputController;

    ResourceController _resourceController;
    //saved this line from lab in case of reference to add sound
    //std::shared_ptr<cugl::Sound> _redSound;
    std::shared_ptr<cugl::scene2::TextField> _field;
    std::shared_ptr<cugl::scene2::Label> _resourceCount;
    std::shared_ptr<cugl::scene2::Label> _currEvent;
    std::shared_ptr<cugl::scene2::Button> _response1;
    std::shared_ptr<cugl::scene2::NinePatch> _responseTexture1;
    std::shared_ptr<cugl::scene2::NinePatch> _responseCard1;
    std::shared_ptr<cugl::scene2::NinePatch> _responseGlow1;
    std::shared_ptr<cugl::scene2::Label> _responseText1;
    std::shared_ptr<cugl::scene2::Label> _responseCost1;
    std::shared_ptr<cugl::scene2::Label> _responseOutcome1;
    std::shared_ptr<cugl::scene2::Button> _response2;
    std::shared_ptr<cugl::scene2::Label> _responseText2;
    std::shared_ptr<cugl::scene2::Label> _responseCost2;
    std::shared_ptr<cugl::scene2::Label> _responseOutcome2;
    std::shared_ptr<cugl::scene2::NinePatch> _responseTexture2;
    std::shared_ptr<cugl::scene2::NinePatch> _responseCard2;
    std::shared_ptr<cugl::scene2::NinePatch> _responseGlow2;
    std::shared_ptr<cugl::scene2::Button> _response3;
    std::shared_ptr<cugl::scene2::Label> _responseText3;
    std::shared_ptr<cugl::scene2::Label> _responseCost3;
    std::shared_ptr<cugl::scene2::Label> _responseOutcome3;
    std::shared_ptr<cugl::scene2::NinePatch> _responseTexture3;
    std::shared_ptr<cugl::scene2::NinePatch> _responseCard3;
    std::shared_ptr<cugl::scene2::NinePatch> _responseGlow3;
    
    std::shared_ptr<cugl::scene2::Label> _shuffle1;
    std::shared_ptr<cugl::scene2::Label> _shuffle2;
    std::shared_ptr<cugl::scene2::Label> _shuffle3;
    
    std::shared_ptr<cugl::scene2::Label> _bladeText;
    std::shared_ptr<cugl::scene2::Label> _brawnText;
    std::shared_ptr<cugl::scene2::Label> _flourishText;
    std::shared_ptr<cugl::scene2::Label> _lungeText;
    std::shared_ptr<cugl::scene2::NinePatch> _sword;
    std::shared_ptr<cugl::scene2::Label> _goon;
    std::shared_ptr<cugl::scene2::NinePatch> _goonLabel;
    std::shared_ptr<cugl::scene2::Label> _goonName;
    std::shared_ptr<cugl::scene2::Label> _goonNumber;
    std::shared_ptr<cugl::scene2::NinePatch> _underline;
    std::shared_ptr<cugl::scene2::Label> _burnLabel;

    std::shared_ptr<cugl::scene2::NinePatch> _tutorialBox;
    std::shared_ptr<cugl::scene2::Label> _tutorialText[16];
    std::shared_ptr<cugl::scene2::Button> _tutorialButton;
    
    std::shared_ptr<cugl::scene2::NinePatch> _black;
    std::shared_ptr<cugl::scene2::Button> _pauseButton;
    std::shared_ptr<cugl::scene2::SceneNode> _pause;
    std::shared_ptr<cugl::scene2::SceneNode> _nextFight;
    std::shared_ptr<cugl::scene2::SceneNode> _nextFightPoison;
    std::shared_ptr<cugl::scene2::SceneNode> _nextFightBrawler;
    std::shared_ptr<cugl::scene2::SceneNode> _nextFightWorm;
    std::shared_ptr<cugl::scene2::SceneNode> _nextFightCultist;
    std::shared_ptr<cugl::scene2::Label> _nextFightText;
    std::shared_ptr<cugl::scene2::SceneNode> _musicSliderNode;
    std::shared_ptr<cugl::scene2::SceneNode> _soundSliderNode;
    std::shared_ptr<cugl::scene2::Label> _paused;
    std::shared_ptr<cugl::scene2::Slider> _musicSlider;
    float _musicVolume;
    std::shared_ptr<cugl::scene2::Slider> _soundSlider;
    float _soundVolume;
    
    std::shared_ptr<cugl::scene2::Button> _burn;
    std::shared_ptr<cugl::scene2::Label> _burnText;
    std::shared_ptr<cugl::scene2::NinePatch> _burnTexture;
    std::shared_ptr<cugl::scene2::NinePatch> _displayCard;
    std::shared_ptr<cugl::scene2::NinePatch> _removeCard1;
    std::shared_ptr<cugl::scene2::NinePatch> _removeCard2;
    std::shared_ptr<cugl::scene2::NinePatch> _removePreviewBurnTexture;
    std::shared_ptr<cugl::scene2::NinePatch> _removePreviewResponseType;
    std::shared_ptr<cugl::scene2::Label> _removePreviewBurnText;

    std::shared_ptr<cugl::scene2::NinePatch> _displayCardBurnTexture;
    std::shared_ptr<cugl::scene2::NinePatch> _displayCardResponseType;
    std::shared_ptr<cugl::scene2::Label> _displayCardBurnText;
    std::shared_ptr<cugl::scene2::NinePatch> _displayCardResponseBurn;
    std::shared_ptr<cugl::scene2::Button> _currCardButton;
    std::shared_ptr<cugl::scene2::PolygonNode> _cardFront;
    std::shared_ptr<DeckNode> _deckNode;
    std::shared_ptr<cugl::scene2::AnimationNode> _shuffleFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _shuffleBackFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _currentBackFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _currentFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _prevFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _prevBackFlip;
    std::shared_ptr<cugl::scene2::AnimationNode> _cardCut;
    std::shared_ptr<cugl::scene2::AnimationNode> _enemyIdle;
    std::shared_ptr<cugl::scene2::AnimationNode> _nextEnemy;
    std::shared_ptr<cugl::scene2::AnimationNode> _currentBurn;
    std::shared_ptr<cugl::scene2::NinePatch> _cardHolder;
    std::shared_ptr<cugl::scene2::NinePatch> _middleColumn;
    std::shared_ptr<cugl::scene2::NinePatch> _rightBox;
    std::shared_ptr<cugl::scene2::NinePatch> _topBar;
    std::shared_ptr<cugl::scene2::NinePatch> _bottomBar;
    std::shared_ptr<cugl::scene2::Button> _mainMenu;
    std::shared_ptr<cugl::scene2::Button> _autoFlipButton;
    std::shared_ptr<cugl::scene2::Label> _autoFlipLabel;
    std::shared_ptr<cugl::scene2::Label> _mainMenuLabel;
    std::shared_ptr<cugl::scene2::Label> _saving;
    int _cardBack;
    
    bool _allRunes;
    bool _autoFlip;
    int _responseId1;
    int _responseId2;
    int _responseId3;
    std::map<int, Card> _cards;
    std::map<int, Response> _responses;
    std::map<int, EnemyFight> _enemyFights;
    std::vector<int> _resources;
    std::vector<int> _currentDeck;
    std::vector<int> _nextDeck;
    std::vector<int> _removeOptions;
    cugl::Size _dimen;
    cugl::Vec2 _vel;
    cugl::Vec2 _vel2;
    cugl::Vec2 _prev;
    float _scl;
    int _movement;
    bool _display1;
    bool _display2;
    bool _display3;
    Card _currentCard;
    bool _flipSwitch;
    int _act = 0;
    cugl::Mouse* _mouse;
    bool _keepCards; // true when trying to select response with insufficient resources
    bool _win;
    bool _doBurn;
    int _fight;
    float _idleBuffer;
    float _enemyIdleBuffer;
    int _enemyIdleSheet;
    int _enemyIdleSheetNum;
    double _ratio;
    bool _usedSecondWind;
    bool _tutorial;
    int _item;
    int _goonInt;
    int _burnInt;
    int _highestLevel;
    int _mod;
    //std::shared_ptr<cugl::AudioEngine> _audioEngine;
    std::shared_ptr<cugl::AudioQueue> _audioQueue;
    const float WIDTH_SCALE = 0.517f;
    const float DECK_SCALE = 0.01f;
    const float HEIGHT_SCALE = 0.45f;
    const float GOON_HEIGHT_SCALE = 0.72f;
    const float SHUFFLE_WIDTH_SCALE = 0.9085f;
    const float R1_HEIGHT_SCALE = 0.6245f;
    const float R2_HEIGHT_SCALE = 0.4f;
    const float R3_HEIGHT_SCALE = 0.175f;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, int item, double ratio, bool tutorial, bool savedGame);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

    /**
     * Draws all this scene to the given SpriteBatch.
     *
     * The default implementation of this method simply draws the scene graph
     * to the sprite batch.  By overriding it, you can do custom drawing
     * in its place.
     *
     * @param batch     The SpriteBatch to draw with.
     */
    void render(const std::shared_ptr<cugl::SpriteBatch>& batch) override;

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset() override;
    void deckLoad(std::vector<int> deck);
    
    void buttonPress(const int r);
    
    Card getCard(const int id);


    /**
     * Converts a resource vector into a string representation for displaying.
     * 
     * @param resources     int vector of length 4
     */
    string resourceString(std::vector<int> resources);
    
    void responseUpdate(const int responseId, const int response);
    
    void setResources();
    
    void setResponseResources(const int response);
    
    void setBurnText();

    void setDisplayCardBurnText(std::shared_ptr<cugl::scene2::NinePatch> nodeTexture, 
                                std::shared_ptr<cugl::scene2::Label> nodeText, 
                                Card displayCard);
    
    void setDisplayCardResponseType(std::shared_ptr<cugl::scene2::NinePatch> nodeResponseType, 
                                        std::shared_ptr<cugl::scene2::NinePatch> nodeBrawler, 
                                        Card displayCard, 
                                        bool brawn);
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for a mouse release event.
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
    
    /**
     * Handles touchBegan and mousePress events using shared logic.
     *
     * Depending on the platform, the appropriate callback (i.e. touch or mouse)
     * will call into this method to handle the Event.
     *
     * @param pos         the position of the touch
     */
    void touchBegan(const cugl::Vec2& pos);


    /**
     * Handles touchEnded and mouseReleased events using shared logic.
     *
     * Depending on the platform, the appropriate callback (i.e. touch or mouse)
     * will call into this method to handle the Event.
     *
     * @param pos         the position of the touch
     */
    void touchEnded(const cugl::Vec2& pos);
    
    /**
     * Handles touchMoved and mouseDragged events using shared logic.
     *
     * Depending on the platform, the appropriate callback (i.e. touch or mouse)
     * will call into this method to handle the Event.
     *
     * @param timestamp     the timestamp of the event
     * @param pos         the position of the touch
     */
    void touchMoved(const cugl::Vec2& pos);
    
    Card getItem(const int i);
    
    void removeCard(const int id);
    
    void gameOver();
    
    void setProgressJson();
    
    void setGameJson(bool startingDeck);
    
    void resourceIncrease();
};

#endif /* __GAME_SCENE_H__ */
