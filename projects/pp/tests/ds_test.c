#include <stdio.h>
#include <stdint.h>
#include "../ds/ds.h"


int main () {
    struct value_specifications v;

    v.type         = random;
    v.init         = 10;
    v.range.start  = 64;
    v.range.stop   = 1500;
    v.range.step   = 5;
    v.repeat_after = 100;

    printf ("Size of vgen = %d bytes --------------------\n", sizeof v);
    dbg_print_vgen (&v);

    struct stream_control_specifications s;

    s.type = continuous_pkt;
    s.traffic.pkts_per_burst = 100;
    s.traffic.burst_per_stream = 1;
    s.traffic.inter_burst_gap = 1024;
    s.traffic.inter_stream_gap = 4096;
    s.start_delay = 2048;
    s.rate.type = percentage;
    s.rate.value = 100;

    // printf ("Size of sgen = %d bytes --------------------\n", sizeof s);
    dbg_print_sgen (&s);

    struct data_specifications p;

    p.size.type = increment;
    p.size.increment.min = 1;
    p.size.increment.max = 2;
    p.size.increment.step = 3;
    p.size.decrement.min = 4;
    p.size.decrement.max = 5;
    p.size.decrement.step = 6;
    p.size.random.type = udist;
    p.size.random.udist.min = 7;
    p.size.random.udist.max = 8;
    p.size.random.nof_wpairs = 2;
    p.size.random.wpairs[0].size = 9;
    p.size.random.wpairs[0].weight = 10;
    p.size.random.wpairs[1].size = 14;
    p.size.random.wpairs[1].weight = 15;
    p.data.type = rpt_pattern;
    p.data.increment_word_size = 11;
    p.data.decrement_word_size = 12;
    p.data.pattern = "0a 0a";
    p.data.repeat_count = 100;
    printf ("Size of pgen = %d bytes --------------------\n", sizeof p);
    dbg_print_pgen (&p);

    return 0;
}
