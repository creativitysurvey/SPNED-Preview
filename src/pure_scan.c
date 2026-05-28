
    #define MAX_SIZE 100000000
    int in_buf[MAX_SIZE];
    int out_buf[MAX_SIZE];

    __attribute__((export_name("scan_seq")))
    void scan_seq(int len) {
        for(int i=0; i<len; i++) out_buf[i] = in_buf[i];
    }

    __attribute__((export_name("scan_sel")))
    int scan_sel(int len) {
        int out_idx = 0;
        for(int i=0; i<len; i++) {
            if (in_buf[i] % 10 == 0) out_buf[out_idx++] = in_buf[i];
        }
        return out_idx;
    }
    