#ifndef __DATA_STRUCTURES__
#define __DATA_STRUCTURES__


#include <stdio.h>
#include <stdint.h>


#define PACKED __attribute__((packed))


typedef enum {
    // value modifiers
    fixed,                  // init value
    random,                 // from 0 to max int
    rrandom,                // random in range
    increment,              // from start to stop with steps
    decrement,              // from start to stop with steps
    cincrement,             // starts from 0 and loops at repeat after 
    cdecrement,             // starts from ff and loops at repeat after
    value_list,             // TODO
    range_list,             // TODO
    // data modifiers
    incr_byte,              // start from zero
    incr_word,              // start from zero
    decr_byte,              // start from FF
    decr_word,              // start from FF
    rpt_pattern,            // repeat a data pattern
    fixed_pattern,          // use data pattern only once, rest 0s
    udist,                  // TODO
    wpairs,                 // TODO
    // stream modifiers
    continuous_pkt,            
    continuous_burst,          
    stop_after_this_stream,    
    goto_next_stream,
    // stream rate modifiers
    ipg,
    percentage,
    pkts_per_sec,
    bitrate
} types;


struct stream_control_specifications {
    types type;
    struct traffic {
        uint32_t pkts_per_burst;
        uint32_t burst_per_stream;
        uint32_t inter_burst_gap;
        uint32_t inter_stream_gap;
    } traffic;
    uint32_t start_delay;
    struct rate {
        types type;
        uint32_t value;
    } rate;
};


struct value_specifications {
    types type;
    uint32_t init;
    struct range {
        uint32_t start;
        uint32_t stop;
        uint32_t step;
    } range;
   uint32_t repeat_after;
   uint32_t step;
};


struct data_specifications {
    struct size {
        types type;
        struct increment {
            uint32_t min;
            uint32_t max;
            uint32_t step;
        } increment;
        struct decrement {
            uint32_t min;
            uint32_t max;
            uint32_t step;
        } decrement;
        struct random {
            types type;
            struct udist {
                uint32_t min;
                uint32_t max;
            } udist;
            struct wpairs {
                uint32_t size;
                uint32_t weight;
            } wpairs[16];
            uint32_t nof_wpairs;
        } random;
    } size;
    struct data {
        types type;
        uint32_t increment_word_size;
        uint32_t decrement_word_size;
        char *pattern;
        uint32_t repeat_count;
    } data;
};


struct stream_specifications {
    struct stream_control_specifications control;
    struct data_specifications data;
};


void dbg_print_pgen (struct data_specifications *d) {
    printf ("pgen.size.type                    : %d\n", d->size.type);
    printf ("pgen.size.incr.min                : %d\n", d->size.increment.min);
    printf ("pgen.size.incr.max                : %d\n", d->size.increment.max);
    printf ("pgen.size.incr.step               : %d\n", d->size.increment.step);
    printf ("pgen.size.decr.min                : %d\n", d->size.decrement.min);
    printf ("pgen.size.decr.max                : %d\n", d->size.decrement.max);
    printf ("pgen.size.decr.step               : %d\n", d->size.decrement.step);
    printf ("pgen.size.random.type             : %d\n", d->size.random.type);
    printf ("pgen.size.random.udist.min        : %d\n", d->size.random.udist.min);
    printf ("pgen.size.random.udist.max        : %d\n", d->size.random.udist.max);
    uint32_t i;
    for (i=0;  i<d->size.random.nof_wpairs; i++) {
    printf ("pgen.size.random.wpairs[%d].size   : %d\n", i, d->size.random.wpairs[i].size);
    printf ("pgen.size.random.wpairs[%d].weight : %d\n", i, d->size.random.wpairs[i].weight);
    }
    printf ("pgen.data.type                    : %d\n", d->data.type);
    printf ("pgen.data.incr_word_size          : %d\n", d->data.increment_word_size);
    printf ("pgen.data.decr_word_size          : %d\n", d->data.decrement_word_size);
    printf ("pgen.data.pattern                 : %s\n", d->data.pattern);
    printf ("pgen.data.repeat_count            : %d\n", d->data.repeat_count);
}


void dbg_print_vgen (struct value_specifications *v) {
    printf ("vgen.type         : %d\n", v->type);
    printf ("vgen.init         : %d\n", v->init);
    printf ("vgen.range.start  : %d\n", v->range.start);
    printf ("vgen.range.stop   : %d\n", v->range.stop);
    printf ("vgen.range.step   : %d\n", v->range.step);
    printf ("vgen.repeat_after : %d\n", v->repeat_after);
}


void dbg_print_sgen (struct stream_control_specifications *s) {
    printf ("sgen.type                     : %d\n", s->type);
    printf ("sgen.traffic.pkts_per_burst   : %d\n", s->traffic.pkts_per_burst);
    printf ("sgen.traffic.burst_per_stream : %d\n", s->traffic.burst_per_stream);
    printf ("sgen.traffic.inter_burst_gap  : %d\n", s->traffic.inter_burst_gap);
    printf ("sgen.traffic.inter_stream_gap : %d\n", s->traffic.inter_stream_gap);
    printf ("sgen.start_delay              : %d\n", s->start_delay);
    printf ("sgen.rate.type                : %d\n", s->rate.type);
    printf ("sgen.rate.value               : %d\n", s->rate.value);
}


#endif
