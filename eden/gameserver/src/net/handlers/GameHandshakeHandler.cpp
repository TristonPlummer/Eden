#include <shaiya/common/net/packet/PacketRegistry.hpp>
#include <shaiya/game/net/GameSession.hpp>

#include <crypto++/osrng.h>

using namespace shaiya::net;

/**
 * Handles an incoming game handshake request.
 * @param session   The session instance.
 * @param request   The inbound handshake request.
 */
void handleHandshake(Session& session, const GameHandshakeRequest& request)
{
    auto& game     = dynamic_cast<GameSession&>(session);  // The game session
    auto& api      = game.context().getApiService();
    auto& identity = request.identity;

    // Attempt to find the transfer request for a provided identity.
    auto transfer = api.getTransferForIdentity(identity);
    if (!transfer)
    {
        LOG(INFO) << "Couldn't find transfer request for user id: " << request.userId;
        game.close();
        return;
    }

    // If the transfer doesn't originate from the same ip address, disconnect the session.
    if (game.remoteAddress() != transfer->ipaddress())
    {
        LOG(INFO) << "Transfer request was found for user id: " << request.userId << ", but request address "
                  << game.remoteAddress() << " did not match transfer origin address " << transfer->ipaddress();
        game.close();
        return;
    }

    // Respond with the expanded key
    GameHandshakeResponse response;

    // Generate the expanded key to use for game world encryption and provide it to the client
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock((byte*) response.expandedKeySeed.data(), response.expandedKeySeed.size());
    game.write(response);

    // The aes key and iv
    std::array<byte, 16> key{ 0 };
    std::array<byte, 16> iv{ 0 };

    // Populate the AES values
    std::memcpy(key.data(), transfer->key().data(), key.size());
    std::memcpy(iv.data(), transfer->iv().data(), iv.size());

    // Initialise the encryption based on the previous AES keys.
    game.initEncryption(key, iv);

    // Show the character selection screen
    game.showCharacterScreen();
}

/**
 * A template specialization used for registering a game handshake request handler.
 */
template<>
void PacketRegistry::registerPacketHandler<GameHandshakeOpcode>()
{
    registerHandler<GameHandshakeOpcode, GameHandshakeRequest>(&handleHandshake, ExecutionType::Asynchronous);
}