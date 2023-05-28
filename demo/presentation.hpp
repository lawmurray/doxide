/**
 * Function with documentation demonstrating various presentation features.
 * 
 * @ingroup presentation
 * 
 * #### Lists
 *
 * Itemized list:
 * 
 *   - Itemized list item.
 *   - Itemized list item.
 *   - Itemized list item.
 * 
 * Numbered list:
 * 
 *   1. Enumerated list item.
 *   2. Enumerated list item.
 *   3. Enumerated list item.
 * 
 * #### Tables
 * 
 * | Column 1 | Column 2 | Column 3 |
 * | -------- | -------- | -------- |
 * | Row 1, Column 1 | Row 1, Column 2 | Row 1, Column 3 |
 * | Row 2, Column 1 | Row 2, Column 2 | Row 2, Column 3 |
 * | Row 3, Column 1 | Row 3, Column 2 | Row 3, Column 3 |
 * 
 * #### Code
 * 
 * ```cpp
 * void f(int x, int y);
 * ```
 *
 * ```cpp
 * /* nested comment in code *@/
 * void f(int x, int y);
 * ```
 * 
 * #### Mathematics
 * 
 * Inline math: $y = Ax + \epsilon$. Display math:
 * 
 * $$
 * y = Ax + \epsilon.
 * $$
 * 
 * #### Images
 * 
 * ![Doxide logo](/assets/logo_red.svg){width="256"}
 * 
 * #### Admonitions
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
int f(int x, int y);

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
int f(int x, int y, int& z, int& w);

/**
 * Function demonstrating a link.
 * 
 * @ingroup presentation
 * 
 * @see [f](#f)
 */
int g();
