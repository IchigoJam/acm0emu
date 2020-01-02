#define CPU_ERR_NOERR 0
#define CPU_ERR_UNDEFOP 1
#define CPU_ERR_SEGFAULT 2

struct CPU_FLG {
	uint8_t n:1;
	uint8_t z:1;
	uint8_t c:1;
	uint8_t v:1;
};

struct CPU {
	int32_t reg[16];
	struct CPU_FLG flg;

	const uint8_t* rom;
	uint8_t* ram;
	uint8_t* stack;

	uint32_t ignorejump;
	uint32_t steps;
};

void put_bin(int n, int beam) {
	for (int i = 0; i < beam; i++) {
		//x_printf("%d", (n >> (beam - 1 - i)) & 1);
	}
}

#define RAM_START 0x10000000
#define RAM_SIZE (4 * 1024)
#define ROM_START 0
#define ROM_SIZE (32 * 1024)
#define STACK_START 0x10010000
#define STACK_SIZE (4 * 1024)

void cpu_init(struct CPU* cpu, const uint8_t* rom, uint8_t* ram, uint8_t* stack) {
	cpu->steps = 0;
	cpu->rom = rom;
	cpu->ram = ram;
	cpu->stack = stack;
	for (int i = 0; i < 16; i++)
		cpu->reg[i] = 0;
	cpu->flg.z = cpu->flg.n = cpu->flg.c = cpu->flg.v = 0;
	cpu->ignorejump = 0;
}

int cpu_mem_valid(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 2) {
		return 1;
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 2) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t cpu_mem_get1u(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 2) {
		////x_printf("get RAM %04x\n", ad);
		return *(uint8_t*)(cpu->ram + ad - RAM_START);
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 2) {
		////x_printf("get ROM %04x\n", ad);
		return *(uint8_t*)(cpu->rom + ad - ROM_START);
	} else {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
		return 0;
	}
}
int8_t cpu_mem_get1(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 1) {
		////x_printf("get RAM %04x\n", ad);
		return *(int8_t*)(cpu->ram + ad - RAM_START);
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 1) {
		////x_printf("get ROM %04x\n", ad);
		return *(int8_t*)(cpu->rom + ad - ROM_START);
	} else {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
		return 0;
	}
}
int cpu_mem_set1u(struct CPU* cpu, uint32_t ad, uint8_t n) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 1) {
		//printf("set RAM %04x <- %d\n", ad, n);
		*(uint8_t*)(cpu->ram + ad - RAM_START) = n;
		return 0;
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 1) {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
	}
	return CPU_ERR_SEGFAULT;
}
uint16_t cpu_mem_get2u(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 2) {
		////x_printf("get RAM %04x\n", ad);
		return *(uint16_t*)(cpu->ram + ad - RAM_START);
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 2) {
		////x_printf("get ROM %04x\n", ad);
		return *(uint16_t*)(cpu->rom + ad - ROM_START);
	} else {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
		return 0;
	}
}
int16_t cpu_mem_get2(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 2) {
		////x_printf("get RAM %04x\n", ad);
		return *(int16_t*)(cpu->ram + ad - RAM_START);
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 2) {
		////x_printf("get ROM %04x\n", ad);
		return *(int16_t*)(cpu->rom + ad - ROM_START);
	} else {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
		return 0;
	}
}
int cpu_mem_set2u(struct CPU* cpu, uint32_t ad, uint16_t n) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 2) {
		////x_printf("get RAM %04x\n", ad);
		*(uint16_t*)(cpu->ram + ad - RAM_START) = n;
		return 0;
	} else if (ad >= ROM_START && ad < ROM_SIZE + ROM_START - 2) {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
	}
	return CPU_ERR_SEGFAULT;
}
uint32_t cpu_mem_get4u(struct CPU* cpu, uint32_t ad) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 4) {
		////x_printf("get RAM %04x\n", ad);
		return *(uint32_t*)(cpu->ram + ad - RAM_START);
	} else if (ad >= STACK_START && ad <= STACK_START + STACK_SIZE - 4) {
		//printf("get stack %04x\n", ad);
		return *(uint32_t*)(cpu->stack + ad - STACK_START);
	} else if (ad >= ROM_START && ad < ROM_START + ROM_SIZE - 4) {
		////x_printf("get ROM %04x\n", ad);
		return *(uint32_t*)(cpu->rom + ad - ROM_START);
	} else {
		//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
		return 0;
	}
}
int cpu_mem_set4u(struct CPU* cpu, uint32_t ad, uint32_t n) {
	if (ad >= RAM_START && ad <= RAM_START + RAM_SIZE - 4) {
//		//x_printf("set %04x, %d\n", ad, n);
		*(uint32_t*)(cpu->ram + ad - RAM_START) = n;
		return 0;
	} else if (ad >= STACK_START && ad <= STACK_START + STACK_SIZE - 4) {
		//printf("set stack %04x, %d\n", ad, n);
		*(uint32_t*)(cpu->stack + ad - STACK_START) = n;
		return 0;
	}
	//x_printf("access violation! %04x %04x\n", ad, (RAM_START + RAM_SIZE - 4));
	return CPU_ERR_SEGFAULT;
}

static const char* CPU_COND[] = {
	"EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "AL",
};

enum CPU_COND {
	EQ, // 0000	EQ	等しい	Z = 1
	NE, // 0001	NE	等しくない	Z = 0
	CS, // 0010	CS or HS	キャリーセット	C = 1
	CC, // 0011	CC or LO	キャリークリア	C = 0
	MI, // 0100	MI	負	N = 1
	PL, // 0101	PL	ゼロまたは正	N = 0
	VS, // 0110	VS	オーバーフロー	V = 1
	VC, // 0111	VC	オーバーフローなし	V = 0
	HI, // 1000	HI	＞ 大きい (符号無し)	(C = 1) and (Z = 0)
	LS, // 1001	LS	≦ 小さいか等しい (符号無し)	(C = 0) or (Z =1)
	GE, // 1010	GE	≧ 大きいか等しい (符号付)	N = V
	LT, // 1011	LT	＜ 小さい (符号付)	N <> V
	GT, // 1100	GT	＞ 大きい (符号付)	(Z = 0) and (N = V)
	LE, // 1101	LE	≦ 小さいか等しい (符号付)	(Z = 1) or (N <> V)
	AL, // 1110	AL	Always	Any
	NV, // 1111	NV	Always	Any
};

void cpu_changeFlag(struct CPU* cpu, int32_t n) {
	////x_printf("chgflg %d\n", n);
	cpu->flg.z = n == 0;
	cpu->flg.c = (int32_t)n < 0; // ?
	cpu->flg.n = n >> 31;
	cpu->flg.v = 0;
}
int cpu_checkFlag(struct CPU* cpu, int cond) {
	////x_printf("flg: n:%d z:%d c:%d v:%d\n", cpu->flg.n, cpu->flg.z, cpu->flg.c, cpu->flg.v);
	switch (cond) {
		case EQ: return cpu->flg.z == 1;
		case NE: return cpu->flg.z == 0;
		case CS: return cpu->flg.c == 1;
		case CC: return cpu->flg.c == 0;
		case MI: return cpu->flg.n == 1;
		case PL: return cpu->flg.n == 0;
		case VS: return cpu->flg.v == 1;
		case VC: return cpu->flg.v == 0;
		case HI: return cpu->flg.c == 1 && cpu->flg.z == 0;
		case LS: return cpu->flg.c == 0 || cpu->flg.z == 1;
		case GE: return cpu->flg.n == cpu->flg.v;
		case LT: return cpu->flg.n != cpu->flg.v;
		case GT: return cpu->flg.z == 0 && cpu->flg.n == cpu->flg.v;
		case LE: return cpu->flg.n == 1 || cpu->flg.n != cpu->flg.v;
	}
	return 1;
}
int cpu_oplength(struct CPU* cpu) {
//	uint16_t op = *(uint16_t*)(cpu->rom + cpu->reg[15]);
	uint16_t op = cpu_mem_get2u(cpu, cpu->reg[15]);
	if ((op >> 11) == 0b11110) {
		return 4;
	}
	return 2;
}
int cpu_execute(struct CPU* cpu) {
	uint32_t ad = cpu->reg[15];
	if (!cpu_mem_valid(cpu, ad))
		return CPU_ERR_SEGFAULT;
	uint16_t op = cpu_mem_get2u(cpu, ad);
	//*(uint16_t*)(cpu->rom + cpu->reg[15]);
	//x_printf("%04x: %04x\t", cpu->reg[15], op);
	if ((op >> 11) == 0b00100) {
		int rd = (op >> 8) & 7;
		uint8_t n = op & 0xff;
		//x_printf("R%d = %d\n", rd, n);
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 8) == 0b01000110) {
		int rd = (op & 0x80) >> 4 | (op & 7);
		int rm = (op >> 3) & 0xf;
		//x_printf("R%d = R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rm]);
	} else if ((op >> 11) == 0b00110) {
		int rd = (op >> 8) & 7;
		uint8_t n = op & 0xff;
		//x_printf("R%d += %d\n", rd, n);
		cpu_changeFlag(cpu, cpu->reg[rd] += n);
	} else if ((op >> 11) == 0b00111) {
		int rd = (op >> 8) & 7;
		uint8_t n = op & 0xff;
		//x_printf("R%d -= %d\n", rd, n);
		cpu_changeFlag(cpu, cpu->reg[rd] -= n);
	} else if ((op >> 9) == 0b0001110) {
		int n = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d + %d\n", rd, rn, n);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rn] + n);
	} else if ((op >> 9) == 0b0001111) {
		int n = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d - %d\n", rd, rn, n);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rn] - n);
	} else if ((op >> 11) == 0b10100) {
		int rd = (op >> 8) & 7;
		uint8_t n = op & 0xff;
		//x_printf("R%d = PC + %d\n", rd, n);
		cpu->reg[rd] = cpu->reg[15] + (n << 2);
	} else if ((op >> 8) == 0b01000100) {
		int rd = (op & 0x80) >> 4 | (op & 7);
		int rm = (op >> 3) & 0xf;
		//x_printf("R%d += R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] += cpu->reg[rm]);
	} else if ((op >> 11) == 0b01001) {
		int rd = (op >> 8) & 7;
		uint8_t n = op & 0xff;
		uint32_t data = *((uint32_t*)(cpu->rom + cpu->reg[15] + 2) + n);
		//x_printf("R%d = [PC + %d]L (%04x)\n", rd, n, data);
		cpu->reg[rd] = data;
	} else if ((op >> 11) == 0b00000) {
		int n = (op >> 6) & 0x1f;
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d << %d\n", rd, rm, n);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rm] << n);
	} else if ((op >> 11) == 0b00001) {
		int n = (op >> 6) & 0x1f;
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d >> %d\n", rd, rm, n);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rm] >> n);
	} else if ((op >> 6) == 0b0100000010) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d <<= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] <<= cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100000011) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d >>= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] >>= cpu->reg[rm]);
	} else if ((op >> 11) == 0b00010) {
		int n = (op >> 6) & 0x1f;
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = ASR(R%d, %d)\n", rd, rm, n);
		int32_t m = cpu->reg[rd];
		m = m >> n;
		cpu_changeFlag(cpu, cpu->reg[rd] = (uint32_t)n);
	} else if ((op >> 6) == 0b0100000100) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("ASR R%d, R%d\n", rd, rm);
		int32_t n = cpu->reg[rd];
		n = n >> cpu->reg[rd];
		cpu_changeFlag(cpu, cpu->reg[rd] = (uint32_t)n);
	} else if ((op >> 6) == 0b0100001110) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("BIC R%d, R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] &= ~(1 << cpu->reg[rm])); //?
	} else if ((op >> 6) == 0b01011101000) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = REV(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		n = ((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24);
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b01011101001) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = REV16(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		n = ((n & 0xff) << 8) | ((n & 0xff00) >> 8) | ((n & 0xff0000) << 8) | ((n & 0xff000000) >> 8);
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b01011101011) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = REVSH(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		n = ((n & 0xff) << 8) | ((n & 0xff00) >> 8); // ?
		if (n & 0x8000)
			n |= 0xffff0000;
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b0100000101) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("ADC R%d, R%d\n", rd, rm); // Rn+=Rn+C
		cpu_changeFlag(cpu, cpu->reg[rd] += cpu->reg[rm] + cpu->flg.c); // ?
	} else if ((op >> 6) == 0b0100000110) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("SDC R%d, R%d\n", rd, rm); // Rn-=Rn+C
		cpu_changeFlag(cpu, cpu->reg[rd] -= cpu->reg[rm] + cpu->flg.c); // ?
	} else if ((op >> 6) == 0b0100000111) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("ROR R%d, R%d\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		uint32_t m = cpu->reg[rd];
		m = (m >> n) | (m << (32 - n));
		cpu_changeFlag(cpu, cpu->reg[rd] = m); // ?
	} else if ((op >> 6) == 0b0100001010) {
		int rm = (op >> 3) & 7;
		int rn = op & 7;
		cpu_changeFlag(cpu, cpu->reg[rn] - cpu->reg[rm]);
		//printf("R%d(%d) - R%d(%d) C:%d\n", rn, cpu->reg[rn], rm, cpu->reg[rm], cpu->flg.c);
	} else if ((op >> 8) == 0b01000101) {
		int rm = (op >> 3) & 0xf;
		int rn = ((op >> 4) & 8) | (op & 7);
		cpu_changeFlag(cpu, cpu->reg[rn] - cpu->reg[rm]);
		//printf("R%d - R%d C:%d\n", rn, rm, cpu->flg.c);
	} else if ((op >> 6) == 0b0100001000) {
		int rm = (op >> 3) & 7;
		int rn = op & 7;
		//x_printf("R%d & R%d\n", rn, rm);
		cpu_changeFlag(cpu, cpu->reg[rn] & cpu->reg[rm]);
	} else if ((op >> 11) == 0b00101) {
		int rn = (op >> 8) & 7;
		int n = op & 0xff;
		//x_printf("R%d - %d\n", rn, n);
		cpu_changeFlag(cpu, cpu->reg[rn] - n);
	} else if ((op >> 6) == 0b0100001100) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d |= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] |= cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100000000) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d &= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] &= cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100000001) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d ^= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] ^= cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100001101) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d *= R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] *= cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100001001) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = -R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] = -cpu->reg[rm]);
	} else if ((op >> 6) == 0b0100001111) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = ~R%d\n", rd, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] = ~cpu->reg[rm]);
	} else if ((op >> 7) == 0b101100000) {
		int n = op & 0x7f;
		//x_printf("SP += %d\n", n);
		cpu->reg[13] += n << 2;
	} else if ((op >> 7) == 0b101100001) {
		int n = op & 0x7f;
		//x_printf("SP -= %d\n", n);
		cpu->reg[13] -= n << 2;
	} else if ((op >> 9) == 0b0001100) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d + R%d\n", rd, rn, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rn] + cpu->reg[rm]);
	} else if ((op >> 9) == 0b0001101) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = R%d - R%d\n", rd, rn, rm);
		cpu_changeFlag(cpu, cpu->reg[rd] = cpu->reg[rn] - cpu->reg[rm]);
	} else if ((op >> 11) == 0b10101) {
		int rd = (op >> 8) & 7;
		int n = op & 0xff;
		//x_printf("R%d = SP + %d\n", rd, n);
		cpu->reg[rd] = cpu->reg[13] + (n << 2);
	} else if ((op >> 11) == 0b10010) {
		int rd = (op >> 8) & 7;
		int n = op & 0xff;
		//x_printf("[SP + %d]L = R%d\n", n, rd);
		// ?
	} else if ((op >> 11) == 0b10011) {
		int rd = (op >> 8) & 7;
		int n = op & 0xff;
		//x_printf("R%d = [SP + %d]L\n", rd, n);
		// ?

		// memory access
	} else if ((op >> 9) == 0b0101100) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + R%d]L\n", rd, rn, rm);
		cpu->reg[rd] = cpu_mem_get4u(cpu, cpu->reg[rn] + cpu->reg[rm] * 4);
	} else if ((op >> 9) == 0b0101110) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + R%d]\n", rd, rn, rm);
		cpu->reg[rd] = cpu_mem_get1u(cpu, cpu->reg[rn] + cpu->reg[rm]);
	} else if ((op >> 9) == 0b0101011) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + R%d]C\n", rd, rn, rm);
		cpu->reg[rd] = cpu_mem_get1(cpu, cpu->reg[rn] + cpu->reg[rm]);
	} else if ((op >> 9) == 0b0101101) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + R%d]W\n", rd, rn, rm);
		cpu->reg[rd] = cpu_mem_get2u(cpu, cpu->reg[rn] + cpu->reg[rm] * 2);
	} else if ((op >> 9) == 0b0101111) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + R%d]S\n", rd, rn, rm);
		cpu->reg[rd] = cpu_mem_get2(cpu, cpu->reg[rn] + cpu->reg[rm] * 2);
	} else if ((op >> 9) == 0b0101010) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + R%d] = R%d\n", rn, rm, rd);
		cpu_mem_set1u(cpu, cpu->reg[rn] + cpu->reg[rm], cpu->reg[rd]);
	} else if ((op >> 9) == 0b0101001) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + R%d]W = R%d\n", rn, rm, rd);
		cpu_mem_set2u(cpu, cpu->reg[rn] + cpu->reg[rm] * 2, cpu->reg[rd]);
	} else if ((op >> 9) == 0b0101000) {
		int rm = (op >> 6) & 7;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + R%d]L = R%d\n", rn, rm, rd);
		cpu_mem_set4u(cpu, cpu->reg[rn] + cpu->reg[rm] * 4, cpu->reg[rd]);
	} else if ((op >> 11) == 0b01100) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + %d]L = R%d\n", rn, n, rd);
		cpu_mem_set4u(cpu, cpu->reg[rn] + n * 4, cpu->reg[rd]);
	} else if ((op >> 11) == 0b10000) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + %d]W = R%d\n", rn, n, rd);
		cpu_mem_set2u(cpu, cpu->reg[rn] + n * 2, cpu->reg[rd]);
	} else if ((op >> 11) == 0b01111) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + %d]\n", rd, rn, n);
		cpu->reg[rd] = cpu_mem_get1u(cpu, cpu->reg[rn] + n);
	} else if ((op >> 11) == 0b10001) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + %d]W\n", rd, rn, n);
		cpu->reg[rd] = cpu_mem_get2u(cpu, cpu->reg[rn] + n * 2);
	} else if ((op >> 11) == 0b01101) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = [R%d + %d]L\n", rd, rn, n);
		cpu->reg[rd] = cpu_mem_get4u(cpu, cpu->reg[rn] + n * 4);
	} else if ((op >> 11) == 0b01110) {
		int n = (op >> 6) & 0x1f;
		int rn = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("[R%d + %d] = R%d\n", rn, n, rd);
		cpu_mem_set1u(cpu, cpu->reg[rn] + n, cpu->reg[rd]);

		// sign extend
	} else if ((op >> 6) == 0b1011001000) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = SXTH(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		if (n & 0x8000) { // ?
			n |= 0xffff0000;
		}
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b1011001001) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = SXTB(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		if (n & 0x80) { // ?
			n |= 0xffffff00;
		}
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b1011001010) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = UXTH(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		n &= 0xffff;
		cpu_changeFlag(cpu, cpu->reg[rd] = n);
	} else if ((op >> 6) == 0b1011001011) {
		int rm = (op >> 3) & 7;
		int rd = op & 7;
		//x_printf("R%d = UXTB(R%d)\n", rd, rm);
		uint32_t n = cpu->reg[rm];
		n &= 0xff;
		cpu_changeFlag(cpu, cpu->reg[rd] = n);

		// stack
	} else if ((op >> 9) == 0b1011010) {
		int regs = op & 0x1ff;
		//x_printf("PUSH {");
		int flg = 0;
		for (int i = 0; i < 9; i++) {
			if (regs & (1 << i)) {
				if (!flg) {
					flg = 1;
				} else {
					//x_printf(",");
				}
				uint32_t n = 0;
				if (i < 8) {
					//x_printf("R%d", i);
					n = cpu->reg[i];
				} else {
					//x_printf("LR");
					n = cpu->reg[14];
				}
				cpu->reg[13] -= 4;
				cpu_mem_set4u(cpu, cpu->reg[13], n);
				//printf("PUSH %x %x(%d)\n", cpu->reg[13], n, i);
			}
		}
		//x_printf("}\n");
	} else if ((op >> 9) == 0b1011110) {
		int regs = op & 0x1ff;
		//x_printf("POP {");
		int flg = 0;
		for (int i = 8; i >= 0; i--) {
			if (regs & (1 << i)) {
				if (!flg) {
					flg = 1;
				} else {
					//x_printf(",");
				}
				uint32_t n = cpu_mem_get4u(cpu, cpu->reg[13]);
				//printf("POP %x %x(%d)\n", cpu->reg[13], n, i);
				cpu->reg[13] += 4;
				if (i < 8) {
					//x_printf("R%d[%d]", i, n);
					cpu->reg[i] = n;
				} else {
					//x_printf("PC[%04x]", n);
					cpu->reg[15] = n - 2;
				}
			}
		}
		//x_printf("}\n");
	} else if ((op >> 11) == 0b11000) {
		int rn = (op >> 8) & 7;
		int regs = op & 0xff;
		//x_printf("STM R%d, {", rn);
		int flg = 0;
		for (int i = 0; i < 8; i++) {
			if (regs & (1 << i)) {
				if (!flg) {
					flg = 1;
				} else {
					//x_printf(",");
				}
				//x_printf("R%d", i);
				uint32_t n = cpu->reg[i];
				cpu_mem_set4u(cpu, cpu->reg[rn], n);
				cpu->reg[rn] += 4;
				//printf("STM %x %d %x(%d)\n", cpu->reg[rn], cpu->reg[0], n, i);
			}
		}
		//x_printf("}\n");
	} else if ((op >> 11) == 0b11001) {
		int rn = (op >> 8) & 7;
		int regs = op & 0xff;
		//x_printf("LDM R%d, {", rn);
		int flg = 0;
		for (int i = 0; i < 8; i++) {
			if (regs & (1 << i)) {
				if (!flg) {
					flg = 1;
				} else {
					//x_printf(",");
				}
				uint32_t n = cpu_mem_get4u(cpu, cpu->reg[rn]);
				//printf("LDM %x %x(%d)\n", cpu->reg[rn], n, i);
				cpu->reg[i] = n;
				cpu->reg[rn] += 4;
			}
		}
		//x_printf("}\n");
	} else if ((op >> 12) == 0b1101) {
		int cond = (op >> 8) & 0xf;
		int n = op & 0xff;
		if (n & 0x80)
			n -= 0x100;
		int ad = (n * 2) + 4 + cpu->reg[15];
		//x_printf("IF %s GOTO %04x\n", CPU_COND[cond], ad);

		//if (cond == 14 || !cpu_is_called(cpu, ad)) {
		if (cpu_checkFlag(cpu, cond)) {
			////x_printf("flg!\n");
			cpu->reg[15] = ad - 2;
		}
	} else if ((op >> 11) == 0b11110) {
		int n = op & 0x7ff;
		cpu->reg[15] += 2;
		uint16_t op2 = cpu_mem_get2u(cpu, cpu->reg[15]);
		if ((op2 >> 11) != 0b11111) {
			//x_printf("GOSUB err %4x\n", op2);
			return 2;
		}
		int m = (n << 11) | (op2 & 0x7ff);
		if (m & (1 << 21)) {
			m -= 1 << 22;
		}
		int ad = (m * 2) + 2 + cpu->reg[15];
		//x_printf("GOSUB %04x %0d\n", ad, m);
		cpu->reg[14] = cpu->reg[15];
		cpu->reg[15] = ad - 2;
		////x_printf("new address: %d\n", ad);
	} else if ((op >> 7) == 0b010001110) {
		int rm = (op >> 3) & 0xf;
		// GOTO
		cpu->reg[15] = cpu->reg[rm] - 2;
	} else if ((op >> 7) == 0b010001111) {
		int rm = (op >> 3) & 0xf;
		//x_printf("GOSUB R%d\n", rm);
		cpu->reg[14] = cpu->reg[15];
		cpu->reg[15] = cpu->reg[rm] - 2;
	} else if ((op >> 11) == 0b11100) {
		int n = op & 0x7ff;
		if (n & 0x400)
			n = n - 0x800;
		int ad = (n * 2) + 4 + cpu->reg[15];
		//x_printf("GOTO %04x pc:%04x\n", ad, cpu->reg[15]);
		cpu->reg[15] = ad - 2;

		// other
	} else if (op == 0b1011011001110010) {
		//x_printf("CPSID\n");
	} else if (op == 0b1011011001100010) {
		//x_printf("CPSIE\n");
	} else if (op == 0b1011111100110000) {
		//x_printf("WFI\n");
	} else {
		//put_bin(op, 16);
		//printf(" undef %x\n", op);
		return CPU_ERR_UNDEFOP;
	}
	cpu->reg[15] += 2;
	cpu->steps++;
	return 0;
}
