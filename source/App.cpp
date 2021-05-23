//
//  GLApp.cpp
//  Programming Lab
//
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  Author: Walker White
//  Version: 2/21/21
//
#include "App.h"

using namespace cugl;

#pragma mark -
#pragma mark Gameplay Control

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void LabApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input
#ifdef CU_MOBILE
    Input::activate<Touchscreen>();
    Input::activate<Accelerometer>();
#else
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
    Input::activate<Keyboard>();
    Input::activate<TextInput>();
#endif

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    /*
    if (filetool::file_exists(Application::getSaveDirectory() + "savedGame.json")){
         filetool::file_delete(Application::getSaveDirectory() + "savedGame.json");
    }
     */
    // Create a "loading" screen
    _itemChosen = false;
    _loaded = false;
    _loading.init(_assets);
    
    // Queue up the other assets
    ratio = Application::getDisplayWidth()/(double)Application::getDisplayHeight();
    // CULog(to_string(ratio).c_str());
    if(ratio <= 1.5) {
        _assets->loadDirectoryAsync("json/assets-ipad.json",nullptr);
    } else {
        _assets->loadDirectoryAsync("json/assets.json",nullptr);
    }
    AudioEngine::start();
    if (!filetool::file_exists(Application::getSaveDirectory() + "settings.json")){
        std::shared_ptr<TextWriter> textWriter = TextWriter::alloc(Application::getSaveDirectory() + "settings.json");
        textWriter->write("{\"Progress\":{\"HighestLevel\": 0},\"Volume\":{\"Music\":0.5,\"Sound\":0.5, \"Autoflip\":false}}");
        textWriter->close();
    }
    //filetool::file_delete(Application::get()->getSaveDirectory() + "settings.json");
    //filetool::file_delete(Application::get()->getSaveDirectory() + "savedGame.json");
    /*
    ifstream ifile;
    //cout<<Application::getSaveDirectory();
    ifile.open(Application::getSaveDirectory() + "progress.json");
    if (!ifile) {
        ofstream progress(Application::getSaveDirectory() + "progress.json");
        progress << "{\"Progress\":{\"HighestLevel\": 0}}";
        progress.close();
    } else {
        ifile.close();
    }*/
    Application::onStartup(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void LabApp::onShutdown() {
    if (_loading.isActive()) {
        _loading.dispose();
    }
    else if (_item.isActive()) {
        _item.dispose();
    } 
    else if (_gameplay.isActive()) {
        _gameplay.dispose();
    }
    
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
    Input::deactivate<Accelerometer>();
#else
    Input::deactivate<Mouse>();
    Input::deactivate<Keyboard>();
#endif

    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void LabApp::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void LabApp::onResume() {
    AudioEngine::get()->resume();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LabApp::update(float timestep) {
    if (!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _mainGame = _loading.goToMainGame();
        _skip = _loading.goToBrawler();
        _continueGame = _loading.continueMainGame();
        _loading.dispose(); // Disables the input listeners in this mode
        if (_mainGame || _skip ) {
            _item.init(_assets);
        } else if (_continueGame){
            _itemChosen = true;
            _gameplay.init(_assets, -1, ratio, false, true);
        } else {
            _itemChosen = true;
            _gameplay.init(_assets, -1, ratio, true, false);
        }
        _loaded = true;
    }
    else if (!_itemChosen && _item.isActive()) {
        _item.update(timestep);
    }
    else if (!_itemChosen) {
        if (_item.getContinue()) {
            _equippedItem = _item.getItem();
            _item.dispose();
            if (_mainGame) {
                if (filetool::file_exists(Application::getSaveDirectory() + "savedGame.json")) {
#if defined (__WINDOWS__)
                    string path = Application::getSaveDirectory() + "savedGame.json";
                    std::remove(path.c_str());
#else
                    filetool::file_delete(Application::getSaveDirectory() + "savedGame.json");
#endif
                }
                _gameplay.init(_assets, _equippedItem, ratio, false, false);
                _itemChosen = true;
            }
            else if (_skip) {
                string deck = "[ 0, 0, 0, 3, 3 ]";
                string resources = "[ 20, 20, 20, 20 ]";
                string gameSave = "{\"Fight\":" + to_string(5) + ",\"StartingDeck\":" + "true" + ",\"CurrentDeck\":" + deck + ",\"Item\":" + to_string(_equippedItem) + ",\"SecondWindUsed\":" + "false" + ",\"Resources\":" + resources + "}";
                std::shared_ptr<TextWriter> textWriter = TextWriter::alloc(Application::get()->getSaveDirectory() + "savedGame.json");
                textWriter->write(gameSave);
                textWriter->close();
                _gameplay.init(_assets, -1, ratio, false, true);
                _itemChosen = true;
            }
        }
        else {
            _item.dispose();
            _loading.init(_assets);
            _loaded = false;
        }
    } else if (_gameplay.isActive()) {
        _gameplay.update(timestep);
    }
    else {
        _gameplay.dispose(); 
        _loading.init(_assets);
        _loaded = false;
        _itemChosen = false;
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void LabApp::draw() {
    if (!_loaded & !_itemChosen) {
        _loading.render(_batch);
    }
    else if (_loaded & (!_itemChosen)) {
        _item.render(_batch);
    } else {
        _gameplay.render(_batch);
    }
}


