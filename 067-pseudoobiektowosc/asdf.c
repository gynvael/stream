#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct MyClass;
typedef struct MyClass MyClass;
typedef MyClass MyBetterClass;

struct MyClass {
  int a;

  // vtable - virtual table
  void (*DoWork)(MyClass *self);  //
};

void MyClassPrint(MyClass *self) {
  printf("%i\n", self->a);
}

void MyClassBaseDoWork(MyClass *self) {
  self->a *= 2;
}

void MyClassDoWork(MyClass *self) {
  self->DoWork(self);
}

MyClass *MyClassNew(int a) {
  MyClass *obj = malloc(sizeof(MyClass));
  obj->a = a;
  obj->DoWork = MyClassBaseDoWork;
  return obj;
}

void MyClassBetterDoWork(MyClass *self) {
  self->a *= 3;
}

MyBetterClass *MyBetterClassNew(int a) {
  MyClass *obj = MyClassNew(a);
  obj->DoWork = MyClassBetterDoWork;
  return obj;
}

int main(void) {
  MyClass *a = MyClassNew(10);
  MyBetterClass *b = MyBetterClassNew(10);

  MyClassPrint(a);
  MyClassDoWork(a);
  MyClassPrint(a);

  MyClassPrint(b);
  MyClassDoWork(b);
  MyClassPrint(b);




  return 0;
}










