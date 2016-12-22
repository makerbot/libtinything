# JsonToolpath (JT) v. 0.0.2
JT is a set of toolpaths that will be used by the birdwing family of 3d printers to execute a print.  The file extension used will be: ".jsontoolpath"

## Glossary
<table>
<tr>
   <th>Name</th>
   <th>Definition</th>
</tr>
<tr>
   <td>Json Serializable Data</td>
   <td>Any of the following values: int, float, string, bool, list, dict, null</td>
</tr>
<tr>
    <td>Plain Old Data (POD)</td>
    <td>Any of the following values: int, float, string, bool, null</td>
</tr>
<tr>
   <td>Toolpath</td>
   <td>A set of instructions for a machine to make it produce a desired object, along with any comments/meta-data that make it easier for a human or computer to read and/or understand.</td>
</tr>
</table>

## JT File
JT is a valid json file that is an array at its base.  Inside the array are a series of json dicts.  Those dicts contain all movement/actions necessary to print an object.

### File Structure:

    [
        {<packet_type>(string): <packet_data>(json_serializable_data)},
        {<packet_type>(string): <packet_data>(json_serializable_data)},
        ...
        {<packet_type>(string): <packet_data>(json_serializable_data)}
    ]

### Toolpath
The "toolpath" contains all movement information in the .jsontoolpath file.  All information is encapsulatd in a packet, where a packet is comprised of a key in the form of a string, and some json serializable data as its value.  Packets are assumed to be _in order_.  The JT structure supports any number of packet types in this form:

    <packet_type>(string): <packet_data>(json_serializable_data)

There are currently two packet_types supported by this spec: "command" and (as of v3.0) "comment".  Toolpath parsers are encouraged to ignore all other packet types for future compatibility.


#### Command Packet
A Command Packet contains fields that will result in some action taken by the printer (i.e. move some motors, heat some heaters, etc).  All command packets are wrapped in {}, to denote a map.

   {
        "function": "",
        "parameters": {
            <str>: <json_serializable_data>,
            <str>: <json_serializable_data>,
            ...
         },
        "metadata": {},
        "tags": [],
    }

The function key will only have a single string as its value.  The parameters dict will have a subdict as its value, where each key is in the form of a string and its value is some POD type.
Metadata's value is also a dict.  Similar to parameters, values can only be POD types.  Toolpath parsers should ignore any unknown metadata keys for future compatibility.  Toolpath parsers are encouraged to treat a missing metadata key the same as an empty metadata dict.

Tags are used to keep track of the current type of move.  Since any specific command packet can be part of several different categories (i.e. outtermost shell + shell + floor), we are storing them as a list.  All elements of the list must be strings.  Toolpath creators are encouraged not to use strings that are not present [here](https://github.com/makerbot/MBCoreUtils/blob/master/include/mbcoreutils/toolpath_tags.h), and toolpath parsers should ignore all strings not listed there.  Toolpath parsers are ncouraged to treat a missing tags key the same as as empty tags list.

JT will have a small set of understood commands; since birdwing will have its own understanding of start/end routines, and JT is not intended to be a scripting language, there is no reason to have many of the superfluous commands (i.e. play_song, display_message, etc).  Toolpath parsers should ignore any unknown commands for future compatibility.  Toolpath parsers are also encouraged to accept integer values when a float is expected, but not a float when an integer is expected.

##### move

    "command": {
        "function": "move",
        "parameters": {
            "x": <float>,          //mm
            "y": <float>,          //mm
            "z": <float>,          //mm
            "a": <float>,          //mm
            "feedrate": <float>,   //mm_per_sec
        },
        "metadata": {
            "relative": {
                "x": <bool>,
                "y": <bool>,
                "z": <bool>,
                "a": <bool>
            },
        }
    }

##### set_toolhead_temperature

    "command": {
        "function": "set_toolhead_temperature",
        "parameters": {
            "temperature": <float>,      //C
            "index": <int>,
        },
    }

##### toggle_fan

    "command": {
        "function": "toggle_fan",
        "parameters": {
            "value": <bool>,
            "index": <int>,
        }
    }

##### fan_duty

    "command": {
        "function": "fan_duty",
        "parameters": {
            "value": <float>,
            "index": <int>,
        }
    }

##### change_toolhead

    Not supported before version 3.0

    "command": {
        "function": "change_toolhead",
        "parameters": {
            "index": <int>,
        }
    }

#### Comment Packet

    A comment packet in version 3.0 should be formatted as

    { "comment": "THIS IS A COMMENT" }

    A comment may also be formatted (in versions 2.0 and in 3.0) as:

    {   "command": {
            "function": "comment",
            "parameters": {
                "comment": "THIS IS A COMMENT"
            }
        }
    }

    There is no way to add a comment in version 1.0

