# Musicat - Discord Music Bot written in C++

Discord Music Bot written in C++

- Just install dependencies, compile and fill out `exe/sha_cfg.json`, config file should be in one directory as the binary.
- Musicat is fully slash command and need to register the commands to discord, run `./Shasha reg g` or `./Shasha reg <guild_id>` to register slash commands.

## Dependencies

Need to install:

* [FFmpeg](https://github.com/FFmpeg/FFmpeg) - Should be installed on your machine
* [libpq](https://github.com/postgres/postgres/tree/master/src/interfaces/libpq) - Library
* libopus - Library
* libogg - Library
* libcurl - Library
* libsodium - Library
* openssl - Library
* opus - Library

Included:

* [DPP](https://github.com/brainboxdotcc/DPP) - Library (included as submodule)
* [yt-dlp](https://github.com/yt-dlp/yt-dlp) - CLI App (included as submodule)
* [uWebSockets](https://github.com/uNetworking/uWebSockets) - Library (included as submodule)
* [curlpp](https://github.com/jpbarrette/curlpp) - Library (included as submodule)
* [liboggz](https://github.com/kfish/liboggz.git) - Library (included as submodule)
* [yt-search.h](https://github.com/Neko-Life/yt-search.h) - Included (included as submodule)
* [nekos-bestpp](https://github.com/Neko-Life/nekos-bestpp) - Included (included as submodule)
* [encode.h](https://gist.github.com/arthurafarias/56fec2cd49a32f374c02d1df2b6c350f) - Included (included as submodule)
* [nlohmann/json](https://github.com/nlohmann/json/tree/develop/single_include/nlohmann) - Headers only, included
* [ICU 73](https://icu.unicode.org/download/73) - Library (downloaded at compile time)

## Docker

* Clone the repo

```sh
git clone https://github.com/Neko-Life/Musicat.git --recurse-submodules

cd Musicat
```

* Build the image

```sh
docker build . -t shasha/musicat:latest
```

* Register the commands. Can be as simple as

```sh
docker run -it --rm -v ./exe/sha_conf-docker.json:/root/Musicat/exe/sha_conf.json --name Musicat-Register shasha/musicat:latest ./Shasha reg g
```

* Running

```sh
docker run -it --rm -p 3000:3000 -v ./exe/sha_conf-docker.json:/root/Musicat/exe/sha_conf.json -v ./exe/music:/root/music --name Musicat shasha/musicat:latest
```

You can create a docker-compose.yml file to integrate with postgresql, configure however you want.

* Misc
    * Creating container
        ```sh
        docker container create -p 9001:9001 -v ./exe/sha_conf.docker.json:/root/Musicat/exe/sha_conf.json -v ./exe/music:/root/music --name Musicat shasha/musicat:latest
        ```
    * Running container
        ```sh
        docker container start Musicat
        ```

## Installing Dependencies

Ubuntu 22.04.2 LTS:

* Upgrade first is recommended

```sh
apt update
apt upgrade
```

* Install

```sh
apt install build-essential pkg-config cmake make zlib1g-dev autoconf automake libtool libcurl4-openssl-dev libssl-dev libsodium-dev libopus-dev libogg-dev ffmpeg postgresql libpq-dev
```

## Compiling

* Make sure to have `pkg-config` installed as cmake need it to be able to detect libsodium and libopus

* Clone the repo and cd into it

```sh
git clone https://github.com/Neko-Life/Musicat.git --recurse-submodules

cd Musicat
```

* Compile

```sh
mkdir build
cd build

cmake ..

# configure the `-j` flag according to your available RAM as needed
make all -j$(nproc)
```

Some distro have `liboggz2` or `liboggz2-dev`, if you got compile error about
`oggz/oggz.h` not found but you already have `liboggz` installed,
you might need to install that.

* Move the built binary to `exe/` (or wherever you want along with a config file)

```sh
mv Shasha ../exe
```

* Create and fill the config file (config file must be named `sha_conf.json`, see `exe/sha_conf.example.json`) and run the binary

```sh
# register the commands
./Shasha reg g

# run the bot
./Shasha
```
