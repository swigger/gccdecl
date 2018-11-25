#include "test.hpp"

namespace foo { }

typedef int new_t;

namespace ns1 {
  class c1 {
    int m;
  };

  namespace ns2 {
    void foo();

    void
    foo() {

    }
  }
}

int glob;

int
nl;

struct Foo{};
typedef Foo * Foop;

extern unsigned int fooo1(int a, Foop b);
extern const float fooo2(const int a, const char *const*const b);
static long foo3(float f, float g, const double x);
extern __typeof(foo3) foo4;


       //typedef void (*sighandler_t)(int);
       typedef long (*sighandler_t)(float, double, char);

       sighandler_t signal(int signum, sighandler_t handler);



int main() {
  int i = 1;
  return i + glob;
}
