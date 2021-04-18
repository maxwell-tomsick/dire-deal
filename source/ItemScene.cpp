//
// ItemScene.cpp
//
// This module provides the item selection interface, to be shown to 
// players at the start of a run. 
//
// Created by Iain Pile on 4/16/21
// Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//
#include "ItemScene.h"

using namespace cugl;

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
bool ItemScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE / dimen.width;
    }
    else {
        dimen *= SCENE_SIZE / dimen.height;
    }
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    _assets->loadDirectory("json/item.json");
    auto background = _assets->get<scene2::SceneNode>("item");
    background->setContentSize(dimen);
    background->doLayout(); // This rearranges the children to fit the screen
    addChild(background);
    
    // Initialize scene elements
    _play = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_play"));
    _play->addListener([=](const std::string& name, bool down) {
        this->_active = down;
        _continue = true;
        });
    _play->setVisible(true);
    _play->activate();
    _menu = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_menu"));
    _menu->addListener([=](const std::string& name, bool down) {
        this->_active = down;
        _continue = false;
        });
    _menu->setVisible(true);
    _menu->activate();
    _back = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_back"));
    _back->addListener([=](const std::string& name, bool down) {
        _undisplay = true;
        });
    _equip = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_equip"));
    _equipLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_equip_up_label"));
    _equip->addListener([=](const std::string& name, bool down) {
        equipItem();
        });
    _lockedItemDisplay = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_selected-card-locked"));
    _displayedItem = std::make_shared<scene2::NinePatch>();
    _displayedItem->setVisible(false);

    // Initializing the buttons
    // TODO: Check save file to determine which should be locked/unlocked
    _items[0] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item0-locked"));
    _items[1] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item1-locked"));
    _items[2] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item2-locked"));
    _items[3] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item3-locked"));
    _items[4] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item4-locked"));
    _items[5] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item5-locked"));
    for (int i = 0; i < 6; i++) {
        _itemAcquired[i] = false; // will replace once we can actually check this
        _items[i]->addListener([=](const std::string& name, bool down) {
            _displayedItemId = i;
            displayItem(i);
            });
        _items[i]->setVisible(true);
        _items[i]->activate();
    }

    // Initialize Models
    _equippedItem = -1; // start with no item selected
    _displayedItemId = -1;
    _undisplay = false;
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ItemScene::dispose() {
    removeAllChildren();
    _equippedItem = 0;
    _continue = false;
}

#pragma mark -
#pragma mark Scene Handling
/**
 * The method called to update the game mode.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ItemScene::update(float timestep) {
    if (_undisplay) {   // there has to be a better way to do this
        undisplayItem();
        _undisplay = false;
    }
}


/**
 * Display the information associated with the selected item.
 * @param id        The index of the item
 * @param acquired  Whether or not the card has been unlocked
 */
void ItemScene::displayItem(int id) {
    if (_menu->isActive()) {
        _menu->setVisible(false);
        _menu->deactivate();
        _play->setVisible(false);
        _play->deactivate();
        _back->setVisible(true);
        _back->activate();
        _equip->setVisible(true);
        _equip->activate();
    }
    if (!_itemAcquired[id]) {
        _displayedItem = _lockedItemDisplay;
        _displayedItem->setVisible(true);
        _equipLabel->setText("Locked");
    }
    else if (_displayedItemId == _equippedItem) {
        _equipLabel->setText("Unequip");
    }
    // code to set item 
}

void ItemScene::equipItem() {
    if (_itemAcquired[_displayedItemId]) {
        _equippedItem = _displayedItemId;
    }
}

/**
 * Remove the information associated with the selected item.
 */
void ItemScene::undisplayItem() {
    _displayedItem->setVisible(false);
    _back->setVisible(false);
    _back->deactivate();
    _equip->setVisible(false);
    _equip->deactivate();
    _menu->setVisible(true);
    _menu->activate();
    _play->setVisible(true);
    _play->activate();
}