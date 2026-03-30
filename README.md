# Leaderboard

A Linux CLI leaderboard application built with C++ and Redis. Create your own leaderboards and interface with the example.

## Features

- Prompts to manage leaderboards.
- Leaderboards saved as ZSET keys to a persistent Redis database.
- Metadata SET key leaderboard:names that stores leaderboard names.
- Leaderboard actions controlled by user.
- Interactive example example_game:leaderboard.

## Requirements

### Supported Platforms

- Linux

### Dependencies

- g++
- hiredis
- Redis

## Quick Start (Docker)

Docker sandboxes a development environment with the required dependencies.

```sh
make docker-dev
```

This starts a Redis container in the background and drops into a bash shell inside of the dev container. 

Build and run the application.

```sh
make
./bin/main
```

Enter `exit` in the dev container to exit it. Then tear down the containers with:

```sh
make docker-down
```

Docker Redis is mapped to `localhost:6380` to avoid conflict with a local instance on the default Redis port 6379.

## Building Locally

Local setup is simple. Enter the following commands to install dependencies:

```sh
sudo apt update
sudo apt install -y build-essential redis-server libhiredis-dev
```

Then start the Redis server. This will start Redis on port 6379.

```sh
redis-server
```

Build the project and run the application.

```sh
make
./bin/main
```

## Usage

https://github.com/user-attachments/assets/2a3b4ecf-48e2-4ee0-992a-af31c45559a1

## License

MIT — Use it, modify it, share it.
