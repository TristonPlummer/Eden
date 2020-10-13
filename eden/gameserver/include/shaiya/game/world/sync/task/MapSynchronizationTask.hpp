#pragma once
#include <shaiya/game/world/model/actor/character/Character.hpp>
#include <shaiya/game/world/model/item/GroundItem.hpp>

namespace shaiya::game
{
    /**
     * Handles the synchronization of the map for the current character. This includes displaying and removing ground items,
     * as well as graphic effects.
     */
    class MapSynchronizationTask
    {
    public:
        /**
         * Initialise the synchronization task.
         * @param character The character we're currently synchronizing.
         */
        explicit MapSynchronizationTask(Character& character);

        /**
         * Synchronizes the character.
         */
        void sync();

        /**
         * Adds an item to the current character's viewport.
         * @param groundItem The item to add.
         */
        void addItem(const GroundItem& groundItem);

        /**
         * Removes an item from the current character's viewport.
         * @param item The item to remove.
         */
        void removeItem(const GroundItem& item);

    private:
        /**
         * The character we're currently synchronizing.
         */
        Character& character_;
    };
}