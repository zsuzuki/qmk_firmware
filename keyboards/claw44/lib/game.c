#include <stdio.h>
#include "claw44.h"

int  last_time    = 0;
int  elapsed_time = 0;
char gameline[21] = {"press s key"};
int  next_key     = 0;
int  check_key    = 0;
int  step         = -1;
int  score        = 0;
int  interval     = 250;
bool on_game      = false;
bool success      = false;
bool on_miss      = false;

void update_key(void) {
    int next  = abs(last_time) % 25;
    next_key  = next + 'A';
    check_key = next + 4;
}

void on_update(void) {
    if (step < 0 || success) {
        // start
        step    = 19;
        success = false;
    } else {
        step--;
    }
    if (step < 0) {
        // lose
        on_game = false;
        snprintf(gameline, sizeof(gameline), "gameover: %d", score);
        return;
    }

    for (int i = 0; i < 20; i++) {
        gameline[i] = i == step ? next_key : ' ';
    }

    on_miss      = false;
    gameline[20] = '\0';
}

void game_update(void) {
    elapsed_time = timer_elapsed(last_time);
    if (elapsed_time >= interval) {
        last_time = timer_read();
        if (on_game) on_update();
    }
}
void game_keyupdate(uint16_t keycode) {
    if (on_game) {
        if (step == 0 && keycode == check_key && on_miss == false) {
            // win
            update_key();
            success = true;
            score++;
            interval -= interval > 50 ? 8 : 0;
        } else {
            on_miss = true;
        }
    } else {
        if (keycode == 22) {
            // game start
            update_key();
            on_game  = true;
            score    = 0;
            interval = 200;
        }
    }
}

const char* read_game(void) { return success ? "success" : gameline; }
