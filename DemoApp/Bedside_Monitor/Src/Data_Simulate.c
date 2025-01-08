#include "Helpers.h"

/**
 * @brief Simulate reading data from time
 * @param sample_rate Sample rate in Hz
 * @param time_start_ms Start time in millisecond
 * @return Number of samples
 */
int read_time_simulate(int sample_rate, int time_start_ms)
{
	int now_ms = EVE_millis();
	int duration_ms = now_ms - time_start_ms;

	int num_samples = duration_ms * sample_rate / 1000;
	return 5;
	return num_samples;
}

/**
 * @brief Simulate ECG data
 * @param data Pointer to int array to store data
 * @param data_size Pointer to int to store size of data
 * @return Pixel x coordinate
 */
int new_data_heartbeat(int** data, int *data_size)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	static int ecg_data[] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,															  // P-wave (small bump)
		0, 2, 4, 6, 8, 6, 4, 2, 0,																  // PR segment (flat)
		0, 0, 0, 0, 0,																			  // QRS complex (sharp spike)
		2, 4, 8, 20, 40, 70, 30, 10, 5, 2,														  // ST segment (flat)
		2, 2, 2, 2, 2, 2, 2,																	  // T-wave (broad bump)
		2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,													  // Baseline (flat)
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // end
	};
	int data_duration = 1; // unit: second
	int dividor = 2;
	int multipler = 3;
	const int sample_total = sizeof(ecg_data) / sizeof(int);

	static uint32_t time_start_ms = 0;
	const int sample_per_second = sample_total / data_duration;
	static int sample_offset = 0;
	
	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
		for(int i=0; i< sample_total;i++){
			ecg_data[i] = ecg_data[i] *multipler / dividor;
		}
	}

	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0) {
		time_start_ms = EVE_millis();
		num_samples = min(num_samples, sample_total - sample_offset);
	}

	*data = &ecg_data[sample_offset];
	*data_size = num_samples;

	// get sample (x,y) coordinate
	int pixel_y = num_samples;
	int pixel_x = ecg_data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
	sample_offset = (sample_offset + num_samples) % sample_total;
	return pixel_x;
}


/**
 * @brief Simulate Plethysmography data
 * @param data Pointer to int array to store data
 * @param data_size Pointer to int to store size of data
 * @return Pixel x coordinate
 */
int new_data_pleth(int** data, int* data_size)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	static int pleth_data[] = {
		0, 6, 12, 18, 30, 42, 60, 78, 102, 126,			     // Cycle 1 - Inhalation
		150, 174, 192, 210, 228, 240, 252, 264, 270, 276,    // Cycle 1 - Continued Inhalation
		282, 288, 285, 282, 288, 285, 276, 270, 264, 252,    // Cycle 1 - Peak
		// 240, 228, 210, 192, 174, 160, 140, 120, 130, 110, // Cycle 1 - Exhalation with fluctuations
		// 90, 100, 85, 75, 95, 65, 50, 42, 30, 35,		     // Cycle 1 - Continued Exhalation with fluctuations
		240, 228, 210, 192, 174, 160, 140, 130, 120, 110,    // Cycle 1 - Exhalation no fluctuations
		100, 85, 75, 65, 50, 42, 35, 30,         		     // Cycle 1 - Continued Exhalation no fluctuations
		20, 15, 12, 10, 8, 6, 9, 12, 18, 30,			     // Cycle 1 - Trough
		42, 70, 100, 120, 140, 160, 180, 200, 210, 220,	     // Cycle 2 - Inhalation
		225, 230, 235, 240, 238, 236, 240, 235, 225, 210,    // Cycle 2 - Peak and decline
		195, 180, 165, 150, 130, 110, 90, 70, 50, 30,	     // Cycle 2 - Faster Exhalation
		20, 10, 5, 0, 0, 0, 0, 0, 0, 0					     // Cycle 2 - End
	};
	int data_duration = 2; // unit: second
	int dividor = 2;
	int multipler = 1;
	const int sample_total = sizeof(pleth_data) / sizeof(int);

	static uint32_t time_start_ms = 0;
	const int sample_per_second = sample_total / data_duration;
	static int sample_offset = 0;

	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
		for (int i = 0; i < sample_total; i++) {
			pleth_data[i] = pleth_data[i] * multipler / dividor;
		}
	}

	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0) {
		time_start_ms = EVE_millis();
		num_samples = min(num_samples, sample_total - sample_offset);
	}

	*data = &pleth_data[sample_offset];
	*data_size = num_samples;

	// get sample (x,y) coordinate
	int pixel_y = num_samples;
	int pixel_x = pleth_data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
	sample_offset = (sample_offset + num_samples) % sample_total;
	return pixel_x;
}

/**
 * @brief Simulate CO2 data
 * @param data Pointer to int array to store data
 * @param data_size Pointer to int to store size of data
 * @return Pixel x coordinate
 */
int new_data_co2(int** data, int* data_size)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	static int co2_data[] = {												  // 1 second sample
							30, 45, 60, 75, 90, 105, 120, 135, 150, 165,	  //
							180, 195, 210, 225, 240, 255, 255, 255, 255, 255, //
							255, 255, 255, 255, 255, 240, 225, 210, 195, 180, //
							165, 150, 135, 120, 105, 90, 75, 60, 45, 30,	  //
							15, 10, 5, 0, 0, 0, 0, 0, 0, 0,					  //
							0, 0, 0, 0, 0, 5, 10, 15, 15 };

	int data_duration = 1; // unit: second
	int dividor = 2;
	int multipler = 1;
	const int sample_total = sizeof(co2_data) / sizeof(int);

	static uint32_t time_start_ms = 0;
	const int sample_per_second = sample_total / data_duration;
	static int sample_offset = 0;

	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
		for (int i = 0; i < sample_total; i++) {
			co2_data[i] = co2_data[i] * multipler / dividor;
		}
	}

	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0) {
		time_start_ms = EVE_millis();
		num_samples = min(num_samples, sample_total - sample_offset);
	}

	*data = &co2_data[sample_offset];
	*data_size = num_samples;

	// get sample (x,y) coordinate
	int pixel_y = num_samples;
	int pixel_x = co2_data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
	sample_offset = (sample_offset + num_samples) % sample_total;
	return pixel_x;
}
