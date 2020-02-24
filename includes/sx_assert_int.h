#ifndef SX_ASSERT_INT_H_
#define SX_ASSERT_INT_H_

/* Internal details of Simulix assertion support.
 */

#include <setjmp.h>
#include <stdbool.h>

extern jmp_buf fmu_exit;
extern bool has_jmp;

#endif
