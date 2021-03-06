#ifndef _TEST_TEST_H_
#define _TEST_TEST_H_

#include <candor.h>
#include <heap.h>
#include <heap-inl.h>
#include <zone.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <assert.h>

using namespace candor;
using namespace internal;

#define TEST_START(name)\
    int main(void) {\
      fprintf(stdout, "-- %s --\n", name);

#define TEST_END(name)\
      fclose(stdout);\
      return 0;\
    }

#define PARSER_TEST(code, expected)\
    {\
      Zone z;\
      char out[1024];\
      Parser p(code, strlen(code));\
      AstNode* ast = p.Execute();\
      p.Print(out, 1000);\
      assert(ast != NULL);\
      assert(strcmp(expected, out) == 0);\
      ast = NULL;\
    }

#define SCOPE_TEST(code, expected)\
    {\
      Zone z;\
      char out[1024];\
      Parser p(code, strlen(code));\
      AstNode* ast = p.Execute();\
      Scope::Analyze(ast);\
      p.Print(out, 1000);\
      assert(ast != NULL);\
      assert(strcmp(expected, out) == 0);\
      ast = NULL;\
    }

#define FUN_TEST(code, block)\
    {\
      Isolate i;\
      Function* f = Function::New(code, strlen(code));\
      Value* argv[0];\
      Value* result = f->Call(NULL, 0, argv);\
      block\
    }

#define BENCH_START(name, num)\
    timeval __bench_##name##_start;\
    gettimeofday(&__bench_##name##_start, NULL);

#define BENCH_END(name, num)\
    timeval __bench_##name##_end;\
    gettimeofday(&__bench_##name##_end, NULL);\
    double __bench_##name##_total = __bench_##name##_end.tv_sec -\
                                    __bench_##name##_start.tv_sec +\
                                    __bench_##name##_end.tv_usec * 1e-6 -\
                                    __bench_##name##_start.tv_usec * 1e-6;\
    if ((num) != 0) {\
      fprintf(stdout, #name " : %f ops/sec\n",\
              (num) / __bench_##name##_total);\
    } else {\
      fprintf(stdout, #name " : %fs\n",\
              __bench_##name##_total);\
    }

#endif //  _TEST_TEST_H_
