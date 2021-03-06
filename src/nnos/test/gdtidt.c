/**
 * ===================NNOS-gdt&idt============================
 *【GDT和IDT处理函数】
 *#结构体见systructual.h
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

/*GDT和IDT初始化函数*/
void initGdtIdt(){ 
	int i;
	SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) GDT_ADR; //定义GDT，指定基址。GDT范围为0x270000~0x27ffff（此范围可自定义）
	GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) IDT_ADR;	//定义IDT，指定基址。IDT范围为0x26f800~0x26ffff（此范围可自定义）。
	
	for(i = 0;i <= GDT_LIMIT / 8;i++){ //GDT初始化，16位段寄存器，低3位不可用，共可表示8192个段号，即可定义8192个段号（0~8191）
		setSegmDesc(gdt + i,0,0,0); //初始化所有段，从0开始，每次加1知道8191，8字节结构体+1，每次相当于地址加8。上限0，基址0，权限0。
	}
	setSegmDesc(gdt + 1,FULL_ADR,ZERO_ADR,DATE32_RW_PRE); //设置段1，上限0xffffffff（4GB），基址0，权限0x4092
	setSegmDesc(gdt + 2,BOOT_LIMIT,BOOT_ADR,CODE32_ER_PRE); //设置段2，上限0x0007ffff（512KB）,基址,0x00280000,权限0x409a,存放bootpack
	load_gdtr(GDT_LIMIT,GDT_ADR); //段2涵盖了整个bootpack
	
	for(i = 0;i <= IDT_LIMIT / 8;i++){ //IDT初始化
		setGateDesc(idt + i,0,0,0);
	}
	load_idtr(IDT_LIMIT,IDT_ADR); //装载到地址到全局描述符寄存器，此函数在osfun.asm中，使用汇编语言完成
	setGateDesc(idt + 0x20,(int)asm_inthandler20,2 * 8,INTGATE32_PRE); //注册定时器中断处理函数	
	setGateDesc(idt + 0x21,(int)asm_inthandler21,2 * 8,INTGATE32_PRE); //将保护现场函数注册到IDT中，发生中断，CPU将自动调用asm_inthandler21
	setGateDesc(idt + 0x27,(int)asm_inthandler27,2 * 8,INTGATE32_PRE); //2*8为asm_inthandler*所属的段，段号为2,乘以8左移3位，低3位另有它有用，须为0
	setGateDesc(idt + 0x2c,(int)asm_inthandler2c,2 * 8,INTGATE32_PRE); //INTGATE32_PRE使中断处理有效
	//setGateDesc(idt + 0x40,(int)asm_sysprint,2*8,INTGATE32_PRE); //注册用于字符输出API的中断处理函数
	//setGateDesc(idt + 0x40,(int)asm_sys_api,2*8,INTGATE32_PRE); //注册用于处理系统API中断的函数
	setGateDesc(idt + 0x40,(int)asm_sys_api,2*8,INTGATE32_PRE + 0x60); //注册用于处理系统API中断的函数，权限偏移标识应用程序
	setGateDesc(idt + 0x0d,(int)asm_inthandler0d,2 * 8,INTGATE32_PRE); //应用程序一般异常中断处理函数
	setGateDesc(idt + 0x0c,(int)asm_inthandler0c,2 * 8,INTGATE32_PRE); //应用程序栈异常中断处理函数
}

/*全局描述符设置*/
void setSegmDesc(SEGMENT_DESCRIPTOR *segmentDescriptor,unsigned int limit,int base,int accessPre){  //limit,上限为段字节数-1;base,基址，accessPre,访问权限
	if(limit > 0xfffff){
		accessPre |= 0x8000; //G_bit = 1
		//accessPre |= BOOT_ADR; //G_bit = 1
		limit /= 0x1000;
	}
	segmentDescriptor->LOW_LIMIT = limit & 0xffff;
	segmentDescriptor->LOW_BASE = base & 0xffff;
	segmentDescriptor->MID_BASE = (base >> 16) & 0xff; //右移16位，舍弃右侧溢出位，左侧补0
	segmentDescriptor->ACCESS_PER = accessPre & 0xff;
	segmentDescriptor->HIGH_LIMIT = ((limit >> 16) & 0x0f) | ((accessPre >> 8) & 0xf0);
	segmentDescriptor->HIGH_BASE = (base >> 24) & 0xff;
}

/*中断描述符设置*/
void setGateDesc(GATE_DESCRIPTOR *gateDescriptor,int offset,int selector,int accessPre){
	gateDescriptor->SELECTOR = selector;
	gateDescriptor->LOW_OFFSET = offset & 0xffff;
	gateDescriptor->HIGH_OFFSET = (offset >> 16) & 0xffff;
	gateDescriptor->ACCESS_PER = accessPre & 0xff;
	gateDescriptor->DW_COUNT = (accessPre >> 8) & 0xff;
}