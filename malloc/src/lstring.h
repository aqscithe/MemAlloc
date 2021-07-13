typedef struct lstring_t
{
    unsigned int    length;
    char*           buffer;          
} lstring_t;

void        lstringFree(lstring_t lstr);

lstring_t   lstringNew(const char* str);

lstring_t   lstringCat(lstring_t dest, lstring_t str);

unsigned int getStrLen(const char* str);

