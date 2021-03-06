#ifndef _SRC_X64_MARCOASSEMBLER_H_
#define _SRC_X64_MARCOASSEMBLER_H_

#include "assembler-x64.h"
#include "assembler-x64-inl.h"
#include "ast.h" // AstNode
#include "code-space.h" // CodeSpace
#include "heap.h" // Heap::HeapTag and etc

namespace candor {
namespace internal {

// Forward declaration
class BaseStub;

class Masm : public Assembler {
 public:
  Masm(CodeSpace* space);

  // Save/restore all valuable register
  void Pushad();
  void Popad(Register preserve);

  class Align {
   public:
    Align(Masm* masm);
    ~Align();
   private:
    Masm* masm_;
    int32_t align_;
  };

  // Skip some bytes to make code aligned
  void AlignCode();

  // Alignment helpers
  inline void ChangeAlign(int32_t slots) { align_ += slots; }

  // Allocate some space in heap's new space current page
  // Jmp to runtime_allocate label on exhaust or fail
  void Allocate(Heap::HeapTag tag,
                Register size_reg,
                uint32_t size,
                Register result);

  // Allocate context and function
  void AllocateContext(uint32_t slots);
  void AllocateFunction(Register addr, Register result);

  // Allocate heap numbers
  void AllocateNumber(DoubleRegister value, Register result);

  // Allocate boolean value, `value` should be either 0 or 1
  void AllocateBoolean(Register value, Register result);

  // Allocate heap string (symbol)
  void AllocateString(const char* value, uint32_t length, Register result);

  // Allocate object&map
  void AllocateObjectLiteral(Heap::HeapTag tag, Register size, Register result);

  // Fills memory segment with immediate value
  void Fill(Register start, Register end, Immediate value);

  // Fill stack slots with nil
  void FillStackSlots(uint32_t slots);

  // Generate enter/exit frame sequences
  void EnterFramePrologue();
  void EnterFrameEpilogue();
  void ExitFramePrologue();
  void ExitFrameEpilogue();

  // Perform garbage collection if needed (heap flag is set)
  void CheckGC();

  void IsNil(Register reference, Label* not_nil, Label* is_nil);
  void IsUnboxed(Register reference, Label* not_unboxed, Label* unboxed);

  // Checks if object has specific type
  void IsHeapObject(Heap::HeapTag tag,
                    Register reference,
                    Label* mismatch,
                    Label* match);
  void IsTrue(Register reference, Label* is_false, Label* is_true);

  // Sets correct environment and calls function
  void Call(Register addr);
  void Call(Operand& addr);
  void Call(Register fn, uint32_t args);
  void Call(char* stub);

  enum BinOpUsage {
    kIntegral,
    kDouble
  };

  inline void Push(Register src);
  inline void Pop(Register src);
  inline void PushTagged(Register src);
  inline void PopTagged(Register src);
  inline void PreservePop(Register src, Register preserve);
  inline void Save(Register src);
  inline void Restore(Register src);
  inline void Result(Register src);
  inline uint64_t TagNumber(uint64_t number);
  inline void TagNumber(Register src);
  inline void Untag(Register src);
  inline Condition BinOpToCondition(BinOp::BinOpType type, BinOpUsage usage);

  // See VisitForSlot and VisitForValue in fullgen for disambiguation
  inline Register result() { return result_; }
  inline Operand* slot() { return slot_; }
  inline Heap* heap() { return space_->heap(); }
  inline Stubs* stubs() { return space_->stubs(); }

  Register result_;
  Operand* slot_;

 protected:
  CodeSpace* space_;

  int32_t align_;

  friend class Align;
};

} // namespace internal
} // namespace candor

#endif // _SRC_X64_MARCOASSEMBLER_H_
