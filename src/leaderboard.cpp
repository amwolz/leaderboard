#include "leaderboard.h"

#include <iostream>
#include <utility>

namespace {
// resolve the redis host to use for connection
std::string resolve_host(const std::string &host) {
    // host is nonempty and not the default IP address
    if (!host.empty() && host != "127.0.0.1") {
        return host;
    }

    // read REDIS_HOST env variable
    char *env_host = std::getenv("REDIS_HOST");
    if (env_host != nullptr) {
        return env_host;
    }

    return host;
}

// resolve the redis port to use for connection
int resolve_port(int port) {
    // port is not the default TCP port
    if (port != 6379) {
        return port;
    }

    // read REDIS_PORT env variable
    char *env_port = std::getenv("REDIS_PORT");
    if (env_port != nullptr) {
        return std::atoi(env_port);
    }

    return port;
}
} 

// create the connection to a redis server
RedisLeaderboard::RedisLeaderboard(const std::string &key, const std::string &host, int port)
    : context(nullptr), leaderboard_key(key) {
    const std::string resolved_host = resolve_host(host);
    int resolved_port = resolve_port(port);
    context = redisConnect(resolved_host.c_str(), resolved_port);

    if (context == nullptr || !context) {
        std::cerr << "Redis connection error" << std::endl;
        redisFree(context);
    // add key to leaderboard:names set to manage names of leaderboards
    } else if (leaderboard_key != "") {
        redisReply *reply = (redisReply*)redisCommand(context, "SADD leaderboard:names %s", leaderboard_key.c_str());
    }
}

// release redis connection owned by this instance (like an automatic free for associated memory when the connection becomes inaccessible)
RedisLeaderboard::~RedisLeaderboard() {
    if (context != nullptr) {
        redisFree(context);
    }
}

// get the context used by the instance
redisContext *RedisLeaderboard::get_context(void) {
    return context;
}

// there is a valid connection to a redis server
bool RedisLeaderboard::is_connected(void) {
    return context != nullptr;
}

// the leaderboard contains input player
bool RedisLeaderboard::contains_player(const std::string &player) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZSCORE %s %s", leaderboard_key.c_str(), player.c_str());
    if (!reply || reply->type == REDIS_REPLY_NIL) {
        return false;
    }
    return true;
}

// add player to leaderboard or replace their existing score
void RedisLeaderboard::add_score(const std::string &player, double score) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZADD %s %f %s", leaderboard_key.c_str(), score, player.c_str());
}

// get input player's score
double RedisLeaderboard::get_score(const std::string &player) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZSCORE %s %s", leaderboard_key.c_str(), player.c_str());

    double score = -1;
    if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
        score = std::stod(reply->str);
    }
    return score;
}

// get input player's rank
int RedisLeaderboard::get_rank(const std::string &player) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZREVRANK %s %s", leaderboard_key.c_str(), player.c_str());

    int rank = -1;
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        rank = reply->integer + 1;
    }
    return rank;
}

// get and print top input count players and their scores
void RedisLeaderboard::show_top_players(int count) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZREVRANGE %s 0 %d WITHSCORES", leaderboard_key.c_str(), count - 1);

    std::cout << "\nTop " << count << " Players:" << std::endl;

    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i += 2) {
            std::string player = reply->element[i]->str;
            std::string score = reply->element[i + 1]->str;
            std::cout << (i / 2 + 1) << ". " << player << " - " << score << " points" << std::endl;
        }
    }
}

// increment input player's score by input increment
void RedisLeaderboard::increment_score(const std::string &player, double increment) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZINCRBY %s %f %s", leaderboard_key.c_str(), increment, player.c_str());
}

// remove input player from the leaderboard
void RedisLeaderboard::remove_player(const std::string &player) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZREM %s %s", leaderboard_key.c_str(), player.c_str());
}

// get total number of players in the leaderboard
int RedisLeaderboard::get_total_players(void) {
    redisReply *reply = (redisReply*)redisCommand(context, "ZCARD %s", leaderboard_key.c_str());

    int count = 0;
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        count = reply->integer;
    }
    return count;
}

// delete the entire leaderboard key from redis and the metadata set leaderboard:names
void RedisLeaderboard::clear_leaderboard() {
    redisReply *reply = (redisReply*)redisCommand(context, "DEL %s", leaderboard_key.c_str());
    reply = (redisReply*)redisCommand(context, "SREM leaderboard:names %s", leaderboard_key.c_str());
}

// get all leaderboard keys in database
std::vector<std::string> get_all_leaderboards(redisContext *context) {
    std::vector<std::string> result;

    redisReply *reply = (redisReply*)redisCommand(context, "SMEMBERS leaderboard:names");
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY) {
        return result;
    }

    for (size_t i = 0; i < reply->elements; i++) {
        if (reply->element[i] != nullptr && reply->element[i]->str != nullptr) {
            result.push_back(reply->element[i]->str);
        }
    }

    return result;
}

// input key is a leaderboard key in database
bool key_exists(redisContext *context, const std::string &key) {
    redisReply *reply = (redisReply*)redisCommand(context, "SISMEMBER leaderboard:names %s", key.c_str());
    if (!reply || reply->type != REDIS_REPLY_INTEGER) {
        return false;
    }
    return reply->integer == 1;
}

