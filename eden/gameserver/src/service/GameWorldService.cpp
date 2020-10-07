#include <shaiya/common/util/Async.hpp>
#include <shaiya/game/io/impl/DatabaseCharacterSerializer.hpp>
#include <shaiya/game/net/GameSession.hpp>

#include <chrono>

using namespace shaiya::game;

/**
 * Initialise this game world service.
 * @param db            The database service.
 * @param itemService   The item definition service.
 * @param worldId       The id of this world service.
 */
GameWorldService::GameWorldService(shaiya::database::DatabaseService& db, ItemDefinitionService& itemService, size_t worldId)
    : db_(db)
{
    synchronizer_        = std::make_unique<ParallelClientSynchronizer>();
    characterSerializer_ = std::make_unique<DatabaseCharacterSerializer>(db, itemService, worldId);
}

/**
 * Loads the game world service.
 * @param config    The configuration instance.
 */
void GameWorldService::load(boost::property_tree::ptree& config)
{
    mapRepository_.load(config.get<std::string>("World.MapFilePath"));  // Load the game's maps.
}

/**
 * Handles the main tick of the world.
 * @param tickRate  The tick frequency
 */
void GameWorldService::tick(size_t tickRate)
{
    using namespace std::chrono;

    // While this service is running
    while (running_)
    {
        // The time we should sleep until, for the next tick
        auto nextTick = steady_clock::now() + milliseconds(tickRate);

        // Finalise the registration and unregistrations for characters
        finaliseRegistrations();
        finaliseUnregistrations();

        // Process all the queued incoming packets
        for (auto&& character: characters_)
            character->session().processQueue();

        // Pulse the game world
        scheduler_.pulse();

        // Synchronize the characters with the world state
        synchronizer_->synchronize(characters_);

        // Sleep until the next tick
        std::this_thread::sleep_until(nextTick);
    }
}

/**
 * Handles the registration of a character to this game world.
 * @param character The character to register.
 */
void GameWorldService::registerCharacter(std::shared_ptr<Character> character)
{
    // Lock the mutex
    std::lock_guard lock{ mutex_ };

    // Add the character to the queue of characters that need to be registered
    newCharacters_.push(std::move(character));
}

/**
 * Removes a character from this game world.
 * @param character The character to remove.
 */
void GameWorldService::unregisterCharacter(std::shared_ptr<Character> character)
{
    // Lock the mutex
    std::lock_guard lock{ mutex_ };

    // Adds the character to the queue of characters that need to be unregistered
    oldCharacters_.push(std::move(character));
}

/**
 * Finalises the registration of characters that are queued to be registered.
 */
void GameWorldService::finaliseRegistrations()
{
    // Lock the mutex
    std::lock_guard lock{ mutex_ };

    // Process the registrations
    while (!newCharacters_.empty())
    {
        auto character = newCharacters_.front();
        newCharacters_.pop();
        characters_.push_back(character);

        auto load = [&, character]() {
            characterSerializer_->load(*character);
            character->init();
        };
        ASYNC(load)
    }
}

/**
 * Finalises the unregistration of characters that are queued to be unregistered.
 */
void GameWorldService::finaliseUnregistrations()
{
    // Lock the mutex
    std::lock_guard lock{ mutex_ };

    // Process the unregistrations
    while (!oldCharacters_.empty())
    {
        // Deactivate the character
        auto character = oldCharacters_.front();
        oldCharacters_.pop();
        character->deactivate();

        // Remove the character from their map
        auto map = mapRepository_.forId(character->position().map());
        map->remove(character);

        auto finalise = [&, character = std::move(character)]() {
            characterSerializer_->save(*character);

            // Find the character
            auto predicate = [&](auto& element) { return element.get() == character.get(); };
            auto pos       = std::find_if(characters_.begin(), characters_.end(), predicate);

            // Remove the character
            if (pos != characters_.end())
                characters_.erase(pos);
        };
        ASYNC(finalise)
    }
}

/**
 * Schedules a task to be executed in the future.
 * @param task  The task.
 */
void GameWorldService::schedule(std::shared_ptr<ScheduledTask> task)
{
    scheduler_.schedule(std::move(task));
}