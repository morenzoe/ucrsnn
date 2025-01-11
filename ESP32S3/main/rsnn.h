#ifndef RSNN_H
#define RSNN_H

#include <stdint.h>

// Define constants
#define RECURRENT_NEURONS_NUM 1
#define OUTPUT_NEURONS_NUM 1
#define ALPHA_DECAY 32767      // Example decay factor for recurrent neurons
#define KAPPA_DECAY 32767      // Example decay factor for output neurons
#define FIRING_THRESHOLD 6  // Spike threshold
#define SCALE_FACTOR 1
#define OUTPUT_SCALE_FACTOR 1

// Function prototypes
void rsnn_reset(void);
void rsnn_update(uint16_t *input_z, int32_t *output_potentials, uint16_t input_size, uint16_t output_size, uint16_t input_z_num);
void apply_leakage(int32_t *potentials, uint16_t n, uint32_t leakage);
void apply_spikes(int32_t *potentials, uint16_t *spikes, int8_t *weights, uint16_t n, uint16_t m, uint16_t spikes_num, uint8_t w_scale);
void generate_spikes(int32_t *potentials, uint16_t *spikes, uint16_t n, int32_t threshold, uint16_t *spikes_num_p);

#endif // RSNN_H
