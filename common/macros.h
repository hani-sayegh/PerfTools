#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))

#define arr(Array, Index) \
    (assert((Index) >= 0 && (Index) < ArrayCount(Array) && "Array index out of bounds!"), \
    (Array)[(Index)])


#define DIV_F64(num, den) ((f64)(num) / (f64)(den))

#define GB 1L * 1024 * 1024 * 1024
#define MB 1L * 1024 * 1024