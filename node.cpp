/*
 * node.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: derekhancock
 */

#pragma once
#include <limits.h>
#include <iostream>
#include <vector>

class Node {
public:
	int state[8][8];
	int value;
	int isMax;
	int player;
	vector<Node*> children;
	Node* parent;
	bool visited;
	int moveFromParent;
	int const weights[8][8] = {{120, -20,  20,   5,   5,  20, -20, 120},
						    {-20, -40,  -5,  -5,  -5,  -5, -40, -20},
						    {20,  -5,  15,   3,   3,  15,  -5,  20},
						    {5,  -5,   3,   3,   3,   3,  -5,   5},
						    {5,  -5,   3,   3,   3,   3,  -5,   5},
						    {20,  -5,  15,   3,   3,  15,  -5,  20},
						    {-20, -40,  -5,  -5,  -5,  -5, -40, -20},
						    {120, -20,  20,   5,   5,  20, -20, 120}};

	Node(bool isMax,int in_player, int in_state[8][8], Node* parent, int moveFromParent) {
		this->isMax = isMax;
		this->player = in_player;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				this->state[i][j] = in_state[i][j];
			}
		}
		this->parent = parent;
		this->moveFromParent = moveFromParent;
		this->value = -1;
		this->visited = false;
	}

	~Node() {
		delete parent;
	}

	int getHeuristicValue(int me) {
		int diff = 0;
		bool killed = true;
		bool died = true;
		int count = 0;
		int state_value = 0;
		int opponent;
		if (me == 1)
			opponent = 2;
		else
			opponent = 1;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (state[i][j] == me) {
					diff += 1;
					count += 1;
					died = false;
					state_value += weights[i][j];
				}
				else if (state[i][j] == opponent) {
					diff -= 1;
					count += 1;
					killed = false;
				}
			}
		}

		// Corner occupancy
		int my_tiles = 0;
		int opp_tiles = 0;
		if(state[0][0] == me) my_tiles++;
		else if(state[0][0] == opponent) opp_tiles++;
		if(state[0][7] == me) my_tiles++;
		else if(state[0][7] == opponent) opp_tiles++;
		if(state[7][0] == me) my_tiles++;
		else if(state[7][0] == opponent) opp_tiles++;
		if(state[7][7] == me) my_tiles++;
		else if(state[7][7] == opponent) opp_tiles++;
		int c = 25 * (my_tiles - opp_tiles);

		//cout << state_value << "; " << c << "\n";
		//cout << "  " << my_tiles << "; " << opp_tiles << "\n";
		state_value = state_value + c;
		if (killed || (count == 64 && diff > 0))
			return state_value + 100;
		else if (died || (count == 64 && diff < 0))
			return state_value - 100;
		else
			return state_value;
	}

	int calculateValue(int me) {

		if (children.size() == 0) {
			value = getHeuristicValue(me);
		}
		else {
			int cur_value;
			if (isMax) {
				cur_value = -INT_MAX + 100;
				for (int i = 0; i < children.size(); i++) {
					if (children[i]->value > cur_value)
						cur_value = children[i]->value;
				}
				if (cur_value == -INT_MAX)
					cout << "ERROR: CUR_VALUE IS STILL INT_MIN\n";
			}
			else {
				cur_value = INT_MAX - 100;
				for (int i = 0; i < children.size(); i++) {
					if (children[i]->value < cur_value)
						cur_value = children[i]->value;
				}
				if (cur_value == INT_MAX)
					cout << "ERROR: CUR_VALUE IS STILL INT_MAX\n";
			}

			value = cur_value;
		}
		return value;

	}


};
