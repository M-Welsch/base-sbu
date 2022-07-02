/* function protoypes */
void enable_5v0_smps(void);
void disable_5v0_smps(void);
void enable_bpi_sply(void);
void disable_bpi_sply(void);
void transition_to_pwr_state(enum pwr_states pwr_state);

bool wait_until_bpi_ready_for_shutdown();
