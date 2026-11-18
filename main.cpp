#include <iostream>
#include <string>
#include "bowling_frame.h"
#include <random>

using namespace std;

std::random_device dev_main;
std::mt19937 rng_main(dev_main());

int current_game_total(Bowling_Frame * first_frame)
{
	Bowling_Frame * temp = first_frame;
	int game_total = 0;
	int temp_total = 0;
	while(temp)
	{
		game_total += temp->frame_score();
		temp = temp->next_frame();
	}
	return game_total;	
}

int main(int argc, char* argv[])
{
	//parameter check
	if (argc < 2) ///error if no amount of players is given
	{
		cout << "Usage: ./a.out <num_players>"<< endl;
		return 0;
	}
	int num_player = stoi(argv[1]);

	if (num_player < 1 || num_player > 3) ///error if the number of players is not between 1 and 3
	{
		cout << "Error: Number of players must be between 1 and 3" << endl;
		return 0;
	}
	
	cout << "Player count is: " << num_player << endl;

	//make frames
	Bowling_Frame * players[3] = {nullptr, nullptr, nullptr};
	
	for (int i = 0; i < num_player; i++)
	{
		Bowling_Frame * frames[10];

		for (int j = 0; j < 10; j++) ///create 10 frames for each player
		{
			frames[j] = new Bowling_Frame(j+1);
		}
		for (int j = 0; j < 9; j++) ///linking the frames together
		{
			frames[j]->next_frame(frames[j+1]);
			frames[j+1]->prev_frame(frames[j]);
		}
		players[i] = frames[0]; //store the first frame of each player in the players array
	}

	// update scores for each player
	for (int i = 0; i < num_player; i++)
	{
		for (int k = 0; k < 30; k++)   // enough calls for 10 frames + 10th frame bonus rolls
			players[i]->update_scores();
	}

	// printing scores for each player
	for (int i = 0; i < num_player; i++)
	{
		Bowling_Frame* t = players[i];

		while (t != NULL)
		{
			cout << "Frame " << t->frame_num() << ": " << endl
			     << " Score 1: " << t->score1() << endl
			     << " Score 2: " << t->score2() << endl;

			// 10th frame spare/strike: print bonus roll(s)
			if (t->frame_num() == 10 && t->strike())
			{
				// strike: Score 2 is bonus 1, Score 3 is bonus 2
				cout << " Score 3: " << t->score3() << endl;
			}
			else if (t->frame_num() == 10 && t->spare())
			{
				// spare: one bonus roll
				cout << " Score 3: " << t->score3() << endl;
			}

			t = t->next_frame();
		}

		cout << "Total Game Score for player " << (i + 1) << ": "
		     << current_game_total(players[i]) << endl;
	}

	return 0;
}
