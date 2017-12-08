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
#include <vector>

#include "client_player.cpp"
#include "node.cpp"
#include "stopwatch.hpp"

using namespace std;

class ClientMiniMaxPlayer : public ClientPlayer {
public:

	Node* root;
	int depth_limit;
	unsigned int total_nodes_expanded;
	std::string message_prefix;
	double time_left;
	bool hit_time_limit;
	bool run_omp;
	int times_hit_time_limit;
	double total_time_spent;

	ClientMiniMaxPlayer(string host, int port, int in_me, int depth_limit=5, bool run_omp=false) : ClientPlayer(host, port, in_me) {
		this->depth_limit = depth_limit;
		this->root = NULL;
		total_nodes_expanded = 0;

		stringstream ss;
		if (!run_omp)
			ss << "[baseline minimax " << me << "] ";
		else
			ss << "[openmp minimax " << me << "] ";
		message_prefix = ss.str();
		time_left = -1;
		hit_time_limit = false;
		this->run_omp = run_omp;
		times_hit_time_limit = 0;
		total_time_spent = 0;
	}

	virtual ~ClientMiniMaxPlayer() { }

	int randomMove() {
		cout << message_prefix << "Returning random move\n";
		int validMoves[64];
		int numValidMoves = get_valid_moves(state, me, validMoves);

		cout << message_prefix << "numValidMoves: ";
		cout << numValidMoves << "\n";
		int myMove;
		if (numValidMoves > 0) {
			int randomIndex = rand() % numValidMoves;
			//int randomIndex = 17 % numValidMoves; //to make repatable. DELETE ME LATER
			myMove = validMoves[randomIndex];
		}
		else {
			myMove = -1;
		}
		return myMove;
	}

	int move() {
		cout << message_prefix << "Entering move function\n";
		if (round < 4) {
			//can only choose middle 4 squares, so just choose one randomly
			return randomMove();
		}
		else {
			//make the tree
			std::cout << message_prefix << "Creating minimax tree\n";

			root = new Node(true, me, state, NULL, -1);
			int validMoves[64];
			int numValidMoves = get_valid_moves(state, me, validMoves);

			std::cout << "\n";
			std::cout << message_prefix << " Expanding root\n";
			unsigned int old_total = total_nodes_expanded;
			time_left = (me == 1)? t1 : t2;
			stopwatch<std::milli, double> sw;
			sw.start();
			expandNode(root, 1,sw);
			sw.stop();
			total_time_spent += sw.count();
			std::cout << message_prefix << " Nodes expanded on this move: " << total_nodes_expanded - old_total << "\n";
			std::cout << message_prefix << " Total nodes expanded so far: " << total_nodes_expanded << "\n";
			std::cout << message_prefix << " Time spent on this move: " << sw.count() << "\n";
			if (hit_time_limit) {
				std::cout << message_prefix << " Hit time limit on expansion\n";
				times_hit_time_limit++;
			}
			std::cout << message_prefix << " Times hit time limit: " << times_hit_time_limit << "\n";
			std::cout << message_prefix << " Total time spent so far: " << total_time_spent << "\n";
			std::cout << message_prefix << " Current node/time spent ratio: " << total_nodes_expanded/total_time_spent << "\n";
			hit_time_limit = false;

			std::cout << message_prefix << " Searching children for best node\n";
			int bestValue = INT_MIN;
			Node* bestNode = NULL;
			int k = 0;
			std::cout << "root children size: " << root->children.size() << "\n";
			for (int i = 0; i < root->children.size(); i++) {
				Node* child = root->children[i];
				if (child->value > bestValue) {
					bestValue = child->value;
					bestNode = child;
					k = i;
				}
			}

			if (bestValue == INT_MIN) {
				//handle edge cases at end

				//delete tree
				delete root;

				return randomMove();
			}
			else {
				int validMoves[64];
				int numValidMoves = get_valid_moves(state, me, validMoves);
				int move = validMoves[k];
				std::cout << message_prefix << "Best Move: " << (move/8) << "," << (move%8) << " val: " << bestValue << "\n";
				//cout << message_prefix << "Returning the best move: " << move << "\n";

				//delete tree
				delete root;

				return move;
			}
		}
	}

	void expandNode(Node* cur_node, int current_depth, stopwatch<std::milli, double>& sw) {
		//cout << "[baseline minimax " << me << "] Expanding " << cur_node << " at depth " << current_depth << "\n";
		total_nodes_expanded++;
		//Flip next player
		int next_player = -1;
		if (cur_node->player == 1)
			next_player = 2;
		else
			next_player = 1;

		sw.stop();
		if (sw.count() > 2000) { //give max 2 seconds per move so finish in time
			hit_time_limit = true;
			cur_node->value = cur_node->getHeuristicValue(me);
		}
		else if (current_depth < depth_limit) {
			//cout << "    getting valid moves\n";
			int validMoves[64];
			int numValidMoves = get_valid_moves(cur_node->state, cur_node->player, validMoves);

			//if you reach point where no valid moves, this node would be other player's turn
			if (numValidMoves == 0) {
				//cout << message_prefix << " No valid moves at this node \n";

				//discourage not being able to move and giving other guy another turn
				if (cur_node->player == me) //no move for me
					cur_node->value = cur_node->calculateValue(me) - 120;
				else //no move for him
					cur_node->value = cur_node->calculateValue(me) + 120;
				return;
			}

			int n = ((current_depth == 1 && run_omp) ? 6 : 1);
			//cur_node->children.reserve(numValidMoves);

			#pragma omp parallel num_threads(n)
			{
				#pragma omp for schedule(dynamic)
				for (int i = 0; i < numValidMoves; i++) {
					//if (current_depth == 1 && run_omp)
					//	cout << "Starting " << i << ", N is " << n << "\n";
					int move = validMoves[i];

					int next_state[8][8];
					for (int i = 0; i < 8; i++) {
						for (int j = 0; j < 8; j++) {
							next_state[i][j] = cur_node->state[i][j];
						}
					}

					next_state[(move/8)][(move % 8)] = cur_node->player;
					updateState(move,cur_node->player,next_state);

					Node* next_node = new Node(!cur_node->isMax, next_player, next_state, cur_node, move);

					#pragma omp critical
					cur_node->children.push_back(next_node);
					//cur_node->children[i] = next_node;

					expandNode(next_node, current_depth + 1, sw);
					//if (current_depth == 1 && run_omp)
					//	cout << "Ending " << i << "\n";
				}
			}

			cur_node->calculateValue(me);
		}
		else {
			//cout << "No valid moves, so just calculating value\n";
			cur_node->calculateValue(me);
		}
	}

	void updateState(int move,int player,int in_state[8][8]) {
		//std::cout << "updating state\n";
		int opponent = -1;
		if (player == 1)
			opponent = 2;
		else {
			opponent = 1;
		}
		int directions[8][2] = {{1,0},{0,1},{0,-1},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
		//std::cout << "exploring each direction\n";
		for (int i = 0; i < 8; i++) {
			//std::cout << "  " << i << "\n";
			int rdir = directions[i][0];
			int cdir = directions[i][1];

			int starting_row = (move / 8);
			int starting_col = (move % 8);
			int row = starting_row + rdir;
			int col = starting_col + cdir;
			if (row >= 0 && col >= 0 && row < 8 && col < 8 && in_state[row][col] == opponent) {
				int sequence[8][2];
				int seq_len = 0;
				while (row >= 0 && col >= 0 && row < 8 && col < 8 && in_state[row][col] == opponent) {
					sequence[seq_len][0] = row;
					sequence[seq_len][1] = col;
					seq_len++;
					if (seq_len >= 8) {
						cout << message_prefix << "WARNING: Seq len >= 8\n";
						cout << message_prefix << "instate = opponent at " << row << "," << col << "\n";
					}
					row += rdir;
					col += cdir;
				}
				if (row >= 0 && col >= 0 && row < 8 && col < 8 && in_state[row][col] == player) {
					for (int j = 0; j < seq_len; j++) {
						int r = sequence[j][0];
						int c = sequence[j][1];
						if (r >= 0 && c >= 0 && r < 8 && c < 8) {
							in_state[r][c] = player;
						}
					}
				}
			}

		}
	}

	void print_nodes() {
		for (int i = 0; i < root->children.size(); i++) {
			//std::cout << "    " << root->children[i]->moveFromParent << "\n";
			std::cout << "    node: " << root->children[i]->value << "\n";
			for (int j = 0; j < root->children[i]->children.size(); j++) {
				std::cout << "        node: " << root->children[i]->children[j]->value << "\n";
				for (int k = 0; k < root->children[i]->children[j]->children.size(); k++) {
					std::cout << "            node: " << root->children[i]->children[j]->children[k]->value << "\n";
				}
			}
		}
	}

};
