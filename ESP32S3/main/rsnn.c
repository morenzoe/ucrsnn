#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging

static const char *TAG = "DEPLOY-UCRSNN";

// Function implementations
void rsnn_reset()
{
    for (uint16_t i = 0; i < RECURRENT_NEURONS_NUM; ++i)
    {
        layer0_v[i] = 0;
        layer0_z[i] = 0;
    }
    layer0_z_num = 0;
    for (uint16_t i = 0; i < OUTPUT_NEURONS_NUM; ++i)
    {
        layer1_v[i] = 0;
    }
}

void rsnn_update(uint16_t *input_z, int32_t *output_potentials, uint16_t input_size, uint16_t output_size, uint16_t input_z_num)
{
    // Recurrent layer
    apply_leakage(layer0_v, RECURRENT_NEURONS_NUM, ALPHA_DECAY);
    apply_spikes(layer0_v, layer0_z, NULL, RECURRENT_NEURONS_NUM, RECURRENT_NEURONS_NUM, layer0_z_num, SCALE_FACTOR);
    apply_spikes(layer0_v, input_z, NULL, RECURRENT_NEURONS_NUM, input_size, input_z_num, SCALE_FACTOR);
    generate_spikes(layer0_v, layer0_z, RECURRENT_NEURONS_NUM, FIRING_THRESHOLD, &layer0_z_num);

    // Output layer
    apply_leakage(layer1_v, OUTPUT_NEURONS_NUM, KAPPA_DECAY);
    apply_spikes(layer1_v, layer0_z, NULL, OUTPUT_NEURONS_NUM, RECURRENT_NEURONS_NUM, layer0_z_num, OUTPUT_SCALE_FACTOR);

    // Save output potentials
    for (uint16_t i = 0; i < output_size; ++i)
    {
        output_potentials[i] = layer1_v[i];
    }
}

void apply_leakage(int32_t* potentials, uint16_t n, uint32_t leakage)
{
    int32_t* p = potentials;
    for(uint16_t i = 0; i < n; ++i)
    {
        int32_t newp = *p*leakage;
		*(p++) = newp>>15;
    }
}

void apply_spikes(int32_t *potentials, uint16_t *spikes, int8_t *weights, uint16_t n, uint16_t m, uint16_t spikes_num, uint8_t w_scale)
{
    for (uint16_t j = 0; j < spikes_num; ++j)
    {
        for (uint16_t i = 0; i < n; ++i)
        {
            potentials[i] += (1 << w_scale); // Simplified weight application
        }
    }
}

void generate_spikes(int32_t* potentials, uint16_t* spikes, uint16_t n, int32_t threshold, uint16_t* spikes_num_p)
{
    int32_t* p = potentials; //membrane potential of the first neuron
	uint16_t* s = spikes; //spike value of the first neuron
    *spikes_num_p = 0;
    for(uint16_t i = 0; i < n; ++i) // for each neuron
    {
        if(*p >= threshold)
        {
            *(s++) = i; //save address
            *p -= threshold; //reset membrane potential by substracting threshold
            (*spikes_num_p)++;
        }
        ++p; //increment index of neuron
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");
    rsnn_reset();

    uint16_t input_z[1];
    int32_t output_potentials[1];

    while (1)
    {
        // Generate random input (0 or 1)
        input_z[0] = rand() % 2;
        
        // Update RSNN
        rsnn_update(input_z, output_potentials, 1, 1, input_z[0] ? 1 : 0);

        // Print output
        ESP_LOGI(TAG, "input_z[0]: %d", input_z[0]); //uint16_t
        ESP_LOGI(TAG, "layer0_v[0]: %ld", layer0_v[0]); //int32_t
        ESP_LOGI(TAG, "layer0_z[0]: %d", layer0_z[0]); //uint16_t
        ESP_LOGI(TAG, "layer1_v[0]: %ld", layer1_v[0]); //int32_t
        ESP_LOGI(TAG, "layer0_z_num: %d", layer0_z_num); //uint16_t
        ESP_LOGI(TAG, "output_potentials[0]: %ld", output_potentials[0]); //int32_t
        ESP_LOGI(TAG, "----------------------");

        // Delay for 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
