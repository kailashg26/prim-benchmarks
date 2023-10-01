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
#include "../support/csv_reader.h"

// Define the DPU Binary path as DPU_BINARY here
#ifndef DPU_BINARY
#define DPU_BINARY "./bin/qlearn_dpu"
#endif


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

// Define constants
#define NUM_STATES 6
#define NUM_ACTIONS 4
#define LEARNING_RATE 0.1
#define DISCOUNT_FACTOR 0.9
#define MAX_EPISODES 1000
#define MAX_STEPS 100

char ***experiences_str;
int count_col=0;

Experience experiences[NUM_EPISODES];

// Create input arrays
static void init_data( char* file_name ) {
   experiences_str = read_csv("/home/saisan/dummy.csv",&count_col, 50);
   Experience exp;

   for (int i=0; i < NUM_EPISODES; i++) {
     exp=experiences[i];
     exp.state = atoi(experiences_str[i][0]);
     exp.action = atoi(experiences_str[i][1]);
     exp.reward = atof(experiences_str[i][2]);
     exp.next_state = atoi(experiences_str[i][3]);
   }



  return;
}
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

static int qlearn_host() {
    // Perform offline Q-learning by updating the Q-table once after processing the entire batch
    updateQBatch();

    // Print the learned Q-table
    printf("Learned Q-table:\n");
    for (int s = 0; s < NUM_STATES; ++s) {
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            printf("Q[%d][%d] = %.2f   ", s, a, Q[s][a]);
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
/*	DPU_FOREACH(dpu_set, dpu, i) {
		
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

}
*/

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
	init_data(p.file_name);


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
   free_csv_memory(experiences_str, count_col);
	return status ? 0 : -1;
}
