void rtc_wait_for_all_status_bits_zero();

void rtc_set_prescaler_and_enable();
void rtc_enable();
bool rtc_enabled();

void rtc_set_compare_interrupt();

void rtc_use_internal_1k024_oscillator();

void rtc_setup(void);

void rtc_set_compare_value(void);

void rtc_write_seconds_to_cmp();