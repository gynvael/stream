#include <stdint.h>
#include "idt.h"
#include "int_handlers.h"

struct IDTEntry {
  uint16_t offset_15_0;
  uint16_t seg_selector;
  uint16_t flags;
  uint16_t offset_31_16;
  uint32_t offset_63_32;
  uint32_t reserved;
} __attribute__((packed)) ;

typedef struct IDTEntry IDTEntry;

#define IDTFLAGS(ist, type, dpl, p) \
  (ist | (type << 8) | (dpl << 13) | (p << 15))

#define OFFSET_15_0(func) ((uint64_t)func & 0xffff)
#define OFFSET_31_16(func) (((uint64_t)func & 0xffff0000) >> 16)
#define OFFSET_63_32(func) ((uint64_t)func >> 32)

#define IDTITEM(e, func, ist) { \
    e.offset_15_0 = OFFSET_15_0(func); \
    e.seg_selector = 0x8; \
    e.flags = IDTFLAGS(ist, 0xE, 0, 1); \
    e.offset_31_16 = OFFSET_31_16(func); \
    e.offset_63_32 = OFFSET_63_32(func); \
  }


IDTEntry IDT[256];

struct IDTPtr {
  uint16_t limit;
  uint64_t ptr;
} __attribute__((packed)) ;
typedef struct IDTPtr IDTPtr;

extern void Int_Wrapper_DE(void);

void SetIDTR(void) {
  IDTITEM(IDT[0], Int_Wrapper_DE, 0);

  IDTPtr ptr = {
    256 * 16 - 1,
    (uint64_t)&IDT
  };

  __asm("lidt %0" : : "m"(ptr));
}

