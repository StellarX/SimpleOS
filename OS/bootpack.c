void io_hlt(void);
void write_mem8(int addr, int data);


void HariMain(void)
{
	int i; /* 32位整型 */
	char *p; /* 变量p，用于BYTE型地址/8bit */
	p = (char *) 0xa0000; /* 代入显存起始地址*/ 

	for (i = 0; i <= 0xffff; i++) {//往显存写入数据
		p[i] = i & 0x0f;/* 这可以替代write_mem8(i, i & 0x0f); */
	}

	for (;;) {
		io_hlt();
	}
}
