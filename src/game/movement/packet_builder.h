/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2025 MaNGOS <https://www.getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#ifndef MANGOSSERVER_PACKET_BUILDER_H
#define MANGOSSERVER_PACKET_BUILDER_H

class ByteBuffer;
class WorldPacket;

namespace Movement
{
    class MoveSpline;

    /**
     * @brief The PacketBuilder class is responsible for constructing movement-related packets.
     */
    class PacketBuilder
    {
            /**
             * @brief Writes the common part of a monster move packet.
             * @param mov The MoveSpline object containing movement data.
             * @param data The WorldPacket to write the data to.
             */
            static void WriteCommonMonsterMovePart(const MoveSpline& mov, WorldPacket& data);
        public:

            /**
             * @brief Writes a monster move packet.
             * @param mov The MoveSpline object containing movement data.
             * @param data The WorldPacket to write the data to.
             */
            static void WriteMonsterMove(const MoveSpline& mov, WorldPacket& data);

            /**
             * @brief Writes the creation data of a MoveSpline to a ByteBuffer.
             * @param mov The MoveSpline object containing movement data.
             * @param data The ByteBuffer to write the data to.
             */
            static void WriteCreateBits(const MoveSpline& mov, ByteBuffer& data);
            static void WriteCreateBytes(const MoveSpline& mov, ByteBuffer& data);
    };
}
#endif // MANGOSSERVER_PACKET_BUILDER_H
