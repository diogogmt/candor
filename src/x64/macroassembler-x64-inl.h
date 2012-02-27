#ifndef _SRC_X64_MARCOASSEMBLER_INL_H_
#define _SRC_X64_MARCOASSEMBLER_INL_H_

#include "macroassembler-x64.h"

namespace dotlang {

inline void Masm::Push(Register src) {
  ChangeAlign(1);
  push(src);
}


inline void Masm::Pop(Register src) {
  pop(src);
  ChangeAlign(-1);
}


inline void Masm::PushTagged(Register src) {
  shl(src, 1);
  inc(src);
  Push(src);
  shr(src, 1);
}


inline void Masm::PopTagged(Register src) {
  Pop(src);
  shr(src, 1);
}


inline void Masm::PreservePop(Register src, Register preserve) {
  if (src.is(preserve)) {
    pop(scratch);
  } else {
    pop(src);
  }
}


inline void Masm::Save(Register src) {
  if (!result().is(src)) Push(src);
}


inline void Masm::Restore(Register src) {
  if (!result().is(src)) Pop(src);
}


inline void Masm::Result(Register src) {
  if (!result().is(src)) movq(result(), src);
}


inline uint64_t Masm::TagNumber(uint64_t number) {
  return number << 1 | 1;
}


inline void Masm::Untag(Register src) {
  shr(src, 1);
}

} // namespace dotlang

#endif // _SRC_X64_MARCOASSEMBLER_INL_H_