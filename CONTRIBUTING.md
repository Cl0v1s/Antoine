# Contributing

## Prerequisites

Docker is required to build the client. Install it for your OS: [docs.docker.com](https://docs.docker.com/get-started/get-docker/).

> **Note (Linux with SELinux — Fedora, RHEL…):** add `:z` to the volume flag so the container can access your files:
> `-v $(pwd):/project:z`

## Building the NDS client

```bash
docker run --rm -v $(pwd):/project -w /project devkitpro/devkitarm make -C client/nds
```

The output file `client.nds` will be placed in `client/nds/`.

## Building the 3DS client

```bash
docker run --rm -v $(pwd):/project devkitpro/devkitarm make -C client/3ds
```

The output file `client.3dsx` will be placed in `client/3ds/`.

## Testing the NDS client

### Required files

The following files are not included in the repository and must be provided manually:

- `client/nds/_nds/nds-bootstrap.nds` — download from [DS-Homebrew/nds-bootstrap](https://github.com/DS-Homebrew/nds-bootstrap/releases)
- `client/nds/misc/ww.nds` — your Animal Crossing: Wild World ROM

### Emulator setup

[melonDS](https://melonds.kuribo64.net/) is the recommended emulator. It supports DLDI patching, which is required for the client to access the virtual SD card.

In melonDS settings, enable **DLDI** and set it to **sync** mode so that file writes are immediately flushed to disk. This lets you inspect save files between runs without restarting the emulator.

### ac.txt configuration

`client/nds/ac.txt` is already configured for local testing. Adjust it to match your setup:

```
localhost         # server address
8080              # server port
french            # language
true              # launch AC:WW via nds-bootstrap
fat:/_nds/nds-bootstrap.nds
fat:/_nds/nds-bootstrap.ini
fat:/misc/ww.nds  # ROM path
fat:/misc/<your-save>.sav
```

In melonDS, set the SD card root to `client/nds/` so that the `fat:/` paths resolve correctly.

## Running unit tests

The test suite only requires a standard C++ compiler (no Docker needed):

```bash
cd client && make test
```
