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
    _menuLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_menu_up_label"));

    _menu->addListener([=](const std::string& name, bool down) {
        if (!down) {
            if (!_display) {
                this->_active = down;
                _continue = false;
            }
            else {
                undisplayItem();
            }
        }
        });
    _menu->setVisible(true);
    _menu->activate();
    _displayItem = std::make_shared<scene2::NinePatch>();
    _lockedItemTexture = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_selected-card-locked"));
    _equip = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_equip"));
    _equip->addListener([=](const std::string& name, bool down) {
        if (!down) {
            if (_displayedItemId != _equippedItem) {
                equipItem();
            }
            else {
                unequipItem();
            }
        }
        });
    _equip->setVisible(false);
    _equipLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_equip_up_label"));
    _lockedItemTexture->setVisible(false);
    _displayText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text"));
    _displayText->setVisible(false);

    // Initializing the buttons
    // TODO: Check save file to determine which should be locked/unlocked
    _items[0] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item0-locked"));
    _items[1] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item1-locked"));
    _items[2] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item2-locked"));
    _items[3] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item3-locked"));
    _items[4] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item4-locked"));
    _items[5] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item5-locked"));
    for (int i = 0; i < 5; i++) {
        _itemAcquired[i] = true; // will replace once we can actually check this
        _items[i]->addListener([=](const std::string& name, bool down) {
            if (!down) {
                if (_displayedItemId != i) {
                    _displayedItemId = i;
                    displayItem(i);
                }
            }
            });
        _items[i]->setVisible(true);
        _items[i]->activate();
    }
    _currText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_equipped-item-text"));
    _currText->setText("Equipped: None");

    // Initialize Models
    _equippedItem = -1; // start with no item selected
    _displayedItemId = -1;
    _display = false;
    _equippedText[0] = "Equipped: Flourish Regen";
    _itemNames[0] = "Flourish Regen";
    _equippedText[1] = "Equipped: Lunge Regen";
    _itemNames[1] = "Lunge Regen";
    _equippedText[2] = "Equipped: Second Wind";
    _itemNames[2] = "Second Wind";
    _equippedText[3] = "Equipped: Deck Boost";
    _itemNames[3] = "Deck Boost";
    _equippedText[4] = "Equipped: Parasite";
    _itemNames[4] = "Parasite";
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ItemScene::dispose() {
    removeAllChildren();
    for (int i = 0; i < 5; i++) {
        _items[i]->clearListeners();
        _items[i] = nullptr;
    }
    _play->clearListeners();
    _menu->clearListeners();
    _equip->clearListeners();
    _play = nullptr;
    _menu = nullptr;
    _equip = nullptr;
    _equippedItem = -1;
    _displayedItemId = -1;
    _continue = false;
}

#pragma mark -
#pragma mark Scene Handling
/**
 * The method called to update the game mode.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ItemScene::update(float timestep) {}


/**
 * Display the information associated with the selected item.
 * @param id        The index of the item
 * @param acquired  Whether or not the card has been unlocked
 */
void ItemScene::displayItem(int id) {
    //_menuLabel->setText("Back");
    if (!_itemAcquired[id]) {
        _displayItem = _lockedItemTexture;
        _equipLabel->setText("Locked");
    }
    else if (_equippedItem != id) {
        _displayItem = _lockedItemTexture;        
        _equipLabel->setText("Equip");
        }
    else {
        _displayItem = _lockedItemTexture;
        _equipLabel->setText("Unequip");
    }
    _display = true;
    _equip->setVisible(true);
    _equip->activate();
    _menuLabel->setText("Back");
    _displayItem->setVisible(true);
    _displayText->setText(_itemNames[id]);
    _displayText->setVisible(true);
}

void ItemScene::undisplayItem() {
    _display = false;
    _equip->deactivate();
    _equip->setVisible(false);
    _displayItem->setVisible(false);
    _displayText->setVisible(false);
    _menuLabel->setText("Menu");
}

/**
 * Equip the item corresponding to the currently selected item
 */
void ItemScene::equipItem() {
    if (_itemAcquired[_displayedItemId]) {
        _equippedItem = _displayedItemId;
        _currText->setText(_equippedText[_displayedItemId]);
        _equipLabel->setText("Unequip");
    }
}

/**
     * Remove the item the currently equipped item
     */
void ItemScene::unequipItem() {
    _equippedItem = -1;
    _currText->setText("Equipped: None");
    _equipLabel->setText("Equip");
}