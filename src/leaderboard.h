#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <hiredis/hiredis.h>
#include <string>
#include <vector>

class RedisLeaderboard {
private:
    redisContext *context;
    std::string leaderboard_key;

public:
    RedisLeaderboard(const std::string &key = "", const std::string &host = "127.0.0.1", int port = 6379);
    ~RedisLeaderboard();

    redisContext *get_context(void);
    bool is_connected(void);
    bool contains_player(const std::string &player);
    void add_score(const std::string &player, double score);
    double get_score(const std::string &player);
    int get_rank(const std::string &player);
    void show_top_players(int count);
    void increment_score(const std::string &player, double increment);
    void remove_player(const std::string &player);
    int get_total_players(void);
    void clear_leaderboard();
};

std::vector<std::string> get_all_leaderboards(redisContext *context);
bool key_exists(redisContext *context, const std::string &key);

#endif
