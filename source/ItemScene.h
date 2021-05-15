//
// ItemScene.h
//
// This module provides the item selection interface, to be shown to 
// players at the start of a run. 
//
// Created by Iain Pile on 4/16/21
// Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//
#ifndef __ITEM_SCENE_H__
#define __ITEM_SCENE_H__
#include <cugl/cugl.h>

/** 
 * This class provides the user interface for selecting an item before 
 * entering the gamescene. 
 */
class ItemScene : public cugl::Scene2 {
protected:
	/** The asset manager for loading. */
	std::shared_ptr<cugl::AssetManager> _assets;

	// NO CONTROLLER (all in separate thread)

	// VIEW
	/** The button to proceed to the main game */
	std::shared_ptr<cugl::scene2::Button> _play;
	/** The button to return to the main menu */
	std::shared_ptr<cugl::scene2::Button> _menu;
	std::shared_ptr<cugl::scene2::Label> _menuLabel;
	/** The equip button */
	std::shared_ptr<cugl::scene2::Button> _equip;
	std::shared_ptr<cugl::scene2::Label> _equipLabel;
	/** The buttons for each item icon */
	std::shared_ptr<cugl::scene2::Button> _unlockedItems[5];
    std::shared_ptr<cugl::scene2::Button> _lockedItems[5];

	/** Text indicating the currently equipped item */
	std::shared_ptr<cugl::scene2::Label> _currText;
	/** Text indicating the currently displayed item */
    std::shared_ptr<cugl::scene2::Label> _displayText;
	std::shared_ptr<cugl::scene2::Label> _displayText1;
    std::shared_ptr<cugl::scene2::Label> _displayText2;
    std::shared_ptr<cugl::scene2::Label> _displayText3;
    std::shared_ptr<cugl::scene2::Label> _displayText4;
    std::shared_ptr<cugl::scene2::Label> _displayText5;
    std::shared_ptr<cugl::scene2::NinePatch> _flourishBurn;
    std::shared_ptr<cugl::scene2::NinePatch> _lungeBurn;
    std::shared_ptr<cugl::scene2::SceneNode> _parasiteBurn;
	/** The currently selected item card */
	std::shared_ptr<cugl::scene2::NinePatch> _displayItem;

	/** The selection button for an unacquired item */
	std::shared_ptr<cugl::scene2::NinePatch> _lockedItemTexture;

	/** item textures */
	std::shared_ptr<cugl::scene2::NinePatch> _itemTextures[5];

	/** Selection buttons for acquired items */
	std::shared_ptr<cugl::scene2::Button> _itemButtons[5];

	// MODEL
	/** The item the player currently has selected */
	int _equippedItem;
	/** The item the player currently has displayed */
	int _displayedItemId;
	/** Array indicating which items have been found */
    bool _itemAcquired[5] = {false, false, false, false,false};
	/** Text for each item */
	string _equippedText[5];
	string _itemNames[5];
    
    int _highestLevel;

	/** 
	 * Value indicating whether to continue to the game or
	 * return to the menu
	 */
	bool _continue;
	/** Value indicating state of the scene (if display is on or off) */
	bool _display;

public:
#pragma mark -
#pragma mark Constructors
	/**
	 * Creates a new item mode with default values.
	 * 
	 * This constructor does not allocate any objects or start the game.
	 * This allows us to use the object without a heap pointer.
	 */
	ItemScene() : cugl::Scene2(), _equippedItem(-1), _continue(false), _displayedItemId(-1) {}

	/**
	 * Disposes of all (non-static) resources allocated to this mode.
	 * 
	 * This method is different from dispose() in that it ALSO shuts off any 
	 * static resources, like the input controller.
	 */
	~ItemScene() { dispose(); }

	/** 
	 * Disposes of all (non-static) resources allocated to this mode. 
	 */
	void dispose();

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
	bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Scene Handling
	/**
	 * The method called to update the game mode.
	 *
	 * @param timestep  The amount of time (in seconds) since the last frame
	 */
	void update(float timestep) override;

	/** 
	 * Returns the current value of the _continue field.
	 */
	bool getContinue() { return _continue; }

	/**
	 * Returns the current value of the _equippedItem field. 
	 */
	int getItem() { return _equippedItem; }

	/**
	 * Display the information associated with the selected item.
	 * @param id        The index of the item
	 */
	void displayItem(int id);

	/**
	 * Undisplay the currently displayed information.
	 */
	void undisplayItem();

	/**
	 * Equip the item corresponding to the currently selected item
	 */
	void equipItem();

	/**
	 * Remove the item the currently equipped item
	 */
	void unequipItem();
};
#endif /* __ITEM_SCENE_H__ */
