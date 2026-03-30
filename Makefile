CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lhiredis

TARGET = bin/main
SRC = src/main.cpp src/leaderboard.cpp src/user_input.cpp

all: $(TARGET)

$(TARGET): $(SRC) | bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

bin:
	mkdir -p bin

docker-dev:
	docker compose up -d redis
	docker compose run --rm dev

docker-down:
	docker compose down

clean:
	rm -rf bin/* build/*
