#define PSDEV_NULL      0
#define PSDEV_ZERO      1
#define PSDEV_URANDOM   2
#define PSDEV_NULLSTAT  3

void psdevinit(void);
int psdevread(int minor, int user_dst, uint64 dst, int n);
int psdevwrite(int minor, int user_src, uint64 src, int n);