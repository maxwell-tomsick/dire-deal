//
// ItemScene.cpp
//
// This module provides the item selection interface, to be shown to 
// players at the start of a run. 
//
// Created by Iain Pile on 4/16/21
// Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
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
                _displayedItemId = -1;
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
    _displayText1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text-line1"));
    _displayText2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text-line2"));
    _displayText3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text-line3"));
    _displayText4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text-line4"));
    _displayText5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_displayed-item-text-line5"));
    _flourishBurn = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_flourish"));
    _lungeBurn = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_lunge"));
    _parasiteBurn = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("item_ParasiteNode"));
    _displayText->setVisible(false);
    _displayText1->setVisible(false);
    _displayText2->setVisible(false);
    _displayText3->setVisible(false);
    _displayText4->setVisible(false);
    _displayText5->setVisible(false);
    _flourishBurn->setVisible(false);
    _lungeBurn->setVisible(false);
    _parasiteBurn->setVisible(false);

    // Initializing the buttons
    // TODO: Check save file to determine which should be locked/unlocked
    _items[0] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item0-locked"));
    _items[1] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item1-locked"));
    _items[2] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item2-locked"));
    _items[3] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item3-locked"));
    _items[4] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item4-locked"));
    //_items[5] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item5-locked"));
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
    if (id == 0){
        _displayText1->setText("No Responses");
        _displayText1->setVisible(true);
        _displayText2->setText("Burns for");
        _displayText2->setVisible(true);
        _displayText3->setText("Appears once per fight");
        _displayText3->setVisible(true);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(true);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 1) {
        _displayText1->setText("No Responses");
        _displayText1->setVisible(true);
        _displayText2->setText("Burns for");
        _displayText2->setVisible(true);
        _displayText3->setText("Appears once per fight");
        _displayText3->setVisible(true);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(true);
        _parasiteBurn->setVisible(false);
    } else if (id == 2) {
        _displayText1->setText("When this is the last card");
        _displayText1->setVisible(true);
        _displayText2->setText("in the deck, becomes");
        _displayText2->setVisible(true);
        _displayText3->setText("a copy of the last card");
        _displayText3->setVisible(true);
        _displayText4->setText("burned");
        _displayText4->setVisible(true);
        _displayText5->setText("Appears once per hunt");
        _displayText5->setVisible(true);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 3) {
        _displayText1->setText("Burns with a random");
        _displayText1->setVisible(true);
        _displayText2->setText("resource type for the");
        _displayText2->setVisible(true);
        _displayText3->setText("total number of cards");
        _displayText3->setVisible(true);
        _displayText4->setText("Appears once per fight");
        _displayText4->setVisible(true);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 4) {
        _displayText1->setText("Costs");
        _displayText1->setVisible(true);
        _displayText2->setText("to keep in the deck. While");
        _displayText2->setVisible(true);
        _displayText3->setText("in the deck, 1 random");
        _displayText3->setVisible(true);
        _displayText4->setText("response will be free");
        _displayText4->setVisible(true);
        _displayText5->setText("Appears once per fight");
        _displayText5->setVisible(true);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(true);
    } else {
        _displayText1->setVisible(false);
        _displayText2->setVisible(false);
        _displayText3->setVisible(false);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    }
}

void ItemScene::undisplayItem() {
    _display = false;
    _equip->deactivate();
    _equip->setVisible(false);
    _displayItem->setVisible(false);
    _displayText->setVisible(false);
    _displayText1->setVisible(false);
    _displayText2->setVisible(false);
    _displayText3->setVisible(false);
    _displayText4->setVisible(false);
    _displayText5->setVisible(false);
    _flourishBurn->setVisible(false);
    _parasiteBurn->setVisible(false);
    _lungeBurn->setVisible(false);
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
