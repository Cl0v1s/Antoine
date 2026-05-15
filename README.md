# Antoine - Animal Crossing Wild World Letter Response System

This project aims to enhance the gameplay experience of **Animal Crossing: Wild World** by enabling villagers to genuinely respond to letters sent by players. The system consists of two main components: a **client** (a DS homebrew application) and a **server** (responsible for generating responses).

![Client screenshot](./screen.png)

## Overview

The system works as follows:
1. The **client** reads the letters sent by the player from the game's save memory.
2. The **server** generates personalized responses based on the villager's personality and the content of the player's letter.
3. The **client** injects the generated response into the game's memory, replacing the player's letter to prevent duplicate responses.

## Features

- **Read Sent Letters**: Extract letters sent by the player from the game's memory.
- **Generate Responses**: Use a language model (currently Mistral) to create replies tailored to the villager's personality.
- **Inject Responses**: Insert the generated replies into the game's memory.
- **Letter Scoring**: Use the in-game scoring system to influence the tone of villager responses.

### Future Enhancements
- **Prevent duplicates**: Clear the sent letter from memory after processing to prevent the game from generating its own reply.
- **Gift System**: Add gifts to villager responses based on the player's letter content or attached items.

## Technical Details

### Memory Layout
- **Sent Letters**: Stored in a buffer starting at `0x00014640` (replicated at `0x0002a620`). The buffer supports up to 10 letters.
- **Mailbox Letters**: Located at `0x00012010` with a capacity of 10 slots. Letters are cleared from this area once moved to the player's inventory.
- **Pocket Letters**: Found at `0x00001120`, with space for 10 letters. (Unused by this system)

### Letter Format
- Letters follow a specific structure, including headers, body, and footer. Key fields include:
  - **Villager ID**: Identifies the sender or recipient.
  - **Message Body**: Encoded as in-game characters.
  - **Flags**: Control display options, such as showing the NPC's name in the inventory.

For more details, refer to the [ACWW-Web-SaveEditor Letter Format](https://github.com/Universal-Team/ACWW-Web-SaveEditor/blob/main/assets/js/core/letter.js).

### Villager Data
Villager information, such as species and personality, is extracted using scrapped data and mapped to in-game IDs. This ensures accurate and immersive responses.

### Checksum Validation
To prevent tampering, the game uses a checksum mechanism. The checksum is a little-endian sum of all memory values, ensuring data integrity.

## Components

### Client
A DS homebrew application that:
- Reads and writes memory to interact with the game's save data.
- Communicates with the server to fetch generated responses.
- Requires configuration via a file named `ac.txt` located alongside the homebrew on the DS's SD card. The file must be formatted as follows:
  ```
  <server>        # Remote server address
  <port>          # Remote server port
  <lang>          # Language for villager responses (Supported: english, french)
  <launcher>      # true/false - On NDS: launch AC:WW via nds-bootstrap (required) / On 3DS: launch the title defined in <rom>
  <bootstrapNDS>  # Path to nds-bootstrap executable (NDS only, leave empty on 3DS)
  <bootstrapINI>  # Path to nds-bootstrap configuration file (NDS only, leave empty on 3DS)
  <rom>           # On NDS: path to the AC:WW ROM / On 3DS: title id of the nds-bootstrap frontend or AC:WW nds-forwarder
  <save>          # Path to the AC:WW save file
  <empty line >   # The file must end with an empty line
  ```
- Requires an active internet connection to communicate with the server. This connection must be configured beforehand. If running on a 3DS, ensure that a DS connection is set up in the system settings for the homebrew to function properly.

### Server
A backend system that:
- Processes requests from the client.
- Generates villager responses using a language model (currently Mistral, with plans for a local CPU-based model).
- Requires the following environment variables to function correctly:
  - `GIN_MODE`: Specifies the mode in which the server operates (release).
  - `PORT`: Defines the port on which the server listens for incoming connections.
  - `API_KEY`: Contains a valid Mistral API key for accessing the `mistral-small-latest` model.
  
#### Prompt Configuration
The server requires a file named `prompt.txt` located in its directory. This file contains the prompt template sent to the language model.

The template supports named variable interpolation using the `{{variableName}}` syntax. Variables can appear in any order, be repeated, or be omitted. The following variables are available:

| Variable | Description |
|---|---|
| `{{townName}}` | The player's town name |
| `{{traits}}` | The villager's personality description |
| `{{receiverName}}` | The player character's name |
| `{{receivedAttachment}}` | Gift included in the player's letter (`"no gift"` if none) |
| `{{letter}}` | The full letter content from the player |
| `{{tone}}` | Tone derived from the letter score: `"very happy"` (score > 80), `""` (score 50–80), or `"confused or negative"` (score < 50) |
| `{{language}}` | The response language (e.g. `english`, `french`) |
| `{{sentAttachment}}` | Gift the villager will send back, chosen automatically based on letter length (see below) |

#### Reply Gift Generation

The villager's reply can include a gift chosen automatically from the item database based on the length of the player's letter:

| Letter length | Pool |
|---|---|
| Short (~1 line) | A random **shirt** or **fruit** |
| Medium (~2 lines) | A random piece of **furniture** |
| Long (~3+ lines) | A random piece of **furniture**, **carpet**, or **wallpaper** |

Use `{{sentAttachment}}` in the prompt template to inform the LLM of the chosen gift so it can reference it naturally in the reply.

Example usage in `prompt.txt`:
```
You are a villager in {{townName}}. Your traits: {{traits}}
You received a letter from {{receiverName}}: {{letter}}
Your answer letter will contain {{sentAttachment}}.
Write a {{tone}} reply in {{language}}.
```

## References

- [ACWW-Web-SaveEditor](https://github.com/Universal-Team/ACWW-Web-SaveEditor/blob/main/assets/js/core/letter.js)
- [Animal Crossing Letters Project](https://jamchamb.net/projects/animal-crossing-letters)
- [GameFAQs Discussion on Letter Buffers](https://gamefaqs.gamespot.com/boards/920786-animal-crossing-wild-world/41195712)
- [Letter-Writing Tips](https://www.thonky.com/acww/letter-writing-tips)

## Getting Started

1. **Set Up the Client**: Install the DS homebrew application on your device.
2. **Configure the Client**: Create the `ac.txt` file on the SD card with the required settings.
3. **Run the Server**: Deploy the server locally or on a remote machine with the required settings.
4. **Send Letters**: Write letters to villagers in-game

![Bob dancing](http://vignette2.wikia.nocookie.net/animalcrossing/images/f/fb/Bobdance.gif/revision/latest?cb=20130922040310)