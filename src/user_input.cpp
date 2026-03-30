#include "user_input.h"

#include "leaderboard.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

void display_menu(void) {
    std::cout << "\nEnter an integer input to choose leaderboard action" << std::endl;
    std::cout << "1. Add/Update player score" << std::endl;
    std::cout << "2. Increment player score" << std::endl;
    std::cout << "3. View player stats" << std::endl;
    std::cout << "4. Show top n players" << std::endl;
    std::cout << "5. Remove player" << std::endl;
    std::cout << "6. Show all players" << std::endl;
    std::cout << "7. Clear leaderboard" << std::endl;
    std::cout << "8. Main menu" << std::endl;
    std::cout << "9. Exit" << std::endl;
    std::cout << "\nInput: ";
}

// example case
void example(void) {
    RedisLeaderboard ex;
    bool example_already_exists = key_exists(ex.get_context(), "example_game:leaderboard");

    RedisLeaderboard example_leaderboard("example_game:leaderboard");

    if (!example_leaderboard.is_connected()) {
        std::cerr << "Failed to connect to redis. Make sure that redis server is running, enter redis-server" << std::endl;
        return;
    }

    std::cout << "Example Leaderboard" << std::endl;
    std::cout << std::endl;

    if (example_already_exists) {
        std::cout << "Total number of players: " << example_leaderboard.get_total_players() << std::endl;
        example_leaderboard.show_top_players(example_leaderboard.get_total_players());
        std::cout << std::endl;
        return;
    }

    example_leaderboard.add_score("Sophia", 1600);
    example_leaderboard.add_score("Damian", 700);
    example_leaderboard.add_score("Jordan", 1300);
    example_leaderboard.add_score("Charlotte", 1200);
    example_leaderboard.add_score("Jack", 800);

    std::cout << "Total number of players: " << example_leaderboard.get_total_players() << std::endl;
    example_leaderboard.show_top_players(example_leaderboard.get_total_players());

    std::cout << "\n*** Incrementing Jack's score and removing Jordan from leaderboard ***" << std::endl;
    example_leaderboard.increment_score("Jack", 1000);
    example_leaderboard.remove_player("Jordan");
    std::cout << std::endl;

    std::cout << "Total number of players: " << example_leaderboard.get_total_players() << std::endl;
    example_leaderboard.show_top_players(example_leaderboard.get_total_players());

    std::cout << std::endl;
    std::cout << "Jack's rank : " << example_leaderboard.get_rank("Jack") << std::endl;
    std::cout << "Jack's score : " << example_leaderboard.get_score("Jack") << "\n" << std::endl;

    return;
}

// strip string
std::string cstrip(const std::string& str) {
    const char* start = str.c_str();
    const char* end = str.c_str() + str.size();

    while (start < end && (*start == ' ' || *start == '\t')) {
        start++;
    }
    while (end > start && (*(end-1) == ' ' || *(end-1) == '\t')) {
        end--;
    }

    return std::string(start, end);
}

// prompt user with main application
int run_application(void) {
start:
    std::cout << "Redis Leaderboard System\n" << std::endl;
    example();
    std::string leaderboard_name = "";

    while (leaderboard_name == "" || leaderboard_name == "show-leaderboards") {
        std::cout << "Enter your leaderboard name or \"show-leaderboards\" to view active leaderboards" << std::endl;
        std::cout << "Enter your leaderboard name: ";

        if (!std::getline(std::cin, leaderboard_name)) {
            std::cout << "\nEOF received. Exiting\n";
            break;
        }

        leaderboard_name = cstrip(leaderboard_name);

        if (leaderboard_name == "show-leaderboards") {
            RedisLeaderboard gl;
            std::vector<std::string> leaderboards = get_all_leaderboards(gl.get_context());
            for (const std::string &name : leaderboards) {
                std::cout << name << std::endl;
            }
            std::cout << std::endl;
        } else if (leaderboard_name == "") {
            std::cout << "Leaderboard name cannot be empty" << std::endl;
        }
    }

    RedisLeaderboard leaderboard(leaderboard_name);

    if (!leaderboard.is_connected()) {
        std::cerr << "Failed to connect to redis" << std::endl;
        return 0;
    }

    int choice;
    std::string line;
    bool active = true;

    while (active) {
        display_menu();

        if (!std::getline(std::cin, line)) {
            std::cout << "\nEOF received. Exiting\n";
            break;
        }
        choice = std::stoi(line);
        if (choice <= 0 || choice > 9) {
            std::cout << "\nInvalid input. Please choose integer 1-9" << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                std::cout << "\nEnter player name: ";
                std::string player;
                std::getline(std::cin, player);

                std::cout << "Enter score: ";
                std::string line;
                std::getline(std::cin, line);
                double score = std::stod(line);

                leaderboard.add_score(player, score);
                std::cout << "\nSet score for player " << player << ": " << score << std::endl;
                break;
            }

            case 2: {
                std::cout << "\nEnter player name: ";
                std::string player;
                std::getline(std::cin, player);
                double prev_score = leaderboard.get_score(player);

                std::cout << "Enter increment: ";
                std::getline(std::cin, line);
                double increment = std::stod(line);

                if (!leaderboard.contains_player(player)) {
                    std::cout << player << " is not on this leaderboard no action" << std::endl;
                    break;
                }
                leaderboard.increment_score(player, increment);
                std::cout << "\nIncrement set for player " << player << ": " << increment << std::endl;
                std::cout << "Previous score: " << prev_score << std::endl;
                std::cout << "New score: " << leaderboard.get_score(player) << std::endl;
                break;
            }

            case 3: {
                std::string player;
                std::cout << "\nEnter player name: ";
                std::getline(std::cin, player);
                double score = leaderboard.get_score(player);

                if (score == -1) {
                    std::cout << "\nPlayer '" << player << "' not found" << std::endl;
                    break;
                }

                std::cout << "\nPlayer name: " << player << std::endl;
                std::cout << "Player rank: " << leaderboard.get_rank(player) << std::endl;
                std::cout << "Player score: " << leaderboard.get_score(player) << std::endl;
                break;
            }

            case 4: {
                std::string line;
                std::cout << "\nEnter number of top players to show: ";
                std::getline(std::cin, line);
                int n = std::stoi(line);

                if (n < 1) {
                    std::cout << "\nInvalid number, input must be a positive integer" << std::endl;
                    break;
                }

                leaderboard.show_top_players(n);
                break;
            }

            case 5: {
                std::string player;
                std::cout << "\nEnter player to remove: ";
                std::getline(std::cin, player);

                leaderboard.remove_player(player);
                std::cout << "\nRemoved player " << player << " from leaderboard" << std::endl;
                break;
            }

            case 6: {
                int total = leaderboard.get_total_players();

                std::cout << "\nThere are " << total << " active players" << std::endl;
                leaderboard.show_top_players(total);
                break;
            }

            case 7: {
                std::string confirm;
                std::cout << "\nAre you sure you want to clear the leaderboard? (y/n): ";
                std::getline(std::cin, confirm);

                if (confirm == "y") {
                    leaderboard.clear_leaderboard();
                    std::cout << "\nLeaderboard cleared" << std::endl;
                    goto start;
                }

                std::cout << "\nCancelled" << std::endl;
                break;
            }

            case 8: {
                std::cout << std::endl;
                goto start;
            }

            case 9: {
                std::cout << "\nExiting leaderboard application. Results are saved to redis" << std::endl;
                active = false;
                break;
            }
        }
    }

    return 1;
}
