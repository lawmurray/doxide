/**
 * Test macro.
 * 
 * @ingroup parsing
 */
#define MACRO 0

/**
 * Test macro with arguments.
 * 
 * @ingroup parsing
 */
#define MACRO_WITH_ARGS(x, y) (x + y)

#define MACRO_AFTER 0
///< Test macro, documented after entity with `///` comment
///< @ingroup parsing

#define MACRO_WITH_ARGS_AFTER(x, y) (x + y)
///< Test macro with arguments, documented after entity with `///` comment
///< @ingroup parsing

/**
 * Forward class declaration.
 * 
 * @ingroup parsing
 */
class ForwardClass;

/**
 * Documented with `/** ... *@/` style preceding comment.
 * 
 * @ingroup parsing
 */
int a;

/// Documented with `///` style preceding comment. @ingroup parsing
int b;

///
/// Documented with `///` style preceding comment that wraps across multiple
/// lines and has multiple paragraphs.
///
///This is the second paragraph.
///
/// @ingroup parsing
///
/// This is the third paragraph.
///
int c;

int d;  ///< Documented after entity with `///` comment @ingroup parsing

/**
 * Documentation comment with * and / characters.
 *
 * @ingroup parsing
 */
int e;

/**
 * Comment where leading whitespace is important, using `/** ... *@/` style
 * preceding comment. Details should show code if indenting is correctly
 * preserved.
 *
 * @ingroup parsing
 *
 *     int main();
 */
int whitespace1;

/**
 * Comment where leading whitespace is important, using `///` style preceding
 * comment. Details should show code if indenting is correctly preserved.
 *
 * @ingroup parsing
 *
 *     int main();
 */
int whitespace2;

/**
 * Test variable.
 * 
 * @ingroup parsing
 */
int w;

/**
 * Test variable with initializer.
 * 
 * @ingroup parsing
 */
int x = 0;

/**
 * Test variable with parenthetical initializer.
 * 
 * @ingroup parsing
 */
int y(0);

/**
 * Test variable with brace initializer.
 * 
 * @ingroup parsing
 */
int z{0};

/**
 * Test variable of array type.
 * 
 * @ingroup parsing
 */
int arr1[10];

/**
 * Test variable of reference type.
 * 
 * @ingroup parsing
 */
int& ref1;

/**
 * Test variable of pointer type.
 * 
 * @ingroup parsing
 */
int* ptr1;

/**
 * Test variable of array type with initializer.
 * 
 * @ingroup parsing
 */
int arr2[10] = {0};

/**
 * Test variable of reference type with initializer.
 * 
 * @ingroup parsing
 */
int& ref2 = x;

/**
 * Test variable of pointer type with initializer.
 * 
 * @ingroup parsing
 */
int* ptr2 = &x;

/**
 * Test variable with unicode character.
 * 
 * @ingroup parsing
 */
int λ;

/**
 * Test variable of function pointer type.
 * 
 * @ingroup parsing
 */
int (*fp)(int, int);

/**
 * Test function.
 * 
 * @ingroup parsing
 */
int f(int x, int y);

/**
 * Test function that returns a reference.
 * 
 * @ingroup parsing
 */
int& f(int x, int y);

/**
 * Test function that returns a pointer.
 * 
 * @ingroup parsing
 */
int* f(int x, int y);

/**
 * Test function that returns a function pointer.
 * 
 * @ingroup parsing
 */
int (*f(int x, int y))(int, int);

/**
 * Test function template.
 * 
 * @ingroup parsing
 */
template<class T>
T f(T x, T y);

/**
 * Test function template with SFINAE.
 * 
 * @ingroup parsing
 */
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
T f(T x, T y);

/**
 * Test inline function.
 * 
 * @ingroup parsing
 */
inline int f(int x, int y) {
  return 0;
}

/**
 * Test inline function template.
 * 
 * @ingroup parsing
 */
template<class T>
T f(T x, T y) {
  return T();
}

/**
 * Test inline function template with SFINAE.
 * 
 * @ingroup parsing
 */
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
T f(T x, T y) {
  return T();
}

/**
 * Test operator.
 * 
 * @ingroup parsing
 */
int operator+(int x, int y);

/**
 * Test operator template.
 * 
 * @ingroup parsing
 */
template<class T>
T operator+(T x, T y);

/**
 * Test operator template with SFINAE.
 * 
 * @ingroup parsing
 */
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
T operator+(T x, T y);

/**
 * Test inline operator.
 * 
 * @ingroup parsing
 */
inline int operator+(int x, int y) {
  return 0;
}

/**
 * Test inline operator template.
 * 
 * @ingroup parsing
 */
template<class T>
T operator+(T x, T y) {
  return T();
}

/**
 * Test inline operator template with SFINAE.
 * 
 * @ingroup parsing
 */
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
T operator+(T x, T y) {
  return T();
}

/**
 * Test class.
 * 
 * @ingroup parsing
 */
class Class {
  //
};

/**
 * Test struct.
 * 
 * @ingroup parsing
 */
struct Struct {
  //
};

/**
 * Test union.
 * 
 * @ingroup parsing
 */
union Union {
  //
};

/**
 * Test class template.
 * 
 * @ingroup parsing
 */
template<class T>
class ClassTemplate {
  //
};

/**
 * Test struct template.
 * 
 * @ingroup parsing
 */
template<class T>
struct StructTemplate {
  //
};

/**
 * Test union template.
 * 
 * @ingroup parsing
 */
template<class T>
union UnionTemplate {
  //
};

/**
 * Test typedef.
 * 
 * @ingroup parsing
 */
typedef int TypeDef;

/**
 * Test type alias.
 * 
 * @ingroup parsing
 */
using TypeAlias = int;

/**
 * Test typedef of function pointer type.
 * 
 * @ingroup parsing
 */
typedef int (*TypeDefFunctionPointer)(int, int);

/**
 * Test type alias of function pointer type.
 * 
 * @ingroup parsing
 */
using TypeAliasFunctionPointer = int (*)(int, int);

/**
 * Test type alias template.
 * 
 * @ingroup parsing
 */
template<class T>
using TypeAliasTemplate = int;

/**
 * Test concept.
 * 
 * @ingroup parsing
 */
template<class T>
concept Concept = std::is_arithmetic_v<T>;

/**
 * Test trivial concept.
 * 
 * @ingroup parsing
 */
template<class T>
concept TrivialConcept = true;

/**
 * Test enumeration.
 * 
 * @ingroup parsing
 */
enum Enum {
  /**
   * First possibility.
   */
  FIRST,

  /**
   * Second possibility.
   */
  SECOND,

  /**
   * Third possibility.
   */
  THIRD
};

/**
 * Test enumeration with values documented after.
 * 
 * @ingroup parsing
 */
enum EnumAfter {
  FIRST,   ///< First possibility, document after with `///` comment
  SECOND,  ///< Second possibility, document after with `///` comment
  THIRD    ///< Third possibility, document after with `///` comment
};

/**
 * Test scoped enumeration.
 * 
 * @ingroup parsing
 */
enum class EnumClass {
  /**
   * First possibility.
   */
  FIRST,

  /**
   * Second possibility.
   */
  SECOND,

  /**
   * Third possibility.
   */
  THIRD
};

/**
 * Test scoped enumeration.
 * 
 * @ingroup parsing
 */
enum struct EnumStruct {
  /**
   * First possibility.
   */
  FIRST,

  /**
   * Second possibility.
   */
  SECOND,

  /**
   * Third possibility.
   */
  THIRD
};

/**
 * Class with members.
 * 
 * @ingroup parsing
 */
class ClassWithMembers {
public:
  /**
   * Inner class.
   */
  class InnerClass {
    /**
     * Test member variable of inner class.
     */
    int w;
  };

  /**
   * Inner enumeration.
   */
  enum InnerEnum {
    /**
     * First possibility.
     */
    FIRST,

    /**
     * Second possibility.
     */
    SECOND,

    /**
     * Third possibility.
     */
    THIRD
  };

  /**
   * Inner typedef.
   */
  typedef int InnerTypeDef;

  /**
   * Inner type alias.
   */
  using InnerTypeAlias = int;

  /**
   * Test member variable.
   */
  int x;

  /**
   * Test member variable with initializer.
   */
  int y = 1;

  /**
   * Test member variable with brace initializer.
   */
  int z{0};

  /**
   * Test member variable of array type.
   */
  int arr1[10];

  /**
   * Test member variable of reference type.
   */
  int& ref1;

  /**
   * Test member variable of pointer type.
   */
  int* ptr1;

  /**
   * Test member variable of array type with initializer.
   */
  int arr2[10] = {0};

  /**
   * Test member variable of reference type with initializer.
   */
  int& ref2 = x;

  /**
   * Test member variable of pointer type with initializer.
   */
  int* ptr2 = &x;

  /**
   * Test member variable with unicode character.
   */
  int λ;

  /**
   * Test member variable of function pointer type.
   */
  int (*fp)(int, int);

  /**
   * Test constructor.
   */
  ClassWithMembers();

  /**
   * Test destructor.
   */
  ~ClassWithMembers();

  /**
   * Test member function.
   */
  int f(int x, int y);

  /**
   * Test member function that returns a reference.
   */
  int& f(int x, int y);

  /**
   * Test member function that returns a pointer.
   */
  int* f(int x, int y);

  /**
   * Test member function that returns a function pointer.
   */
  int (*f(int x, int y))(int, int);

  /**
   * Test member function template.
   */
  template<class T>
  T f(T x, T y);

  /**
   * Test member function template with SFINAE.
   */
  template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  T f(T x, T y);

  /**
   * Test inline member function.
   */
  inline int f(int x, int y) {
    return 0;
  }

  /**
   * Test inline member function template.
   */
  template<class T>
  T f(T x, T y) {
    return T();
  }

  /**
   * Test inline member function template with SFINAE.
   */
  template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  T f(T x, T y) {
    return T();
  }

  /**
   * Test member operator.
   */
  int operator+(int x, int y);

  /**
   * Test member operator template.
   */
  template<class T>
  T operator+(T x, T y);

  /**
   * Test member function template with SFINAE.
   */
  template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  T operator+(T x, T y);

  /**
   * Test inline member operator.
   */
  inline int operator+(int x, int y) {
    return 0;
  }

  /**
   * Test inline member operator template.
   */
  template<class T>
  T operator+(T x, T y) {
    return T();
  }

  /**
   * Test inline member operator template with SFINAE.
   */
  template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  T operator+(T x, T y) {
    return T();
  }

  /**
   * Test assignment operator.
   */
  ClassWithMembers& operator=(const ClassWithMembers& o);

  /**
   * Test conversion operator declaration.
   */
  operator int() const;

  /**
   * Test reference conversion operator declaration.
   */
  operator int&() const;

  /**
   * Test pointer conversion operator declaration.
   */
  operator int*() const;

  /**
   * Test conversion operator definition.
   */
  operator double() const {
    return 0.0;
  }

  /**
   * Attempt to move a member out of its class with `@ingroup`, but should
   * remain in `ClassWithMembers`.
   * 
   * @ingroup parsing
   */
  int escapee;
};

/**
 * Class template with members.
 * 
 * @ingroup parsing
 */
template<class T>
class ClassTemplateWithMembers {
public:
  /**
   * Test member variable.
   */
  int w;
};

/**
 * Namespace `ns1`.
 */
namespace ns1 {
  /**
   * Namespace `ns1::ns2a`.
   */
  namespace ns2a {
    //
  }
}

/**
 * Namespace `ns1::ns2b` declared with nested namespace specifier.
 */
namespace ns1::ns2b {
  //
}

/**
 * Namespace `ns1::ns2a::ns3` declared with nested namespace specifier.
 */
namespace ns1::ns2a::ns3 {
  //
}
