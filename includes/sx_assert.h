#ifndef SX_ASSERT_H_
#define SX_ASSERT_H_

/* Simulix assertion support.
 */

extern void sx_assert_f(int cond, const char *file, int line, const char *msg);

#define sx_assert(expr, msg) sx_assert_f((expr), __FILE__, __LINE__, (msg))

#endif
