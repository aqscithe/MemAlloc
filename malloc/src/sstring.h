typedef char* sstring_t;

sstring_t   sstringNew(const char* str);

unsigned int sstringLength(const sstring_t sstr);

void sstringFree(sstring_t sstr);

