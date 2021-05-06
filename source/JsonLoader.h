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
#ifndef __JSON_LOADER_H__
#define __JSON_LOADER_H__
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "Card.h"
#include <vector>
#include <map>
#include "Response.h"
#include "EnemyFight.h"
using namespace cugl;
/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
namespace JsonLoader{
    /** The asset manager for this game mode. */

    /**
     * Activates the UI elements to make them interactive
     *
     * The elements do not actually do anything.  They just visually respond
     * to input.
     */

    //NEED TO PROPERLY FREE MEMORY LATER
    // for (int i = 0; i < cardsJson->size(); i++){
    //       std::shared_ptr<JsonValue> jsonCard = cardsJson->get(i);
    //       int id = jsonCard->get("id")->asInt();
    //       string name = jsonCard->get("name")->asString();
    //       string tex = jsonCard->get("texture")->asString();
    //       auto texture = _assets->get<Texture>(tex);
    //       std::vector<int> responses = jsonCard->get("reactions")->asIntArray();
    //       std::vector<int> resources = jsonCard->get("resources")->asIntArray();
    //       int level = jsonCard->get("level")->asInt();
    //       Card card;
    //       card.allocate(name, id, texture, responses, resources, level);
    //       _cards[id] = card;
    //  }

    //TODO: PROPERLY FREE MEMORY LATER
    static std::shared_ptr<JsonValue> getJsonItem(std::shared_ptr<JsonReader> jsonReader, string type) {
        std::shared_ptr<JsonValue> jsonObject = jsonReader->readJson()->get(type);
        return jsonObject;
    }
    // convert to private class
    static std::map<int, Card> getJsonCards(std::shared_ptr<JsonReader> jsonReader, std::map<int, Card> cards, std::shared_ptr<cugl::AssetManager> assets) {
        std::shared_ptr<JsonValue> cardsJson = getJsonItem(jsonReader, "Cards");
        for (int i = 0; i < cardsJson->size(); i++){
          std::shared_ptr<JsonValue> jsonCard = cardsJson->get(i);
          int id = jsonCard->get("id")->asInt();
          string name = jsonCard->get("name")->asString();
          string tex = jsonCard->get("texture")->asString();
          auto texture = assets->get<Texture>(tex);
          std::vector<int> responses = jsonCard->get("reactions")->asIntArray();
          std::vector<int> resources = jsonCard->get("resources")->asIntArray();
          int level = jsonCard->get("level")->asInt();
          Card card;
          card.allocate(name, id, texture, responses, resources, level);
          cards[id] = card;
        }
        return cards;
    }

    static std::map<int, Response> getJsonResponses(std::shared_ptr<JsonReader> jsonReader, std::map<int, Response> responses) {
        std::shared_ptr<JsonValue> responsesJson = getJsonItem(jsonReader, "Responses");
        for (int i = 0; i < responsesJson->size(); i++) {
            std::shared_ptr<JsonValue> jsonResponse = responsesJson->get(i);
            int id = jsonResponse->get("id")->asInt();
            string name = jsonResponse->get("name")->asString();
            string description = jsonResponse->get("description")->asString();
            std::vector<int> cost = jsonResponse->get("cost")->asIntArray();
            std::vector<int> addToDeck = jsonResponse->get("addToDeck")->asIntArray();
            bool win = jsonResponse->get("win")->asBool();
            bool lose = jsonResponse->get("lose")->asBool();
            float fontSize = jsonResponse->get("fontSize")->asFloat();
            int animation = jsonResponse->get("animation")->asInt();
            Response response;
            response.allocate(name, description, cost, addToDeck, win, lose, fontSize, animation);
            responses[id] = response;
            //CULog("resp");
        }
        return responses;
    }

    static std::map<int, EnemyFight> getJsonEnemyFights(std::shared_ptr<JsonReader> jsonReader, std::map<int, EnemyFight>& enemyFights) {
        //std::cout << "asdf";
        std::shared_ptr<JsonValue> fightsJson = getJsonItem(jsonReader, "Fights");
        for (int i = 0; i < fightsJson->size(); i++) {
            std::shared_ptr<JsonValue> jsonItem = fightsJson->get(i);
            string enemyName = jsonItem->get("enemyName")->asString();
            string enemyTexture = jsonItem->get("enemyTexture")->asString();
            std::vector<int> deck = jsonItem->get("deck")->asIntArray();
            std::vector<int> nextDeck = jsonItem->get("nextDeck")->asIntArray();
            int rows = jsonItem->get("rows")->asInt();
            int cols = jsonItem->get("cols")->asInt();
            int frames = jsonItem->get("frames")->asInt();
            float wscale = jsonItem->get("wscale")->asFloat();
            float hscale = jsonItem->get("hscale")->asFloat();
            float scale = jsonItem->get("scale")->asFloat();
            int id = jsonItem->get("id")->asInt();
            EnemyFight enemyFight;
            enemyFight.allocate(enemyName, deck, nextDeck, enemyTexture, rows, cols, frames, wscale, hscale, id, scale);
            // index at 1
            enemyFights[id] = enemyFight;
        }
        return enemyFights;
    }
};

#endif /* __JSON_LOADER_H__ */
