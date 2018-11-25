typedef struct __locale_struct * locale_t;

extern int strcoll (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) ;

extern int strcoll_l (const char *__s1, const char *__s2, locale_t __l)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) ;

	 int printf(const char *fmt, ...);

	        //typedef void (*sighandler_t)(int);
       typedef long (*sighandler_t)(float, double, char);

       sighandler_t signal(int signum, sighandler_t handler);

typedef enum
{
  TD_OK,
  TD_ERR,
  TD_NOTHR,
  TD_NOSV,
  TD_NOLWP,
  TD_BADPH,
  TD_BADTH,
  TD_BADSH,
  TD_BADTA,
  TD_BADKEY,
  TD_NOMSG,
  TD_NOFPREGS,
  TD_NOLIBTHREAD,
  TD_NOEVENT,
  TD_NOCAPAB,
  TD_DBERR,
  TD_NOAPLIC,
  TD_NOTSD,
  TD_MALLOC,
  TD_PARTIALREG,
  TD_NOXREGS,
  TD_TLSDEFER,
  TD_NOTALLOC = TD_TLSDEFER,
  TD_VERSION,
  TD_NOTLS
} td_err_e;

td_err_e eeefunc();



typedef union
{
  char __size[40];
  long int __align;
} pthread_mutex_t;
void pthread_lock(pthread_mutex_t * xx);


union sigval
{
  int sival_int;
  void *sival_ptr;
};
typedef int __pid_t;
extern int sigqueue (__pid_t __pid, int __sig, const union sigval __val)
     __attribute__ ((__nothrow__ ));


struct KK{};

int vv(struct KK const *a, struct KK b);



