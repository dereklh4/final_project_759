/*
 * client_player.cpp
 *
 *  Created on: Oct 21, 2017
 *      Author: derekhancock
 */
#pragma once
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

using namespace std;

class ClientPlayer {
public:
	double t1;
	double t2;
	int me;
	int boardState;
	int state[8][8]; // state[0][0] is the bottom left corner of the board (on the GUI)
	int turn = -1;
	int round;

	int sockfd;

	ClientPlayer(string host, int port, int in_me) {
		me = in_me;
		sockfd = init_client(host, port);

		//initialize state
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				state[i][j] = 0;
			}
		}

		srand(time(NULL));
		//srand (101);

	}

	virtual ~ClientPlayer() {
	}
	;

	void play() {

		int myMove;
		while (true) {
			//cout << "[client " << me << "] " << "Waiting for message\n";

			bool success = read_message(sockfd);
			//cout << "[client " << me << "] " << "Finished read message\n";

			if (!success && turn != -999) {
				cout << "[client " << me << "] "
						<< "Read not successful. Breaking...\n";
				break;
			}

			//cout << "[client " << me << "] " << "Turn: " << turn << endl;
			//cout << "[client " << me << "] " << "Me: " << me << endl;
			if (turn == me) {
				//cout << "[client " << me << "] " << "Getting valid states\n";
				//get_valid_moves(state, me);

				//cout << "[client " << me << "] " << "Getting move\n";
				myMove = move();

				//cout << "[client " << me << "] " << "Packaging up move " << myMove << "\n";
				std::stringstream ss;
				if (myMove == -1)
					ss << -1 << "\n" << -1 << "\n";
				else
					ss << myMove / 8 << "\n" << myMove % 8 << "\n";
				string sel = ss.str();

				//cout << "[client " << me << "] " << "\nSelection: " << myMove / 8 << "," << myMove % 8 << "\n";
				//cout << "[client " << me << "] " << "Sending: " << "\n" << sel << "\n";

				send(sockfd, sel.c_str(), sizeof(sel), 0);

			} else if (turn == -999) {
				cout << "[client " << me << "] " << "Told that the game is over\n";
				return;
			} else {
				// cout << "[client " << me << "] " << "Not my turn\n\n";
			}
		}
	}

	bool read_message(int sock) {

		char buffer[1000];
		const int result = recv(sock, buffer, sizeof(buffer), 0);
		if (result == -1) {
			cout << "[client " << me << "] "
					<< "Error receiving from socket.\n";
			return false;

		}

		string s(buffer, result);
		std::stringstream ss(s);

		int i, j;
		string status;
		string line;
		try {

			if (result == 4) { //first message
				std::getline(ss, line);
				std::getline(ss, line);
				t1 = stoi(line);
				cout << endl;
				return true;
			}

			//System.out.println("Ready to read again");
			std::getline(ss, line);
			turn = stoi(line);

			if (turn == -999) {
				cout << "[client " << me << "] " << "Got that turn is -999 (game is over) \n";
				return false;
			}

			//cout << "[client " << me << "] " << "Turn: " << turn << "\n";
			std::getline(ss, line);
			round = stoi(line);
			//cout << "[client " << me << "] " << "Round: " << round << "\n";
			if (result == 4) {
				//cout << "[client " << me << "] " << "End of stream\n";
				return true;
			}
			std::getline(ss, line);
			t1 = atof(line.c_str());
			//cout << "t1: " << t1 << "\n";
			std::getline(ss, line);
			t2 = atof(line.c_str());
			//cout << "t2:" << t2 << "\n";
			for (i = 0; i < 8; i++) {
				for (j = 0; j < 8; j++) {
					std::getline(ss, line);

					state[i][j] = stoi(line);
				}
			}
			std::getline(ss, line);
		} catch (...) {
			cout << "[client " << me << "] " << "Exception in get message\n";
			return false;
		}

		cout << "State:\n";
		for (i = 7; i >= 0; i--) {
			for (j = 0; j < 8; j++) {
				cout << state[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		return true;
	}

	void *get_in_addr(struct sockaddr *sa) {
		if (sa->sa_family == AF_INET) {
			return &(((struct sockaddr_in*) sa)->sin_addr);
		}

		return &(((struct sockaddr_in6*) sa)->sin6_addr);
	}

	int init_client(string host, int in_port) {
		int port = in_port;
		cout << "[client " << me << "] " << "Connecting on port: " << port
				<< endl;

		int sockfd, numbytes;
		char buf[1000];
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		string port_str = to_string(port);

		if ((rv = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &servinfo))
				!= 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for (p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
					== -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		return sockfd;
	}

	int get_valid_moves(int state[8][8], int player, int validMoves[64]) {

//		cout << "get_valid_moves. Player: " << player << "Given state: \n";
//		for (int i = 0; i < 8; i++) {
//			for (int j = 0; j < 8; j++) {
//				cout << state[i][j] << " ";
//			}
//			cout << "\n";
//		}

		int i, j;

		int numValidMoves = 0;
		if (round < 4) {
			if (state[3][3] == 0) {
				validMoves[numValidMoves] = 3 * 8 + 3;
				numValidMoves++;
			}
			if (state[3][4] == 0) {
				validMoves[numValidMoves] = 3 * 8 + 4;
				numValidMoves++;
			}
			if (state[4][3] == 0) {
				validMoves[numValidMoves] = 4 * 8 + 3;
				numValidMoves++;
			}
			if (state[4][4] == 0) {
				validMoves[numValidMoves] = 4 * 8 + 4;
				numValidMoves++;
			}
			//cout << "[client " << me << "] " << "Valid Moves:\n";
			//for (i = 0; i < numValidMoves; i++) {
			//    cout << validMoves[i] / 8 << "," << validMoves[i] % 8 << "\n";
			//}
		}
		else {
			//cout << "[client " << me << "] " << "Valid Moves:\n";
			for (i = 0; i < 8; i++) {
				for (j = 0; j < 8; j++) {
					if (state[i][j] == 0) {
						if (could_be(state, i, j, player)) {
							validMoves[numValidMoves] = i * 8 + j;
							numValidMoves++;
							//cout << i << "," << j << "; ";
						}
					}
				}
			}
			//cout << "\n";
		}

//		cout << "Returning the valid moves: \n";
//		for (int i = 0; i < numValidMoves; i++) {
//			cout << "(" << (validMoves[i]/8) << "," << (validMoves[i]%8) << ") ";
//		}
//		cout << "\n";
		return numValidMoves;
	}

	bool could_be(int state[8][8], int row, int col, int player) {
		int incx, incy;

		for (incx = -1; incx < 2; incx++) {
			for (incy = -1; incy < 2; incy++) {
				if ((incx == 0) && (incy == 0))
					continue;

				if (check_direction(state, row, col, incx, incy, player))
					return true;
			}
		}

		return false;
	}

	bool check_direction(int state[8][8], int row, int col, int incx, int incy,
			int player) {
		int sequence[7];
		int seqLen;
		int i, r, c;

		seqLen = 0;
		for (i = 1; i < 8; i++) {
			r = row + incy * i;
			c = col + incx * i;

			if ((r < 0) || (r > 7) || (c < 0) || (c > 7))
				break;

			sequence[seqLen] = state[r][c];
			seqLen++;
		}

		int count = 0;
		for (i = 0; i < seqLen; i++) {
			if (player == 1) {
				if (sequence[i] == 2)
					count++;
				else {
					if ((sequence[i] == 1) && (count > 0))
						return true;
					break;
				}
			} else {
				if (sequence[i] == 1)
					count++;
				else {
					if ((sequence[i] == 2) && (count > 0))
						return true;
					break;
				}
			}
		}

		return false;
	}

	virtual int move() {
		std::cout << "I SHOULDN'T BE CALLED\n";
		return 100;
	}

};
