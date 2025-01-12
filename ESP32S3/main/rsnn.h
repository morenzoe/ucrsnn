#ifndef RSNN_H
#define RSNN_H

#include <stdint.h>

// RSNN CONFIG

#define SCALE_FACTOR 0
#define OUTPUT_SCALE_FACTOR 0
#define FIRING_THRESHOLD 6  // Spike threshold
#define ALPHA_DECAY 32767      // Example decay factor for recurrent neurons
#define KAPPA_DECAY 32767      // Example decay factor for output neurons

#define INPUT_NEURONS_NUM 1
#define RECURRENT_NEURONS_NUM 1
#define OUTPUT_NEURONS_NUM 1

#define WIN_BUFF_SIZE 1 // RECURRENT_NEURONS_NUM * INPUT_NEURONS_NUM
#define WREC_BUFF_SIZE 1 // RECURRENT_NEURONS_NUM * RECURRENT_NEURONS_NUM
#define WOUT_BUFF_SIZE 1 // OUTPUT_NEURONS_NUM * RECURRENT_NEURONS_NUM

# define T 10 // total timesteps

/*****/
// VARIABLES AND BUFFERS

// Weights matrices, encoded row-first (NxM).
// Each row encodes the interconnections from one input spike.
// Each column encodes the interconnections to one neuron.
int8_t layer0_w[WIN_BUFF_SIZE] = {3};
int8_t layer0_rw[WREC_BUFF_SIZE] = {2};
int8_t layer1_w[WOUT_BUFF_SIZE] = {4};

// Neuron state variables
int32_t layer0_v[RECURRENT_NEURONS_NUM]; // membrane potentials of recurrent layer
uint16_t layer0_z[RECURRENT_NEURONS_NUM]; // spiking outputs of recurrent layer
int32_t layer1_v[OUTPUT_NEURONS_NUM]; // membrane potentials of output layer

/*****/
// FUNCTIONS

// Reset all neurons potentials.
void rsnn_reset(void);
// Update rsnn applying input spikes (input_z) and generate the output potentials. To be called for each time step.
void rsnn_update(uint16_t *input_z, int32_t *output_potentials, uint16_t input_size, uint16_t output_size, uint16_t input_z_num);
// Apply leakage to all the potentials in the input vector.
void apply_leakage(int32_t *potentials, uint16_t n, uint32_t leakage);
// Apply spikes proportional to the weights.
void apply_spikes(int32_t *potentials, uint16_t *spikes, int8_t *weights, uint16_t n, uint16_t m, uint8_t w_scale);
// Generate spikes given a firing threshold and reset potentials.
void generate_spikes(int32_t *potentials, uint16_t *spikes, uint16_t n, int32_t threshold);

#endif // RSNN_H