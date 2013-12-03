#include "tinything/TinyThingReader.hh"
#include "TinyThingWriter.hh"
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>

namespace LibTinyThing {

/**
 * @brief A simple class that makes sure miniunzip handles get closed
 * when they go out of scope. This base holds an arbitrary handle type
 * and a function or function object to close it on destruction
 */
template <typename VALUE_TYPE, typename CLOSE_FUNC>
class basic_unz_holder {
public:
    typedef VALUE_TYPE value_type;
    typedef CLOSE_FUNC close_func;
    typedef basic_unz_holder<value_type, close_func> basic_unz_holder_;
    explicit basic_unz_holder(
            const value_type& vt = nullptr, 
            const close_func& cf = close_func())
            : m_value(vt), m_closer(cf) {}
    basic_unz_holder(basic_unz_holder_&& other)
            : m_value(std::move(other.m_value)), 
            m_closer(std::move(other.m_closer)) {
        other.m_value = nullptr;
    }
    ~basic_unz_holder() {
        m_closer(m_value);
    }
    basic_unz_holder_& operator =(basic_unz_holder_&& rhs) {
        m_closer(m_value);
        m_value = std::move(rhs.m_value);
        m_closer = std::move(rhs.m_closer);
        rhs.m_value = nullptr;
        return *this;
    }
    basic_unz_holder(const basic_unz_holder_&) = delete;
    basic_unz_holder& operator =(const basic_unz_holder_&) = delete;
    value_type& get() { return m_value; }
    const value_type& get() const { return m_value; }
private:
    value_type m_value;
    close_func m_closer;
};

void safelyClose(unzFile f) {
    if(nullptr != f) {
        unzClose(f);
    }
}

void safelyCloseCurrent(unzFile f) {
    if(nullptr != f) {
        unzCloseCurrentFile(f);
    }
}

template <typename VALUE_TYPE, typename CLOSE_FUNC>
basic_unz_holder<VALUE_TYPE, CLOSE_FUNC> make_unz_holder(
        const VALUE_TYPE& vt, const CLOSE_FUNC& cf) {
    return basic_unz_holder<VALUE_TYPE, CLOSE_FUNC>(vt, cf);
}

class TinyThingReader::Private {
 public:
  Private(const std::string& filePath)
      : m_filePath(filePath),
        m_toolpathFile(NULL),
        m_toolpathSize(0),
        m_toolpathPos(0) {
  }

  bool unzipFile(
      const std::string& fileName,
      std::string &output) {
      auto tinyThingFileHolder = make_unz_holder(unzOpen(m_filePath.c_str()),
              &safelyClose);

    if (tinyThingFileHolder.get() != NULL   &&
        unzLocateFile(tinyThingFileHolder.get(), 
            fileName.c_str(), 0) == UNZ_OK &&
        unzOpenCurrentFile(tinyThingFileHolder.get()) == UNZ_OK) {
        auto currentThingHolder = make_unz_holder(tinyThingFileHolder.get(),
                &safelyCloseCurrent);
      unz_file_info fileInfo;

      if (unzGetCurrentFileInfo(tinyThingFileHolder.get(), 
              &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {

        // Create a buffer large enough to hold the uncompressed file
        output.resize(fileInfo.uncompressed_size);

        if (unzReadCurrentFile(tinyThingFileHolder.get(),
                               const_cast<char*>(output.c_str()),
                               fileInfo.uncompressed_size)){
          return true;
        }
      }
    }
    return false;

  }

  const std::string m_filePath;

  std::string m_toolpathFileContents;
  std::string m_thumbnailFileContents;
  std::string m_metadataFileContents;

  // variables to support incremental toolpath unzipping
  unzFile m_toolpathFile;
  int m_toolpathSize;
  int m_toolpathPos;
};

TinyThingReader::TinyThingReader(const std::string& filePath)
    : m_private(new Private(filePath)) {
}

    TinyThingReader::~TinyThingReader() {
	    if (m_private->m_toolpathFile != NULL) unzClose(m_private->m_toolpathFile);
    }

	bool TinyThingReader::unzipMetadataFile() {
		return m_private->unzipFile(TinyThingWriter::METADATA_FILENAME, m_private->m_metadataFileContents);
	}

	bool TinyThingReader::unzipThumbnailFile() {
		return m_private->unzipFile(TinyThingWriter::THUMBNAIL_FILENAME, m_private->m_thumbnailFileContents);
	}

	bool TinyThingReader::unzipToolpathFile() {
		return m_private->unzipFile(TinyThingWriter::TOOLPATH_FILENAME, m_private->m_toolpathFileContents);
	}

	std::string TinyThingReader::getMetadataFileContents(){
		return m_private->m_metadataFileContents;
	}
	std::string TinyThingReader::getThumbnailFileContents(){
		return m_private->m_thumbnailFileContents;
	}
	std::string TinyThingReader::getToolpathFileContents(){
		return m_private->m_toolpathFileContents;
	}

	bool TinyThingReader::hasJsonToolpath(){
		auto tinyThingFileHolder = make_unz_holder(
                unzOpen(m_private->m_filePath.c_str()), &safelyClose);
		bool isValid = (tinyThingFileHolder.get() != NULL &&
				unzLocateFile(tinyThingFileHolder.get(), 
                TinyThingWriter::TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK);
        if(isValid) {
            auto currentFileHolder = make_unz_holder(
                    tinyThingFileHolder.get(), &safelyCloseCurrent);
            isValid = unzOpenCurrentFile(tinyThingFileHolder.get()) == UNZ_OK;
        }
		return isValid;
	}


	bool TinyThingReader::isValid(){
        auto tinyThingFileHolder = make_unz_holder(
                unzOpen(m_private->m_filePath.c_str()), &safelyClose);
		bool isValid = (tinyThingFileHolder.get() != NULL &&
				unzLocateFile(tinyThingFileHolder.get(), 
                TinyThingWriter::METADATA_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFileHolder.get(),
                TinyThingWriter::THUMBNAIL_FILENAME.c_str(), 0) == UNZ_OK &&
				unzLocateFile(tinyThingFileHolder.get(),
                TinyThingWriter::TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK);
        if(isValid) {
            auto currentFileHolder = make_unz_holder(
                    tinyThingFileHolder.get(), &safelyCloseCurrent);
            isValid = unzOpenCurrentFile(tinyThingFileHolder.get()) == UNZ_OK;
        }

		return isValid;
	}

    // returns true if succesful
    bool TinyThingReader::resetToolpath(){

        m_private->m_toolpathPos = 0;

        if (m_private->m_toolpathFile != NULL) unzClose(m_private->m_toolpathFile);

        m_private->m_toolpathFile = unzOpen(m_private->m_filePath.c_str());

		if (m_private->m_toolpathFile != NULL   &&
			unzLocateFile(m_private->m_toolpathFile, 
            TinyThingWriter::TOOLPATH_FILENAME.c_str(), 0) == UNZ_OK ){
            auto currentFileHolder = make_unz_holder(
                    m_private->m_toolpathFile, &safelyCloseCurrent);
            if(unzOpenCurrentFile(m_private->m_toolpathFile) == UNZ_OK) {
                unz_file_info fileInfo;
                if (unzGetCurrentFileInfo(m_private->m_toolpathFile, 
                        &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
                    m_private->m_toolpathSize = fileInfo.uncompressed_size;
                    return true;
                }
            }
        }
        return false;        
    }

    // if length of return string is < bytes, you have reached end of file
    std::string TinyThingReader::getToolpathIncr(const int chars) {

        if (m_private->m_toolpathFile == NULL) resetToolpath();

        int chars_to_read = chars;
        if (m_private->m_toolpathPos >= m_private->m_toolpathSize) {
            // at end of file
            return "";
        } else if ((m_private->m_toolpathPos + chars) > m_private->m_toolpathSize) {
            // reached end of file
            chars_to_read = m_private->m_toolpathSize - m_private->m_toolpathPos;
        }

        m_private->m_toolpathPos += chars_to_read;

        std::string output;
		output.resize(chars_to_read);

		unzReadCurrentFile(m_private->m_toolpathFile,
			const_cast<char*>(output.c_str()),
			chars_to_read);
        return output;
	}




}
