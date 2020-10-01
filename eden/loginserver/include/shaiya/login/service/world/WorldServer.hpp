#pragma once

#include <array>
#include <string>

namespace shaiya::login
{
    /**
     * Represents an external world server.
     */
    class WorldServer
    {
    public:
        /**
         * Initialises a representation of a remote world server.
         * @param id                The id of the server.
         * @param name              The name of the server.
         * @param ipAddress         The ip address of the server.
         * @param revision          The client revision to accept.
         * @param playerCapacity    The maximum capacity of online players.
         */
        WorldServer(uint8_t id, std::string name, std::string ipAddress, uint32_t revision, uint16_t playerCapacity);

        /**
         * Queries the remote world server, and updates our internal data.
         */
        void update();

        /**
         * If this world server is online.
         * @return  If the server is online.
         */
        [[nodiscard]] bool isOnline() const
        {
            return online_;
        }

        /**
         * If the world is full with players, and no longer accepting new players.
         * @return  If the world is full.
         */
        [[nodiscard]] bool isFull() const
        {
            return playerCount_ >= playerCapacity_;
        }

        /**
         * Gets the world's id.
         * @return  The id.
         */
        [[nodiscard]] uint8_t getId() const
        {
            return id_;
        }

        /**
         * Gets the number of players that are currently online.
         * @return  The number of players.
         */
        [[nodiscard]] uint16_t playerCount() const
        {
            return playerCount_;
        }

        /**
         * Gets the maximum number of players this server will hold.
         * @return  The capacity of online players.
         */
        [[nodiscard]] uint16_t playerCapacity() const
        {
            return playerCapacity_;
        }

        /**
         * Gets the client revision that this server will accept.
         * @return  The client revision.
         */
        [[nodiscard]] uint32_t revision() const
        {
            return revision_;
        }

        /**
         * Gets the name of this world server.
         * @return  The name.
         */
        [[nodiscard]] const std::string& getName() const
        {
            return name_;
        }

        [[nodiscard]] const std::array<char, 4>& ipAddress() const
        {
            return ipAddressBytes_;
        }

    private:
        /**
         * The id of this server.
         */
        uint8_t id_{ 0 };

        /**
         * The number of currently online players.
         */
        uint16_t playerCount_{ 0 };

        /**
         * The maximum number of players that can be connected to this world.
         */
        uint16_t playerCapacity_{ 0 };

        /**
         * The client revision to accept
         */
        uint32_t revision_{ 0 };

        /**
         * If this world server is online.
         */
        bool online_{ false };

        /**
         * The name of the server.
         */
        std::string name_{};

        /**
         * The ip address of the world server.
         */
        std::string ipAddress_{};

        /**
         * The IPV4 address split into the 4 digits.
         */
        std::array<char, 4> ipAddressBytes_{ 0 };
    };
}