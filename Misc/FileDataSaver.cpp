//
//    AsyncDataSaver.h: save high bandwidth data to a file
//    Copyright (C) 2019 Gonzalo José Carracedo Carballal
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this program.  If not, see
//    <http://www.gnu.org/licenses/>
//

#include "FileDataSaver.h"
#include <unistd.h>

using namespace SigDigger;

namespace SigDigger {
  class FileDataWriter : public GenericDataWriter {
    struct suscan_sink *sink;
    std::string lastError;

  public:
    FileDataWriter(struct suscan_sink *sink);

    bool prepare(void);
    bool canWrite(void) const;
    std::string getError(void) const;
    ssize_t write(const SUCOMPLEX *data, size_t len);
    ssize_t write_blob(const void *data, size_t len);
    bool close(void);
    ~FileDataWriter();
  };
}

std::string
FileDataWriter::getError(void) const
{
  return this->lastError;
}

bool
FileDataWriter::prepare(void)
{
  return true;
}

FileDataWriter::FileDataWriter(struct suscan_sink *sink)
{
  this->sink = sink;
}

bool
FileDataWriter::canWrite(void) const
{
  return this->sink != NULL;
}

ssize_t
FileDataWriter::write(const SUCOMPLEX *data, size_t len)
{
  ssize_t result;

  if (!this->sink)
    return 0;

  result = suscan_sink_write(this->sink, data, len);

  if (result < 1)
    lastError = "write() failed: " + std::string(strerror(errno));

  return result;
}

bool
FileDataWriter::close(void)
{
  bool ok = true;

  if (this->sink) {
    suscan_sink_close(this->sink);
    this->sink = nullptr;
  }

  return ok;
}

FileDataWriter::~FileDataWriter(void)
{
  this->close();
}

//////////////////////////// FileDataSaver /////////////////////////////////////
FileDataSaver::FileDataSaver(struct suscan_sink *sink, QObject *parent) :
  GenericDataSaver(this->writer = new FileDataWriter(sink), parent)
{
}

FileDataSaver::~FileDataSaver(void)
{
  if (this->writer != nullptr)
    delete this->writer;
}

