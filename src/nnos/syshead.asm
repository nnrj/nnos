;nnos_syshead（系统实模式）
;===================NNOS-06i======================
;#[功能更新]由汇编语言进入C语言开发阶段
;           由实模式切换到32位保护模式
;           修改Makefile兼容Linux
;           新增getnas程序，自动将nasm程序转换为
;           nask程序，兼容nask和nasm
;#[运行结果]NNOS切换到了32位保护模式运行   
;=================================================
;[INSTRSET "i486p"] 			;使用80486的指令集以支持386以后的LGDT、EAX、CR0等关键字  

    BOTPAK      EQU 0x00280000  ;bootpack加载位置信息保存位置
    DSKCAC      EQU 0x00100000  ;磁盘缓存首地址信息保存位置
    DSKCAC0     EQU 0x00008000  ;实模式下磁盘高速缓存首地址信息保存位置
	VBEMODE 	EQU 0x105		;VBE模式

;BOOT_INFO                   ;系统参数
    CYLS_MAX    EQU 0x0ff0   ;启动区，为最大最大装载柱面号的地址信息保存位置
    LEDS        EQU 0x0ff1   ;键盘LED指示灯状态信息保存位置
    VMODE       EQU 0x0ff2   ;显卡模式颜色位数信息保存位置，不同模式像素数不同，使用的显存区域也不同。
    SCRX        EQU 0x0ff4   ;行分辨率信息保存位置
    SCRY        EQU 0x0ff6   ;列分辨率信息保存位置
    VRAM        EQU 0x0ff8   ;图像缓冲区（显存）首地址信息保存位置，显存的各个地址对应画面上的像素，可以绘制图案。
    
    ORG 0xc200  ;指明操作系统装载位置（软盘文件名保存于0x2600以后，文件内容在0x4200以后，启动区位于内存0x8000，则系统装载到内存0x8000+0x4200=0xc4200）

;确认是否支持VBE
	MOV AX,0x9000 
	MOV ES,AX ;ES赋值为0x9000
	MOV DI,0 ;DI赋值为0x004f，显卡的VBE信息须写入内存中ES:DI开始的512字节中，赋值指定写入地址
	MOV AX,0x4f00 ;如果显卡不支持VBE,AX的值会自动变回其他值
	INT 0x10 ;BIOS中断
	CMP AX,0x004f ;判断是否支持VBE，即AX是否成功赋值0x4f00
	JNE scr320 ;如果不存在，则跳转到320×200×8的低分辨率模式运行
;检查VBE版本，须为2.0以上
	MOV AX,[ES:DI+4]
	CMP AX,0x0200
	JB scr320 ;若VBE版本低于2.0，跳转到320×200×8的低分辨率模式运行
;获取VBE画面模式信息
	MOV CX,VBEMODE
	MOV AX,0x4f01 
	INT 0x10 
	CMP AX,0x004f
	JNE scr320	;检查AX的值，若非0x004f则指定画面模式不可用，跳转到320×200×8的低分辨率模式运行
	;画面模式信息被写入内存从ES:DI开始的256字节，覆盖掉之前的VBE版本信息
	;WORD [ES:DI+0x00] ;模式属性，bit7须为1（能加0x4000），待确认
	;WORD [ES:DI+0x12] ;屏幕横向分辨率
	;WORD [ES:DI+0x14] ;屏幕纵向分辨率
	;WORD [ES:DI+0x19] ;屏幕颜色位数，须为8bit,待确认
	;WORD [ES:DI+0x1b] ;颜色指定方法，须为4（调色板模式），待确认
	;WORD [ES:DI+0x28] ;VRAM首地址
;画面模式信息确认
	CMP BYTE [ES:DI+0x19],8 ;判断颜色位数是否为8
	JNE scr320				;颜色位数非8则跳转到320×200×8的低分辨率模式运行
	CMP BYTE [ES:DI+0x1b],4 ;判断颜色指定方法是否为调色板模式
	JNE scr320				;非调色板模式则跳转到320×200×8的低分辨率模式运行
	MOV AX,[ES:DI+0x00]		
	AND AX,0x0080			;判断模式属性是否可加0x4000，即判断第8个比特位(bit7)是否为1，通过与运算确定
	JZ	scr320				;bit7为0则跳转到320×200×8的低分辨率模式运行
;画面模式切换
	MOV BX,VBEMODE+0x4000
	MOV AX,0x4f02
	INT 0x10 
	MOV BYTE [VMODE],8 ;将画面模式信息写入地址VMODE
	MOV AX,[ES:DI+0x12] ;将屏幕横向分辨率信息写入AX寄存器
	MOV [SCRX],AX 		;将AX中的屏幕横向分辨率信息写入地址SCRX
	MOV AX,[ES:DI+0x14] ;将屏幕纵向分辨率信息写入AX寄存器
	MOV [SCRY],AX 		;将AX中的屏幕纵向分辨率信息写入地址SCRY
	MOV EAX,[ES:DI+0x28];将显卡首地址信息写入寄存器EAX
	MOV [VRAM],EAX 		;将EAX中的显卡首地址信息写入地址VRAM
	JMP keystart		;切换完毕跳转到keystart继续运行
	
scr320:			;低分辨率模式
    MOV AL,0x13 ;调用VGA显卡（320x200x8位彩色,8位可使用256种颜色。此模式显存地址为0xa0000~0xaffff，共64KB）
    MOV AH,0x00 ;设置显卡模式
    INT 0x10    ;BIOS中断
    MOV BYTE    [VMODE],8           ;画面模式写入VMODE
    MOV WORD    [SCRX],320          ;行分辨率写入SCRX
    MOV WORD    [SCRY],200          ;列分辨率写入SCRY
    MOV DWORD   [VRAM],0x000a0000   ;显存起始位置写入VRAM
	
	; MOV BX,0x4101 ;调用VBE显卡（640x480x8位彩色,8位可使用256种颜色。此模式显存地址为0xa0000~0xaffff，共64KB）
    ; MOV AX,0x4f02 ;设置显卡模式
    ; INT 0x10    ;调用显卡BIOS
    ; MOV BYTE    [VMODE],8           ;画面模式写入VMODE
    ; MOV WORD    [SCRX],640          ;行分辨率写入SCRX
    ; MOV WORD    [SCRY],480           ;列分辨率写入SCRY
    ; MOV DWORD   [VRAM],0xe0000000   ;显存起始位置写入VRAM
	
	; MOV BX,0x4105 ;调用VBE显卡（640x480x8位彩色,8位可使用256种颜色。此模式显存地址为0xa0000~0xaffff，共64KB）
    ; MOV AX,0x4f02 ;设置显卡模式
    ; INT 0x10    ;调用显卡BIOS
    ; MOV BYTE    [VMODE],8           ;画面模式写入VMODE
    ; MOV WORD    [SCRX],1024          ;行分辨率写入SCRX
    ; MOV WORD    [SCRY],768           ;列分辨率写入SCRY
    ; MOV DWORD   [VRAM],0xe0000000   ;显存起始位置写入VRAM

keystart:
;调用键盘BIOS获取键盘LED指示灯状态
    MOV AH,0x02
    INT 0x16                        ;中断，调用键盘BIOS
    MOV [LEDS],AL                   ;将键盘LED指示灯状态信息写入LEDS  
    
;防止PCI被中断						;CPU进行模式转换时，不允许中断。
    MOV AL,0xff                     ;0xff（全1）写入AL，默认使用AL，大于0xff则使用DX。输出字节默认使用AL，输出字默认使用AX
    OUT 0x21,AL                     ;IO指令，把AL的数据写入IO端口21H。禁止主PIC的全部中断。
    NOP                             ;空指令(无操作数指令)补足字节，执行后PC+1，占用一个机器周期，从而保持I/O间隙的稳定时间。当OUT命令连续时，此操作在某些模式不起作用。
    OUT 0xa1,AL                     ;将AL的数据写入端口A1H。禁止从PIC的全部中断。
    CLI                             ;禁止CPU级别中断。CLI指令，禁止中断;STL指令，允许中断。两个指令只能在内核模式下执行，不可在用户模式下执行。
    
;设置A20地址线（A20 GATE）。（打开A20地址线，从而允许访存1MB以上,支持全部32根地址线，最大访存4GB）
    CALL waitkbdout                 ;等待控制电路准备完毕。相当于bootpack.c中的waitKBCReady()，结构相同，指令不同
    MOV  AL,0xd1                    ;向设备号为0x64的端口写入0xd1(11010001,00000010)
    OUT  0x64,AL                    ;将AL的数据写入IO端口64H
    CALL waitkbdout                 ;等待输出完毕
    MOV  AL,0xdf                    ;将代表启用A20地址线的操作数0xdf(键盘控制电路负数端口，不同指令不同功能)写入AL。0xdf让A20GATE信号线变为ON状态
    OUT  0x60,AL                    ;启用A20地址线,将AL中的操作数写入IO端口60H。
    CALL waitkbdout                 ;等待输出完毕（冗余指令，确保A20GATE处理完成）
    
;过渡到保护模式（准备工作）            
    LGDT [GDTR0]                    ;设置临时GDT
    MOV EAX,CR0                     ;将32位寄存器CRO的数据写入EAX
    AND EAX,0x7fffffff              ;将第31位设置为0。AND,按位与，当且仅当两操作数对应位都为1时结果为1否则为0。设置bit31，(第31位，从0开始，即最高位)为0，禁止分页
    OR  EAX,0x00000001              ;将第0位（最低位）设置为1，用于保护模式转换，与EAX按位或，有1即1。设置bit0为1，切换到保护模式
    MOV CR0,EAX                     ;将更新后的EAX的数据写回CRO，完成保护模式转换，进入不分页的保护模式。CR0(Contrl Register 0)只有操作系统才能操作。
    JMP pipelineflush               ;跳转到pipelineflush程序段，刷新管道
;保护模式中，段寄存器之解释不是16被，而是能够使用GDT。此模式下应用程序不能随便改变段设定，不能使用操作系统专用段。操作系统受到CPU的保护。
;保护模式分为带保护的16位模式和带保护的32位模式。本程序切换到了带保护的32位模式。
;切换后要立刻执行JMP，因为机器语言解释发生变化，而CPU管道(pipeline))机制在上一条指令未执行完时边解释下一条，模式改变后须重新解释一遍。
;保护模式中，除CS以外所有段寄存器的值都从0x0000变为了0x0008，相当于gdt+1的段
    
pipelineflush:                      ;流水线清空程序段
    MOV AX,1*8                      ;可读可写段32位
    MOV DS,AX                       ;各寄存器初始化
    MOV ES,AX
    MOV FS,AX
    MOV GS,AX
    MOV SS,AX
 
;转移bootpack
    MOV     ESI,bootpack            ;将bootpack的首地址写入寄存器ESI（首地址）
    MOV     EDI,BOTPAK              ;将bootpack加载位置写入寄存器EDI（目的地址）
    MOV     ECX,512*1024/4          ;转换为逻辑地址。把从bootpack开始的512KB内容复制到0x00280000号地址（分配比实际使用多）
    CALL    memcpy                  ;调用memcpy程序段，转移bootpack。
    
;将磁盘数据传输到原始位置
;引导扇区的数据（第一部分）
    MOV     ESI,0x7c00              ;引导扇区首地址为0x7c00，写入ESI（首地址）
    MOV     EDI,DSKCAC              ;磁盘缓存首地址为DSKCAC，写入EDI（目的地址）
    MOV     ECX,512/4               ;引导扇区共512字节，转换为逻辑地址。从0x7c00复制512字节到0x00100000（自此往后地址未占用）。即启动扇区被复制到1MB以后的内存中。
    CALL    memcpy                  ;调用memcpy程序段，转移磁盘数据的引导扇区部分
    
;引导扇区之外的剩余数据（第二部分）
    MOV     ESI,DSKCAC0 + 512      ;剩余数据起始地址，实模式下磁盘高速缓冲区首地址偏移512字节
    MOV     EDI,DSKCAC + 512        ;剩余数据目的地址，磁盘缓存首地址，偏移512字节。
    MOV     ECX,0                   ;ECX寄存器置0
    MOV     CL,BYTE [CYLS_MAX]      ;柱面最大编号存入CL(ECX的低地址)
    IMUL    ECX,512*18*2/4          ;将CHS地址转换为LBA（Logical Block Address）逻辑地址，写入ECX寄存器，IMUL为有符号乘(整数乘法)。从柱面数变为字节数/4。
    ;IMUL    ECX,512*63*1/4
    SUB     ECX,512/4               ;ECX=ECX-512/4,ECX偏移512/4，形成20位段地址（减去IPL<启动区>长度，512字节）
    CALL    memcpy                  ;调用mempy程序段（复制内存），转移引导扇区之外的剩余数据。将从0x00008200开始的磁盘内容复制到0x00100200
;memcpy(源地址，目的地址，数据大小);
;memcpy(bootpack,BOTPAK,512*1024/4);memcpy(0x7c00,DSKCAC,512/4);memcpy(DSKCAC0+512,DSKCAC+512,cyls*512*18*2/4-512/4);
;转送数据大小以双字为单位，故数据大小用字节数除以4指定

;启动bootpack						;解析bootpack.bin的头部内容，传送执行所需必要数据
    MOV     EBX,BOTPAK              ;bootpack加载位置写入EBX基址寄存器
    MOV     ECX,[EBX + 16]          ;(0x11a8)加载位置偏移16位形成20位段地址，所指向的地址内容写入ECX。亦可"shl eax,4"逻辑左移4位。80x86CPU中，段首地址低4位须为0，左移4位使之为0然后与16位偏移地址相加
    ADD     ECX,3                   ;ECX中的20位段地址与偏移量3相加存入ECX
    SHR     ECX,2                   ;ECX逻辑右移2位，相当于ECX/4，得到数据块数量，4字节为一个数据块，数据块数存入ECX，
    JZ      skip                    ;ECX为0则跳转到skip程序段执行，否则继续往下执行
    MOV     ESI,[EBX + 20]          ;(0x10c8)基地址偏移20，所指向的内容写入ESI，作为起始地址
    ADD     ESI,EBX                 ;将EBX中的基址与ESI相加写入ESI,作为起始地址
    MOV     EDI,[EBX + 12]          ;(0x00310000);EBX偏移12所指内容写入EDI作为目的地址
    CALL    memcpy                  ;调用memcpy程序段，调入内存。将0x10c8字节开始的0x11a8字节复制到0x00310000号地址
skip:                               ;跳过启动bootpack之程序段
    MOV     ESP,[EBX + 12]          ;(0x00310000)若ECX逻辑右移2位后零标志位为0，则没有数据可以装载，栈顶指针寄存器偏移12。前12位（4字节）为可执行文件标识。
    JMP     DWORD 2*8:0x0000001b    ;跳转到已被装入内存的引导扇区起始位置（将2*8带入CS,移动到0x1b。即第二个段的0x1b。第2个段基地址为0x28001b，实际从0x28001b开始执行，即bootpack.bin的0x1b号地址）
    
waitkbdout:                         ;等待输出完毕程序段
    IN  AL,0x64                     ;从64H端口读取一字节数据写入AL
    AND AL,0x02                     ;将AL中的数据与0x02按位与，结果存入零标志寄存器，用于判断是否输出完毕，完毕时零标志位为0
	IN 	AL,0x60 					;空读（清空数据接收缓冲区中的垃圾数据）
	JNZ waitkbdout                  ;零标志位非0，则跳转到waitkbout继续循环等待，否则继续执行下一条指令
    RET                             ;零标志位为0，继续执行到此指令，结束循环，返回到调用者

memcpy:                             ;内存写入之程序段，亦可使用string命令写入（复制内存）
    MOV EAX,[ESI]                   ;ESI所指内容写入EAX
    ADD ESI,4                       ;ESI加4
    MOV [EDI],EAX                   ;EAX内容写入EDI中目的地址所指位置
    ADD EDI,4                       ;目的地址偏移4
    SUB ECX,1                       ;ECX中记录的数据库总数减1，相减结果影响零标志位
    JNZ memcpy                      ;零标志位非0，继续循环执行mecpy，转移剩余的数据,否则继续执行下一条指令（减法运算结构非0则跳转到内存复制继续复制）
    RET                             ;零标志位为0，继续执行到此指令，结束循环，返回调用者
    ;ALIGNB  16                     ;指令按16位对齐，防止RET出错（一直添加DB 0，直到时机合适<地址能被16整除时>）。否则向段寄存器复制的MOV指令会变慢。
	ALIGN	16						;#[兼容nasm][代]   

GDT0:                                   ;临时GDT。0号空区域(null sector)，不能在此处定义段。
    ;RESB   8                           ;从当前位置向后空出8个字节，RESB(Reserve byte)
    TIMES   8  DB 0                    ;#[兼容nasm][代]
    DW      0xffff,0x0000,0x9200,0x00cf ;可读可写段，32位
    DW      0xffff,0x0000,0x9a28,0x0047 ;可执行段，32位（用于bootpack）
    DW      0
    
GDTR0:									;通知GDT0已有GDT在GDT0中（16位段上限，32位段起始地址）
    DW      8 * 3 - 1                   ;(?)
    DD      GDT0                        ;(?)
    ;ALIGNB  16							;指令按16位对齐（一直添加DBO，直到时机合适）
	ALIGN	16                          ;#[兼容nasm][代]
;最初GDT在syshead中，并不在0x00270000~0x0027ffff范围之内，而IDT尚未设定，故处于中断禁止状态。
;bootpack.c中，应在初始化调色板前抓紧创建GDT、IDT并初始化它们和PIC,然后执行io_sti()允许中断。

bootpack:                               ;bootpack暂为空。nnos.sys通过syshead.bin和bootpak.bin连接生成，在syshead结束出紧接着bootpack.bin
    
;fin:        ;fin程序段
;    HLT     ;CPU停机等待指令
;    JMP fin ;无限循环

;系统内存分布图
;0x00000000 ~ 0x000fffff ：启动时多次调用，启动后变为空，共1MB（里面还有BIOS和VRAM等内容）
;0x00100000 ~ 0x00267fff : 保存软盘内容，共1440KB
;0x00268000 ~ 0x0026f7ff : 空，共32KB
;0x0026f800 ~ 0x0026ffff : IDT，2KB
;0x00270000 ~ 0x0027ffff : GDT，64KB
;0x00280000 ~ 0x002fffff : bootpack.bin，512KB
;0x00300000 ~ 0x003fffff : 栈及其他，1MB
;0x00400000以后，空

