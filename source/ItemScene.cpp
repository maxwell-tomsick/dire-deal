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
    _itemTextures[0] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_item0"));
    _itemTextures[1] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_item1"));
    _itemTextures[2] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_item2"));
    _itemTextures[3] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_item3"));
    _itemTextures[4] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("item_item4"));
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
    for( int i = 0; i < 5; i++ ){
        _itemTextures[i]->setVisible(false);
    }
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
    std::shared_ptr<JsonReader> jsonReaderHighestLevel = JsonReader::alloc(Application::get()->getSaveDirectory() + "settings.json");
    std::shared_ptr<JsonValue> progress = jsonReaderHighestLevel->readJson()->get("Progress");
    jsonReaderHighestLevel->close();
    _highestLevel = progress->get("HighestLevel")->asInt();
    /*
    if (_highestLevel >= 2) {
        _itemAcquired[0] = true;
        if (_highestLevel >= 4) {
            _itemAcquired[1] = true;
            if (_highestLevel >= 5) {
                _itemAcquired[2] = true;
                if (_highestLevel >= 6) {
                    _itemAcquired[3] = true;
                    if (_highestLevel >= 7) {
                        _itemAcquired[4] = true;
                    }
                }
            }
        }
    }
     */
    if (_highestLevel >= 0) {
        _itemAcquired[0] = true;
        if (_highestLevel >= 0) {
            _itemAcquired[1] = true;
            if (_highestLevel >= 0) {
                _itemAcquired[2] = true;
                if (_highestLevel >= 0) {
                    _itemAcquired[3] = true;
                    if (_highestLevel >= 0) {
                        _itemAcquired[4] = true;
                    }
                }
            }
        }
    }
    

    _unlockedItems[0] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item0-unlocked"));
    _unlockedItems[1] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item1-unlocked"));
    _unlockedItems[2] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item2-unlocked"));
    _unlockedItems[3] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item3-unlocked"));
    _unlockedItems[4] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item4-unlocked"));
    
    _lockedItems[0] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item0-locked"));
    _lockedItems[1] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item1-locked"));
    _lockedItems[2] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item2-locked"));
    _lockedItems[3] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item3-locked"));
    _lockedItems[4] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_items-locked_item4-locked"));
    //_items[5] = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("item_item5-locked"));

    // Textures of items
    for (int i = 0; i < 5; i++) {
        if (_itemAcquired[i] == true) {
            _unlockedItems[i]->addListener([=](const std::string& name, bool down) {
                if (!down) {
                    if (_displayedItemId != i) {
                        _displayedItemId = i;
                        undisplayItem();
                        displayItem(i);
                    }
                }
                });
            _unlockedItems[i]->setVisible(true);
            _unlockedItems[i]->activate();
        } else {
            _lockedItems[i]->addListener([=](const std::string& name, bool down) {
                if (!down) {
                    if (_displayedItemId != i) {
                        _displayedItemId = i;
                        undisplayItem();
                        displayItem(-1);
                    }
                }
                });
            _lockedItems[i]->setVisible(true);
            _lockedItems[i]->activate();
        }
    }
    _currText = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("item_equipped-item-text"));
    _currText->setText("Equipped: None");

    // Initialize Models
    _equippedItem = -1; // start with no item selected
    _displayedItemId = -1;
    _display = false;
    _equippedText[0] = "Equipped: Dance of Steel";
    _itemNames[0] = "Dance of Steel";
    _equippedText[1] = "Equipped: Sprint";
    _itemNames[1] = "Sprint";
    _equippedText[2] = "Equipped: Second Wind";
    _itemNames[2] = "Second Wind";
    _equippedText[3] = "Equipped: Hoarder";
    _itemNames[3] = "Hoarder";
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
        _unlockedItems[i]->clearListeners();
        _unlockedItems[i] = nullptr;
        _lockedItems[i]->clearListeners();
        _lockedItems[i] = nullptr;
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
    // CULog(to_string(id).c_str());
    if (!_itemAcquired[id]) {
        _displayItem = _lockedItemTexture;
        _equipLabel->setText("Locked");
    }
    else if (_equippedItem != id) {
        _displayItem = _itemTextures[id];
        _equipLabel->setText("Equip");
        _displayItem->setVisible(true);
        }
    else {
        _displayItem = _itemTextures[id];
        _equipLabel->setText("Unequip");
        _displayItem->setVisible(true);
    }
    _display = true;
    _equip->setVisible(true);
    _equip->activate();
    _menuLabel->setText("Hide");
    
    //_displayItem->setVisible(true);
    _displayText->setText(_itemNames[id]);
    _displayText->setVisible(false);
    if (id == 0){
        _displayText1->setText("No Responses. Burns for");
        _displayText1->setVisible(true);
        _displayText2->setText("Appears once per fight.");
        _displayText2->setVisible(true);
        _displayText3->setVisible(false);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(true);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 1) {
        _displayText1->setText("No Responses. Burns for");
        _displayText1->setVisible(true);
        _displayText2->setText("Appears once per fight.");
        _displayText2->setVisible(true);
        _displayText3->setVisible(false);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(true);
        _parasiteBurn->setVisible(false);
    } else if (id == 2) {
        _displayText1->setText("When this is the last card in the deck,");
        _displayText1->setVisible(true);
        _displayText2->setText("this card becomes a copy of the last card burned.");
        _displayText2->setVisible(true);
        _displayText3->setText("Appears once per hunt.");
        _displayText3->setVisible(true);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 3) {
        _displayText1->setText("Burns with a random resource type for");
        _displayText1->setVisible(true);
        _displayText2->setText("double the total number of cards in");
        _displayText2->setVisible(true);
        _displayText3->setText("the deck. Appears once per fight.");
        _displayText3->setVisible(true);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
    } else if (id == 4) {
        _displayText1->setText("Costs               to keep in the deck. When the");
        _displayText1->setVisible(true);
        _displayText2->setText("deck is shuffled, if this card is present, 1 random");
        _displayText2->setVisible(true);
        _displayText3->setText("response will be free. Appears once per fight.");
        _displayText3->setVisible(true);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(true);
    } else if (id == -1){
        _displayText1->setText("Defeat targets to unlock this item!");
        _displayText1->setVisible(true);
        _displayText2->setVisible(false);
        _displayText3->setVisible(false);
        _displayText4->setVisible(false);
        _displayText5->setVisible(false);
        _flourishBurn->setVisible(false);
        _lungeBurn->setVisible(false);
        _parasiteBurn->setVisible(false);
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
