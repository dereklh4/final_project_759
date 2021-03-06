/*
 * main.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: derekhancock
 */

#include "client_random_player.cpp"
#include <iostream>
#include <string>
#include "client_minimax_player.cpp"

int main(int argc, char *argv[]) {

	if (argc != 6) {
		cout << "ERROR. USAGE: main <player_type> host port player_number depth_limit";
		return -1;
	}

	if (std::string(argv[1]) == "client_random_player") {
		ClientRandomPlayer player(argv[2],stoi(argv[3]),stoi(argv[4]));
		player.play();
	}
	else if (std::string(argv[1]) == "client_baseline_minimax") {
		ClientMiniMaxPlayer player(argv[2],stoi(argv[3]),stoi(argv[4]),stoi(argv[5]),false);
		player.play();
	}
	else if (std::string(argv[1]) == "client_openmp_minimax") {
		ClientMiniMaxPlayer player(argv[2],stoi(argv[3]),stoi(argv[4]),stoi(argv[5]),true);
		player.play();
	}
	else {
		cout << "ERROR: Invalid player type\n";
		return -1;
	}

	return 0;
}
