//
//  GLLoadingScene.cpp
//  Programming Lab
//
//  This module provides a very barebones loading screen.  Most of the time you
//  will not need a loading screen, because the assets will load so fast.  But
//  just in case, this is a simple example you can use in your games.
//
//  We know from 3152 that you all like to customize this screen.  Therefore,
//  we have kept it as simple as possible so that it is easy to modify. In
//  fact, this loading screen uses the new modular JSON format for defining
//  scenes.  See the file "loading.json" for how to change this scene.
//
//  Author: Walker White
//  Version: 2/21/21
//
#include "LoadingScene.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the controller contents, making it ready for loading
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool LoadingScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _continuable = filetool::file_exists(Application::get()->getSaveDirectory() + "savedGame.json");
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _logo1 = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("load_logo1"));
    _logo2 = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("load_logo2"));
    _logo2->setVisible(false);
    _studio1 = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("load_studio1"));
    _studio2 = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("load_studio2"));
    _studio2->setVisible(false);
    _play = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_play"));
    _playLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("load_play_up_label"));
    _continue = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_continue"));
    _continueLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("load_continue_up_label"));
    _continue->setVisible(false);
    _play->setVisible(false);
    _playLabel->setText("Play");
    _continue->addListener([=](const std::string& name, bool down) {
        _continueGame = true;
        this->_active = down;
    });
    _continueGame = false;
    _play->addListener([=](const std::string& name, bool down) {
        _mainGame = true;
        this->_active = down;
    });
    _tutorial = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_tutorial"));
    _tutorialLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("load_tutorial_up_label"));
    _tutorial->addListener([=](const std::string& name, bool down) {
        _mainGame = false;
        this->_active = down;
        });
    _mainGame = false;
    
    Application::get()->setClearColor(Color4(192,192,192,255));
    addChild(layer);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadingScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _play->deactivate();
        _continue->deactivate();
        _tutorial->deactivate();
    }
    removeAllChildren();
    _play->clearListeners();
    _continue->clearListeners();
    _continue = nullptr;
    _tutorial->clearListeners();
    _play = nullptr;
    _tutorial = nullptr;
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoadingScene::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _bar->setVisible(false);
            _studio1->setVisible(false);
            _studio2->setVisible(true);
            _logo1->setVisible(false);
            _logo2->setVisible(true);
            _play->setVisible(true);
            if (_continuable){
                _playLabel->setText("Restart");
                _continue->setVisible(true);
                _continue->activate();
            } 
            _play->activate();
            _tutorial->setVisible(true);
            _tutorial->activate();
        }
        _bar->setProgress(_progress);
    }
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool LoadingScene::isPending( ) const {
    return _play != nullptr && _play->isVisible();
}

