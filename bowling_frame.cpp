#include "bowling_frame.h"
#include <random>

std::random_device dev;
std::mt19937 rng(dev());

Bowling_Frame::Bowling_Frame(int fnum)
{
	score1_ = 0;
	score2_ = 0;
	strike_ = false;
	spare_ = false;
	frame_num_ = fnum;
	frame_score_ = 0;

	next_frame_ = NULL;
	prev_frame_ = NULL;

}

Bowling_Frame::~Bowling_Frame()
{
	free(next_frame_);
	free(prev_frame_);
}

Bowling_Frame* Bowling_Frame::next_frame() { return next_frame_; }
void Bowling_Frame::next_frame( Bowling_Frame * new_frame ) { next_frame_ = new_frame; }

Bowling_Frame* Bowling_Frame::prev_frame() { return prev_frame_; }
void Bowling_Frame::prev_frame( Bowling_Frame * new_frame ) { prev_frame_ = new_frame; }

int Bowling_Frame::score1()				{ return score1_; }
void Bowling_Frame::score1( int score )	{ score1_ = score; }

int Bowling_Frame::score2()				{ return score2_; }
void Bowling_Frame::score2( int score )	{ score2_ = score; }

bool Bowling_Frame::strike() 			{ return strike_; }
void Bowling_Frame::strike(bool value )	{ strike_ = value; }

bool Bowling_Frame::spare() 			{ return spare_; }
void Bowling_Frame::spare(bool value )	{ spare_ = value; }

int Bowling_Frame::frame_num()
{
	return frame_num_;
}

int Bowling_Frame::frame_score()
{
	return frame_score_; 
}

void Bowling_Frame::update_scores()
{
	struct RollState
    {
        Bowling_Frame* frame;
        int rolls[3];
        int count;
        RollState* next;
    };

    static RollState* states = nullptr;

    RollState* state = states;

    while (state && state->frame != this)
        state = state->next;

    if (!state)
    {
        state = new RollState;
        state->frame = this;
        state->count = 0;
        state->next = states;
        states = state;
    }

    int allowed = 2;

    if (frame_num_ < 10 &&
        state->count > 0 &&
        state->rolls[0] == 10)
    {
        allowed = 1;
    }

    if (frame_num_ == 10 && state->count >= 2)
    {
        if (state->rolls[0] == 10 ||
            state->rolls[0] + state->rolls[1] == 10)
        {
            allowed = 3;
        }
    }

    if (state->count < allowed)
    {
        int pins = 10;

        if (state->count == 1 &&
            state->rolls[0] != 10)
        {
            pins = 10 - state->rolls[0];
        }

        if (frame_num_ == 10 &&
            state->count == 2 &&
            state->rolls[0] == 10 &&
            state->rolls[1] != 10)
        {
            pins = 10 - state->rolls[1];
        }

        std::uniform_int_distribution<int> dist(0, pins);
        int score = dist(rng);

        state->rolls[state->count] = score;
        state->count++;

        score1_ = state->rolls[0];

        if (state->count >= 2)
            score2_ = state->rolls[1];

        roll_one_ = true;
        roll_two_ = (state->count >= 2);

        strike_ = (score1_ == 10);

        spare_ = (!strike_ &&
                  state->count >= 2 &&
                  score1_ + score2_ == 10);
    }

    Bowling_Frame* first = this;

    while (first->prev_frame_)
        first = first->prev_frame_;

    Bowling_Frame* current = first;

    while (current)
    {
        RollState* currentState = states;

        while (currentState &&
               currentState->frame != current)
        {
            currentState = currentState->next;
        }

        if (currentState)
        {
            int rolls[21];
            int count = 0;

            Bowling_Frame* temp = current;

            while (temp)
            {
                RollState* s = states;

                while (s && s->frame != temp)
                    s = s->next;

                if (s)
                {
                    for (int i = 0; i < s->count; i++)
                        rolls[count++] = s->rolls[i];
                }

                temp = temp->next_frame_;
            }

            int score = 0;

            if (current->frame_num_ == 10)
            {
                for (int i = 0;
                     i < currentState->count; i++)
                {
                    score += currentState->rolls[i];
                }
            }
            else if (currentState->rolls[0] == 10)
            {
                score = 10;

                if (count > 1)
                    score += rolls[1];

                if (count > 2)
                    score += rolls[2];
            }
            else if (currentState->count >= 2 &&
                     currentState->rolls[0] +
                     currentState->rolls[1] == 10)
            {
                score = 10;

                if (count > 2)
                    score += rolls[2];
            }
            else
            {
                for (int i = 0;
                     i < currentState->count; i++)
                {
                    score += currentState->rolls[i];
                }
            }

            current->frame_score_ = score;
        }

        current = current->next_frame_;
    }
}

