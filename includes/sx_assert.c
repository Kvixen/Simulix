#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>

#include "sx_assert.h"
#include "sx_assert_int.h"

jmp_buf fmu_exit;
bool has_jmp = false;

/*TODO proper logging */
void sx_assert_f(int cond, const char *file, int line, const char *msg) {
	if (cond)
		return;
	fprintf(stderr, "%s:%d: %s\n", file, line, msg);
	if (has_jmp)
		longjmp(fmu_exit, 1);
}
