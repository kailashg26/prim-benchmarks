#include <perfcounter.h>
#include <stdio.h>

__host uint32_t nb_ins;

#define NUM_STATES 50
#define NUM_ACTIONS 10
#define LEARNING_RATE 0.1
#define DISCOUNT_FACTOR 0.9
#define NUM_EPISODES 50

// Define Q-table
double Q[NUM_STATES][NUM_ACTIONS] = {0};

// Define experiences (state, action, reward, next_state)
typedef struct {
    int state;
    int action;
    double reward;
    int next_state;
} Experience;

// Define an array to store experiences
Experience experiences[NUM_EPISODES] = {
    {0, 2, 1, 1},
    {1, 0, 2, 2},
    {2, 3, 0, 0},
    {3, 1, 1, 4},
    {4, 2, -1, 7},
    {5, 0, 2, 2},
    {6, 1, 0, 3},
    {7, 3, 1, 9},
    {8, 2, -1, 5},
    {9, 0, 2, 6},
    {10, 1, 0, 2},
    {11, 3, 1, 5},
    {12, 2, -1, 8},
    {13, 0, 2, 6},
    {14, 1, 0, 4},
    {15, 3, 1, 8},
    {16, 2, -1, 9},
    {17, 0, 2, 0},
    {18, 1, 0, 1},
    {19, 3, 1, 3},
    {20, 4, 0.5, 6}, // Additional fake experience
    {21, 5, -0.5, 8}, // Additional fake experience
    {22, 6, 0.2, 7}, // Additional fake experience
    {23, 7, -0.2, 4}, // Additional fake experience
    {24, 8, 0.3, 9}, // Additional fake experience
    {25, 9, -0.3, 1}, // Additional fake experience
    {26, 0, 0.5, 3}, // Additional fake experience
    {27, 1, -0.5, 5}, // Additional fake experience
    {28, 2, 0.4, 2}, // Additional fake experience
    {29, 3, -0.4, 6}, // Additional fake experience
    {30, 4, 0.6, 4}, // Additional fake experience
    {31, 5, -0.6, 8}, // Additional fake experience
    {32, 6, 0.7, 2}, // Additional fake experience
    {33, 7, -0.7, 9}, // Additional fake experience
    {34, 8, 0.8, 7}, // Additional fake experience
    {35, 9, -0.8, 3}, // Additional fake experience
    {36, 0, 0.9, 0}, // Additional fake experience
    {37, 1, -0.9, 5}, // Additional fake experience
    {38, 2, 0.1, 1}, // Additional fake experience
    {39, 3, -0.1, 8}, // Additional fake experience
    {40, 4, 0.2, 6}, // Additional fake experience
    {41, 5, -0.2, 4}, // Additional fake experience
    {42, 6, 0.3, 9}, // Additional fake experience
    {43, 7, -0.3, 3}, // Additional fake experience
    {44, 8, 0.4, 7}, // Additional fake experience
    {45, 9, -0.4, 2}, // Additional fake experience
    {46, 0, 0.5, 5}, // Additional fake experience
    {47, 1, -0.5, 1}, // Additional fake experience
    {48, 2, 0.6, 8}, // Additional fake experience
    {49, 3, -0.6, 6} // Additional fake experience
};

// Q-learning update function for a batch of experiences
void updateQBatch() {
    for (int episode = 0; episode < NUM_EPISODES; ++episode) {
        int state = experiences[episode].state;
        int action = experiences[episode].action;
        double reward = experiences[episode].reward;
        int next_state = experiences[episode].next_state;

        double max_next_q = Q[next_state][0];
        for (int a = 1; a < NUM_ACTIONS; ++a) {
            if (Q[next_state][a] > max_next_q) {
                max_next_q = Q[next_state][a];
            }
        }

        // Compute the Q-value update for this experience (but don't update the Q-table yet)
        double q_update = LEARNING_RATE * (reward + DISCOUNT_FACTOR * max_next_q - Q[state][action]);

        // Accumulate Q-value updates (offline)
        Q[state][action] += q_update;
    }
}

int main() {
    // Perform offline Q-learning by updating the Q-table once after processing the entire batch
    perfcounter_config(COUNT_INSTRUCTIONS, true);
    updateQBatch();

    // Print the learned Q-table
    printf("Learned Q-table:\n");
    for (int s = 0; s < NUM_STATES; ++s) {
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            printf("Q[%d][%d] = %.2f   ", s, a, Q[s][a]);
        }
        printf("\n");
    }

    nb_ins = perfcounter_get();

    return 0;
}

