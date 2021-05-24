//
//  GLGameScene.cpp
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
#include "GameScene.h"

using namespace cugl;
using namespace JsonLoader;
//using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/** The key for the event handlers */
#define LISTENER_KEY        1
#define SWIPE_LENGTH    50

#pragma mark -
#pragma mark Constructors
void GameScene::deckLoad(std::vector<int> deck) {
     for (int i = 0; i < deck.size(); i++){
          _currentDeck.push_back(deck[i]);
     }
}
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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, int equippedItem, double ratio, bool tutorial, bool savedGame) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
     _dimen = dimen;
    // Start up the input handler
    _assets = assets;
    _ratio = ratio;
     
    // Acquire the scene built by the asset loader and resize it the scene
     _background = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("background"));
     _background->setContentSize(dimen);
     _background->doLayout();
     addChild(_background);
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;
     _pause = _assets->get<scene2::SceneNode>("pause");
     _pause->setContentSize(dimen);
     _pause->doLayout();
     
     std::shared_ptr<JsonReader> jsonReaderHighestLevel = JsonReader::alloc(Application::get()->getSaveDirectory() + "settings.json");
     std::shared_ptr<JsonValue> readJson = jsonReaderHighestLevel->readJson();
     std::shared_ptr<JsonValue> progress = readJson->get("Progress");
     std::shared_ptr<JsonValue> volume = readJson->get("Volume");
     jsonReaderHighestLevel->close();
     _highestLevel = progress->get("HighestLevel")->asInt();
     _musicVolume = volume->get("Music")->asFloat();
     _soundVolume = volume->get("Sound")->asFloat();
     _autoFlip = volume->get("Autoflip")->asBool();
     
     bool startingDeck = true;
     if (filetool::file_exists(Application::get()->getSaveDirectory() + "savedGame.json") & savedGame){
          std::shared_ptr<JsonReader> jsonReaderSaveFile = JsonReader::alloc(Application::get()->getSaveDirectory() + "savedGame.json");
          std::shared_ptr<JsonValue> readJ = jsonReaderSaveFile->readJson();
          int fight = readJ->get("Fight")->asInt();
          bool startDeck = readJ->get("StartingDeck")->asBool();
          std::vector<int> currentDeck = readJ->get("CurrentDeck")->asIntArray();
          int item = readJ->get("Item")->asInt();
          bool secondWindUsed = readJ->get("SecondWindUsed")->asBool();
          std::vector<int> resources = readJ->get("Resources")->asIntArray();
          jsonReaderSaveFile->close();
          
          _fight = fight;
          _item = item;
          _usedSecondWind = secondWindUsed;
          _resources = resources;
          startingDeck = startDeck;
          if (!startDeck){
               _currentDeck = currentDeck;
          }
     } else {
          _usedSecondWind = false;
          _fight = 1;
          _item = equippedItem;
          if (!tutorial) {
               _resources = { 20, 20, 20, 20 };
              //_resources = { 40, 40, 40, 40 };
          } else {
              _resources = { 0, 20, 0, 0 };
          }
          //setGameJson(true);
     }
     
     _cards = {};
     _responses = {};
     _enemyFights = {};
     //RESEARCH WHETHER TO DELETE POINTER LATER
     if (!tutorial) {
         string enemyFightsJsonName = "json/enemyFights.json";
         std::shared_ptr<JsonReader> jsonReaderEnemyFights = JsonReader::alloc(enemyFightsJsonName);
         _enemyFights = getJsonEnemyFights(jsonReaderEnemyFights, _enemyFights, ratio);
         jsonReaderEnemyFights->close();
         string cardstring = "json/level" + to_string(_fight) + ".json";
         std::shared_ptr<JsonReader> jsonReaderCardString = JsonReader::alloc(cardstring);
         _cards = getJsonCards(jsonReaderCardString, _cards, _assets);
         jsonReaderCardString->close();
     }
     else {
         string enemyFightsJsonName = "json/tutorialFights.json";
         std::shared_ptr<JsonReader> jsonReaderEnemyFights = JsonReader::alloc(enemyFightsJsonName);
         _enemyFights = getJsonEnemyFights(jsonReaderEnemyFights, _enemyFights, ratio);
         jsonReaderEnemyFights->close();
         std::shared_ptr<JsonReader> jsonReaderTutorial = JsonReader::alloc("json/tutorial1.json");
         _cards = getJsonCards(jsonReaderTutorial, _cards, _assets);
         jsonReaderTutorial->close();
     }
     Card item = getItem(_item);
     _cards[-1] = item;
     if (!tutorial) {
          std::shared_ptr<JsonReader> jsonReaderResponses = JsonReader::alloc("json/responses" + to_string(_fight) + ".json");
         _responses = getJsonResponses(jsonReaderResponses, _responses);
         jsonReaderResponses->close();
     }
     else {
         std::shared_ptr<JsonReader> jsonReaderResponses = JsonReader::alloc("json/responsestutorial1.json");
         _responses = getJsonResponses(jsonReaderResponses, _responses);
         jsonReaderResponses->close();
     }
     //cout << cardsArray->asString();
     _nextDeck = {};
     if (startingDeck){
          _currentDeck = {};
          if ((_item >= 0) & !((_item == 2) & _usedSecondWind)){
               _currentDeck.push_back(-1);
          }
          std::vector<int> currDeck = _enemyFights[_fight].getDeck();
          for (int i = 0; i < currDeck.size(); i++){
               _currentDeck.push_back(currDeck[i]);
          }
          std::vector<int> nextDeck = _enemyFights[_fight].getNextDeck();
          for (int i = 0; i < nextDeck.size(); i++){
               _nextDeck.push_back(nextDeck[i]);
          }
          std::random_device rd;
          std::mt19937 g(rd());
          std::shuffle(_currentDeck.begin(), _currentDeck.end(), g);
     }
     _keepCards = false;
     _win = false;
     _doBurn = false;
     if (_autoFlip){
        _goonInt = 0;
        _burnInt = 0;
        _movement = 6;
        AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
     } else {
        _movement = 5;
     }
     _tutorialBox = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_tutorialBox"));
     if (tutorial) {
         for (int i = 0; i < 16; i++) {
             string assetname = "lab_tutorialBox_tutorial-line-" + to_string(i + 1);
             _tutorialText[i] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>(assetname));
         }
         if (ratio <= 1.5) {
             _tutorialText[0]->setText("Swipe the card, and choose");
             _tutorialText[1]->setText("responses from the right");
             _tutorialText[2]->setText("to shuffle new cards into");
             _tutorialText[3]->setText("the next deck (above). The");
             _tutorialText[4]->setText("old card is removed.");
             _tutorialText[5]->setText("");
             _tutorialText[6]->setText("Above responses are your");
             _tutorialText[7]->setText("resources. Each response");
             _tutorialText[8]->setText("costs a specific resource.");
             _tutorialText[9]->setText("");
             _tutorialText[10]->setText("The gems at the bottom of");
             _tutorialText[11]->setText("a card show its level. Win");
             _tutorialText[12]->setText("by shuffling in a level");
             _tutorialText[13]->setText("five card. Darker responses");
             _tutorialText[14]->setText("are higher leveled.");
             _tutorialText[15]->setText("");
         }
         else {
             _tutorialText[0]->setText("Swipe the card, and choose");
             _tutorialText[1]->setText("responses from the right to");
             _tutorialText[2]->setText("shuffle new cards into the");
             _tutorialText[3]->setText("next deck (above). The old");
             _tutorialText[4]->setText("card is removed from deck.");
             _tutorialText[5]->setText("");
             _tutorialText[6]->setText("Above responses are your");
             _tutorialText[7]->setText("resources. Each response");
             _tutorialText[8]->setText("costs a specific resource.");
             _tutorialText[9]->setText("");
             _tutorialText[10]->setText("The gems at the bottom of");
             _tutorialText[11]->setText("a card show its level. Win");
             _tutorialText[12]->setText("by shuffling in a level");
             _tutorialText[13]->setText("five card. Darker responses");
             _tutorialText[14]->setText("are higher leveled.");
             _tutorialText[15]->setText("");
         }
         _tutorialBox->setVisible(true);
         for (int i = 0; i < 16; i++) {
             _tutorialText[i]->setVisible(true);
         };
     }
     else {
         _tutorialBox->setVisible(false);
     }
     _display1 = true;
     _display2 = true;
     _display3 = true;
     _tutorial = tutorial;
     // _currentDeck = (_enemyFights[1].getDeck());
     // _currentDeck.push_back(0);
     bool itemFound = false;
     int r = 0;
     _mod = 1;
     bool redFound = false;
     bool blueFound = false;
     bool greenFound = false;
     for (int i = 0; i < _currentDeck.size(); i++){
          if (_currentDeck[i] == 16){
               blueFound = true;
          }
          if (_currentDeck[i] == 17){
               greenFound = true;
          }
          if (_currentDeck[i] == 18){
               redFound = true;
          }
          if (_currentDeck[i] == 14){
               _mod += 1;
          }
          if (_currentDeck[i] == -1){
               itemFound = true;
          } else if (_currentDeck[i] != 13){
               if (_cards[_currentDeck[i]].getResponses() >= 3){
                    r += 3;
               } else {
                    r += _cards[_currentDeck[i]].getResponses();
               }
          }
     }
     _allRunes = redFound & blueFound & greenFound;
     _currentCard = _cards[_currentDeck.back()];
     _currentDeck.pop_back();
     if (_currentCard.getId() == -1 & _item == 3){
          int i = rand() % 4;
          int r = (int) _currentDeck.size() + (int)_nextDeck.size() + 1;
          _currentCard.setResources(i, r * 2);
     }
     
     _deckNode = DeckNode::alloc();
     _deckNode->setSize(int(_currentDeck.size()));
     _deckNode->setNextSize(int(_nextDeck.size()));
     auto cardBackTexture1 = _assets->get<Texture>("cardBack1");
     auto cardBackTexture2 = _assets->get<Texture>("cardBack2");
     _deckNode->setBackTexture(cardBackTexture1);
     _deckNode->setNextBackTexture(cardBackTexture2);
     _deckNode->setDimen(_dimen);
     _deckNode->setFrontTexture(_currentCard.getTexture());
     _deckNode->setDrawFront(0);
     _deckNode->setDrag(false);
     _deckNode->reset();
     //_resources = { 99, 99, 99, 99 };
     _goonLabel = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_enemyLabel"));
     if (!tutorial) {
         _goonLabel->setVisible(true);
     }
     else {
         _goonLabel->setVisible(false);
     }
     _goonNumber = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_enemyLabel_number"));
     if (!_tutorial) {
         _goonNumber->setText("Target " + std::to_string(_fight) + ":");
     }
     else {
         _goonNumber->setText("Lesson " + std::to_string(_fight) + ":");
     }
     _goonName = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_enemyLabel_name"));
     _enemyIdle = std::make_shared<scene2::AnimationNode>();
     if (!tutorial) {
         _enemyIdle->setVisible(true);
     }
     else {
         _enemyIdle->setVisible(false);
     }
     _sword = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_topsword"));
     
     /*
     _enemyIdle->initWithFilmstrip(assets->get<Texture>("thugIdle"), 3, 4, 12);
     _enemyIdle->setScale(_enemyFights[_fight].getScale());
     _idleBuffer = 0;
     _enemyIdle->setPosition(dimen.width * 0.2f, dimen.height*0.44f);
      */
     EnemyFight currFight = _enemyFights[_fight];
     _enemyIdleBuffer = currFight.getIdleBuffer();
     _enemyIdleSheet = 0;
     _enemyIdleSheetNum = currFight.getNumSheets();
     _goonName->setText(currFight.getEnemyName());
     //CULog("beforefilmstrip");
     _enemyIdle->initWithFilmstrip(_assets->get<Texture>(
          currFight.getEnemyTexture()),
          currFight.getRows(),
          currFight.getCols(),
          currFight.getFrames());
     //CULog("afterfilmstrip");
     //_enemyIdle->setScale(0.69f);
     _enemyIdle->setFrame(0);
     _enemyIdle->setScale(currFight.getScale());
     _idleBuffer = 0;
     _enemyIdle->setPosition(_dimen.width * currFight.getWscale(), _dimen.height * currFight.getHscale());
     _enemyIdle->setScale(currFight.getScale());
     addChild(_enemyIdle);
     
     addChild(scene);
     addChild(_deckNode);
     _black = std::dynamic_pointer_cast<scene2::TexturedNode>(assets->get<scene2::SceneNode>("darken"));
     _black->setContentSize(dimen);
     _black->doLayout();
     _black->setBlendEquation( GL_FUNC_ADD );
     _black->setVisible(false);
     addChild(_black);
     addChild(_pause);
     _shuffleFlip = std::make_shared<scene2::AnimationNode>();
     _shuffleFlip->initWithFilmstrip(assets->get<Texture>("SlashFlip"), 3, 6, 15);
     _shuffleFlip->setScale(0.36914f);
     _shuffleFlip->setFrame(_shuffleFlip->getSize() - 1);
     _shuffleFlip->setVisible(false);
     _shuffleBackFlip = std::make_shared<scene2::AnimationNode>();
     _shuffleBackFlip->initWithFilmstrip(assets->get<Texture>("BackFlip"), 3, 6, 15);
     _shuffleBackFlip->setScale(0.36914f);
     _shuffleBackFlip->setFrame(_shuffleBackFlip->getSize() - 1);
     _shuffleBackFlip->setVisible(false);
     _currentFlip = std::make_shared<scene2::AnimationNode>();
     _currentFlip->initWithFilmstrip(assets->get<Texture>("SlashFlip"), 3, 6, 15);
     _currentFlip->setScale(1.0283f);
     _currentFlip->setFrame(0);
     _currentFlip->setVisible(false);
     addChild(_currentFlip);
     _currentBackFlip = std::make_shared<scene2::AnimationNode>();
     _currentBackFlip->initWithFilmstrip(assets->get<Texture>("BackFlip"), 3, 6, 15);
     _currentBackFlip->setScale(1.0283f);
     _currentBackFlip->setFrame(0);
     _currentBackFlip->setVisible(false);
     addChild(_currentBackFlip);
     addChild(_shuffleBackFlip);
     addChild(_shuffleFlip);
     _currentBurn = std::make_shared<scene2::AnimationNode>();
     _currentBurn->initWithFilmstrip(assets->get<Texture>("SlashBurn"), 10, 10, 100);
     _currentBurn->setScale(1.752f);
     _currentBurn->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     _currentBurn->setFrame(0);
     _currentBurn->setVisible(false);
     addChild(_currentBurn);
     _prevFlip = std::make_shared<scene2::AnimationNode>();
     _prevFlip->initWithFilmstrip(assets->get<Texture>("SlashFlip"), 3, 6, 15);
     _prevFlip->setPosition(dimen.width * 0.2f, dimen.height * HEIGHT_SCALE);
     _prevFlip->setScale(1.0283f);
     _prevFlip->setFrame(0);
     _prevFlip->setVisible(false);
     addChild(_prevFlip);
     _prevBackFlip = std::make_shared<scene2::AnimationNode>();
     _prevBackFlip->initWithFilmstrip(assets->get<Texture>("BackFlip"), 3, 6, 15);
     _prevBackFlip->setPosition(dimen.width * 0.2f, dimen.height * HEIGHT_SCALE);
     _prevBackFlip->setScale(1.0283f);
     _prevBackFlip->setFrame(0);
     _prevBackFlip->setVisible(false);
     addChild(_prevBackFlip);
     _cardCut = std::make_shared<scene2::AnimationNode>();
     _cardCut->initWithFilmstrip(assets->get<Texture>("cardCut"), 4, 6, 24);
     _cardCut->setPosition(dimen.width * 0.2f, dimen.height * HEIGHT_SCALE);
     _cardCut->setScale(1.02f);
     _cardCut->setFrame(0);
     _cardCut->setVisible(false);
     addChild(_cardCut);
     
     _pauseButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("PauseNode_Pause"));
     std::shared_ptr<scene2::SceneNode> pause = _assets->get<scene2::SceneNode>("PauseNode");
     pause->setContentSize(dimen);
     pause->doLayout();
     addChild(pause);
     
     _nextFightText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("nextFight_text"));
     /*
     _nextEnemy->initWithFilmstrip(assets->get<Texture>("thugIdle"), 3, 4, 12);
     _nextEnemy->setScale(_enemyFights[_fight].getScale());
     _nextEnemy->setPosition(dimen.width * 0.2f, dimen.height*0.44f);
      */
     if (_fight < _enemyFights.size()){
          _nextEnemy =std::make_shared<scene2::AnimationNode>();
          EnemyFight nextFight = _enemyFights[_fight + 1];
          _nextEnemy->initWithFilmstrip(_assets->get<Texture>(nextFight.getEnemyTexture()),
                                                              nextFight.getRows(),
                                                              nextFight.getCols(),
                                                              nextFight.getFrames());
          _nextEnemy->setFrame(0);
          _nextEnemy->setScale(nextFight.getScale());
          _nextEnemy->setPosition(_dimen.width * nextFight.getWscale(), _dimen.height * nextFight.getHscale());
          _nextEnemy->setScale(nextFight.getScale());
          if (!_tutorial) {
              _nextFightText->setText("Next Fight: " + nextFight.getEnemyName());
          }
          else {
              _nextFightText->setText("Next Lesson: " + nextFight.getEnemyName());
          }
          _nextEnemy->setVisible(false);
          addChild(_nextEnemy);
     }
     _nextFight = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("nextFight"));
     _nextFight->setContentSize(dimen);
     _nextFight->doLayout();
     _nextFight->setVisible(false);
     //_nextFight->setVisible(true);
     addChild(_nextFight);
     _nextFightPoison = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("nextFight_poison"));
     _nextFightPoison->setContentSize(dimen);
     _nextFightPoison->doLayout();
     _nextFightBrawler = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("nextFight_brawler"));
     _nextFightBrawler->setContentSize(dimen);
     _nextFightBrawler->doLayout();
     _nextFightWorm = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("nextFight_worm"));
     _nextFightWorm->setContentSize(dimen);
     _nextFightWorm->doLayout();
     _nextFightCultist = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("nextFight_cultist"));
     _nextFightCultist->setContentSize(dimen);
     _nextFightCultist->doLayout();
     
     _audioQueue = AudioEngine::get()->getMusicQueue();
     if (_fight < 3){
          _audioQueue->play(_assets->get<Sound>("introThug"), false,_musicVolume, false);
          _audioQueue->enqueue(_assets->get<Sound>("repeatThug"), true, _musicVolume, false);
     } else if (_fight < 5){
          _audioQueue->play(_assets->get<Sound>("introSlime"), false, _musicVolume, false);
          _audioQueue->enqueue(_assets->get<Sound>("repeatSlime"), true, _musicVolume, false);
     } else if (_fight >= 5){
          _audioQueue->play(_assets->get<Sound>("introMenu"), false,_musicVolume, false);
          _audioQueue->enqueue(_assets->get<Sound>("repeatMenu"), true, _musicVolume, false);
     }
     if (_fight > 4){
          _background->setTexture(_assets->get<Texture>("background2"));
     } else {
          _background->setTexture(_assets->get<Texture>("background"));
     }
     
     _currEvent = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_currEvent"));
     _musicSliderNode = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("pause_musicSlider"));
     _soundSliderNode = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("pause_soundSlider"));
     _paused = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_paused"));
     _musicSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("pause_musicSlider_action"));
     _musicSlider->setValue(_musicVolume);
     _audioQueue->setVolume(_musicVolume);
     _musicSlider->addListener([=](const std::string& name, float value) {
         if (_musicVolume != value & _movement == 14) {
             _musicVolume = value;
              setProgressJson();
         }
     });
     _soundSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("pause_soundSlider_action"));
     _soundSlider->setValue(_soundVolume);
     std::shared_ptr<cugl::scene2::Button> knob = std::dynamic_pointer_cast<scene2::Button>(_soundSlider->getKnob());
     knob->addListener([=](const std::string& name, bool down) {
          if (_movement == 14) {
               if (!down){
                    if (AudioEngine::get()->getState("slashSound") != AudioEngine::State::PLAYING){
                         AudioEngine::get()->play("slashSound", _assets->get<Sound>("slashSound"), false, _soundVolume, false);
                    }
               }
          }
          });
     _soundSlider->addListener([=](const std::string& name, float value) {
         if ( _soundVolume != value & _movement == 14) {
             _soundVolume = value;
              setProgressJson();
         }
     });
     
     _shuffle1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_shuffle"));
     _shuffle2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_shuffle"));
     _shuffle3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_shuffle"));
     _shuffle1->setText("Shuffle:");
     _shuffle2->setText("Shuffle:");
     _shuffle3->setText("Shuffle:");
    _response1 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response1"));
    _responseText1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_label"));
    _responseCost1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response1_up_costs"));
    _response2 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response2"));
    _responseText2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response2_up_label"));
    _response3 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_response3"));
    _responseText3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_response3_up_label"));
     _responseTexture1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up"));
     _responseCard1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up_card"));
     _responseCard2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up_card"));
     _responseCard3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up_card"));
     _responseGlow1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response1_up_glow"));
     _responseTexture2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up"));
     _responseGlow2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response2_up_glow"));
     _responseTexture3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up"));
     _responseGlow3 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_response3_up_glow"));
     _burn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_burn"));
     _burnTexture =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_burnPreview"));
     _burnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_burnPreview_amount"));
     _bladeText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_blade_amount"));
     _brawnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_brawn_amount"));
     _flourishText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_flourish_amount"));
     _lungeText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_lunge_amount"));
     _displayCard =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_displayCard"));
     _removeCard1 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_removePreview1"));
     _removeCard2 =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_removePreview2"));
     _currCardButton =std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lab_currCard"));
     _removePreviewBurnTexture =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_removePreview1_burnAmount"));
     _removePreviewBurnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_removePreview1_burnAmount_amount"));
     _removePreviewResponseType=std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_removePreview1_responseType"));
     _displayCardBurnTexture =std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_displayCard_burnAmount"));
     _displayCardBurnText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_displayCard_burnAmount_amount"));
     _displayCardResponseType=std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_displayCard_responseType"));
     _displayCardResponseBurn=std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_displayCard_responseTypeBrawn"));
     _goon = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lab_goon"));
     _burnLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_cardHolder_amount"));
     _underline = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lab_goon_underline"));
     _cardHolder = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("background_cardHolder"));
     _middleColumn = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("background_middleColumn"));
     _topBar = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("background_topEdgeBar"));
     _bottomBar = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("background_bottomEdgeBar"));
     _rightBox = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("background_middleColumn_rightBox"));
     _mainMenu = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_mainMenu"));
     _mainMenuLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_mainMenu_up_label"));
     _autoFlipButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_autoFlip"));
     _autoFlipLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_autoFlip_up_label"));
     if (_autoFlip) {
          _autoFlipLabel->setText("Autoflip: ON");
     } else {
          _autoFlipLabel->setText("Autoflip: OFF");
     }
     _autoFlipButton->addListener([=](const std::string& name, bool down) {
          if ( (_movement == 14) & down) {
               _autoFlip = !_autoFlip;
               if (_autoFlip) {
                    _autoFlipLabel->setText("Autoflip: ON");
               } else {
                    _autoFlipLabel->setText("Autoflip: OFF");
               }
               setProgressJson();
          }
         });
     _saving = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("background_saving"));
     _saving->setVisible(false);
     _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     _currCardButton->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     // start make these more transparent
     _middleColumn->setColor(Color4(255,255,255,200));
     _rightBox->setColor(Color4(255,255,255,80));
     _topBar->setColor(Color4(255,255,255,80));
     _bottomBar->setColor(Color4(255,255,255,80));
     // // end make these transparent
     _burnLabel->setVisible(false);
     _cardHolder->setVisible(true);
     _burnTexture->setVisible(false);
     _response1->setVisible(false);
     _response2->setVisible(false);
     _response3->setVisible(false);
     _currentBackFlip->setVisible(true);
     _mainMenu->setVisible(false);
     _mainMenu->setToggle(false);
     _goonName->setVisible(true);
     _goonNumber->setVisible(true);
     _currentFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     _currentBackFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     _goon->setVisible(false);
     _deckNode->setDrawFront(2);
     _deckNode->setFrontTexture(_currentCard.getTexture());
     string flipTexture = _currentCard.getText() + "Flip";
     _currentFlip->setTexture(_assets->get<Texture>(flipTexture));
     _goonName->setText(_enemyFights[_fight].getEnemyName());
     _goonNumber->setText("Target " + std::to_string(_fight) + ":");
     bool success = true;
     _pauseButton->addListener([=](const std::string& name, bool down) {
          if ( (_movement == 0) & down) {
               _response1->setVisible(false);
               _response2->setVisible(false);
               _response3->setVisible(false);
               _black->setVisible(true);
               _mainMenuLabel->setText("Main Menu");
               _mainMenu->setVisible(true);
              _pause->setVisible(true);
               _movement = 14;
          } else if ((_movement == 14) & down){
               if (_display1){
                    _response1->setVisible(true);
               }
               if (_display2){
                    _response2->setVisible(true);
               }
               if (_display3){
                    _response3->setVisible(true);
               }
               _pause->setVisible(false);
               _mainMenu->setVisible(false);
               _black->setVisible(false);
               _movement = 0;
          }
          });
#ifndef CU_TOUCH_SCREEN
     success = Input::activate<Keyboard>();
     success = success && Input::activate<Mouse>();
     _mouse = Input::get<Mouse>();
     _mouse->setPointerAwareness(cugl::Mouse::PointerAwareness::ALWAYS);
     _currCardButton->addListener([=](const std::string& name, bool down) {
          if ( (_movement == 0) & down & _act == 60) {
               Vec2 pos = _deckNode->screenToNodeCoords(_mouse->pointerPosition());
               _deckNode->setOffset(Vec2(_dimen.width * WIDTH_SCALE - pos.x, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()) - pos.y));
               _deckNode->setDrag(true);
               if (AudioEngine::get()->getState("cardSound") != AudioEngine::State::PLAYING){
                    AudioEngine::get()->play("cardSound", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
               }
          }
          if ( (_movement == 5) & down) {
               _goonInt = 0;
               _burnInt = 0;
               _movement = 6;
               AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
          }
          if (!down){
               _deckNode->setDrag(false);
               if (_doBurn){
                    if (AudioEngine::get()->getState("crumpleSound") != AudioEngine::State::PLAYING){
                         AudioEngine::get()->play("crumpleSound", _assets->get<Sound>("crumpleSound"), false, _soundVolume, false);
                    } else {
                         AudioEngine::get()->play("crumpleSound2", _assets->get<Sound>("crumpleSound"), false, _soundVolume, false);
                    }
                    buttonPress(-1);
                    _doBurn = false;
               } else if (_movement == 0){
                    if (AudioEngine::get()->getState("cardSound2") != AudioEngine::State::PLAYING){
                         AudioEngine::get()->play("cardSound2", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
                         
                    }
               }
          }
         });
     _response1->addListener([=](const std::string& name, bool down) {
          if ( (_movement == 0) & down & _display1) {
              buttonPress(0);
          }
         });
     _response2->addListener([=](const std::string& name, bool down) {
         if ( (_movement == 0) & down & _display2) {
              buttonPress(1);
         }
         });
     _response3->addListener([=](const std::string& name, bool down) {
         if ( (_movement == 0) & down & _display3) {
              buttonPress(2);
         }
         });
     if (_active) {
          _currCardButton->activate();
         _response1->activate();
         _response2->activate();
         _response3->activate();
     }
#else
//     _burnLabel->setPosition(_dimen.width * 0.11f, _dimen.height * (-0.05f));
     if (ratio > 1.5) {
          _burnTexture->setPosition(_dimen.width * 0.580f, _dimen.height * 0.15f);
     }
//     _burnTexture->setPosition(_dimen.width * 0.580f, _dimen.height * 0.15f);
     
     Touchscreen* touch = Input::get<Touchscreen>();
     touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
         this->touchBeganCB(event,focus);
     });
     touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
         this->touchEndedCB(event,focus);
     });
     touch->addMotionListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus) {
       this->touchesMovedCB(event, previous, focus);
     });
#endif
    if (_active) {
         _burn->activate();
         _mainMenu->activate();
         _pauseButton->activate();
         _musicSlider->activate();
         _soundSlider->activate();
         _autoFlipButton->activate();
    }
    _currEvent->setVisible(false);
    _currEvent->setText(_currentCard.getText());
    _resourceController.setBurnText(_currentCard, _burnText, _assets, _burnTexture);
    _mainMenu->addListener([=](const std::string& name, bool down) {
         if (_movement == 11 || _movement == 14) {
              _audioQueue->clear();
              if (_movement == 14){
                   _pause->setVisible(false);
                   _mainMenu->setVisible(false);
                   _black->setVisible(false);
                   _movement = 11;
              }
              this->_active = down;
         } else if (_movement == 12){
              _movement = 13;
         }
         });
     //_burnText->setText(resourceString({_currentCard.getResource(0),_currentCard.getResource(1),_currentCard.getResource(2),_currentCard.getResource(3)}));
    //_resourceCount->setText(resourceString(_resources));
    std::vector<int> displayedResponses = _currentCard.getRandomResponses();
     if (_item == 4 && itemFound){
          int i = rand() % r;
          _resourceController.setFree(i);
     }
    _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
    return success;
}

void GameScene::responseUpdate(const int responseId, const int response) {
     int id = _responses[responseId].getCards()[0];
     int level = _cards[id].getLevel();
     int mod = 1;
     if (level > _currentCard.getLevel()){
          mod = _mod;
     }
     string responseAddress = "response" + std::to_string(level);
     string glowAddress = "glow" + std::to_string(level);
     auto responseTexture = _assets->get<Texture>(responseAddress);
     auto responseGlow = _assets->get<Texture>(glowAddress);
     switch (response){
          case 1:
               _responseTexture1->setTexture(responseTexture);
               _responseGlow1->setTexture(responseGlow);
               _resourceController.setResponseResources(_responses, _responseId1, 1, _assets, mod);
               return;
          case 2:
               _responseTexture2->setTexture(responseTexture);
               _responseGlow2->setTexture(responseGlow);
               _resourceController.setResponseResources(_responses, _responseId2, 2, _assets, mod);
               return;
          case 3:
               _responseTexture3->setTexture(responseTexture);
               _responseGlow3->setTexture(responseGlow);
               _resourceController.setResponseResources(_responses, _responseId3, 3, _assets, mod);
               return;
     }
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
#ifdef CU_TOUCH_SCREEN
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->removeBeginListener(LISTENER_KEY);
    touch->removeEndListener(LISTENER_KEY);
    touch->removeMotionListener(LISTENER_KEY);
#endif
    removeAllChildren();
    _active = false;
    _assets = nullptr;
    _deckNode = nullptr;
    _mainMenu->clearListeners();
    _mainMenu = nullptr;
    _currCardButton->clearListeners();
    _currCardButton = nullptr;
    _response1->clearListeners();
    _response1 = nullptr;
    _response2->clearListeners();
    _response2 = nullptr;
    _response3->clearListeners();
    _response3 = nullptr;
    _soundSlider->clearListeners();
    _soundSlider = nullptr;
    _musicSlider->clearListeners();
    _musicSlider = nullptr;
    _pauseButton->clearListeners();
    _pauseButton = nullptr;
     _autoFlipButton->clearListeners();
     _autoFlipButton= nullptr;
    Scene2::dispose();
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
     if ((_fight == 3) & !_tutorial){
     _audioQueue->clear();
     _audioQueue->play(_assets->get<Sound>("introSlime"), false, _musicVolume, false);
     _audioQueue->enqueue(_assets->get<Sound>("repeatSlime"), true, _musicVolume, false);
     } else if (_fight == 5){
          _audioQueue->clear();
          _audioQueue->play(_assets->get<Sound>("introMenu"), false,_musicVolume, false);
          _audioQueue->enqueue(_assets->get<Sound>("repeatMenu"), true, _musicVolume, false);
     }
     if (_fight > 4){
          _background->setTexture(_assets->get<Texture>("background2"));
     } else {
          _background->setTexture(_assets->get<Texture>("background"));
     }
     
     _goonNumber->setText("Target " + std::to_string(_fight) + ":");
     string cardstring = "json/cards.json";
     if (_fight < _enemyFights.size() + 1){
          EnemyFight currFight = _enemyFights[_fight];
          _enemyIdleBuffer = currFight.getIdleBuffer();
          _enemyIdleSheet = 0;
          _enemyIdleSheetNum = currFight.getNumSheets();
          _goonName->setText(currFight.getEnemyName());
          _enemyIdle->dispose();
          //CULog("beforefilmstrip");
          _enemyIdle->initWithFilmstrip(_assets->get<Texture>(
               currFight.getEnemyTexture()), 
               currFight.getRows(), 
               currFight.getCols(), 
               currFight.getFrames());
          //CULog("afterfilmstrip");
          //_enemyIdle->setScale(0.69f);
          _enemyIdle->setFrame(0);
          _enemyIdle->setScale(currFight.getScale());
          _idleBuffer = 0;
          _enemyIdle->setPosition(_dimen.width * currFight.getWscale(), _dimen.height * currFight.getHscale());
          _enemyIdle->setScale(currFight.getScale());
          
          if (_fight < _enemyFights.size()){
               EnemyFight nextFight = _enemyFights[_fight + 1];
               _nextEnemy->dispose();
               _nextEnemy->initWithFilmstrip(_assets->get<Texture>(nextFight.getEnemyTexture()),
                                                                   nextFight.getRows(),
                                                                   nextFight.getCols(),
                                                                   nextFight.getFrames());
               _nextEnemy->setFrame(0);
               _nextEnemy->setScale(nextFight.getScale());
               _nextEnemy->setPosition(_dimen.width * nextFight.getWscale(), _dimen.height * nextFight.getHscale());
               _nextEnemy->setScale(nextFight.getScale());
               _nextFightText->setText("Next Fight: " + nextFight.getEnemyName());
          }
          if (!_tutorial) {
              cardstring = "json/level" + to_string(_fight) + ".json";
          }
          else {
              cardstring = "json/tutorial" + to_string(_fight) + ".json";
          }
          
     }
     //_fight = 4;
     // if (_fight == 1){
     //      cardstring = "json/level2.json";
     // } else if (_fight == 2){
     //      cardstring = "json/level3.json";
     // } else if (_fight == 3){
     //      cardstring = "json/level4.json";
     // } else if (_fight == 4){
     //      _goonName->setText("CAVE SLIME");
     //      _enemyIdle->dispose();
     //      _enemyIdle->initWithFilmstrip(_assets->get<Texture>("slimeIdle"), 3, 5, 12);
     //      //_enemyIdle->setScale(0.69f);
     //      _enemyIdle->setFrame(0);
     //      _idleBuffer = 0;
     //      _enemyIdle->setPosition(_dimen.width * 0.2f, _dimen.height*0.375f);
     //      cardstring = "json/level5.json";
     // }
     _cards = {};
     _responses = {};
     std::shared_ptr<JsonReader> jsonReaderCardString = JsonReader::alloc(cardstring);
     _cards = getJsonCards(jsonReaderCardString, _cards, _assets);
     Card item = getItem(_item);
     _cards[-1] = item;
     if (!_tutorial) {
         std::shared_ptr<JsonReader> jsonReaderResponses = JsonReader::alloc("json/responses" + to_string(_fight) + ".json");
         _responses = getJsonResponses(jsonReaderResponses, _responses);
     }
     else {
         std::shared_ptr<JsonReader> jsonReaderResponses = JsonReader::alloc("json/responsestutorial" + to_string(_fight) + ".json");
         _responses = getJsonResponses(jsonReaderResponses, _responses);
         if (_fight == 2) {
             _resources = { 1, 1, 0, 0 };
             _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
             if (_ratio <= 1.5) {
                 _tutorialText[0]->setText("Burn a card to gain the");
                 _tutorialText[1]->setText("resources shown under it");
                 _tutorialText[2]->setText("by dragging it to the fire");
                 _tutorialText[3]->setText("at the bottom of the");
                 _tutorialText[4]->setText("screen. This is a vital");
                 _tutorialText[5]->setText("way to gain resources.");
                 _tutorialText[6]->setText("");
                 _tutorialText[7]->setText("Burnt cards are removed");
                 _tutorialText[8]->setText("from the current deck, and");
                 _tutorialText[9]->setText("don't shuffle in new cards.");
                 _tutorialText[10]->setText("");
                 _tutorialText[11]->setText("You lose if both the");
                 _tutorialText[12]->setText("current and the next");
                 _tutorialText[13]->setText("deck are empty.");
                 _tutorialText[14]->setText("");
                 _tutorialText[15]->setText("");
             }
             else {
                 _tutorialText[0]->setText("Burn a card to gain the");
                 _tutorialText[1]->setText("resources shown under it");
                 _tutorialText[2]->setText("by dragging it to the fire");
                 _tutorialText[3]->setText("at the bottom of the screen.");
                 _tutorialText[4]->setText("This is a vital way to gain");
                 _tutorialText[5]->setText("resources.");
                 _tutorialText[6]->setText("");
                 _tutorialText[7]->setText("Burnt cards are removed");
                 _tutorialText[8]->setText("from the current deck, and");
                 _tutorialText[9]->setText("don't shuffle in new cards.");
                 _tutorialText[10]->setText("");
                 _tutorialText[11]->setText("You lose if both the current");
                 _tutorialText[12]->setText("and the next deck are empty.");
                 _tutorialText[13]->setText("");
                 _tutorialText[14]->setText("");
                 _tutorialText[15]->setText("");
             }
         }
         else if (_fight == 3) {
             _resources = { 15, 15, 0, 0 };
             _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
             if (_ratio <= 1.5) {
                 _tutorialText[0]->setText("Both response costs and");
                 _tutorialText[1]->setText("the reward for burning");
                 _tutorialText[2]->setText("increase with a card's");
                 _tutorialText[3]->setText("level.");
                  _tutorialText[4]->setText("");
                 _tutorialText[5]->setText("Hover a response to see the");
                 _tutorialText[6]->setText("card it shuffles in. The top");
                 _tutorialText[7]->setText("left icon of the card shows");
                 _tutorialText[8]->setText("what resource its responses");
                 _tutorialText[9]->setText("use. The top right icon shows");
                 _tutorialText[10]->setText("how much it burns for.");
                 _tutorialText[11]->setText("");
                 _tutorialText[12]->setText("Actively burning cards will");
                 _tutorialText[13]->setText("give you valuable resources");
                 _tutorialText[14]->setText("for future fights.");
                 _tutorialText[15]->setText("");
             }
             else {
                 _tutorialText[0]->setText("Both response costs and the");
                 _tutorialText[1]->setText("reward for burning increase");
                 _tutorialText[2]->setText("with a card's level.");
                 _tutorialText[3]->setText("");
                 _tutorialText[4]->setText("Hold a response to see the card");
                 _tutorialText[5]->setText("it shuffles in, with two icons.");
                 _tutorialText[6]->setText("The top left icon shows what");
                 _tutorialText[7]->setText("resource its responses use.");
                 _tutorialText[8]->setText("The top right icon shows how");
                 _tutorialText[9]->setText("much it burns for.");
                 _tutorialText[10]->setText("");
                 _tutorialText[11]->setText("Actively burning cards will");
                 _tutorialText[12]->setText("give you valuable resources");
                 _tutorialText[13]->setText("for future fights.");
                 _tutorialText[14]->setText("");
                 _tutorialText[15]->setText("");
             }
         }
         else if (_fight == 4) {
             if (_ratio <= 1.5) {
                 _tutorialText[0]->setText("Between fights, your");
                 _tutorialText[1]->setText("resources will carry over");
                 _tutorialText[2]->setText("and you'll gain five of");
                  _tutorialText[3]->setText("each resource. The cards");
                  _tutorialText[4]->setText("in the deck will not ");
                  _tutorialText[5]->setText("carry over. Leveling up");
                 _tutorialText[6]->setText("too many cards at once");
                 _tutorialText[7]->setText("can leave you short on");
                 _tutorialText[8]->setText("resources, but is necessary ");
                 _tutorialText[9]->setText("to gain them.");
                 _tutorialText[10]->setText("For this fight, you only");
                 _tutorialText[11]->setText("have enough resources to");
                 _tutorialText[12]->setText("level one of your cards");
                 _tutorialText[13]->setText("to max. Upgrade one, and");
                 _tutorialText[14]->setText("either burn or avoid");
                 _tutorialText[15]->setText("upgrading the other.");
             }
             else {
                 _tutorialText[0]->setText("Between fights, your ");
                 _tutorialText[1]->setText("resources will carry over");
                 _tutorialText[2]->setText("and you'll gain five of each ");
                 _tutorialText[3]->setText("resource. The cards in the ");
                 _tutorialText[4]->setText("deck will not carry over.");
                 _tutorialText[5]->setText("");
                 _tutorialText[6]->setText("Leveling up too many cards");
                 _tutorialText[7]->setText("at once can leave you short");
                 _tutorialText[8]->setText("on resources, but is ");
                 _tutorialText[9]->setText("necessary to gain them.");
                 _tutorialText[10]->setText("");
                 _tutorialText[11]->setText("For this fight, you only have");
                 _tutorialText[12]->setText("enough resources to level one");
                 _tutorialText[13]->setText("of your cards to max.");
                 _tutorialText[14]->setText("Upgrade one, and either burn");
                 _tutorialText[15]->setText("or avoid upgrading the other.");
             }
         }
     }
     // std::shared_ptr<JsonReader> jsonReaderResponses = JsonReader::alloc("json/responses1.json");
     //cout << cardsArray->asString();
     _currentDeck = {};
     _nextDeck = {};
     _keepCards = false;
     _win = false;
     _doBurn = false;
     if (_autoFlip){
        _goonInt = 0;
        _burnInt = 0;
        _movement = 6;
        AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
     } else {
        _movement = 5;
     }
     if (_tutorial) {
         _goonLabel->setVisible(false);
         _tutorialBox->setVisible(true);
         for (int i = 0; i < 16; i++) {
             _tutorialText[i]->setVisible(true);
         }
     }
     _display2 = true;
     _display3 = true;
     if ((_item >= 0) & !_usedSecondWind){
          _currentDeck.push_back(-1);
     }
     if (_fight < _enemyFights.size() + 1){
          std::vector<int> currDeck = _enemyFights[_fight].getDeck();
          for (int i = 0; i < currDeck.size(); i++){
               _currentDeck.push_back(currDeck[i]);
          }
          std::vector<int> nextDeck = _enemyFights[_fight].getNextDeck();
          for (int i = 0; i < nextDeck.size(); i++){
               _nextDeck.push_back(nextDeck[i]);
          }
     }

     std::random_device rd;
     std::mt19937 g(rd());
      std::shuffle(_currentDeck.begin(), _currentDeck.end(), g);
     setGameJson(true);
     _currentCard = _cards[_currentDeck.back()];
     bool itemFound = false;
     int r = 0;
     _mod = 1;
     for (int i = 0; i < _currentDeck.size(); i++){
          if (_currentDeck[i] == 14){
               _mod += 1;
          }
          if (_currentDeck[i] == -1){
               itemFound = true;
          } else if (_currentDeck[i] != 13){
               if (_cards[_currentDeck[i]].getResponses() >= 3){
                    r += 3;
               } else {
                    r += _cards[_currentDeck[i]].getResponses();
               }
          }
     }
     //std::printf("r: %d\n", r);
     if (_item == 4 && itemFound){
          int i = rand() % r;
          //std::printf("i: %d\n", i);
          _resourceController.setFree(i);
     } else {
          _resourceController.setFree(-1);
     }
     _currentDeck.pop_back();
     if (_currentCard.getId() == -1 & _item == 3){
          int i = rand() % 4;
          int r = (int) _currentDeck.size() + (int)_nextDeck.size() + 1;
          _currentCard.setResources(i, r * 2);
     }
     _deckNode->setSize(int(_currentDeck.size()));
     _deckNode->setNextSize((int)(_nextDeck.size()));
     auto cardBackTexture1 = _assets->get<Texture>("cardBack1");
     auto cardBackTexture2 = _assets->get<Texture>("cardBack2");
     _deckNode->setBackTexture(cardBackTexture1);
     _deckNode->setNextBackTexture(cardBackTexture2);
     _deckNode->setDimen(_dimen);
     _deckNode->setFrontTexture(_currentCard.getTexture());
     _deckNode->setDrawFront(2);
     _deckNode->setDrag(false);
     _burnTexture->setVisible(false);
     _burnLabel->setVisible(false);
     _underline->setVisible(false);
     _saving->setVisible(false);
     _goon->setVisible(false);
     _deckNode->reset();
     _idleBuffer = 0;
}
/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
     if (_act < 60){
          _act ++;
     }
#ifndef CU_TOUCH_SCREEN
     if (_deckNode->getDrag()){
          _deckNode->setCurrCardPos(_deckNode->screenToNodeCoords(_mouse->pointerPosition()));
     }
     if (_doBurn & !(_tutorial == true & _fight == 1)){
          std::vector<int> resources = _resources;
          for (int i = 0; i < _resources.size(); i++) {
               if (_currentCard.getResource(i) > 0){
                    resources[i] += _currentCard.getResource(i);
                    _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
               }
          }
     } else {
          _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
     }
#endif
     if ((_movement != 0) & (_movement != 14) & _pauseButton->isVisible()){
          _pauseButton->setVisible(false);
     } else if ((_movement == 0) || (_movement == 14) & !_pauseButton->isVisible()) {
          _pauseButton->setVisible(true);
     }
     if (_audioQueue->getVolume() != _musicVolume){
          _audioQueue->setVolume(_musicVolume);
     }
     if (_movement == 1){
          _displayCard->setVisible(false);
          _removeCard1->setVisible(false);
          _removeCard2->setVisible(false);
          _shuffleFlip->setPosition(_shuffleFlip->getPosition() + _vel);
          _prevFlip->setPosition(_prevFlip->getPosition() + _vel2);
          _shuffleFlip->setScale(_shuffleFlip->getScaleX() + (_scl - 0.36914f)/40.0f);
          bool stop = _shuffleFlip->getScaleX() >= 1.0283f || (_shuffleFlip->getScaleX() +(_scl - 0.36914f)/40.0f > 1.0283f);
          
          if (stop){
               _goonInt = 0;
               _burnInt = 0;
               _shuffleBackFlip->setPosition(_dimen.width * 0.825f, _dimen.height*HEIGHT_SCALE);
               _shuffleBackFlip->setScale(_shuffleFlip->getScale());
               _prevBackFlip->setPosition(_prevFlip->getPosition());
               AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
               _movement = 2;
          }
     } else if (_movement == 2){
          int frame = _shuffleFlip->getFrame();
          frame -= 1;
          
          if (frame < 0){
               _shuffleBackFlip->setVisible(true);
               _shuffleFlip->setVisible(false);
               _prevFlip->setVisible(false);
               _prevBackFlip->setVisible(true);
               frame = _shuffleBackFlip->getFrame();
               frame -= 1;
               if (frame <= 0){
                    _movement = 3;
                    _prevBackFlip->setVisible(false);
                    _cardCut->setVisible(true);
                    _cardCut->setFrame(0);
                    _vel =Vec2(_dimen.width * (0.45f + 0.0125 * (_nextDeck.size()-1)), _dimen.height * 0.875f) - _shuffleFlip->getPosition();
                    _vel.scale(0.025f);
                    _scl = 0.3389;
                    
                    AudioEngine::get()->play("slashSound", _assets->get<Sound>("slashSound"), false, 0.3f * _soundVolume, false);
               }
               _shuffleBackFlip->setFrame(frame);
               _prevBackFlip->setFrame(frame);
               frame += _shuffleFlip->getSize();
          } else {
               _shuffleFlip->setFrame(frame);
               _prevFlip->setFrame(frame);
          }
          
          if (frame < 20){
               _underline->setVisible(false);
               string goonText = "";
               string currentEvent = "Current Event";
               if (_goonInt <= 13){
                    for (int i = 0; i < _goonInt; i ++){
                         goonText = goonText + " ";
                    }
                    goonText = goonText + currentEvent.substr(_goonInt,13);
                    _goon->setText(goonText);
                    _goonInt += 1;
               }
          }
          if (frame < 30){
               //_burnLabel ->setVisible(true);
               string burnText = "";
               string burnCard = "Burn card to receive";
               if (_burnInt <= 20){
                    for (int i = 0; i < _burnInt; i ++){
                         burnText = burnText + " ";
                    }
                    burnText = burnText + burnCard.substr(_burnInt,20);
                    _burnLabel->setText(burnText);
                    _burnInt += 1;
               } else {
                    _burnTexture->setVisible(false);
               }
          }
     } else if (_movement == 3){
          int frame = _cardCut->getFrame();
          frame += 1;
          if (frame < _cardCut->getSize()){
               _cardCut->setFrame(frame);
          } else {
               _cardCut->setVisible(false);
          }
          _goon->setVisible(false);
          _burnLabel ->setVisible(false);
          _shuffleBackFlip->setPosition(_shuffleBackFlip->getPosition() + _vel);
          _shuffleBackFlip->setScale(_shuffleBackFlip->getScaleX() + (_scl - 1.0283f)/40.0f);
          if (_shuffleBackFlip->getPosition().y >= _dimen.height * 0.875f || _shuffleBackFlip->getPosition().y + _vel.y > _dimen.height * 0.875f){
               _movement = 4;
          }
     }
     _idleBuffer += timestep;
     //printf("%f",_idleBuffer);
     if (_idleBuffer >= _enemyIdleBuffer){
          int enemyFrame = _enemyIdle->getFrame();
          enemyFrame += 1;
          if (enemyFrame == _enemyIdle->getSize()){
               enemyFrame = 0;
               _enemyIdleSheet += 1;
               if (_enemyIdleSheet == _enemyIdleSheetNum){
                    _enemyIdleSheet = 0;
               }
               _enemyIdle->setTexture(_assets->get<Texture>(_enemyFights[_fight].getEnemyTexture(_enemyIdleSheet)));
          }
          _enemyIdle->setFrame(enemyFrame);
          _idleBuffer = 0;
     }
     if(_movement == 4){
          _goon->setVisible(false);
          _burnLabel ->setVisible(false);
          _shuffleFlip->setVisible(false);
          _shuffleFlip->setFrame(_shuffleFlip->getSize() - 1);
          _shuffleBackFlip->setVisible(false);
          _shuffleBackFlip->setFrame(_shuffleBackFlip->getSize() - 1);
          //_currEvent->setColor(Color4::WHITE);
          _response1->setColor(Color4::WHITE);
          _responseText1->setForeground(Color4::BLACK);
          _responseText2->setForeground(Color4::BLACK);
          _responseText3->setForeground(Color4::BLACK);
          _response2->setColor(Color4::WHITE);
          _response3->setColor(Color4::WHITE);
          if (_currentDeck.size() == 0){
               _currEvent->setText("Shuffling Next Event Deck...");
               //_currEvent->setColor(Color4::BLACK);
               if (_nextDeck.size() > 0) {
                    if (_nextDeck.size() == 1) {
                         if (_nextDeck[0] == 13 || _nextDeck[0] == 14 || _nextDeck[0] == 15){
                              gameOver();
                              return;
                         }
                         if (_nextDeck[0] == -1 & _item != 2) {
                              gameOver();
                              return;
                         }
                    }
                    if (_nextDeck.size() == 3) {
                         if (_nextDeck[0] == 14 || _nextDeck[0] == 15){
                              if (_nextDeck[1] == 14 || _nextDeck[1] == 15){
                                   if (_nextDeck[2] == 14 || _nextDeck[2] == 15){
                                        gameOver();
                                        return;
                                   }
                              }
                         }
                    }
                    _deckNode->setNextSize(int(_nextDeck.size()));
                    _currentDeck = _nextDeck;
                    std::random_device rd;
                    std::mt19937 g(rd());
                     std::shuffle(_currentDeck.begin(), _currentDeck.end(), g);
                    _nextDeck = {};
                    setGameJson(false);
                    bool itemFound = false;
                    int r = 0;
                    _mod = 1;
                    bool redFound = false;
                    bool blueFound = false;
                    bool greenFound = false;
                    for (int i = 0; i < _currentDeck.size(); i++){
                         if (_currentDeck[i] == 16){
                              blueFound = true;
                         }
                         if (_currentDeck[i] == 17){
                              greenFound = true;
                         }
                         if (_currentDeck[i] == 18){
                              redFound = true;
                         }
                         if (_currentDeck[i] == 14){
                              _mod += 1;
                         }
                         if (_currentDeck[i] == -1){
                              itemFound = true;
                         } else if (_currentDeck[i] != 13){
                              if (_cards[_currentDeck[i]].getResponses() >= 3){
                                   r += 3;
                              } else {
                                   r += _cards[_currentDeck[i]].getResponses();
                              }
                         }
                    }
                    _allRunes = redFound & blueFound & greenFound;
                    //std::printf("r: %d\n", r);
                    if (_item == 4 && itemFound){
                         int i = rand() % r;
                         //std::printf("i: %d\n", i);
                         _resourceController.setFree(i);
                    } else {
                         _resourceController.setFree(-1);
                    }
                    if (_currentDeck.size() > 1){
                    AudioEngine::get()->play("cardSound4", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
                    }
                    _movement = 9;
               } else {
                    gameOver();
                    return;
               }
          } else if (_currentDeck.size() == 1 && _nextDeck.size() == 0) {
               if (_currentDeck[0] == 13 || _currentDeck[0] == 14 || _currentDeck[0] == 15) {
                    gameOver();
                    return;
               }
               if (_currentDeck[0] == -1){
                    if (_item != 2)  {
                         gameOver();
                         return;
                    } else if (_usedSecondWind){
                         gameOver();
                         return;
                    }
               }
          }
          else if (_fight == 7 && _currentDeck.size() + _nextDeck.size() == 3) {
              if (_currentDeck[0] == 14 || _currentDeck[0] == 15) {
                  gameOver();
                  return;
              }
              if (_currentDeck[0] == -1) {
                  if (_item != 2) {
                      gameOver();
                      return;
                  }
                  else if (_usedSecondWind) {
                      gameOver();
                      return;
                  }
              }
          }
          else {
               _currEvent->setVisible(false);
          }
          if (_currentCard.getId() < 13 || _currentCard.getId() > 15){
               _lastCard = _currentCard;
          }
          _currentCard = _cards[_currentDeck.back()];
          _currentDeck.pop_back();
          if (_currentCard.getId() == -1 & _item == 2){
               bool nonSpecialFound = false;
               for (int i = 0; i < _currentDeck.size(); i++){
                    if (_currentDeck[i] < 13 || _currentDeck[i] > 15){
                         nonSpecialFound = true;
                    }
               }
               for (int i = 0; i < _nextDeck.size(); i++){
                    if (_nextDeck[i] < 13 || _nextDeck[i] > 15){
                         nonSpecialFound = true;
                    }
               }
               if (!nonSpecialFound){
                    _currentCard = _lastCard;
                    _usedSecondWind = true;
               }
          }
          if (_currentCard.getId() == -1 & _item == 3){
               int i = rand() % 4;
               int r = (int) _currentDeck.size() + (int)_nextDeck.size() + 1;
               _currentCard.setResources(i, r * 2);
          }
          _deckNode->setFrontTexture(_currentCard.getTexture());
          _currEvent->setText(_currentCard.getText());
          string flipTexture = _currentCard.getText() + "Flip";
          _currentFlip->setTexture(_assets->get<Texture>(flipTexture));
          _resourceController.setBurnText(_currentCard, _burnText, _assets, _burnTexture);
          _currCardButton->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
          _currentBackFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
          _currentFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
          if (_movement == 4){
               _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE + DECK_SCALE * (_currentDeck.size())));
               _deckNode->setSize(int(_currentDeck.size()));
               _deckNode->setNextSize(int(_nextDeck.size()));
               _currentFlip->setVisible(false);
               _currentBackFlip->setVisible(true);
               if (_autoFlip){
                    _goonInt = 0;
                    _burnInt = 0;
                    _movement = 6;
                    AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
               } else {
                    _movement = 5;
               }
          }
     }
          if (_movement == 6){
          int flipFrame = _currentBackFlip->getFrame();
          flipFrame += 1;
          if (flipFrame >= _currentBackFlip->getSize()){
               _currentFlip->setVisible(true);
               _currentBackFlip->setVisible(false);
               flipFrame = _currentFlip->getFrame();
               flipFrame += 1;
               if (flipFrame >= _currentFlip->getSize()-1){
                    _movement = 7;
                    flipFrame = 0;
                    _currentBackFlip->setFrame(0);
                    _currentFlip->setVisible(false);
               }
               _currentFlip->setFrame(flipFrame);
               flipFrame += _currentBackFlip->getSize();
          } else {
               _currentBackFlip->setFrame(flipFrame);
          }
          if (flipFrame > 10){
               //_goon->setHorizontalAlignment(scene2::Label::HAlign(2));
               _goon->setVisible(true);
               _underline->setVisible(false);
               string goonText = "";
               string currentEvent = "Current Event";
               if (_goonInt < 13){
                    for (int i = 12; i >= _goonInt; i --){
                         goonText = goonText + " ";
                    }
                    goonText = goonText + currentEvent.substr(12 - _goonInt,13);
                    _goon->setText(goonText);
                    _goonInt += 1;
               } else {
                    _underline->setVisible(true);
               }
          }
          if (_currentCard.getId() != 13 & _currentCard.getId() != 14 & _currentCard.getId() != 15 & !(_tutorial == true & _fight == 1)) {
               if (flipFrame > 7){
                    _burnTexture->setVisible(true);
               }
               if (flipFrame > 8){
                    _burnLabel ->setVisible(true);
                    string burnText = "";
                    string burnCard = "Burn card to receive";
                    if (_burnInt < 20){
                         for (int i = 19; i >= _burnInt; i --){
                              burnText = burnText + " ";
                         }
                         burnText = burnText + burnCard.substr(19 - _burnInt,20);
                         _burnLabel->setText(burnText);
                         _burnInt += 1;
                    }
               }
          }
     }
     if (_movement == 7){
          //_underline->setVisible(true);
          _removeOptions = {-2,-2,-2};
          if (!_keepCards) {
               std::vector<int> displayedResponses = _currentCard.getRandomResponses();
               if (_currentCard.getId() == 13){
                    std::vector<int> cardsInDeck = _currentDeck;
                    cardsInDeck.insert(cardsInDeck.end(), _nextDeck.begin(), _nextDeck.end());
                    sort( cardsInDeck.begin(), cardsInDeck.end() );
                    cardsInDeck.erase( unique( cardsInDeck.begin(), cardsInDeck.end() ), cardsInDeck.end() );
                    int r = (int)cardsInDeck.size();
                    if (r > 0){
                         int i = rand() % r;
                         _removeOptions[0] = cardsInDeck[i];
                         if (r > 1){
                              int j = i;
                              while (j == i){
                                   j = rand() % r;
                              }
                              _removeOptions[1] = cardsInDeck[j];
                              if (r > 2){
                                   int k = i;
                                   while (k == i || k == j){
                                        k = rand() % r;
                                   }
                                   _removeOptions[2] = cardsInDeck[k];
                                   displayedResponses = {50, 50, 50};
                              } else {
                                   displayedResponses = {50, 50};
                              }
                         }
                    }
               }
               if (displayedResponses.size() > 1){
                    if ((displayedResponses[0] == 48) & !_allRunes) {
                         displayedResponses = {displayedResponses[1]};
                    } else if ((displayedResponses[1] == 48) & !_allRunes) {
                         displayedResponses = {displayedResponses[0]};
                    }
               }
               if (displayedResponses.size() > 2) {
                    _responseId1 = displayedResponses[0];
                    _responseId2 = displayedResponses[1];
                    _responseId3 = displayedResponses[2];
                    _display1 = true;
                    _display2 = true;
                    _display3 = true;
               } else if (displayedResponses.size() > 1) {
                    _responseId1 = displayedResponses[0];
                    _responseId2 = displayedResponses[1];
                    _display1 = true;
                    _display2 = true;
                    _display3 = false;
               } else if (displayedResponses.size() > 0) {
                    _responseId1 = displayedResponses[0];
                    _display1 = true;
                    _display2 = false;
                    _display3 = false;
               } else {
                    _display1 = false;
                    _display2 = false;
                    _display3 = false;
               }
          }
          if (_display1) {
               _responseText1->setText(_responses[_responseId1].getText());
               _responseText1->setScale(_responses[_responseId1].getFontSize());
               responseUpdate(_responseId1, 1);
               if (_removeOptions[0] == -2){
                    _responseCard1->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
                    _shuffle1->setText("Shuffle:");
               } else {
                    _responseCard1->setTexture(_cards[_removeOptions[0]].getTexture());
                    _shuffle1->setText("Remove:");
               }
          }
          if (_display2) {
               _responseText2->setText(_responses[_responseId2].getText());
               _responseText2->setScale(_responses[_responseId2].getFontSize());
               responseUpdate(_responseId2, 2);
               if (_removeOptions[1] == -2){
                    _responseCard2->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
                    _shuffle2->setText("Shuffle:");
               } else {
                    _responseCard2->setTexture(_cards[_removeOptions[1]].getTexture());
                    _shuffle2->setText("Remove:");
               }
          }
          if (_display3){
               _responseText3->setText(_responses[_responseId3].getText());
               _responseText3->setScale(_responses[_responseId3].getFontSize());
               responseUpdate(_responseId3, 3);
               if (_removeOptions[2] == -2){
                    _responseCard3->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
                    _shuffle3->setText("Shuffle:");
               } else {
                    _responseCard3->setTexture(_cards[_removeOptions[2]].getTexture());
                    _shuffle3->setText("Remove:");
               }
          }

          _response1->setVisible(_display1);
          _response2->setVisible(_display2);
          _response3->setVisible(_display3);
          _deckNode->setDrawFront(0);
          _movement = 0;
     }
     if (_movement == 8){
          _underline->setVisible(false);
          _burnTexture->setVisible(false);
          string goonText = "";
          string currentEvent = "Current Event";
          if (_goonInt <= 13){
               for (int i = 0; i < _goonInt; i ++){
                    goonText = goonText + " ";
               }
               goonText = goonText + currentEvent.substr(_goonInt,13);
               _goon->setText(goonText);
               _goonInt += 1;
          }
          _burnLabel ->setVisible(true);
          string burnText = "";
          string burnCard = "Burn card to receive";
          if (_burnInt <= 20){
               for (int i = 0; i < _burnInt; i ++){
                    burnText = burnText + " ";
               }
               burnText = burnText + burnCard.substr(_burnInt,20);
               _burnLabel->setText(burnText);
               _burnInt += 1;
          } 
          int burnFrame = _currentBurn->getFrame();
          burnFrame += 1;
          if (burnFrame == 40){
               AudioEngine::get()->play("burnSound", _assets->get<Sound>("burnSound"),false, _soundVolume, false);
          }
          if (burnFrame == _currentBurn->getSize()){
               _movement = 4;
               _currentBurn->setVisible(false);
               burnFrame = 0;
          }
          _currentBurn->setFrame(burnFrame);
     }
     if (_movement == 9){
          float offset = _deckNode->getHOffset();
          if (offset > 0){
               _deckNode->setHOffset(offset - 0.0003125f);
          } else {
               _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE + DECK_SCALE * (_currentDeck.size())));
               _movement = 10;
               if (_currentDeck.size() > 0){
                    AudioEngine::get()->play("shuffleSound", _assets->get<Sound>("shuffleSound"), false, 2.0f * _soundVolume, false);
               } else {
                    AudioEngine::get()->play("cardSound", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
               }
          }
     }
     if (_movement == 10){
          float offset = _deckNode->getVOffset();
          if (offset < DECK_SCALE){
               _deckNode->setVOffset(offset + 0.00025f);
               _deckNode->setScaler(_deckNode->getScaler() + (0.41f - 0.135f)/40.0f);
               Vec2 dir = Vec2(_dimen.width * WIDTH_SCALE, _dimen.height * HEIGHT_SCALE) - Vec2(_dimen.width * 0.45f, _dimen.height *0.875f);
               _deckNode->setOffsetVector(_deckNode->getOffsetVector() + dir.scale(0.025f));
          } else {
               _deckNode->reset();
               _deckNode->setNextSize(0);
               _deckNode->setSize(int(_currentDeck.size()));
               _deckNode->setNextSize(int(_nextDeck.size()));
               _currentBackFlip->setVisible(true);
               _saving->setVisible(false);
               if (_autoFlip){
                    _goonInt = 0;
                    _burnInt = 0;
                    _movement = 6;
                    AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
               } else {
                    _movement = 5;
               }
          }
     }
     if (_movement == 11) {
          _mainMenuLabel->setText("Main Menu");
         _mainMenu->setVisible(true);
     }
     if (_movement == 12){
          if (!_tutorial){
               _nextEnemy->setVisible(true);
               _nextFight->setVisible(true);
          }
          if (!_tutorial) {
              _mainMenuLabel->setText("Start Fight");
          }
          else {
              _mainMenuLabel->setText("Continue");
          }
          if ((_fight == 3 || _fight == 4) && !_tutorial){
               _nextFightPoison->setVisible(true);
               _nextFightBrawler->setVisible(false);
               _nextFightWorm->setVisible(false);
          } else if (_fight == 5 && !_tutorial) {
               _nextFightBrawler->setVisible(true);
               _nextFightPoison->setVisible(false);
               _nextFightWorm->setVisible(false);
          } else if ((_fight == 6 || _fight == 7) && !_tutorial) {
               _nextFightBrawler->setVisible(false);
               _nextFightPoison->setVisible(false);
               _nextFightWorm->setVisible(true);
          } else {
               _nextFightPoison->setVisible(false);
               _nextFightBrawler->setVisible(false);
               _nextFightWorm->setVisible(false);
          }
          _mainMenu->setVisible(true);
     }
     if (_movement == 13){
          _mainMenu->setVisible(false);
          reset();
         _shuffleFlip->setVisible(false);
         _shuffleFlip->setFrame(_shuffleFlip->getSize() - 1);
          _shuffleBackFlip->setVisible(false);
          _shuffleBackFlip->setFrame(_shuffleBackFlip->getSize() - 1);
         //_currEvent->setColor(Color4::WHITE);
         _response1->setColor(Color4::WHITE);
         _responseText1->setForeground(Color4::BLACK);
         _responseText2->setForeground(Color4::BLACK);
         _responseText3->setForeground(Color4::BLACK);
         _response2->setColor(Color4::WHITE);
         _response3->setColor(Color4::WHITE);
         _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE + DECK_SCALE * (_currentDeck.size())));
         _currEvent->setText(_currentCard.getText());
         string flipTexture = _currentCard.getText() + "Flip";
         _currentFlip->setTexture(_assets->get<Texture>(flipTexture));
         _resourceController.setBurnText(_currentCard, _burnText, _assets, _burnTexture);
         _currCardButton->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
          _currentBackFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
         _currentFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
         _currEvent->setVisible(false);
         _deckNode->setVisible(true);
         _currentBackFlip->setVisible(true);
         _cardHolder->setVisible(true);
         if (!_tutorial) {
             _enemyIdle->setVisible(true);
         }
         else {
             _enemyIdle->setVisible(false);
         }
         _black->setVisible(false);
         _soundSlider->setVisible(true);
         _autoFlipButton->setVisible(true);
         _musicSlider->setVisible(true);
         _soundSliderNode->setVisible(true);
         _musicSliderNode->setVisible(true);
         _paused->setVisible(true);
         _pause->setVisible(false);
         _goonName->setVisible(true);
         _goonNumber->setVisible(true);
         _nextEnemy->setVisible(false);
         _nextFight->setVisible(false);
     }
     if (_movement == 15) {
         _goonLabel->setVisible(false);
         _tutorialBox->setVisible(true);
         for (int i = 0; i < 16; i++) {
             _tutorialText[i]->setVisible(true);
         }
     }
#ifndef CU_TOUCH_SCREEN
     if ((_movement == 0) & !_deckNode->getDrag() & (_currentCard.getId() == 13)){
          if (_response1->containsScreen(_mouse->pointerPosition()) & _display1) {
               
               _removeCard2->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[0]].getTexture());
               setDisplayCardBurnText(_removePreviewBurnTexture, _removePreviewBurnText, _cards[_removeOptions[0]]);
               setDisplayCardResponseType(_removePreviewResponseType, _removePreviewResponseType, _cards[_removeOptions[0]], true);
               // setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               // if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
               //      setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, true);
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          }
          else if (_response2->containsScreen(_mouse->pointerPosition()) & _display2) {
               _removeCard2->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[1]].getTexture());
               setDisplayCardBurnText(_removePreviewBurnTexture, _removePreviewBurnText, _cards[_removeOptions[1]]);
               setDisplayCardResponseType(_removePreviewResponseType, _removePreviewResponseType, _cards[_removeOptions[1]], true);
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          }
          else if (_response3->containsScreen(_mouse->pointerPosition()) & _display3) {
               _removeCard2->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[2]].getTexture());
               setDisplayCardBurnText(_removePreviewBurnTexture, _removePreviewBurnText, _cards[_removeOptions[2]]);
               setDisplayCardResponseType(_removePreviewResponseType, _removePreviewResponseType, _cards[_removeOptions[2]], true);
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          } else {
               _displayCard->setVisible(false);
               _removeCard1->setVisible(false);
               _removeCard2->setVisible(false);
          }
     } else if ((_movement == 0) & !_deckNode->getDrag()) {
          Card displayCard;
          if (_response1->containsScreen(_mouse->pointerPosition()) & _display1) {
               displayCard = _cards[_responses[_responseId1].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, true);
               } else {
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId1].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId1].getResources()[i] >= 0){
                             int mod = 1;
                             if (_cards[_responses[_responseId1].getCards()[0]].getLevel() > _currentCard.getLevel()) {
                                 mod = _mod;
                             }
                             resources[i] -= (_responses[_responseId1].getResources()[i] * mod);
                             _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                             _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response2->containsScreen(_mouse->pointerPosition()) & _display2) {
              displayCard = _cards[_responses[_responseId2].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, true);
               } else {
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId2].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId2].getResources()[i] >= 0){
                              int mod = 1;
                              if (_cards[_responses[_responseId2].getCards()[0]].getLevel() > _currentCard.getLevel()) {
                                  mod = _mod;
                              }
                              resources[i] -= (_responses[_responseId2].getResources()[i] * mod);
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response3->containsScreen(_mouse->pointerPosition()) & _display3) {
              displayCard = _cards[_responses[_responseId3].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, true);
               } else {
                    setDisplayCardResponseType(_displayCardResponseType, _displayCardResponseBurn, displayCard, false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId3].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId3].getResources()[i] >= 0){
                              int mod = 1;
                              if (_cards[_responses[_responseId3].getCards()[0]].getLevel() > _currentCard.getLevel()) {
                                 mod = _mod;
                              }
                              resources[i] -= (_responses[_responseId3].getResources()[i] * mod); 
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          } else {
               _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
               _displayCard->setVisible(false);
          }
     } else if ((_movement == 0) & _deckNode->getDrag()) {
          if (_burn->containsScreen(_mouse->pointerPosition()) & (_currentCard.getId() != 13 & _currentCard.getId() != 14 & _currentCard.getId() != 15) & !(_tutorial == true & _fight == 1)){
               _doBurn = true;
          } else {
               _doBurn = false;
          }
     }
#endif
     if (_movement != 0){
          _displayCard->setVisible(false);
     }
}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch){
    // Call SUPER to do standard rendering
    Scene2::render(batch);
    
     /*
    // Now do 3152-style rendering for the photons
    batch->begin(getCamera()->getCombined());
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    _photons->draw(batch);
    batch->end();
      */
}



string GameScene::resourceString(std::vector<int> resources) {
    std::stringstream s;
    s << "Bl: " << resources[0] << ", ";
    s << "F: " << resources[1] << ", ";
    s << "L: " << resources[2] << ", ";
    s << "Br: " << resources[3];

    return s.str();
}

void GameScene::buttonPress(const int r){
     bool win = false;
     if (r == -1){
          for (int i = 0; i < _resources.size(); i++) {
               _resources[i] += _currentCard.getResource(i);
               _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
          }
     } else {
          Response response;
          if (r == 0){
               response=_responses[_responseId1];
               _shuffleFlip->setPosition(_dimen.width * SHUFFLE_WIDTH_SCALE, _dimen.height*R1_HEIGHT_SCALE);
          } else if (r == 1){
               response=_responses[_responseId2];
               _shuffleFlip->setPosition(_dimen.width * SHUFFLE_WIDTH_SCALE, _dimen.height*R2_HEIGHT_SCALE);
          } else {
               response=_responses[_responseId3];
               _shuffleFlip->setPosition(_dimen.width * SHUFFLE_WIDTH_SCALE, _dimen.height*R3_HEIGHT_SCALE);
          }
          std::vector<int> cost = response.getResources();
          int mod = 1;
          if (_cards[response.getCards()[0]].getLevel() > _currentCard.getLevel()){
               mod = _mod;
          }
          if (_resourceController.getFreeResponse() - 1 != r) {
               for (int i = 0; i < cost.size(); i++) {
                   if (_resources[i] < (cost[i] * mod)) {
                       if (r == 0) {
                           _responseText1->setText("Need Resources");
                            _responseText1->setScale(0.55f);
                            _responseText1->setForeground(Color4::WHITE);
                           _response1->setColor(Color4::GRAY);
                       } else if (r == 1) {
                           _responseText2->setText("Need Resources");
                            _responseText2->setScale(0.55f);
                            _responseText2->setForeground(Color4::WHITE);
                           _response2->setColor(Color4::GRAY);
                       }
                       else if (r == 2) {
                           _responseText3->setText("Need Resources");
                            _responseText3->setScale(0.55f);
                            _responseText3->setForeground(Color4::WHITE);
                           _response3->setColor(Color4::GRAY);
                       }

                       return;
                   }
               }
          }
          for (int i = 0; i < cost.size(); i++) {
               if (_resourceController.getFreeResponse() - 1 != r){
                    _resources[i] -= (cost[i] * mod);
               }
               _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
          }

          std::vector<int> cards =response.getCards();
          if (_removeOptions[r] != -2){
               removeCard(_removeOptions[r]);
          }
          for (int i = 0; i < cards.size(); i++){
               int newCard = cards[i];
               string flipTexture = _cards[newCard].getText() + "Flip";
               _shuffleFlip->setTexture(_assets->get<Texture>(flipTexture));
               _nextDeck.push_back(newCard);
          }
          win = response.getWin();
     }
     //CULog("Next Deck:");
     //_nextDeck.printDeck();
     //CULog("Current Deck:");
     //_currentDeck.printDeck();
     _resourceController.setFreeResponse(-1);
     _response1->setVisible(false);
     _response2->setVisible(false);
     _response3->setVisible(false);
     if (_currentDeck.size() == 0){
          _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE));
     } else {
          _goon->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (GOON_HEIGHT_SCALE + DECK_SCALE * (_currentDeck.size() -1)));
     }
     _deckNode->setDrawFront(2);
     _scl = 1.0283f;
     if (!_win & (r != -1)) {
         _shuffleFlip->setScale(0.36914f);
         _shuffleFlip->setVisible(true);
     }
     if (r== -1){
          _burnInt = 0;
          _goonInt = 0;
          _movement = 8;
#ifndef CU_TOUCH_SCREEN
          _currentBurn->setPosition(_deckNode->screenToNodeCoords(_mouse->pointerPosition()) + _deckNode->getOffset());
          string burnTexture = _currentCard.getText() + "Burn";
          _currentBurn->setTexture(_assets->get<Texture>(burnTexture));
#endif
          _currentBurn->setVisible(true);
     } else {
          _prevFlip->setVisible(true);
          AudioEngine::get()->play("cardSound", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
          _movement = 1;
     }
     //_vel = Vec2(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size())) - _shuffleFlip->getPosition();
     _vel = Vec2(_dimen.width * 0.825f, _dimen.height*HEIGHT_SCALE) - _shuffleFlip->getPosition();
     _vel2 = Vec2(_dimen.width * 0.2f, _dimen.height * HEIGHT_SCALE) - Vec2(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     _vel2.scale(0.025f);
     _vel.scale(0.025f);
     _prevFlip->setPosition(_dimen.width * WIDTH_SCALE, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()));
     string flipTexture = _currentCard.getText() + "Flip";
     _prevFlip->setTexture(_assets->get<Texture>(flipTexture));
     _prevFlip->setFrame(_prevFlip->getSize()-1);
     _keepCards = false;
     if (win){
          resourceIncrease();
          _fight += 1;
          if (((_fight - 1 > _highestLevel)) & !_tutorial) {
               _highestLevel = _fight-1;
               setProgressJson();
          }
          _burnTexture->setVisible(false);
          _prevFlip->setVisible(false);
          _prevBackFlip->setVisible(false);
          _deckNode->setVisible(false);
         _displayCard->setVisible(false);
          _removeCard1->setVisible(false);
          _removeCard2->setVisible(false);
         _goon->setVisible(false);
         _currEvent->setText(_enemyFights[_fight-1].getEnemyName() +" Defeated!");
         //_currEvent->setVisible(true);
          _currentFlip->setVisible(false);
          _currentBackFlip->setVisible(false);
          _cardHolder->setVisible(false);
          _enemyIdle->setVisible(false);
          _pause->setVisible(true);
          _soundSlider->setVisible(false);
          _autoFlipButton->setVisible(false);
          _musicSlider->setVisible(false);
          _soundSliderNode->setVisible(false);
          _musicSliderNode->setVisible(false);
          _paused->setVisible(false);
          _black->setVisible(true);
          _cardHolder->setVisible(false);
          _shuffleFlip->setVisible(false);
          _shuffleBackFlip->setVisible(false);
          _goonName->setVisible(false);
          _goonNumber->setVisible(false);
          //_currEvent->setColor(Color4::WHITE);
          if (_fight > _enemyFights.size()){
               if (filetool::file_exists(Application::get()->getSaveDirectory() + "savedGame.json") & !_tutorial){
#if defined (__WINDOWS__)
                string path = Application::get()->getSaveDirectory() + "savedGame.json";
                std::remove(path.c_str());
#else
                filetool::file_delete(Application::get()->getSaveDirectory() + "savedGame.json");
#endif
               }
               _currEvent->setText("Hunt Complete!");
               _currEvent->setVisible(true);
               _movement = 11;
          } else {
               _movement = 12;
          }
     }
}


void GameScene::setDisplayCardBurnText(std::shared_ptr<cugl::scene2::NinePatch> nodeTexture, 
                                        std::shared_ptr<cugl::scene2::Label> nodeText, Card displayCard){
     if (displayCard.getText() == "Incapacitated" || displayCard.getId() == 13 || displayCard.getId() == 14 || displayCard.getId() == 15 || (_tutorial == true & _fight == 1)){
         nodeTexture->setVisible(false);
     } else {
         nodeTexture->setVisible(true);
     }
     for (int i = 0; i < 4; i ++){
          if (displayCard.getResource(i) > 0){
               nodeText->setText(to_string(displayCard.getResource(i)));
               string resource = "brawn";
               if (i == 0){
                    resource = "blade";
               } else if (i == 1){
                    resource = "flourish";
               } else if (i == 2){
                    resource = "lunge";
               }
               nodeTexture->setTexture(_assets->get<Texture>(resource));
               return;
          }
     }
     //displaycardburn formerly
     nodeText->setText(to_string(0));
     nodeTexture->setTexture(_assets->get<Texture>("flourish"));
}

void GameScene::setDisplayCardResponseType(std::shared_ptr<cugl::scene2::NinePatch> nodeResponseType,
                                        std::shared_ptr<cugl::scene2::NinePatch> nodeBrawler,
                                        Card displayCard, bool brawler){
    string resource = displayCard.getResponseType();
    if (resource == "none"){
           nodeResponseType->setVisible(false);
           nodeBrawler->setVisible(false);
    } else {
          nodeResponseType->setTexture(_assets->get<Texture>(resource));
          nodeResponseType->setVisible(true);
          nodeBrawler->setVisible(false);
        if (brawler){
            if (resource != "brawn"){
                 nodeBrawler->setVisible(true);
            }
        }
    }
}
// void GameScene::setDisplayCardResponseType(std::shared_ptr<cugl::scene2::NinePatch> nodeResponseType,
//                                         std::shared_ptr<cugl::scene2::NinePatch> nodeBrawler,
//                                         Card displayCard, bool brawler){
//     string resource = displayCard.getResponseType();
//     if (resource == "none"){
//         nodeResponseType->setVisible(false);
//         _displayCardResponseBurn->setVisible(false);
//     } else {
//         _displayCardResponseType->setTexture(_assets->get<Texture>(resource));
//         _displayCardResponseType->setVisible(true);
//          _displayCardResponseBurn->setVisible(false);
//         if (brawler){
//             if (resource != "brawn"){
//                  _displayCardResponseBurn->setVisible(true);
//             }
//         }
//     }
// }

void GameScene::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
     if ((_movement == 0) & !_deckNode->getDrag() & _act == 60){
          touchBegan(event.position);
     } else if (_movement == 5 && _currCardButton->containsScreen(event.position)){
          _prev = event.position;
     }
}

void GameScene::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
     if (_movement == 0){
          touchEnded(event.position);
     }
}

void GameScene::touchBegan(const cugl::Vec2& pos) {
     Card displayCard;
     if (_currCardButton->containsScreen(pos)) {
          //_deckNode->setOffset(Vec2(_dimen.width * WIDTH_SCALE - pos.x, _dimen.height * (0.33f + DECK_SCALE * _currentDeck.size()) + pos.y));
          Vec2 posi = _deckNode->screenToNodeCoords(pos);
          _deckNode->setOffset(Vec2(_dimen.width * WIDTH_SCALE - posi.x, _dimen.height * (HEIGHT_SCALE + DECK_SCALE * _currentDeck.size()) - posi.y));
          _deckNode->setDrag(true);
          if (AudioEngine::get()->getState("cardSound") != AudioEngine::State::PLAYING){
               AudioEngine::get()->play("cardSound", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
          }
          _deckNode->setCurrCardPos(posi);
     }
     else if (_response1->containsScreen(pos) & _display1) {
         displayCard = _cards[_responses[_responseId1].getCards()[0]];
          _resourceController.setDisplayCardBurnText(displayCard, _displayCardBurnText, _assets, _displayCardBurnTexture);
          if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
          } else {
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
          }
          std::vector<int> resources = _resources;
          for (int i = 0; i < _resources.size(); i++) {
               if (_responses[_responseId1].getResources()[i] > 0){
                    if (resources[i] - _responses[_responseId1].getResources()[i] >= 0){
                         resources[i] -= _responses[_responseId1].getResources()[i];
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                    } else {
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                    }
               }
          }
          _displayCard->setTexture(displayCard.getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response2->containsScreen(pos) & _display2) {
         displayCard = _cards[_responses[_responseId2].getCards()[0]];
          _resourceController.setDisplayCardBurnText(displayCard, _displayCardBurnText, _assets, _displayCardBurnTexture);
          if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
          } else {
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
          }
          std::vector<int> resources = _resources;
          for (int i = 0; i < _resources.size(); i++) {
               if (_responses[_responseId2].getResources()[i] > 0){
                    if (resources[i] - _responses[_responseId2].getResources()[i] >= 0){
                         resources[i] -= _responses[_responseId2].getResources()[i];
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                    } else {
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                    }
               }
          }
          _displayCard->setTexture(displayCard.getTexture());
          _displayCard->setVisible(true);
     }
     else if (_response3->containsScreen(pos) & _display3) {
         displayCard = _cards[_responses[_responseId3].getCards()[0]];
          _resourceController.setDisplayCardBurnText(displayCard, _displayCardBurnText, _assets, _displayCardBurnTexture);
          if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
          } else {
               _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
          }
          std::vector<int> resources = _resources;
          for (int i = 0; i < _resources.size(); i++) {
               if (_responses[_responseId3].getResources()[i] > 0){
                    if (resources[i] - _responses[_responseId3].getResources()[i] >= 0){
                         resources[i] -= _responses[_responseId3].getResources()[i];
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                    } else {
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                    }
               }
          }
          _displayCard->setTexture(displayCard.getTexture());
          _displayCard->setVisible(true);
     } else if (_mainMenu->containsScreen(pos)){
          if (_movement == 11) {
               this->_active = true;
          } else if (_movement == 12){
               _movement = 13;
          }
     } else {
          _displayCard->setVisible(false);
     }
}

void GameScene::touchEnded(const cugl::Vec2& pos) {
     _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
     _displayCard->setVisible(false);
     _removeCard1->setVisible(false);
     _removeCard2->setVisible(false);
     if (!_deckNode->getDrag()){
          if (_response1->containsScreen(pos) & _display1) {
               buttonPress(0);
          }
          else if (_response2->containsScreen(pos) & _display2) {
               buttonPress(1);
          }
          else if (_response3->containsScreen(pos) & _display3) {
               buttonPress(2);
          }
     } else {
          if (_burn->containsScreen(pos) & (_currentCard.getId() != 13 & _currentCard.getId() != 14 & _currentCard.getId() != 15) & !(_tutorial == true & _fight == 1)) {
               _currentBurn->setPosition(_deckNode->screenToNodeCoords(pos) + _deckNode->getOffset());
               string burnTexture = _currentCard.getText() + "Burn";
               _currentBurn->setTexture(_assets->get<Texture>(burnTexture));
               if (AudioEngine::get()->getState("crumpleSound") != AudioEngine::State::PLAYING){
                    AudioEngine::get()->play("crumpleSound", _assets->get<Sound>("crumpleSound"), false, _soundVolume, false);
               } else {
                    AudioEngine::get()->play("crumpleSound2", _assets->get<Sound>("crumpleSound"), false, _soundVolume, false);
               }
               buttonPress(-1);
          } else {
               if (AudioEngine::get()->getState("cardSound2") != AudioEngine::State::PLAYING){
                    AudioEngine::get()->play("cardSound2", _assets->get<Sound>("cardSound"), false, _soundVolume, false);
               }
          }
          _deckNode->setDrag(false);
     }
}

void GameScene::touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus){
     if (_movement == 0 || _movement == 5){
          touchMoved(event.position);
     }
}

void GameScene::touchMoved(const cugl::Vec2& pos){
    Card displayCard;
     if (_movement == 5){
          if (_prev.x - pos.x > SWIPE_LENGTH){
               _goonInt = 0;
               _burnInt = 0;
               _movement = 6;
               AudioEngine::get()->play("flipSound", _assets->get<Sound>("flipSound"), false, _soundVolume, false);
          }
     } else if (!_deckNode->getDrag() & (_currentCard.getId() == 13)){
          if (_response1->containsScreen(pos)) {
               _removeCard2->setTexture(_cards[_responses[_responseId1].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[0]].getTexture());
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          }
          else if (_response2->containsScreen(pos) & _display2) {
               _removeCard2->setTexture(_cards[_responses[_responseId2].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[1]].getTexture());
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          }
          else if (_response3->containsScreen(pos) & _display3) {
               _removeCard2->setTexture(_cards[_responses[_responseId3].getCards()[0]].getTexture());
               _removeCard1->setTexture(_cards[_removeOptions[2]].getTexture());
               _removeCard1->setVisible(true);
               _removeCard2->setVisible(true);
          } else {
               _displayCard->setVisible(false);
               _removeCard1->setVisible(false);
               _removeCard2->setVisible(false);
          }
     } else if (!_deckNode->getDrag()){
          if (_response1->containsScreen(pos)) {
              displayCard = _cards[_responses[_responseId1].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
               } else {
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId1].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId1].getResources()[i] >= 0){
                              resources[i] -= _responses[_responseId1].getResources()[i];
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response2->containsScreen(pos) & _display2) {
              displayCard = _cards[_responses[_responseId2].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
               } else {
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId2].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId2].getResources()[i] >= 0){
                              resources[i] -= _responses[_responseId2].getResources()[i];
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          }
          else if (_response3->containsScreen(pos) & _display3) {
              displayCard = _cards[_responses[_responseId3].getCards()[0]];
               setDisplayCardBurnText(_displayCardBurnTexture, _displayCardBurnText, displayCard);
               if (_fight == 5 & displayCard.getId() != 6 & displayCard.getId() != 7){
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,true);
               } else {
                    _resourceController.setDisplayCardResponseType(displayCard, _assets, _displayCardResponseType, _displayCardResponseBurn,false);
               }
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_responses[_responseId3].getResources()[i] > 0){
                         if (resources[i] - _responses[_responseId3].getResources()[i] >= 0){
                              resources[i] -= _responses[_responseId3].getResources()[i];
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                         } else {
                              _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, i + 3);
                         }
                    }
               }
               _displayCard->setTexture(displayCard.getTexture());
               _displayCard->setVisible(true);
          } else {
               _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
               _displayCard->setVisible(false);
          }
     } else {
          _deckNode->setCurrCardPos(_deckNode->screenToNodeCoords(pos));
          if (_burn->containsScreen(pos) & (_currentCard.getId() != 13 & _currentCard.getId() != 14 & _currentCard.getId() != 15) & !(_tutorial == true & _fight == 1))  {
               std::vector<int> resources = _resources;
               for (int i = 0; i < _resources.size(); i++) {
                    if (_currentCard.getResource(i) > 0){
                         resources[i] += _currentCard.getResource(i);
                         _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, resources, i);
                    }
               }
          } else {
               _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
          }
     }
}

Card GameScene::getItem(const int i){
     Card specialCard;
     switch (i){
          case 0:
               //Flourish Regen
               specialCard.allocate("Dance", -1, _assets->get<Texture>("danceOfSteel"),{}, {0,7,0,0}, 1, "none");
               return specialCard;
          case 1:
               //Lunge Regen
               specialCard.allocate("Sprint", -1, _assets->get<Texture>("sprint"),{}, {0,0,5,0}, 1, "none");
               return specialCard;
          case 2:
               //Second Wind
               specialCard.allocate("SecondWind", -1, _assets->get<Texture>("secondWind"),{45}, {0,0,0,0}, 2, "none");
               return specialCard;
          case 3:
               //Deck Boost
               specialCard.allocate("Hoarder", -1, _assets->get<Texture>("jackOfAllTrades"),{46}, {0,0,0,0}, 3, "none");
               return specialCard;
          case 4:
               //Parasite
               specialCard.allocate("Parasite", -1, _assets->get<Texture>("parasite"),{47}, {0,0,0,0}, 3, "none");
               return specialCard;
     }
     return specialCard;
}

void GameScene::removeCard(const int id){
     for (int i = 0; i < _currentDeck.size(); i++){
          if (_cards[_currentDeck[i]].getId() == id){
               _currentDeck.erase(_currentDeck.begin() + i);
               _deckNode->setSize(int(_currentDeck.size()));
               return;
          }
     }
     for (int i = 0; i < _nextDeck.size(); i++){
          if (_cards[_nextDeck[i]].getId() == id){
               _nextDeck.erase(_nextDeck.begin() + i);
               _deckNode->setNextSize(int(_nextDeck.size()));
               return;
          }
     }
     return;
}

void GameScene::gameOver(){
     _goon->setVisible(false);
     _deckNode->setVisible(false);
     _burnTexture->setVisible(false);
     _burnLabel->setVisible(false);
     _currEvent->setText("YOU DIED!");
     _currEvent->setVisible(true);
     
     _pause->setVisible(true);
     _soundSlider->setVisible(false);
     _autoFlipButton->setVisible(false);
     _musicSlider->setVisible(false);
     _soundSliderNode->setVisible(false);
     _musicSliderNode->setVisible(false);
     _paused->setVisible(false);
     
     _goonName->setVisible(false);
     _goonNumber->setVisible(false);
     
     _black->setVisible(true);
     _cardHolder->setVisible(false);
     
     if (filetool::file_exists(Application::get()->getSaveDirectory() + "savedGame.json") & !_tutorial){
#if defined (__WINDOWS__)
                string path = Application::get()->getSaveDirectory() + "savedGame.json";
                std::remove(path.c_str());
#else
                filetool::file_delete(Application::get()->getSaveDirectory() + "savedGame.json");
#endif
     }
     _movement = 11;
     return;
}

void GameScene::setProgressJson(){
     string sa = "false";
     if (_autoFlip){
          sa = "true";
     }
     string progress = "{\"Progress\":{\"HighestLevel\": " + to_string(_highestLevel) + "},\"Volume\":{\"Music\":" + to_string(_musicVolume) + ",\"Sound\":" + to_string(_soundVolume) +",\"Autoflip\":" + sa + "}}";
     std::shared_ptr<TextWriter> textWriter = TextWriter::alloc(Application::get()->getSaveDirectory() + "settings.json");
     textWriter->write(progress);
     textWriter->close();
}

void GameScene::setGameJson(bool startingDeck){
     if (_tutorial){
          return;
     }
     _saving->setVisible(true);
     string currentDeck = "[";
     for (int i = 0; i < _currentDeck.size(); i++){
          currentDeck += to_string(_currentDeck[i]);
          if (i < _currentDeck.size() - 1){
               currentDeck += ",";
          }
     }
     currentDeck += "]";
     string sDeck = "false";
     if (startingDeck){
          sDeck = "true";
     }
     string sWind = "false";
     if (_usedSecondWind){
          sWind = "true";
     }
     string resources = "[" + to_string(_resources[0]) + "," + to_string(_resources[1]) + "," + to_string(_resources[2]) + "," + to_string(_resources[3]) + "]";
     string gameSave = "{\"Fight\":"+ to_string(_fight)+",\"StartingDeck\":"+ sDeck+",\"CurrentDeck\":"+ currentDeck+",\"Item\":"+ to_string(_item)+",\"SecondWindUsed\":"+ sWind+",\"Resources\":" + resources + "}";
     std::shared_ptr<TextWriter> textWriter = TextWriter::alloc(Application::get()->getSaveDirectory() + "savedGame.json");
     textWriter->write(gameSave);
     textWriter->close();
}

void GameScene::resourceIncrease(){
     /*
     std::vector<int> flatIncrease = { 5, 5, 5, 5 };
     for (int i = 0; i < _currentDeck.size(); i++){
          for (int j = 0; j < 4; j++){
               flatIncrease[j] = flatIncrease[j] + _cards[_currentDeck[i]].getResource(j);
          }
     }
     for (int k = 0; k < _nextDeck.size(); k++){
          for (int m = 0; m < 4; m++){
               flatIncrease[m] = flatIncrease[m] + _cards[_nextDeck[k]].getResource(m);
          }
     }
     for (int n = 0; n < 4; n++){
          _resources[n] = _resources[n] + flatIncrease[n];
     }
     _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources);
      */
     std::vector<int> flatIncrease = { 5, 5, 5, 5 };
     for (int n = 0; n < 4; n++){
          _resources[n] = _resources[n] + flatIncrease[n];
     }
     _resourceController.setResources(_bladeText, _flourishText, _lungeText, _brawnText, _resources, -1);
}
