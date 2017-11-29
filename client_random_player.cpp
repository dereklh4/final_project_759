/*
 * client_player.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: derekhancock
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sstream>
#include <stddef.h>

#include "client_player.cpp"
#include "node.cpp"

using namespace std;

class ClientRandomPlayer : public ClientPlayer {
public:

	ClientRandomPlayer(string host, int port, int in_me) : ClientPlayer(host, port, in_me) { }

	virtual ~ClientRandomPlayer() { }

	int move() {
		// just move randomly for now
		std::cout << "[random player] " << "Getting random move\n";
		int validMoves[64];
		int numValidMoves = get_valid_moves(state, me, validMoves);
		if (numValidMoves > 0) {
			int randomIndex = rand() % numValidMoves;

			int myMove;
			myMove = validMoves[randomIndex];

			std::cout << "[random player] " << "Returning random move " << myMove << "\n";
			return myMove;
		}
		else {
			return -1;
		}
	}

};
