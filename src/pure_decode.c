#define MAX_SIZE 100000000
int in_buf[MAX_SIZE];
int out_buf[MAX_SIZE];

__attribute__((export_name("decode")))
void decode(int batch_size) {
    for(int i=0; i<batch_size; i++) {
        out_buf[i] = in_buf[i];
    }
}