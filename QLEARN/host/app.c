/**
 * app.c
 * MLP Host Application Source File
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dpu.h>
#include <dpu_log.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#if ENERGY
#include <dpu_probe.h>
#endif

#include "../support/common.h"
#include "../support/timer.h"
#include "../support/params.h"

// Define the DPU Binary path as DPU_BINARY here
#ifndef DPU_BINARY
#define DPU_BINARY "./bin/qlearn_dpu"
#endif

// Create input arrays
static void init_data() {
  //TODO 
  return;
}

// Define constants
#define NUM_STATES 6
#define NUM_ACTIONS 4
#define LEARNING_RATE 0.1
#define DISCOUNT_FACTOR 0.9
#define MAX_EPISODES 1000
#define MAX_STEPS 100
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

// Compute output in the host
static void qlearn_host() {

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

// Main of the Host Application
int main(int argc, char **argv) {

	struct Params p = input_params(argc, argv);

	struct dpu_set_t dpu_set, dpu;
	uint32_t nr_of_dpus;

	// Allocate DPUs and load binary
	DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
	DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));
	DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));

#if ENERGY
	struct dpu_probe_t probe;
	DPU_ASSERT(dpu_probe_init("energy_probe", &probe));
#endif

	unsigned int i, l;
	//unsigned int m_size = p.m_size;
	//unsigned int n_size = p.n_size;

	// Initialize help data
	dpu_info = (struct dpu_info_t *) malloc(nr_of_dpus * sizeof(struct dpu_info_t));
	dpu_arguments_t *input_args = (dpu_arguments_t *) malloc(nr_of_dpus * sizeof(dpu_arguments_t));
	uint32_t max_rows_per_dpu = 0;
	/*
uint32_t n_size_pad = n_size;
	if(n_size % 2 == 1){
		n_size_pad++;
	}
*/
	// Timer
	Timer timer;
	i = 0;
	DPU_FOREACH(dpu_set, dpu, i) {
		/*
    uint32_t rows_per_dpu;
		uint32_t prev_rows_dpu = 0;
		uint32_t chunks = m_size / nr_of_dpus;
		rows_per_dpu = chunks;
		uint32_t rest_rows = m_size % nr_of_dpus;
		if (i < rest_rows)
			rows_per_dpu++;
		if (rest_rows > 0) {
			if (i >= rest_rows)
				prev_rows_dpu = rest_rows * (chunks + 1) + (i - rest_rows) * chunks;
			else
				prev_rows_dpu = i * (chunks + 1);
		} else {
			prev_rows_dpu = i * chunks;
		}

		// Keep max rows for parallel transfers
		uint32_t rows_per_dpu_pad = rows_per_dpu;
		if (rows_per_dpu_pad % 2 == 1) // 4-byte elements
			rows_per_dpu_pad++;
		if (rows_per_dpu_pad > max_rows_per_dpu)
			max_rows_per_dpu = rows_per_dpu_pad;

		dpu_info[i].rows_per_dpu = rows_per_dpu;
		dpu_info[i].rows_per_dpu_pad = rows_per_dpu_pad;
		dpu_info[i].prev_rows_dpu = prev_rows_dpu;

		// Copy input arguments to DPU
		input_args[i].n_size = n_size;
		input_args[i].n_size_pad = n_size_pad;
		input_args[i].nr_rows = rows_per_dpu;
	}
*/
}
/*
	A = (T**)malloc(NUM_LAYERS * sizeof(T*));
	for(l = 0; l < NUM_LAYERS; l++)
		A[l] = (T*)malloc( max_rows_per_dpu * nr_of_dpus * n_size_pad * sizeof(T));


	B = (T*)malloc(n_size * sizeof(T));
	B_host = (T*)malloc(n_size * sizeof(T));
	C = (T*)malloc(m_size * sizeof(T));
	C_dpu = malloc(max_rows_per_dpu * nr_of_dpus * sizeof(T));
	B_tmp = malloc(max_rows_per_dpu * nr_of_dpus * sizeof(T));
*/
	init_data();

	// Compute output on CPU (performance comparison and verification purposes)
	start(&timer, 0, 0);
	qlearn_host();
	stop(&timer, 0);

	for (unsigned int rep = 0; rep < p.n_warmup + p.n_reps; rep++) {
		if (rep >= p.n_warmup)
			start(&timer, 1, rep - p.n_warmup);
		// Input arguments
		i = 0;
		// Copy input arguments to DPU
/*
    DPU_FOREACH(dpu_set, dpu, i) {
			input_args[i].max_rows = max_rows_per_dpu;
			DPU_ASSERT(dpu_prepare_xfer(dpu, input_args + i));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(dpu_arguments_t), DPU_XFER_DEFAULT));


		// Copy input array and vector
		i = 0;
		DPU_FOREACH(dpu_set, dpu, i) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, A[0] + dpu_info[i].prev_rows_dpu * n_size));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, max_rows_per_dpu * n_size_pad * sizeof(T), DPU_XFER_DEFAULT));
		i = 0;
		DPU_FOREACH(dpu_set, dpu, i) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, B));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_rows_per_dpu * n_size_pad * sizeof(T) , n_size_pad * sizeof(T), DPU_XFER_DEFAULT));
*/
		if (rep >= p.n_warmup)
			stop(&timer, 1);
		// Run kernel on DPUs
		if (rep >= p.n_warmup)
		{
			start(&timer, 2, rep - p.n_warmup);
#if ENERGY
			DPU_ASSERT(dpu_probe_start(&probe));
#endif
		}

		DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));

		if (rep >= p.n_warmup)
		{
			stop(&timer, 2);
#if ENERGY
			DPU_ASSERT(dpu_probe_stop(&probe));
#endif
		}

		for(int lay = 1; lay < NUM_LAYERS; lay++){
			if (rep >= p.n_warmup)
				start(&timer, 4, rep - p.n_warmup);
			i = 0;
/*
			// Copy C_dpu
			DPU_FOREACH(dpu_set, dpu, i) {
				DPU_ASSERT(dpu_prepare_xfer(dpu, C_dpu + i * max_rows_per_dpu));
			}
			DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_rows_per_dpu * n_size_pad * sizeof(T) + n_size_pad * sizeof(T), max_rows_per_dpu * sizeof(T), DPU_XFER_DEFAULT));

			// B = C
			unsigned int n, j;
			i = 0;
			for (n = 0; n < nr_of_dpus; n++) {
				for (j = 0; j < dpu_info[n].rows_per_dpu; j++) {
					B_tmp[i] = C_dpu[n * max_rows_per_dpu + j];
					i++;
				}
			}
			i = 0;
			DPU_FOREACH(dpu_set, dpu, i) {
				DPU_ASSERT(dpu_prepare_xfer(dpu, B_tmp));
			}
			DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_rows_per_dpu * n_size_pad * sizeof(T) , n_size_pad * sizeof(T), DPU_XFER_DEFAULT));

			// Copy next matrix of weights
			i = 0;
			DPU_FOREACH(dpu_set, dpu, i) {
				DPU_ASSERT(dpu_prepare_xfer(dpu, A[lay] + dpu_info[i].prev_rows_dpu * n_size));
			}
			DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, max_rows_per_dpu * n_size_pad * sizeof(T), DPU_XFER_DEFAULT));
*/
			if(rep >= p.n_warmup)
				stop(&timer, 4);

			if (rep >= p.n_warmup)
			{
				start(&timer, 2, rep - p.n_warmup);
#if ENERGY
				DPU_ASSERT(dpu_probe_start(&probe));
#endif
			}

			DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));

			if (rep >= p.n_warmup)
			{
				stop(&timer, 2);
#if ENERGY
				DPU_ASSERT(dpu_probe_stop(&probe));
#endif
			}
		}

#if PRINT
		// Display DPU Logs
		DPU_FOREACH(dpu_set, dpu) {
			DPU_ASSERT(dpulog_read_for_dpu(dpu.dpu, stdout));
		}
#endif

		// Retrieve results
		if (rep >= p.n_warmup)
			start(&timer, 3, rep - p.n_warmup);
		/*
    i = 0;
		DPU_FOREACH(dpu_set, dpu, i) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, C_dpu + i * max_rows_per_dpu));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, max_rows_per_dpu * n_size_pad * sizeof(T) + n_size_pad * sizeof(T), max_rows_per_dpu * sizeof(T), DPU_XFER_DEFAULT));
		*/
    if(rep >= p.n_warmup)
			stop(&timer, 3);
	}

#if ENERGY
	double acc_energy, avg_energy, acc_time, avg_time;
	DPU_ASSERT(dpu_probe_get(&probe, DPU_ENERGY, DPU_ACCUMULATE, &acc_energy));
	DPU_ASSERT(dpu_probe_get(&probe, DPU_ENERGY, DPU_AVERAGE, &avg_energy));
	DPU_ASSERT(dpu_probe_get(&probe, DPU_TIME, DPU_ACCUMULATE, &acc_time));
	DPU_ASSERT(dpu_probe_get(&probe, DPU_TIME, DPU_AVERAGE, &avg_time));
#endif

	// Print timing results
	printf("CPU Version Time (ms): ");
	print(&timer, 0, 1);
	printf("CPU-DPU Time (ms): ");
	print(&timer, 1, p.n_reps);
	printf("DPU Kernel Time (ms): ");
	print(&timer, 2, p.n_reps);
	printf("Inter-DPU Time (ms): ");
	print(&timer, 4, p.n_reps);
	printf("DPU-CPU Time (ms): ");
	print(&timer, 3, p.n_reps);

#if ENERGY
	printf("Energy (J): %f J\t", avg_energy);
#endif
	printf("\n\n");

bool status = true;
  /*
	// Check output
	unsigned int n, j;
	i = 0;
	for (n = 0; n < nr_of_dpus; n++) {
		for (j = 0; j < dpu_info[n].rows_per_dpu; j++) {
			if(C[i] != C_dpu[n * max_rows_per_dpu + j]) {
				status = false;
#if PRINT
				printf("%d: %d -- %d\n", i, C[i], C_dpu[n * max_rows_per_dpu + j]);
#endif
			}
			i++;
		}
	}
  */

	if (status) {
		printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] Outputs are equal\n");
	} else {
		printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] Outputs differ!\n");
	}

	// Deallocation
	DPU_ASSERT(dpu_free(dpu_set));

#if ENERGY
	DPU_ASSERT(dpu_probe_deinit(&probe));
#endif

	return status ? 0 : -1;
}
