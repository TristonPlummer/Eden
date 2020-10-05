#include <shaiya/common/net/packet/PacketRegistry.hpp>
#include <shaiya/game/net/GameSession.hpp>

using namespace shaiya::net;

/**
 * Handles an incoming loaded viewport signal.
 * @param session   The session instance.
 * @param request   The inbound loaded viewport signal.
 */
void handleViewportLoaded(Session& session, const ViewportLoadedNotification& request)
{
    auto& game     = dynamic_cast<GameSession&>(session);
    auto character = game.character();

    if (character)
        character->activate();
}

/**
 * A template specialization used for registering a viewport loaded handler.
 */
template<>
void PacketRegistry::registerPacketHandler<CharacterEnteredViewportOpcode>()
{
    registerHandler<CharacterEnteredViewportOpcode, ViewportLoadedNotification>(&handleViewportLoaded);
}