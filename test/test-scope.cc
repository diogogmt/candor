#include "test.h"
#include <parser.h>
#include <ast.h>

TEST_START("scope test")
  // Basic
  SCOPE_TEST("a", "[a @stack:0]")
  SCOPE_TEST("a.b[c]",
             "[kMember [kMember [a @stack:0] [kProperty b]] [c @stack:1]]")
  SCOPE_TEST("a + b", "[kAdd [a @stack:0] [b @stack:1]]")
  SCOPE_TEST("a\n{ b }", "[a @stack:1] [kBlock [b @stack:0]]")
  SCOPE_TEST("a()", "[kCall [a @stack:0] @[] ]")
  SCOPE_TEST("a\na() { b }",
             "[a @stack:0] [kFunction [a @stack:0] @[] [b @stack:0]]")
  SCOPE_TEST("a\n() { scope a\n a }",
             "[a @context[0]:0] "
             "[kFunction (anonymous) @[] [kScopeDecl [a]] [a @context[1]:0]]")
  SCOPE_TEST("a\n() { { scope a\n a } }",
              "[a @context[0]:0] "
              "[kFunction (anonymous) @[] "
              "[kBlock [kScopeDecl [a]] [a @context[1]:0]]]")
  SCOPE_TEST("a\n() { scope a \n () { scope a\n a } \n a }",
             "[a @context[0]:0] "
             "[kFunction (anonymous) @[] [kScopeDecl [a]] "
             "[kFunction (anonymous) @[] [kScopeDecl [a]] "
             "[a @context[2]:0]] [a @context[1]:0]]")

  // Global lookup
  SCOPE_TEST("scope a\na", "[kScopeDecl [a]] [a @context[-1]:0]")
  SCOPE_TEST("() {scope a\na}",
             "[kFunction (anonymous) @[] [kScopeDecl [a]] [a @context[-1]:0]]")

  // Advanced context
  SCOPE_TEST("a\nb\n() {scope b\nb}\n() {scope a\na}",
             "[a @context[0]:0] [b @context[0]:1] "
             "[kFunction (anonymous) @[] [kScopeDecl [b]] [b @context[1]:1]] "
             "[kFunction (anonymous) @[] [kScopeDecl [a]] [a @context[1]:0]]")
  SCOPE_TEST("a\nb\n() { () {scope b\nb} }\n() {scope a\na}",
             "[a @context[0]:0] [b @context[0]:1] "
             "[kFunction (anonymous) @[] [kFunction (anonymous) @[] "
             "[kScopeDecl [b]] [b @context[2]:1]]] "
             "[kFunction (anonymous) @[] [kScopeDecl [a]] [a @context[1]:0]]")

  // While
  SCOPE_TEST("i = 1\nj = 1\n"
             "while (--i) {scope i, j\nj = j + 1\n}\n"
             "return j",
             "[kAssign [i @stack:0] [1]] "
             "[kAssign [j @stack:1] [1]] "
             "[kWhile [kPreDec [i @stack:0]] "
             "[kBlock [kScopeDecl [i] [j]] "
             "[kAssign [j @stack:1] [kAdd [j @stack:1] [1]]]]] "
             "[kReturn [j @stack:1]]")
  SCOPE_TEST("i = 1\nj = 1\na() { scope i, j }\n"
             "while (--i) {scope i, j\nj = j + 1\n}\n"
             "return j",
             "[kAssign [i @context[0]:0] [1]] "
             "[kAssign [j @context[0]:1] [1]] "
             "[kFunction [a @stack:2] @[] [kScopeDecl [i] [j]]] "
             "[kWhile [kPreDec [i @context[0]:0]] "
             "[kBlock [kScopeDecl [i] [j]] "
             "[kAssign [j @context[0]:1] [kAdd [j @context[0]:1] [1]]]]] "
             "[kReturn [j @context[0]:1]]")

  // Function arguments
  SCOPE_TEST("c = 0\na(a,b)",
             "[kAssign [c @stack:0] [0]] "
             "[kCall [a @stack:1] @[[a @stack:1] [b @stack:2]] ]")
  SCOPE_TEST("c = 0\na(a,b) {}",
             "[kAssign [c @stack:0] [0]] "
             "[kFunction [a @stack:1] @[[a @stack:0] [b @stack:1]] [kNop ]]")
  SCOPE_TEST("(a,b) { return () { scope b\n b } }",
             "[kFunction (anonymous) @[[a @stack:0] [b @context[0]:0]] "
             "[kReturn [kFunction (anonymous) @[] "
             "[kScopeDecl [b]] [b @context[1]:0]]]]")

  // Complex
  SCOPE_TEST("((fn) {\n"
             "  scope print\n"
             "  print(\"fn\", fn)\n"
             "  fn2 = fn\n"
             "  return () {\n"
             "    scope fn, fn2, print\n"
             "    print(\"fn2\", fn2)\n"
             "    fn2(42)\n"
             "  }\n"
             "})((num) {\n"
             "  scope print\n"
             "  print(\"num\", num)\n"
             "})()",

             "[kCall [kCall [kFunction (anonymous) "
             "@[[fn @context[0]:0]] [kScopeDecl [print]] "
             "[kCall [print @context[-1]:0] "
             "@[[kString fn] [fn @context[0]:0]] ] "
             "[kAssign [fn2 @context[0]:1] [fn @context[0]:0]] "
             "[kReturn [kFunction (anonymous) @[] "
             "[kScopeDecl [fn] [fn2] [print]] "
             "[kCall [print @context[-1]:0] @[[kString fn2] "
             "[fn2 @context[1]:1]] ] "
             "[kCall [fn2 @context[1]:1] @[[42]] ]]]] "
             "@[[kFunction (anonymous) @[[num @stack:0]] "
             "[kScopeDecl [print]] "
             "[kCall [print @context[-1]:0] "
             "@[[kString num] [num @stack:0]] ]]] ] @[] ]")
TEST_END("scope test")
