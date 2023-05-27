/**
 * A class aimed at demonstrating some of the presentation features of Doxide.
 * 
 * @ingroup presentation
 * 
 * ## Admonition examples
 * 
 * @note
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @abstract
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @info
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @tip
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @success
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @question
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @warning
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @failure
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @danger
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @bug
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @example
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 * 
 * @quote
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
 * tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
 * veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
 * commodo consequat.
 */
class Presentation {
public:
  /**
   * Function demonstrating all possible annotations.
   * 
   * @tparam T Template parameter.
   * 
   * @param x Input parameter.
   * @param[in] y Input parameter.
   * @param[out] z Output parameter.
   * @param[in,out] w Input-output parameter.
   * 
   * @return Return value.
   * 
   * @pre Pre-condition.
   * @post Post-condition.
   * @throw Exception.
   */
  template<class T>
  int f(int x, int y, int& z, int& w) {
    //
  }

  /**
   * Function demonstrating a link.
   * 
   * @see [f](#f)
   */
  int g() {
    //
  }

  /**
   * Operator.
   */
  int operator+(int x) {
    //
  }

  /**
   * Variable
   */
  int x = 0;
};

/**
 * Function demonstrating all possible annotations.
 * 
 * @ingroup presentation
 * 
 * @tparam T Template parameter.
 * 
 * @param x Input parameter.
 * @param[in] y Input parameter.
 * @param[out] z Output parameter.
 * @param[in,out] w Input-output parameter.
 * 
 * @return Return value.
 * 
 * @pre Pre-condition.
 * @post Post-condition.
 * @throw Exception.
 */
template<class T>
int f(int x, int y, int& z, int& w) {
  //
}

/**
 * Function demonstrating a link.
 * 
 * @ingroup presentation
 * 
 * @see [f](#f)
 */
int g() {
  //
}
