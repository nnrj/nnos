;nnos_ipl32（启动程序加载器）（FAT32）

;===================NNOS-06f======================
; 作者：年年软件
; 日期：2020年1月5日
;=================================================

; FAT32文件系统结构
; +-----+------+------+------+
; | DBR | FAT1 | FAT2 | DATA |
; +-----+------+------+------+
; 注：DBR部分，包括DBR和保留扇区
; 结构并名词解释：
;   DBR：Dos Boot Record，DOS引导记录，由硬盘MBR装载的程序段
;   FAT1：FAT,文件分配表（File Allocation Table）;FAT32一般设两份FAT,FAT1为第一份（主FAT)
;   FAT2：第二份FAT,FAT1的备份
;   DATA：数据区，包含目录区域，FAT32的主要区域
;------------------------------------------------------------------------------------------
;主引导扇区地图
; +-----------+------------------+------------------------+
; | MBR(512B) | 主引导程序(136B)  | 从活动分区装载引导程序  |
; |           +------------------+------------------------+
; |           | 出错信息数据区    | 出错信息               |
; |           | (310B)           +------------------------+
; |           |                  |全0填充                 |
; |           +------------------+------------------------+
; |           | DPT(64B)         | 分区1(16B)             |
; |           |                  +------------------------+
; |           |                  | 分区2(16B)             | 
; |           |                  +------------------------+
; |           |                  | 分区3(16B)             |  
; |           |                  +------------------------+
; |           |                  | 分区4(16B)             |  
; |           +------------------+------------------------+
; |           | 结束标志         |   55AA                 | 
; +-----------+------------------+------------------------+
;

CYLS_MAX EQU 30  ;定义最大装载柱面号常数CYLS_MAX,最大装载到第10柱面
ORG 0x7c00	     ;指明程序装载地址。#主引导扇区拷贝到内存0x7c0。0x00007c000~0x00007dff:启动分区装载地址(硬规定，IBM)


; FAT32格式描述
JMP	entry       ; 跳转到程序核心入口
DB  0x90	    ; 跳转指令（相当于"JMP 0x4e”，亦可用短跳<如jump short start>后加一个nop空指令补足3字节）
; JMP SHORT entry NOP
DB  "NNOS_IPL"  ; OEM代号，8字节，可自定义（可自定义，须为8字节，不满8字节用空格填充）
DW  0x0200      ; 每扇区字节数（Bytes Per Sector），指明硬件扇区大小。合法取值（十进制）：512、1024、2048、4096。一般默认置为512
DB  8           ; 每簇扇区数（Secotr Per Cluster）。FAT32只能跟踪有限个簇（最多2^32簇），故可改变簇大小以扩大分区。但一般置为0x08。缺省则以分区大小为限。合法值（十进制）：4、8、16、32、64、128
DW  2           ; 保留扇区数（Reserved Sector），第一个FAT开始之前的扇区数（包含引导扇区）。值（十进制）通常置为32
DB  2           ; FAT数（Number of FAT)，该分区上FAT的副本数。值（十进制）通常为2
DW  0           ; 根目录项数（Root Entries），只有FAT12/FAT16使用此字段，FAT32此字段须置0
DW  0           ; 小扇区数（Small Sector），只有FAT12/FAT16使用此字段，FAT32此字段须置0
DB  0xf8        ; 媒体描述符（Media Descriptor）。0xF8：硬盘；0xF0：高密度3.5寸软盘。本字段用于MS-DOS FAT16磁盘，Windwos 2000中未使用
DW  0           ; 每FAT扇区数（Sectors Per FAT）（Number of FATs16），仅供FAT12/FAT16使用。FAT32须置0
DW  0x003f      ; 每道扇区数（Sector Per Track），使用"INT 13h"的磁盘的“每道扇区数”几何结构值（？）
DW  0x00ff      ; 磁头数（Number of Head），使用"INT 13h"的磁盘的“磁头数”几何结构值（？）。3.5英寸1.44MB软盘上，本字段值为2
DD  0           ; 隐藏扇区数（Hidden Sector），该分区上引导扇区之前的扇区数，计算到根目录数据区绝对位移时用到本字段。在无分区的媒体上必须为0 [DD 0x3f]
DD  0xee8c00    ; 总扇区数（Large Sector）

; FAT32分区描述
DD  0x3b84      ; 每FAT扇区数（Sector Per FAT）（Number of FATs32）,仅供FAT32使用，FAT12/FAT16必须置0。本字段与隐藏扇区数共同决定根目录从何处开始，并从目录项数决定该分区用户数据区从何处开始
DW  0           ; 扩展标志（Extended Flag），仅供FAT32使用，共2字节（0~3:活动FAT数，从0开始计数；4~6:保留；7：值为0表示运行时FAT映射所有，值为1表示只有一个活动FAT；8~15：保留）
DW  0           ; 文件系统版本（File System Version），仅供FAT32使用，高字节为主要修订号，低字节为次要修订号。本字段用作将来扩展该FAT32媒体。若非0，则旧版Windows不支持该分区
DD  2           ; 根目录簇号（Root Cluster Number），仅供FAT32使用，为根目录第一簇的簇号。值通常为2（而非总是）
DW  1           ; 文件系统信息扇区号（File System Infomation SectorNumber），仅供FAT32使用。值通常为1。FAT32分区的保留区种文件系统信息（File System Infomation，FSINFO）结构的扇区号。备份引导扇区（Backup Boot Sector）中保留了该FSINFO结构的副本，但该副本不保持更新
DW  6           ; 备份引导扇区，仅供FAT32使用，非零值。表示该分区保存引导扇区副本的保留区中的扇区号。本字段值通常为6，不建议使用其他值
RESB 12         ; FAT32保留区（FAT32 Reserverd），仅供FAT32使用，供以后扩充使用。12个字节，值全部为0
; FAT32分区扩展BPB字段
DB  0x80        ; 物理驱动器号（Physical Drive Number）,与BIOS物理驱动器号有关，与物理驱动器无关。软盘驱动器标识为0x00，物理硬盘标识为0x80。发出"INT 13h”中断时，BIOS调用前设置该值，指定所访问的设备。仅当该设备为引导设备时，该值才有意义
DB  0           ; FAT32保留区（FAT32 Reserverd），仅供FAT32使用，供以后扩充使用，值全部为0
DB  0x29        ; 扩展引导标签（Extended Boot Signature），本字段须为Windows可识别的值，为0x28或0x29
DD  0X08F85C97  ; 分区序号（Volume Serial Number），格式化磁盘时产生的随机序号，有助于区分磁盘(卷标、卷名称，必须为11字节，不足则以空格填充)
DB  "NNOS_OS    "   ; 卷标（Volume Label），用于保存卷标号。卷标作为特殊文件保存在根目录中
DB  "FAT12   "  ; 系统ID（System ID），FAT32文件系统中一般取“FAT32”。文件系统类型(必须为8字节，不足则以空格填充，操作系统并不使用此字段识别磁盘之文件系统)

usbid:
    DB 0
maxhead:
    DB 0
maxsector:
    DB 0
maxcylinder:
    DW 0

getusb:
    MOV SI, usbid
    MOV [SI], DL
    MOV AH, 8
    INT 0x13
    MOV SI, maxhead
    MOV [SI], DH
    MOV DL, CL
    AND CL, 0x3f
    MOV [SI], CL
    AND DL, 0xc0
    MOV SI, maxcylinder
    MOV [SI], CH
    ROR DL, 6
    MOV [SI+1], DL

;covert:
;    MOV AL, [maxsector]
;   mov BL, [maxhead]
;    MUL BL
;    MOV [headmulsector], AX
;    MOV AX, 2               ; 将扇区数（以0开始）转化为柱面，磁头，扇区（以1开始）
;    MOV DX, 0
;    MOV CX, [headmulsector]
;    DIV CX
;    MOV [startcylinder], AX
;    MOV AX, DX
;    MOV BL, [maxsector]
;    DIV BL
;    MOV [starthead], AL
;    ADD AH, 1
;    MOV [startsector], AH
;    MOV AX, 0x0C20           ; 加载至C200处
;    MOV ES, AX
;    MOV CH, [startcylinder]  ; 柱面x
;    MOV DH, [starthead]      ; 磁头y
;    MOV CL, [startsector]    ; 扇区z

;程序核心
	entry:				; 程序核心入口程序段，由开头JMP跳入，相当于JMP 0x7c50b 
        MOV AX,0		; 初始化寄存器。#AX为通用寄存器(通常用作累加器)，SS为堆栈段寄存器,DS为数据段寄存器，ES为附加段寄存器
		MOV SS,AX		; AX、SS、DS、ES全部初始化为0
		MOV	SP,0x7c00 	; 栈顶指针指向0x7c00
		MOV	DS,AX       ; 省略段寄存器会把DS作为默认段寄存器,MOV CX;[1234]相当于MOV CX,[DS:1234],即MOV CX,[DS*16+1234]。故DS须初始化为0。
		;MOV	ES,AX
		
;读磁盘
		MOV AX,0x0820   ; 地址0x820写入给AX,再赋值给ES。无法直接赋值给ES寄存器(设计原因，硬规定)，0x7c00~0x7dff为启动区，0x7e00~0x9fbff供操作系统任意使用
		MOV ES,AX       ; 寄存器AX赋值给寄存器ES，偏移量为0x0820;BX为0。从内存0x0820处开始装载读取的硬盘数据，磁盘第2扇区装载在内存0x0820处
		MOV CH,0        ; 磁盘第0个柱面(自外向内递增，软盘共80个柱面,cylinder)，柱面编号从0开始
		MOV DH,0        ; 磁盘第0个磁头(磁头0正面、磁头1反面)，磁头编号从0开始
		MOV CL,2        ; 第2个扇区(软盘等分为18个扇区,sector)，扇区编号从1开始
readloop:               ; 循环读取下一个扇区的开始处，读盘无错、尚未读到第18扇区时读完一个扇区会回到此处继续读下一个扇区	
		MOV	SI,0		; 读盘失败次数的SI初始为0，开始读新的扇区
retry:                  ; retry程序段，若读盘失败，则重试直至满5次。
		MOV AH,0X02     ; 读盘，AH=0x02为读盘
		MOV AL,1        ; 读取1个扇区，AL用于指定扇区号
		MOV BX,0        ; 基址寄存器置0，ES:BX 为缓冲地址，校验及寻道时不使用
		;MOV DL,0x00     ; 驱动器A，DL用于指定驱动器号。此句和下句之间若改为AL=17即调用磁盘BIOS的函数来连续读取18个扇区，因软盘无法跨越多个磁道，故不适用。
        MOV DL,0x80     ; 驱动器A，DL用于指定驱动器号。调用硬盘中断，读硬盘
		INT 0x13        ; 中断，调用磁盘BIOS(硬规定)，至此若无错则读盘成功。此语句执行结果影响CF标志位，无错置1，有错置0。
        ;JNC oky        ; 没出错则跳转到okey（亦可直接跳转到fin不显示信息)，JNC判断进位标志CF，为0时跳转
        JNC next        ; 无错则跳至next程序段，从而继续读盘到第18扇区
        ADD SI,1        ; 若读盘失败，则上一句不会跳转，继续执行到此处，将出错记录数加1
        CMP SI,5        ; 比较SI与5，比较结果影响零标志寄存器的值;#ZF=1(相等)，ZF=0(不相等），SF=1(左<右)，[ZF,SF]=[1,1](左<=右)，SF=0(左>=右)，[ZF,SF]=[0,0](左>右)
        JAE error       ; SI中的重试次数比较结果与5相等时，跳转到error。JAE(jump if above or equal)指令在SI>=5时跳转。
		;JC  error      ; 出错时跳转到error段，JC(jump if carry)，进位标志(CF,carry flag)为1时跳转
        ;JNC oky        ; 不出错继续跳转到oky段运行,oky段在显示器上显示信息,oky已更名为showmsg
        MOV AH,0x00     ; 读盘出错但出错次数小于无时，JNC、JAE处都未跳转，继续执行此指令，将寄存器AH重置为0，复位
        MOV DL,0x00     ; 驱动器A
        INT 0x13        ; 中断，系统复位，重置驱动器，复位软盘状态
        JMP retry       ; 跳转到retry从头重新尝试读盘 
next:                   ; next程序段，若重试次数小于5、读盘无错，则跳转到此处，继续读盘到第18扇区
        MOV AX,ES       ; 当前段地址(实际地址位ES*16+BX)赋值给累加器AX
        ADD AX,0x0020   ; 将当前地址后移0x0020（00100000即32,32*16+0=512，为一个扇区大小）存入累加器AX，即AX=AX+0x0020。这两句可直接写为ADD BX,512
        MOV ES,AX       ; 将AX中的新地址赋值给ES，注意不能直接ADD ES,0x020，段寄存器ES不能用作累加器
        ADD CL,1        ; 进位标志CL增加1，记录当前读到的扇区，作为是否读到第18扇区的判断参数
        ;CMP CL,18      ; 将进位标志CL的值与18比较比较结果影响ZF、SF标志寄存器的值，根据ZF的值可判断是否读到第18扇区
        CMP CL,0x003f   ; 将进位标志CL的值与18比较比较结果影响ZF、SF标志寄存器的值，根据ZF的值可判断是否读到第18扇区
        JBE readloop    ; 若CL<=18,即[ZF,SF]=[1,1](左<=右)，则尚未读到第18扇区，跳转至readloop从而循环读取下一个扇区，JBE(jump if bleow or equl)，小于等于则跳转
        MOV CL,1        ; 进位标志重置为1，用于记录此柱面所读扇区数(每个磁道有18个扇区)
        ADD DH,1        ; 磁头号加1，软盘只有两个磁头，C0-H0-S18的下一个扇区为C0-H1-S1（每个磁道最大18个扇区，扇区编号从1开始）
        CMP DH,2        ; 将磁头号与2比较，比较结果影响ZF和SF
        JB  readloop    ; 若DH<2，即磁头号小于2，亦即SF=1时，跳转到readloop，继续读此柱面的反面的第一个扇区即磁头1的第一个扇区（C0-H1-S1）
        MOV DH,0        ; 读完C0柱面的反面后，继续读正面，磁头号0
        ADD CH,1        ; 柱面号加1，读完第0号柱面，继续读第1号柱面，以此类推
        CMP CH,CYLS_MAX ; 将当前柱面号与最大装载柱面号常数CYLS_MAX比较，比较结果影响ZF和SF
        JB  readloop    ; 柱面号小于最大装载柱面号常数时跳转到readloop继续读下一个柱面/扇区
        JMP showmsg     ; 读盘完毕显示系统信息
        ; 改进思路：合并循环，读盘扇区交替
;        JMP start
start:                  ; start程序段，启动NNOS操作系统  
        MOV [0x0ff0],CH ; 将磁盘装载内容结束地址存入内存0x0ff0处，以供操作系统使用
        JMP 0xc200      ; 跳转难道系统装载起始位置（软盘文件名保存于0x2600以后，文件内容在0x4200以后，启动区位于内存0x8000，则系统装载到内存0x8000+0x4200=0xc4200）
;        JMP showmsg
        
;fin                    ; 待机
    ;#HLT                 ;cpu停止动作，进入待机状态，省电
    ;#JMP fin             ;无限循环    

;error:
    ;#MOV SI,msg 		; msg程序段首地址写入SI,相当于JMP 0x7c74
	
    fin:				; 结束程序段
		HLT				; CPU停止动作(进入待机状态)，等待指令(只要外部发生变化，如键盘、鼠标事件，CUP就会苏醒)，不加也可以，加了省电
		JMP fin			; 无限循环，从而使得显示器上稳定显示文字
        
    error:              ; 错误程序段，读盘出错时跳转到此处
        MOV SI,errormsg ; 将errormsg的地址写入寄存器SI
    
    showmsg:            ; 正常程序段，读盘正常时跳转到此处
        MOV SI,msg      ; 将msg的地址写入寄存器SI
        JMP start
	
	putloop:            ; 循环打印程序段，在屏幕上稳定输出信息
		MOV AL,[SI]		; 寄存器间接寻址，将SI中所存内存地址之内容写入AL。#AL为累加器，SI为通用寄存器，此处用于间址
		ADD SI,1		; 给SI加1,间接寻址寄存器+1，指向下一个地址
		CMP	AL,0		; 用AL-0，得出比较结果，设置标志位ZF，控制循环,结果为0，ZF置1。#AL为通用寄存器AX的低8位，AH则位高8位，拆为两个寄存器使用		
		JE fin			; 判断ZF，若ZF=1(AL与0的比较结果相等)则跳转到fin结束程序段，否则不跳转继续执行下一条指令。相当于：if(AL==0){goto fin;}
		MOV AH,0x0e		; 指定一个要显示的文字,文字的ASCII码写入AH
		MOV BX,15		; 指定字符颜色,00001111，不闪烁、黑底、高亮、白色，写入BX，BX为基址寄存器
		INT 0x10		; 中断，调用显卡BIOS,INT指令引发中断，中断类型码为0x10，代表由显卡BIOS对显示器所提供的服务程序
		JMP putloop		; 跳转回开头，循环显示出所有字符
        

    errormsg:                                   ; 错误信息程序段
        DB 0x0a,0x0a                            ; 2个换行
        DB "NNOS-IPL:Loaderror.[0X060101001]"   ; 错误信息
        DB 0x0a                                 ; 换行
        DB 0     

	msg:						                ; 信息程序段(要在显示器上显示的信息)
		DB 0x0a,0x0a							; 2个换行,0x0A为LF的十六进制Ascii码
		DB "============NNOS============"
		DB 0x0a
		DB "|  WELCOME TO NNOS WORLD   |"
		DB 0x0a
		DB "|==========================|"	; 欢迎界面信息
		DB 0x0a								; 换行
		DB "|File System:FAT32         |"	; 版本信息、平台信息
		DB 0x0a					
		DB "|Version:0.6i | Based:80x86|"	; 文件系统信息
		DB 0x0a					
		DB "|Company:nnrj | Author:LDX |"	; 公司信息、开发者信息
		DB 0x0a
		DB "|Email:tianhehechu@qq.com  |" 	; 开发者邮箱
		DB 0x0a
		DB "|Update Date:2019-3-6      |" 	; 发行日期	
		DB 0x0a
		DB "=====CREATED IN CHINA======="	; 中国创造			
		DB 0x0a
		DB 0
		
        ;#RESB 0x1fe-$				;填写0x00，直到0x001fe,nask特有语法
        TIMES 0x1fe-($-$$)  DB 0   	;#[兼容nasm][代]。填写0x00[nasm版]，填写到510字节，末2位留给结束标志
        DB 0x55,0xaa				; 结束标志