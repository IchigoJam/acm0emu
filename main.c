#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acm0emu.h"

static const uint8_t ASM[] = {
	0x02, 0x46, 0x00, 0x20, 0x52, 0x1A, 0x01, 0xDD, 0x01, 0x30, 0xFB, 0xE7, 0x51, 0x18, 0x70, 0x47,
};
/*
{R0, R1} calcDiv(n, m)
// long( 商,あまり ) calcDivU(uint32 n, uint32 m) // 16byte
	R2=R0
	R0=0
@LOOP
	R2=R2-R1
	IF LE GOTO @END
	R0+=1
	GOTO @LOOP
@END
	R1=R2+R1
	RET

for asm15 https://ichigojam.github.io/asm15/
(output type : C language)
*/

int main(int argc, char** argv) {
	struct CPU cpu;
	uint8_t memory[1024 * 4];
	uint8_t stack[STACK_SIZE];
	cpu_init(&cpu, (const uint8_t*)ASM, memory, stack);
	cpu.reg[13] = STACK_START + STACK_SIZE;
	cpu.reg[14] = 0xFF000000; // lr
	cpu.reg[15] = ROM_START;

	int n = 10530;
	int m = 303;
	cpu.reg[0] = n;
	cpu.reg[1] = m;

	for (;;) {
		printf("address %04x, op %2x\n", cpu.reg[15], cpu_mem_get2u(&cpu, cpu.reg[15]));
		if (cpu_execute(&cpu)) {
			return 1;
		}
		if (cpu.reg[15] == 0xFF000000)
			break;
	}
	for (int i = 0; i < 16; i++) {
		printf("R[%d]: %d\n", i, cpu.reg[i]);
	}
	printf("%d / %d = %d ... %d\n", n, m, n / m, n % m);
	return 0;
}

