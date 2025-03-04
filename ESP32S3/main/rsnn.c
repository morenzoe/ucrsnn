#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging

static const char *TAG = "DEPLOY-UCRSNN";

// Function implementations
void rsnn_reset()
{
    for(uint16_t i = 0; i < RECURRENT_NEURONS_NUM; ++i)
    {
        layer0_v[i] = 0;
        layer0_z[i] = 0;
    }
    for(uint16_t i = 0; i < OUTPUT_NEURONS_NUM; ++i)
    {
        layer1_v[i] = 0;
    }
}

void rsnn_update(uint16_t *input_z, int32_t *output_potentials, uint16_t input_size, uint16_t output_size)
{
    // Output layer
    apply_leakage(layer1_v, OUTPUT_NEURONS_NUM, KAPPA_DECAY);
    apply_spikes(layer1_v, layer0_z, layer1_w, OUTPUT_NEURONS_NUM, RECURRENT_NEURONS_NUM, OUTPUT_SCALE_FACTOR, 0); // output neuron doesn't reset membrane potential
    
    // Recurrent layer
    apply_leakage(layer0_v, RECURRENT_NEURONS_NUM, ALPHA_DECAY);
    apply_spikes(layer0_v, input_z, layer0_w, RECURRENT_NEURONS_NUM, input_size, SCALE_FACTOR, 0); // input spike doesn't reset membrane potential
    apply_spikes(layer0_v, layer0_z, layer0_rw, RECURRENT_NEURONS_NUM, RECURRENT_NEURONS_NUM, SCALE_FACTOR, FIRING_THRESHOLD); //reset membrane potential
    generate_spikes(layer0_v, layer0_z, RECURRENT_NEURONS_NUM, FIRING_THRESHOLD);

    // Save output potentials
    for(uint16_t i = 0; i < output_size; ++i)
    {
        output_potentials[i] = layer1_v[i];
    }
}

void apply_leakage(int32_t* potentials, uint16_t n, uint32_t leakage)
{
    int32_t* p = potentials; //membrane potential of the first neuron in a layer
    for(uint16_t i = 0; i < n; ++i) //for each neuron in a layer
    {
        int32_t newp = *p*leakage;
		*(p++) = newp>>15;
    }
}

void apply_spikes(int32_t* potentials, uint16_t* spikes, int8_t* weights, uint16_t n, uint16_t m, uint8_t w_scale, int32_t threshold)
{
    uint16_t* s = spikes; //spike value of the first source neuron
	for(uint16_t j = 0; j < m; ++j) //for each source neuron
	{		
		int32_t* p = potentials; //membrane potential of the first destination neuron
		int8_t* w = weights+j*n; //pick weights row for the source neuron

		for(uint16_t i = 0; i < n; ++i) //for each destination neuron
		{
            if(*s) //if the source neuron spiked
            {
                *(p++) += (*(w++)<<w_scale)-threshold; //apply weights to all the neurons corresponding to the spike, reset membrane potential by substracting threshold from reccurent connection
            }
		}
        ++s; //increment index of source neuron
	}
}

void generate_spikes(int32_t* potentials, uint16_t* spikes, uint16_t n, int32_t threshold)
{
    int32_t* p = potentials; //membrane potential of the first neuron in a layer
	uint16_t* s = spikes; //spike value of the first neuron in a layer
    for(uint16_t i = 0; i < n; ++i) // for each neuron in a layer
    {
        if(*p >= threshold) //if the membrane potential cross threshold
        {
            *(s++) = 1; //assign spike value for the neuron
        }
        else //if the membrane potential doesn't cross threshold
        {
            *(s++) = 0; //assign spike value for the neuron
        }
        ++p; //increment index of neuron
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");
    rsnn_reset();

    uint16_t input_z[T][INPUT_NEURONS_NUM] = {{0},{1},{1},{1},{1},{0},{0},{0},{0},{0},{0}};
    int32_t output_v[T][OUTPUT_NEURONS_NUM] = {{0}};

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "       t: %d", i); //int32_t
        ESP_LOGI(TAG, " input_z: %d", input_z[i][0]); //uint16_t
        
        // Update RSNN
        if(i >= 1) //delay based on the math equation
        {
            rsnn_update(input_z[i-1], output_v[i], 1, 1);
        }

        // Print output
        ESP_LOGI(TAG, "layer0_v: %ld", layer0_v[0]); //int32_t
        ESP_LOGI(TAG, "layer0_z: %d", layer0_z[0]); //uint16_t
        ESP_LOGI(TAG, "layer1_v: %ld", layer1_v[0]); //int32_t
        ESP_LOGI(TAG, "output_z: %ld", output_v[i][0]); //int32_t
        ESP_LOGI(TAG, "----------------------");
    }
}
