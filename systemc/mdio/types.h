enum opcode_t {
    op_address,
    op_write,
    op_read,
    op_read_inc
};


enum st_t {
    clause45,
    clause22
};


enum sm_states {
    IDLE        = 0,
    SHIFT_OUT   = 1,
    SHIFT_DONE  = 2
};
