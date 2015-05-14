
#ifndef TINYTHINGREADER_HH_
#define TINYTHINGREADER_HH_

#include <memory>
#include <string>

namespace bwcoreutils {
    enum class TOOL;
}

namespace LibTinyThing {
    // Struct that contains the contents of a metadata file in
    // a version-independent way
    struct TINYTHING_API Metadata {
        float extrusion_mass_g;
        float extrusion_distance_mm;
        int extruder_temperature;
        int chamber_temperature;
        int shells;
        float layer_height;
        float infill_density;
        bool uses_support;
        float duration_s;
        float max_flow_rate;
        uint32_t thing_id;
        bool uses_raft;
        char uuid[100];
        std::string material;
        std::string slicer_name;
    };
    // Struct containing all information required to verify whether
    // the TinyThing has been sliced for a given printer
    
    struct TINYTHING_API VerificationData {
        bwcoreutils::TOOL tool_id;
        uint8_t pid;
    };

    // Struct containing only that data that doesn't absolutely suck
    // to export through a C interface
    struct TINYTHING_API CInterfaceMetadata {
        float extrusion_mass_g;
        float extrusion_distance_mm;
        int extruder_temperature;
        int chamber_temperature;
        uint32_t thing_id;
        float duration_s;
        bool uses_raft;
        char uuid[100];
    };
    
    class TINYTHING_API TinyThingReader {
    public:
        enum Error {
            kOK=0,
            kNotYetUnzipped=1,
            kToolMismatch=2,
            kBotTypeMismatch=3,
            kVersionMismatch=4,
            kMaxStringLengthExceeded=5
        };
        
        // passing in a fd will use an already opened file handle
        TinyThingReader(const std::string& filePath, int fd = -1);
        ~TinyThingReader();

        // these functions unzip the contents of each of these files,
        // and cache them in memory. they return true if the unzip is
        // succesful
        bool unzipMetadataFile();
        bool unzipSmallThumbnailFile();
        bool unzipMediumThumbnailFile();
        bool unzipLargeThumbnailFile();
        bool unzipToolpathFile();

        // these are accessors for the unzipped contents of each of the
        // files. they should only be called after each has been
        // unzipped.
        void getSmallThumbnailFileContents(std::string* contents) const;
        void getMediumThumbnailFileContents(std::string* contents) const;
        void getLargeThumbnailFileContents(std::string* contents) const;
        void getToolpathFileContents(std::string* contents) const;

        // checks to see if toolpath is present
        bool hasJsonToolpath() const;

        // checks to see if all files are present
        bool isValid() const;

        // check to see if the metadata file is acceptable given
        // a config (defined above)
        Error doesMetadataFileMatchConfig(const VerificationData& config) const;
        Error getMetadata(Metadata* out) const;
        Error getMetadata(CInterfaceMetadata* out) const;

        // incremental unzipping of toolpath
        // unzipping any other part of the file
        // or checking if it is valid will reset
        // the incremental pointer
        bool resetToolpath();
        std::string getToolpathIncr(const int chars);

    private:
        class Private;
        std::unique_ptr<Private> m_private;
    };

}


#endif /* TINYTHINGREADER_HH_ */
