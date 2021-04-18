//
// ItemScene.h
//
// This module provides the item selection interface, to be shown to 
// players at the start of a run. 
//
// Created by Iain Pile on 4/16/21
// Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
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
	/** The button to equip/unequip the shown item */
	std::shared_ptr<cugl::scene2::Button> _equip;
	std::shared_ptr<cugl::scene2::Label> _equipLabel;
	/** The button to undisplay the shown item */
	std::shared_ptr<cugl::scene2::Button> _back;
	/** The buttons for each item */
	std::shared_ptr<cugl::scene2::Button> _items[6];

	/** The selected item (or lock image, if card not acquired */
	std::shared_ptr<cugl::scene2::NinePatch> _displayedItem;

	/** The selection image for an unacquired item */
	std::shared_ptr<cugl::scene2::NinePatch> _lockedItemDisplay;

	// MODEL
	/** The item the player currently has selected */
	int _equippedItem;
	/** The item the player currently has displayed */
	int _displayedItemId;
	/** Array indicating which items have been found */
	bool _itemAcquired[6];

	/** 
	 * Value indicating whether to continue to the game or
	 * return to the menu
	 */
	bool _continue;
	/** Value checked in update to undisplay card. */
	bool _undisplay;

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
	 * Display the information associated with the selected item.
	 * @param id        The index of the item
	 */
	void displayItem(int id);

	/** 
	 * Remove the information associated with the selected item.
	 */
	void undisplayItem();

	/**
	 * Equip the item corresponding to the currently selected item, 
	 * or unequip it if it is already equipped
	 */
	void equipItem();
};
#endif /* __ITEM_SCENE_H__ */
