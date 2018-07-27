/**
 * jadaq (Just Another DAQ)
 * Copyright (C) 2018  Troels Blum <troels@blum.dk>
 *
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
 * @section DESCRIPTION
 * Write data to plain text file
 *
 */

#ifndef JADAQ_DATAHANDLERTEXT_HPP
#define JADAQ_DATAHANDLERTEXT_HPP

#include <fstream>
#include <iomanip>
#include <mutex>
#include <cassert>
#include "DataFormat.hpp"
#include "container.hpp"

class DataWriterText
{
private:
    std::fstream* file = nullptr;
    std::mutex mutex;

public:
  DataWriterText(std::string pathname, std::string runID)
    {
        if (!pathname.empty() && *pathname.rbegin() != '/')
          pathname += '/';
        std::string filename = pathname + "jadaq-run-" + runID + ".txt";
        file = new std::fstream(filename,std::fstream::out);
        if (!file->is_open())
        {
            throw std::runtime_error("Could not open text data file: \"" + filename + "\"");
        }
        *file << "# runID: " << runID << std::endl;
    }

    ~DataWriterText()
    {
        assert(file);
        mutex.lock(); // Wait if someone is still writing data
        file->close();
        mutex.unlock();
    }

    void addDigitizer(uint64_t digitizerID)
    {
        mutex.lock();
        *file << "# digitizerID: " << digitizerID << std::endl;
        mutex.unlock();
    }

    static bool network() { return false; }

    template <typename E>
    void operator()(const jadaq::buffer<E>* buffer, uint64_t digitizerID, uint64_t globalTimeStamp)
    {
        mutex.lock();
        *file << "#" << PRINTH(digitizerID) << " ";
        E::headerOn(*file);
        *file << std::endl << "@" << globalTimeStamp << std::endl;
        for(const E& element: *buffer)
        {
            *file << " " << PRINTD(digitizerID) << " " << element << "\n";
        }
        mutex.unlock();
    }
};

#endif //JADAQ_DATAHANDLERTEXT_HPP
