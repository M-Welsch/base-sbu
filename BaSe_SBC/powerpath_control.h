enum pwr_states {
	standby = 0,
	display_on = 1,
	active = 2,
};

/* function protoypes */
void enable_5v0_smps(void);
void disable_5v0_smps(void);
void enable_bpi_sply(void);
void disable_bpi_sply(void);
void goto_pwr_state(enum pwr_states pwr_state);