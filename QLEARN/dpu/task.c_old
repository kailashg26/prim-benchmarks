#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <barrier.h>
#include <seqread.h>

#include "../support/common.h"

// Define constants
#define NUM_STATES 6
#define NUM_ACTIONS 4
#define LEARNING_RATE 0.1
#define DISCOUNT_FACTOR 0.9
#define MAX_EPISODES 1000
#define MAX_STEPS 100

// Barrier
BARRIER_INIT(my_barrier, NR_TASKLETS);

// Define actions
enum Actions {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Define the Q-table
double Q[NUM_STATES][NUM_ACTIONS];

// Define the rewards matrix
int rewards[NUM_STATES][NUM_ACTIONS] = {
    {0, -1, -1, -1},  // State 0
    {-1, -1, -1, 10}, // State 1
    {-1, -1, -1, -1}, // State 2
    {-1, -1, -1, -1}, // State 3
    {-1, -1, -1, -1}, // State 4
    {-1, -1, -1, 0}   // State 5 (Goal)
};

// Helper function to choose an action based on epsilon-greedy policy
int chooseAction(int state, double epsilon) {
    static int exploration_count = 0;
    if ((double)exploration_count / MAX_STEPS < epsilon) {
        // Deterministic exploration: choose an action sequentially
        int action = exploration_count % NUM_ACTIONS;
        exploration_count++;
        return action;
    } else {
        int bestAction = 0;
        for (int a = 1; a < NUM_ACTIONS; ++a) {
            if (Q[state][a] > Q[state][bestAction]) {
                bestAction = a; // Exploitation: choose the action with the highest Q-value
            }
        }
        return bestAction;
    }
}

// Q-learning algorithm
void qLearning() {
    for (int episode = 0; episode < MAX_EPISODES; ++episode) {
        int state = 0; // Start in state 0
        int steps = 0;

        while (state != 5 && steps < MAX_STEPS) { // Continue until goal is reached or max steps
            int action = chooseAction(state, 0.1); // Epsilon-greedy action selection

            // Perform the chosen action
            int next_state = action;
            int reward = rewards[state][action];

            // Update the Q-value using the Q-learning update rule
            Q[state][action] = Q[state][action] + LEARNING_RATE * (reward + DISCOUNT_FACTOR * Q[next_state][chooseAction(next_state, 0.0)] - Q[state][action]);

            state = next_state;
            steps++;
        }
    }
}

// Main function
int main() {
    unsigned int tasklet_id = me();
#if PRINT
    printf("tasklet_id = %u\n", tasklet_id);
#endif
    if (tasklet_id == 0){ // Initialize once the cycle counter
        mem_reset(); // Reset the heap
    }
    // Barrier
	barrier_wait(&my_barrier);

    qLearning();

    // Print the learned Q-table
    printf("Learned Q-Table:\n");
    for (int s = 0; s < NUM_STATES; ++s) {
        printf("State %d: ", s);
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            printf("%.2f  ", Q[s][a]);
        }
        printf("\n");
    }

    return 0;
}

