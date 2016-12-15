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
    
### Variable Definitions
Different variables can be loaded into the JsontoolpathParser.  Variables consist of a string predicated by a "#".

### Variable Replacement
While this is being included in the JsonToolpath spec, it is being considered optional and non-essential.  Only if a valid usecase is encountered will it be included.

Variables are only allowed in the command packet's "parameters" dict.  Variables are strings marked by a "#" sign, and can contain both letters and numbers.  The parser will have an environment in the form of a dict of variables and their respective values.  When the parser encounters a dict keyed by "parameters", it will iterate over all values and, if a variable is encountered, will replace it.

### Expressions
The JsonToolpath Parser allows certain values to be evaluated as expressions.  During the initialization of the JsontoolpathParser, various expression variables will be loaded into the parser.  Expression variables are strings.

Expressions are only allowed int he command packet's "parameters" dict.  Expressions are marked by the "!" sign (so "!temp_a + offset" will be evaluated as an expression; 230 will be evaluated as an int).

We support the following mathematical operations: +, -, * , /.  We also support order of operations with nested parenthesis.

#### User Modifier
Several parameters have a user modifier applied to them.  These modifiers are dynamic, and can change at any point during a print.  They are applied as such:

   value * user_modifier
   
The following parameters have user values applied to them:

   * temperature
   * speed

### Toolpath 
The "toolpath" contains all movement information in the .jsontoolpath file.  All information is encapsulatd in a packet, where a packet is comprised of a key in the form of a string, and some json serializable data as its value.  Packets are assumed to be _in order_.  The JT structure supports any number of packet types in this form:

    <packet_type>(string): <packet_data>(json_serializable_data)

There are currently three packet_types the birdwing software will understand: command_packet, open_tag and close_tag.

When a close tag is encountered, CloseTagCallback(tag_name) will be executed.

Here is the current list of tags:

*  raft
*  support
*  layer (if applicable)
*  infill
*  inner shell
*  outermost shell
*  spurs
*  bridge
*  floor
*  ceiling


#### Command Packet
A Command Packet contains fields that will result in some action.  Either the parser's state machine will change, or the printer will take some action (i.e. move some motors, heat some heaters, etc).  All command packets are wrapped in {}, to denote a map.

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

The function key will only have a single string as its value.  The parameters dict will have a subdict as its value, where each key is in the form of a string and its value is some POD type.  Any parameter's value can be replaced with a variable.  In this case, its data type would be a string (see Variable Substitution section below for more information).  
Metadata's value is also a dict.  Similar to parameters, values can only be POD types.  While we allow any values to be present in the metadata dict, we are reserving several keys for special use.  They are:

    * command_number: <int>
      - The commands numerical placement within the list of all commands.  For example: 57 out of 1025 commands.
    * total_commands: <int>
      - Total number of commands in this .jsontoolpath file.
    * layer: <int>
      - The current layer

Tags are used to keep track of the current type of move.  Since any specific command packet can be part of several different categories (i.e. outtermost shell + shell + floor), we are storing them as a list.

JT will have a small set of understood commands; since birdwing will have its own understanding of start/end routines, and JT is not intended to be a scripting language, there is no reason to have many of the superfluous commands (i.e. play_song, display_message, etc).  In the event a parameter has its value replaced with a variable, that value's data type would be parsed out as a string (since variables are stored as string).  After variable substitution, the value would become its standard data type.

In addition to standard values, all parameter values can be expressions.  Read the expression section for more information on expressions.

##### move

    "command": {
        "function": "move",
        "parameters": {                       //all axes absolute
            "x": <float> | <str>,                     //mm
            "y": <float> | <str>,                     //mm  
            "z": <float> | <str>,                     //mm
            "a": <float> | <str>,                     //mL
            "start_feedrate": <float> | <expression>,   //mm_per_sec
            "end_feedrate": <float> | <expression>      //mm_per_sec
        },
        "metadata": {
            "relative": {
                "x": <bool>,
                "y": <bool>,
                "z": <bool>,
                "a": <bool>
            },
            "units": {
                "x": "mm" | "um",
                "y": "mm" | "um",
                "z": "mm" | "um",
                "a": "mm" | "um" | "ml" | "ul"
            },
        }
    }

##### wait_for_toolhead

    "command": {
        "function": "wait_for_toolhead",
        "parameters": {
            "timeout": <float> | <expression>,
            "index": <int> | <expression>,
        }
    }

##### set_toolhead_temperature

    "command": {
        "function": "set_toolhead_temperature",
        "parameters": {
            "temperature": <float> | <expression>,      //C
            "index": <int> | <expression>,
        },
    }

##### wait_for_platform

    "command": {
        "function": "wait_for_platform",
        "parameters": {
            "timeout": <float> | <expression>,
        }
    }

##### set_platform_temperature

    "command": {
        "function": "set_platform_temperature",
        "parameters": {
            "temperature": <float> | <expression>,      //C        
        },
    }

##### toggle_fan

    "command": {
        "function": "toggle_fan",
        "parameters": {
            "value": <bool> | <expression>,
            "index": <int> | <expression>,
        }
    }

##### fan_duty

    "command": {
        "function": "fan_duty",
        "parameters": {
            "value": <float> | <expression>,
            "index": <int> | <expression>,
        }
    }

##### comment

    "command": {
        "function": "comment",
        "parameters": {
            // contents will be ignored by the machine
            // paremeters must be present, but can be empty
        }
    }

## JT Parser
The JT parser expects a filehandle to a .jsontoolpath file.  The parser will parse the json blob inside the .jsontoolpath file, sequentially executing each command packet.  The parser will disregard all non-command packets 

The parser has a set of functions, where each function is named after the value in the "function" field of the command dict.  Those functions take a dict, where the dict is the value of the "parameters" field in the json blob.


