// C++

class MyClass {
 public:
  static int asdf;
  static int xyz() {}

  int qwer;
  int okl() { this->qwer; }
};

// asdf
// MyClass::xyz();

// MyClass *x = new MyClass;
// x->qwer = 234;
// return x->okl();


// C
struct MyClass {
  int qwer;
};

int MyClassOkl(struct MyClass *this) {
  this->qwer = 1234;
}

struct MyClass *x; // inited
MyClassOkl(x);


FILE *f = fopen("asdf", "wb");
fwrite("asdf", 4, 1, f);
fclose(f);


