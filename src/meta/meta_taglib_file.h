/*
 * meta_taglib_file.h - File Stream used for file reading with taglib
 *
 * Copyright (c) 2014   A. Dilly
 *
 * AirCat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * AirCat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AirCat.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef META_TAGLIB_FILE_H
#define META_TAGLIB_FILE_H

#include <taglib/taglib.h>

#if (TAGLIB_MAJOR_VERSION >= 1) &&  (TAGLIB_MINOR_VERSION >= 9)

#include <taglib/tiostream.h>

extern "C" {
	#include "fs.h"
}

class MetaTaglibFile : public TagLib::IOStream
{
  public:
    /*!
     * Construct a File object and opens the \a file.  \a file should be a
     * be a C-string in the local file system encoding.
     */
    MetaTaglibFile(const std::string& openFileName, bool openReadOnly = true);

    /*!
     * Destroys this FileStream instance.
     */
    virtual ~MetaTaglibFile();

    /*!
     * Returns the file name in the local file system encoding.
     */
    TagLib::FileName name() const;

    /*!
     * Reads a block of size \a length at the current get pointer.
     */
    TagLib::ByteVector readBlock(TagLib::ulong length);

    /*!
     * Attempts to write the block \a data at the current get pointer.  If the
     * file is currently only opened read only -- i.e. readOnly() returns true --
     * this attempts to reopen the file in read/write mode.
     *
     * \note This should be used instead of using the streaming output operator
     * for a ByteVector.  And even this function is significantly slower than
     * doing output with a char[].
     */
    void writeBlock(const TagLib::ByteVector &data);

    /*!
     * Insert \a data at position \a start in the file overwriting \a replace
     * bytes of the original content.
     *
     * \note This method is slow since it requires rewriting all of the file
     * after the insertion point.
     */
    void insert(const TagLib::ByteVector &data, TagLib::ulong start = 0,
                TagLib::ulong replace = 0);

    /*!
     * Removes a block of the file starting a \a start and continuing for
     * \a length bytes.
     *
     * \note This method is slow since it involves rewriting all of the file
     * after the removed portion.
     */
    void removeBlock(TagLib::ulong start = 0, TagLib::ulong length = 0);

    /*!
     * Returns true if the file is read only (or if the file can not be opened).
     */
    bool readOnly() const;

    /*!
     * Since the file can currently only be opened as an argument to the
     * constructor (sort-of by design), this returns if that open succeeded.
     */
    bool isOpen() const;

    /*!
     * Move the I/O pointer to \a offset in the file from position \a p.  This
     * defaults to seeking from the beginning of the file.
     *
     * \see Position
     */
    void seek(long offset, TagLib::IOStream::Position p = Beginning);

    /*!
     * Reset the end-of-file and error flags on the file.
     */
    void clear();

    /*!
     * Returns the current offset within the file.
     */
    long tell() const;

    /*!
     * Returns the length of the file.
     */
    long length();

    /*!
     * Truncates the file to a \a length.
     */
    void truncate(long length);

  protected:
    /*!
     * Returns the buffer size that is used for internal buffering.
     */
    static TagLib::uint bufferSize();

  private:
    struct fs_file *file;
    std::string fileName;
    bool isReadOnly;
};

#endif

#endif

