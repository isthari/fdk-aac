#ifdef __cplusplus
extern "C" {
#endif

int test(void);

int openEncoder(int index, UCHAR *inputBuffer);
int encode();
void * getOutputBuffer(int index);

#ifdef __cplusplus
}
#endif