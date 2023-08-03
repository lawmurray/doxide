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

/**
 * Forward class declaration.
 * 
 * @ingroup parsing
 */
class ForwardClass;

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
 * Test variable of reference type.
 * 
 * @ingroup parsing
 */
int& ref = x;

/**
 * Test variable of pointer type.
 * 
 * @ingroup parsing
 */
int* ptr = &x;

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
   * Test member variable.
   */
  int w;

  /**
   * Test member variable with initializer.
   */
  int x = 0;

  /**
   * Test member variable with brace initializer.
   */
  int z{0};

  /**
   * Test member variable of reference type.
   */
  int& ref = x;

  /**
   * Test member variable of pointer type.
   */
  int* ptr = &x;

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
   * Attempt to move a member out of its class with `@@ingroup`, but should
   * remain in `ClassWithMembers`.
   * 
   * @ingroup demo
   */
  int escapee;
};

/**
 * Parent namespace.
 * 
 * @ingroup parsing
 */
namespace parent {
  /**
   * Child namespace.
   */
  namespace child {
    //
  }
}

/**
 * Child namespace declared with nested namespace specifier.
 * 
 * @ingroup parsing
 */
namespace parent::nested {
  //
}

