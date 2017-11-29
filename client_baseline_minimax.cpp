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

using namespace std;

class ClientBaselineMiniMaxPlayer : public ClientPlayer {
public:

	Node* root;
	int depth_limit;
	unsigned int total_nodes_expanded;
	std::string message_prefix;

	ClientBaselineMiniMaxPlayer(string host, int port, int in_me, int depth_limit=5) : ClientPlayer(host, port, in_me) {
		this->depth_limit = depth_limit;
		this->root = NULL;
		total_nodes_expanded = 0;

		stringstream ss;
		ss << "[baseline minimax " << me << "] ";
		message_prefix = ss.str();
	}

	virtual ~ClientBaselineMiniMaxPlayer() { }

	int randomMove() {
		cout << message_prefix << "Returning random move\n";
		cout << message_prefix << "numValidMoves: ";
		cout << numValidMoves << "\n";
		int myMove;
		if (numValidMoves > 0) {
			int randomIndex = rand() % numValidMoves;
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
			get_valid_moves(state,me);
//			std::cout << "Valid moves for root: \n";
//			for (int i = 0; i < numValidMoves; i++)
//				std::cout << (validMoves[i]/8) << "," << ( (validMoves[i]%8)) << " ; ";

			std::cout << "\n";
			std::cout << message_prefix << " Expanding root\n";
			unsigned int old_total = total_nodes_expanded;
			expandNode(root, 1);
			std::cout << message_prefix << " Nodes expanded on this move: " << total_nodes_expanded - old_total << "\n";
			std::cout << message_prefix << " Total nodes expanded so far: " << total_nodes_expanded << "\n";

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

			if (bestNode == NULL) {
				//handle edge cases at end
				return randomMove();
			}
			else {
				std::cout << message_prefix << bestNode->moveFromParent << " val: " << bestValue << "\n";
				get_valid_moves(state,me);
				int move = validMoves[k];
				cout << message_prefix << "Returning the best move: " << move << "\n";
				return move;
			}
		}
	}

	void expandNode(Node* cur_node, int current_depth) {
		//cout << "[baseline minimax " << me << "] Expanding " << cur_node << " at depth " << current_depth << "\n";

		total_nodes_expanded++;
		//Flip next player
		int next_player = -1;
		if (cur_node->player == 1)
			next_player = 2;
		else
			next_player = 1;

		if (current_depth < depth_limit) {
			//cout << "    getting valid moves\n";
			get_valid_moves(cur_node->state, cur_node->player);

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

			//create new nodes from validMoves
			//cout << "    creating new nodes\n";
			for (int i = 0; i < numValidMoves; i++) {
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
				//next_node->value = next_node->getHeuristicValue(me);
				cur_node->children.push_back(next_node);
				//cout << "Expanding " << cur_node << " at depth " << current_depth + 1 << "\n";
				expandNode(next_node, current_depth + 1);
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
			if (row > 0 && col > 0 && row < 8 && col < 8 && in_state[row][col] == opponent) {
				int sequence[8][2];
				int seq_len = 0;
				while (row > 0 && col > 0 && row < 8 && col < 8 && in_state[row][col] == opponent) {
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
				if (row > 0 && col > 0 && row < 8 && col < 8 && in_state[row][col] == player) {
					for (int j = 0; j < seq_len; j++) {
						int r = sequence[seq_len][0];
						int c = sequence[seq_len][1];
						if (r >= 0 && c >= 0 && r < 8 && c < 8)
							in_state[r][c] = player;
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
