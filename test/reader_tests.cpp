// Copyright MakerBot Inc 2017
#include <boost/test/unit_test.hpp>

#include <tinything/TinyThingReader.hh>
#include <bwcoreutils/tool_mappings.hh>

#include <array>
#include <iostream>  // NOLINT(readability/streams)
#include <string>

BOOST_AUTO_TEST_CASE(no_crash) {
    // Check that in the case of an invalid JSON metafile we don’t crash
    std::array<std::string, 4> corrupts{{"empty_metadata.makerbot",
                "lists_missing.makerbot",
                "lists_wrong_types.makerbot",
                "miracle_config_wrong_type.makerbot"}};
    for (const auto& filename : corrupts) {
        std::cout << "Checking " << filename << std::endl;
        LibTinyThing::TinyThingReader reader(filename);
        BOOST_CHECK(reader.unzipMetadataFile());
        LibTinyThing::Metadata m;
        BOOST_CHECK_EQUAL(reader.getMetadata(&m),
                          LibTinyThing::TinyThingReader::Error::kOK);
        LibTinyThing::CInterfaceMetadata m2;
        BOOST_CHECK_EQUAL(reader.getMetadata(&m2),
                          LibTinyThing::TinyThingReader::Error::kOK);
        LibTinyThing::VerificationData vd;
        vd.pid = 0xb;
        vd.tool_count = 1;
        vd.tool_id[0] = bwcoreutils::TOOL::INVALID_ID;
        BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                          LibTinyThing::TinyThingReader::Error::kOK);
    }
}

BOOST_AUTO_TEST_CASE(future) {
    // Check that a metadata file from the future gives an error any time
    // you try to access metadata
    LibTinyThing::TinyThingReader reader("future_version.makerbot");
    BOOST_REQUIRE(reader.unzipMetadataFile());
    LibTinyThing::Metadata m;
    BOOST_CHECK_EQUAL(reader.getMetadata(&m),
                      LibTinyThing::TinyThingReader::Error::kVersionMismatch);
    LibTinyThing::CInterfaceMetadata m2;
    BOOST_CHECK_EQUAL(reader.getMetadata(&m2),
                      LibTinyThing::TinyThingReader::Error::kVersionMismatch);
    LibTinyThing::VerificationData vd;
    BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                      LibTinyThing::TinyThingReader::Error::kVersionMismatch);
}

BOOST_AUTO_TEST_CASE(corrupt) {
    // Check that a metadata file that isn’t valid json will give appropriate
    // errors
    LibTinyThing::TinyThingReader reader("corrupt.makerbot");
    BOOST_REQUIRE(!reader.unzipMetadataFile());
    LibTinyThing::Metadata m;
    BOOST_CHECK_EQUAL(reader.getMetadata(&m),
                      LibTinyThing::TinyThingReader::Error::kNotYetUnzipped);
    LibTinyThing::CInterfaceMetadata m2;
    BOOST_CHECK_EQUAL(reader.getMetadata(&m2),
                      LibTinyThing::TinyThingReader::Error::kNotYetUnzipped);
    LibTinyThing::VerificationData vd;
    BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                      LibTinyThing::TinyThingReader::Error::kNotYetUnzipped);
}

BOOST_AUTO_TEST_CASE(tool_null) {
    // Check that a null entry in a tool list never fails a match, across
    // versions
    std::array<std::string, 2> versions{{"tool_null_v3.makerbot",
                "tool_null_v11.makerbot"}};
    for (const auto& name : versions) {
        LibTinyThing::TinyThingReader reader(name);
        BOOST_REQUIRE(reader.unzipMetadataFile());
        LibTinyThing::VerificationData vd;
        vd.pid = 0xb;
        vd.tool_count = 1;
        LibTinyThing::Metadata m;
        BOOST_CHECK_EQUAL(reader.getMetadata(&m),
                          LibTinyThing::TinyThingReader::Error::kOK);
        BOOST_CHECK(m.tool_type[0] == bwcoreutils::TYPE::UNKNOWN_TYPE);
        std::array<bwcoreutils::TOOL, 6> ids{{bwcoreutils::TOOL::INVALID_ID,
                    bwcoreutils::TOOL::MARK_11,
                    bwcoreutils::TOOL::MARK_12_1,
                    bwcoreutils::TOOL::MARK_13,
                    bwcoreutils::TOOL::MARK_13_IMPLA,
                    bwcoreutils::TOOL::MK14_0}};
        for (const auto id : ids) {
            vd.tool_id[0] = id;
            BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                              LibTinyThing::TinyThingReader::Error::kOK);
        }
    }
}

BOOST_AUTO_TEST_CASE(metadata_match) {
    // Check that we get failures if tool metadata doesn’t match
    LibTinyThing::TinyThingReader reader("valid.makerbot");
    BOOST_REQUIRE(reader.unzipMetadataFile());
    LibTinyThing::VerificationData vd;
    vd.pid = 0xb;
    vd.tool_count = 2;
    LibTinyThing::Metadata m;
    std::array<bwcoreutils::TOOL, 5> ids{{bwcoreutils::TOOL::MARK_11,
                bwcoreutils::TOOL::MARK_12_1,
                bwcoreutils::TOOL::MARK_13,
                bwcoreutils::TOOL::MARK_13_IMPLA,
                bwcoreutils::TOOL::MK14_0}};
    // If either tool doesn’t match we should get an error
    for (const auto id0 : ids) {
        for (const auto id1 : ids) {
            vd.tool_id[0] = id0;
            vd.tool_id[1] = id1;
            if (vd.tool_id[0] == bwcoreutils::TOOL::MARK_13
                && vd.tool_id[1] == bwcoreutils::TOOL::MK14_0) {
                BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                                  LibTinyThing::TinyThingReader::Error::kOK);
            } else {
                BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                           LibTinyThing::TinyThingReader::Error::kToolMismatch);
            }
        }
    }
    // If we have the wrong number of tools we should get an error even if that
    // tool matches
    vd.tool_count = 1;
    vd.tool_id[0] = bwcoreutils::TOOL::MARK_13;
    BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                      LibTinyThing::TinyThingReader::Error::kToolMismatch);
    // If our bot type is wrong we should have an error
    vd.pid = 0x23;
    BOOST_CHECK_EQUAL(reader.doesMetadataFileMatchConfig(vd),
                      LibTinyThing::TinyThingReader::Error::kBotTypeMismatch);
}

BOOST_AUTO_TEST_CASE(read_good_v3_metadata) {
    // check that our fancy safe json isn’t torpedoing values through automatic
    // type coercion
    LibTinyThing::TinyThingReader reader("valid.makerbot");
    BOOST_REQUIRE(reader.unzipMetadataFile());
    LibTinyThing::Metadata m;
    BOOST_REQUIRE_EQUAL(reader.getMetadata(&m),
                        LibTinyThing::TinyThingReader::kOK);
    BOOST_CHECK_EQUAL(m.extruder_count, 2);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[0], 16.877103, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[1], 15.0, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[0], 1382.914834765467, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[1], 10.0, 0.01);
    BOOST_CHECK_EQUAL(m.extruder_temperature[0], 215);
    BOOST_CHECK_EQUAL(m.extruder_temperature[1], 230);
    BOOST_CHECK_EQUAL(m.shells, 2);
    BOOST_CHECK_CLOSE(m.layer_height, 0.2, 0.01);
    BOOST_CHECK_CLOSE(m.infill_density, 0.1, 0.01);
    BOOST_CHECK(!m.uses_support);
    BOOST_CHECK_CLOSE(m.duration_s, 1048.89, 0.01);
    BOOST_CHECK_EQUAL(m.thing_id, 0);
    BOOST_CHECK(m.uses_raft);
    std::string uuid_str(m.uuid);
    std::string should_be("d4745d48-61aa-41ce-bef9-d582f14d6a91");
    BOOST_REQUIRE_EQUAL(uuid_str, should_be);
    std::string mat_0(m.material[0]), mat_1(m.material[1]);
    BOOST_CHECK_EQUAL(mat_0, std::string("pla"));
    BOOST_CHECK_EQUAL(mat_1, std::string("im-pla"));
    BOOST_CHECK(m.tool_type[0] == bwcoreutils::TYPE::MK13);
    BOOST_CHECK(m.tool_type[1] == bwcoreutils::TYPE::MK14);
    BOOST_CHECK_EQUAL(m.bot_pid, 0xb);
    BOOST_CHECK_CLOSE(m.bounding_box_x_max, 13.98500061035157, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_x_min, -13.98500061035156, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_y_max, 13.98500061035157, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_y_min, -13.98500061035156, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_z_max, 21.03999999999996, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_z_min, 0.3, 0.01);
}

BOOST_AUTO_TEST_CASE(valid_v11) {
    LibTinyThing::TinyThingReader reader("valid_v11.makerbot");
    BOOST_REQUIRE(reader.unzipMetadataFile());
    LibTinyThing::Metadata m;
    BOOST_REQUIRE_EQUAL(reader.getMetadata(&m),
                        LibTinyThing::TinyThingReader::kOK);
    BOOST_CHECK_EQUAL(m.extruder_count, 1);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[0], 5.60674, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[1], 0.f, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[0], 1837.60, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[1], 0, 0.01);
    BOOST_CHECK_EQUAL(m.extruder_temperature[0], 215);
    BOOST_CHECK_EQUAL(m.extruder_temperature[1], 0);
    BOOST_CHECK_EQUAL(m.shells, 2);
    BOOST_CHECK_CLOSE(m.layer_height, 0.204, 0.01);
    BOOST_CHECK_CLOSE(m.infill_density, 0.1, 0.01);
    BOOST_CHECK(!m.uses_support);
    BOOST_CHECK_CLOSE(m.duration_s, 1826.81, 0.01);
    BOOST_CHECK_EQUAL(m.thing_id, 0);
    BOOST_CHECK(m.uses_raft);
    std::string uuid_str(m.uuid);
    std::string should_be("aca5847b-f6c0-4b94-a8c8-17679ec056a8");
    BOOST_REQUIRE_EQUAL(uuid_str, should_be);
    std::string mat_0(m.material[0]), mat_1(m.material[1]);
    BOOST_CHECK_EQUAL(mat_0, std::string("PLA"));
    BOOST_CHECK_EQUAL(mat_1, std::string());
    BOOST_CHECK(m.tool_type[0] == bwcoreutils::TYPE::MK13);
    BOOST_CHECK(m.tool_type[1] == bwcoreutils::TYPE::UNKNOWN_TYPE);
    BOOST_CHECK_EQUAL(m.bot_pid, 0x6);
    BOOST_CHECK_CLOSE(m.bounding_box_x_max, 35.78700256, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_x_min, -36.282745361, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_y_max, 15.58228850, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_y_min, -16.34118672, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_z_max, 11.8560, 0.01);
    BOOST_CHECK_CLOSE(m.bounding_box_z_min, 0.099999, 0.01);
}

BOOST_AUTO_TEST_CASE(valid_v0) {
    LibTinyThing::TinyThingReader reader("valid_v0.makerbot");
    BOOST_REQUIRE(reader.unzipMetadataFile());
    LibTinyThing::Metadata m;
    BOOST_REQUIRE_EQUAL(reader.getMetadata(&m),
                        LibTinyThing::TinyThingReader::kOK);
    BOOST_CHECK_EQUAL(m.extruder_count, 1);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[0], 2.535733, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_mass_g[1], 0.f, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[0], 831.085, 0.01);
    BOOST_CHECK_CLOSE(m.extrusion_distance_mm[1], 0, 0.01);
    BOOST_CHECK_EQUAL(m.extruder_temperature[0], 215);
    BOOST_CHECK_EQUAL(m.extruder_temperature[1], 0);
    BOOST_CHECK_EQUAL(m.shells, 2);
    BOOST_CHECK_CLOSE(m.layer_height, 0.2, 0.01);
    BOOST_CHECK_CLOSE(m.infill_density, 0.05, 0.01);
    BOOST_CHECK(m.uses_support);
    BOOST_CHECK_EQUAL(m.slicer_name, std::string("MIRACLEGRUE"));
    BOOST_CHECK_CLOSE(m.duration_s, 2209.54788, 0.01);
    BOOST_CHECK(m.uses_raft);
}
