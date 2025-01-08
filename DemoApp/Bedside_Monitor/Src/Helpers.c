#include "Helpers.h"

// Function to calculate and return FPS without float/double
int getFPS()
{
	static unsigned long last_time_ms = 0; // Timestamp of the last frame
	static unsigned long frame_count = 0;  // Total frames since start
	static int fps = 0;					   // Calculated FPS

	unsigned long current_time_ms = EVE_millis();

	if (last_time_ms == 0)
	{
		// Initialize on the first frame
		last_time_ms = current_time_ms;
		return 0;
	}

	// Calculate time since last frame
	unsigned long duration_ms = max(1, current_time_ms - last_time_ms);
	fps = (frame_count * 1000) / duration_ms;
	frame_count++;

	return fps;
}

int8_t istouch(EVE_HalContext *phost)
{
	return !(EVE_Hal_rd16(phost, REG_TOUCH_RAW_XY) & 0x8000);
}

// Static variables to store initial date-time
static int init_dd = 0, init_mm = 0, init_yyyy = 0;
static int init_hh = 0, init_m = 0, init_ss = 0, init_ms = 0;

#define MILLIS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define MILLIS_PER_DAY (24 * 60 * 60 * 1000)

// Function to format the date as "dd-mm-yyyy"
char* dd_mm_yyyy() {
	static char dateString[12]; // Buffer for "dd-mm-yyyy"
	static int current_dd = 0, current_mm = 0, current_yyyy = 0;
	unsigned long elapsed_millis = EVE_millis();
	int elapsed_days = elapsed_millis / MILLIS_PER_DAY;

	// Recalculate date if it hasn't been updated yet
	if (current_dd == 0 && current_mm == 0 && current_yyyy == 0) {
		current_dd = init_dd + elapsed_days;
		current_mm = init_mm;
		current_yyyy = init_yyyy;

		static int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		while (1) {
			int days_in_current_month = days_in_month[current_mm - 1];

			// Adjust for leap year in February
			if (current_mm == 2 && ((current_yyyy % 4 == 0 && current_yyyy % 100 != 0) || (current_yyyy % 400 == 0))) {
				days_in_current_month = 29;
			}

			if (current_dd > days_in_current_month) {
				current_dd -= days_in_current_month;
				current_mm++;
				if (current_mm > 12) {
					current_mm = 1;
					current_yyyy++;
				}
			}
			else {
				break;
			}
		}
	}

	snprintf(dateString, sizeof(dateString), "%02d-%02d-%04d", current_dd, current_mm, current_yyyy);
	return dateString;
}

// Function to format the time as "hh:mm"
char* hh_mm() {
	static char timeString[6]; // Buffer for "hh:mm"
	static int current_hh = 0, current_m = 0;
	unsigned long elapsed_millis = EVE_millis();
	unsigned long total_seconds = init_ss + (init_ms + elapsed_millis) / MILLIS_PER_SECOND;
	int total_minutes = init_m + total_seconds / SECONDS_PER_MINUTE;

	current_hh = init_hh + total_minutes / MINUTES_PER_HOUR;
	current_m = total_minutes % MINUTES_PER_HOUR;

	if (current_hh >= 24) {
		current_hh %= 24;
	}

	snprintf(timeString, sizeof(timeString), "%02d:%02d", current_hh, current_m);
	return timeString;
}

// Function to calculate and return the formatted date-time string
char* dd_mm_yyyy_hh_m_ss_ms() {
	static char fullDateTimeString[30]; // Buffer for "dd-mm-yyyy hh:mm:ss:ms"
	static int current_ss = 0, current_ms = 0;

	// Calculate total elapsed time
	unsigned long elapsed_millis = EVE_millis();
	unsigned long total_ms = init_ms + elapsed_millis;
	unsigned long total_seconds = init_ss + total_ms / MILLIS_PER_SECOND;

	current_ms = total_ms % MILLIS_PER_SECOND; // Remaining milliseconds
	current_ss = total_seconds % SECONDS_PER_MINUTE; // Remaining seconds

	// Get the date and time parts
	char* datePart = dd_mm_yyyy();
	char* timePart = hh_mm();

	// Combine all parts into a full date-time string
	snprintf(fullDateTimeString, sizeof(fullDateTimeString), "%s %s:%02d:%03d", datePart, timePart, current_ss, current_ms);

	return fullDateTimeString;
}

// Function to initialize date-time
void init_datetime(int dd, int mm, int yyyy, int hh, int m, int ss, int ms) {
	init_dd = dd;
	init_mm = mm;
	init_yyyy = yyyy;
	init_hh = hh;
	init_m = m;
	init_ss = ss;
	init_ms = ms;
}

int app_random(int range)
{
	if (range <= 0)
		return 0;

	unsigned long millis = EVE_millis() + 17;
	return millis % range;
}


int save_buffer_to_file(const char* filename, const void* buffer, size_t buffer_size) {
	// Open the file for writing in binary mode
	FILE* file = fopen(filename, "wb");
	if (file == NULL) {
		perror("Error opening file");
		return -1;
	}

	// Write the buffer to the file
	size_t written = fwrite(buffer, 1, buffer_size, file);
	if (written != buffer_size) {
		perror("Error writing to file");
		fclose(file);
		return -1;
	}

	// Close the file
	fclose(file);
	return 0;
}

void take_ddr_screenshot(EVE_HalContext *phost, char *name, int ramg_render) {
#if defined(BT82X_ENABLE)
	int offset = 1024 * 1024;
	EVE_CoCmd_renderTarget(phost, offset, RGB8, phost->Width, phost->Height);
	const int bbp_rgb8 = 24;
	int sc_size = phost->Width * phost->Height * bbp_rgb8 / 8;
	char* buffer_screenshot = malloc(sc_size);
	EVE_Hal_rdMem(phost, buffer_screenshot, offset, sc_size);
	save_buffer_to_file("demo_debside.raw", buffer_screenshot, sc_size);
#endif
}

static uint32_t transfer_addr0 = 0;
static uint32_t accumulator32 = 0;
static uint32_t byte_count = 0;

void bt820_transfer_flush(EVE_HalContext* phost, uint32_t addr, uint8_t data32) {
	EVE_Hal_wr32(phost, addr, data32);
	transfer_addr0 = 0;
	byte_count = 0;
	accumulator32 = 0;
}

void bt820_transfer_mem(EVE_HalContext* phost, uint32_t addr, uint8_t* data, uint32_t data_size) {
	for (int i = 0; i < data_size; i++) {
		bt820_transfer8(phost, addr+i, (uint8_t)(data[i] & 0xFF));
	}
}

void bt820_transfer32(EVE_HalContext* phost, uint32_t addr, uint8_t data32) {
	// Transfer the lowest byte
	bt820_transfer8(phost, addr, (uint8_t)(data32 & 0xFF));

	// Transfer the second byte
	bt820_transfer8(phost, addr + 1, (uint8_t)((data32 >> 8) & 0xFF));

	// Transfer the third byte
	bt820_transfer8(phost, addr + 2, (uint8_t)((data32 >> 16) & 0xFF));

	// Transfer the highest byte
	bt820_transfer8(phost, addr + 3, (uint8_t)((data32 >> 24) & 0xFF));
}

void bt820_transfer16(EVE_HalContext* phost, uint32_t addr, uint16_t data16) {
	// Transfer the lower byte
	bt820_transfer8(phost, addr, (uint8_t)(data16 & 0xFF));

	// Transfer the higher byte
	bt820_transfer8(phost, addr + 1, (uint8_t)((data16 >> 8) & 0xFF));
}

void bt820_transfer8(EVE_HalContext* phost, uint32_t addr, uint8_t data8) {
	// Check if address is contiguous
	if (byte_count > 0 && addr != (transfer_addr0 + byte_count)) {
		// Flush the current transfer as the address is not contiguous
		bt820_transfer_flush(phost, transfer_addr0, accumulator32);

		// Reset the accumulator and byte count
		accumulator32 = 0;
		byte_count = 0;
	}

	// Set the new starting address if this is a new transfer
	if (byte_count == 0) {
		transfer_addr0 = addr;
	}

	// Add the byte to the accumulator
	accumulator32 |= ((uint32_t)data8 << (8 * byte_count));
	byte_count++;

	// Flush if 4 bytes are collected
	if (byte_count == 4) {
		bt820_transfer_flush(phost, transfer_addr0, accumulator32);

		// Reset the accumulator and byte count after flushing
		accumulator32 = 0;
		byte_count = 0;
	}
}
