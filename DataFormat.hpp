/**
 * jadaq (Just Another DAQ)
 * Copyright (C) 2018  Troels Blum <troels@blum.dk>
 *
 * @file
 * @author Troels Blum <troels@blum.dk>
 * @section LICENSE
 * This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 *         but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef JADAQ_DATAFORMAT_HPP
#define JADAQ_DATAFORMAT_HPP

#include <chrono>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <H5Cpp.h>
#include <iomanip>

#define VERSION {1,0}

/* TODO: Take care of endianness: https://linux.die.net/man/3/endian
 * We will use little endian for our network protocol since odds
 * are that both ends will be running on intel hardware
 */
namespace Data
{
    const uint16_t currentVersion = *(uint16_t*)(uint8_t[])VERSION;
    enum ElementType: uint16_t
    {
        None,
        List422,
        Waweform
    };
    /* Shared meta data for the entire data package */
    struct Header // 32 bytes
    {
        uint64_t runID;
        uint64_t globalTime;
        uint32_t digitizerID;
        uint16_t elementType;
        uint16_t numElements;
        uint16_t version;
        uint8_t __pad[6];
    };
    struct ListElement422
    {
        uint32_t localTime;
        uint16_t adcValue;
        uint16_t channel;
        bool operator< (const ListElement422& rhs) const
        {
            return localTime < rhs.localTime || (localTime == rhs.localTime && channel < rhs.channel) ;
        };
        void printOn(std::ostream& os) const
        {
            os << std::setw(10) << channel << " " << std::setw(10) << localTime << " " << std::setw(10) << adcValue;
        }
        static ElementType type() { return List422; }
        static H5::CompType h5type()
        {
            H5::CompType datatype(sizeof(ListElement422));
            datatype.insertMember("localTime", HOFFSET(ListElement422, localTime), H5::PredType::NATIVE_UINT32);
            datatype.insertMember("adcValue", HOFFSET(ListElement422, adcValue), H5::PredType::NATIVE_UINT16);
            datatype.insertMember("channel", HOFFSET(ListElement422, channel), H5::PredType::NATIVE_UINT16);
            return datatype;
        }
    };
    static inline size_t elementSize(ElementType elementType)
    {
        switch (elementType)
        {
            case None:
                return 0;
            case List422:
                return sizeof(ListElement422);
            default:
                throw std::runtime_error("Unknown element type." );
        }
    }

    static constexpr const char* defaultDataPort = "12345";
    static constexpr const size_t maxBufferSize = 9000;

} // namespace Data

//TODO: template for all Data::Element types
static inline std::ostream& operator<< (std::ostream& os, const Data::ListElement422& e)
{ e.printOn(os); return os; }
#endif //JADAQ_DATAFORMAT_HPP
