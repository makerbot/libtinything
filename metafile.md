# Meta.json

This is the specification for the metainformation file within a .makerbot. It is a JSON dict with a certain number of required keys and a number of optional ones. It is versioned with SemVer, and this version also serves as the version of the entire .makerbot file.

Within the .makerbot file, the metainformation file should be called meta.json.

Keys at the root level are outputted from Miracle Grue, since they depend on the behavior of the slice.
The full Miracle Grue config is included under the root level key "miracle_config".
The full machine config (see [the Machine Config spec](https://docs.google.com/a/makerbot.com/document/d/1-JIKrlPBehgXxOYXXMeODoY5HF40ijci4clz1MhyhIk/edit))is included under the root level key "machine_config". Elements of this machine config are used to check whether or not the slice is for the current machine.

As of version 1.0.0, if the root level key "is_custom" is true, the machine config will verify.

### Example metadata file: Version 0.0.3
comments are added to indicate the source of various metadata values

    {
        "thing_id", // from Prototype
        "uuid", // from prototype (unique identifier for this .makerbot file)
        "toolhead_0_temperature": 230,  // miracle grue output
        "toolhead_1_temperature": 230,  // miracle grue output
        "total_commands": 44914,  // miracle grue output
        "extrusion_distance_b_mm": 0.0,  // miracle grue output
        "extrusion_mass_a_grams": 2.255707508943489,  // miracle grue output
        "duration_s": 1151.261878252689,  // miracle grue output
        "extrusion_mass_b_grams": 0.0,  // miracle grue output
        "extrusion_distance_a_mm": 2255.707508943489, // miracle grue output
        "printer_settings": { // print profile (miracle grue input)
            "default_raft_extruder": 0,
            "slicer": "MIRACLEGRUE",
            "platform_temperature": 110,
            "shells": 2,
            "default_support_extruder": 0,
            "support": false,
            "layer_height": 0.2,
            "travel_speed": 150,
            "extruder_temperatures": [
                230,
                230
            ],
            "materials": [
                "PLA",
                "PLA"
            ],
            "infill": 0.1,
            "heat_platform": false,
            "raft": true,
            "do_auto_support": false,
            "path": null,
            "print_speed": 90,
            "do_auto_raft": true,
            "extruder": "0"
        }
    }


### Example metadata file: Version 1.0.0

    {
        "version" : "1.0.0",
        "thing_id" : 100, // from prototype
        "uuid" : "xxxxxxxxxxxxxxx", // from prototype (unique identifier for this .makerbot file)
        "extruder_temperature": 215, //miracle grue output
        "chamber_temperature": 30, // miracle grue output
        "total_commands": 44914, //miracle grue output
        "extrusion_distance_mm": 12315123, //miracle grue output
        "extrusion_mass_g": 2131.04, //miracle grue output
        "is_custom": False, //miracle grue output
        "duration_s": 1231.2, //miracle grue output
        "material": "PLA",
        "tool_type": "squid" or null //miracle grue output, null in a custom profile
        "bot_type": "replicator_5", // this should ALWAYS be there, and accurate, even for a custom profile
        "miracle_config": {
            "numberOfShells": 2,
            "layerHeight": 0.2,
            "infillDensity": 0.1,
            "doRaft": false,
            "doSupport": false,
            "max_flow_rate": 5.0,
            "slicer_name": "MiracleGrue",
        },
        "machine_config": {...} or null // null for custom profile, machine config dict dump otherwise
    }

### Example metadata file: Version 1.1.0

    {
        "version": "1.1.0",
        ...all keys and values from version 1.0.0...
        "z_pause_locations": [
            {"layer": 0, "action": "pause", "enabled": true},
            {"layer": 152, "action": "pause", "enabled": false}
        ],
        "max_layer": int
    }

### Example metadata file: Version 2.0.0

    {
        "version": "2.0.0",
        ...all keys and values from version 1.1.0...
        "bounding_box_x_min" : -20.0,
        "bounding_box_x_max" : 20.0,
        "bounding_box_y_min" : -20.0,
        "bounding_box_y_max" : 20.0,
        "bounding_box_z_min" : 0.0,
        "bounding_box_z_max" : 10.0,
        "user_preferences" : {
            "overrides" : {
                ...user overrides...
            }
        },
        "model_counts" : [
            {
                "name" : "model1",
                "count" : 5
            },
            {
                "name" : "model2",
                "count" : 2
            }
            ...
        ]
    }

### Example metadata file: Version 3.0.0
The biggest change in this version is a move to multi-extrusion by default. Any key that has different values on a per-extruder basis must _always_ be a list, with a number of entries equivalent to the number of extruders the machine supports - _not_ the number of extruders used in a print. For instance, if a machine has two extruders but the print uses only one, the `print_temperatures` key must be a list of two entries (though one might be 0).

Here is an example of keys that are different from version 2.0.0. A full example metafile may be found [here](example_meta_v3.json).

	{
		"version": "3.0.0",
		"extruder_temperature": [215, 0],
		"extrusion_distance_mm": [122131, 0],
		"extrusion_mass_g": [220, 0],
		"material": ["pla", "pla"],
		"tool_type": ["mk13", "mk13"]
		...
	}
