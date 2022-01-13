; 系统配置文件
; -------------------------------------loder和kernel的配置文件[开始]-------------------------------------
LOADER_BASE_ADDR equ 0x900 ; 定义loader在内存中的位置。MBR把loader从硬盘读入后放到此处。loader加载到实模式下内存可用区域的0x900处。
LOADER_START_SECTOR equ 0x2 ; 定义loader在硬盘上的逻辑扇区地址，即LBA地址。扇区号为LBA模式下的，从0开始计数，下文同。此处将loader放在了第2块扇区（MBR放在第0扇区，保险起见，间隔一个扇区存loader）
; -------------------------------------loder和kernel的配置文件[结束]-------------------------------------

; -------------------------------------gdt描述符属性[开始]-----------------------------------------------
DESC_G_4K           equ     1_00000000000000000000000b
DESC_D_32           equ     1_0000000000000000000000b
DESC_L              equ     0_000000000000000000000b    ; 64位代码标记，此处标记为0便可
DESC_AVL            equ     0_00000000000000000000b    ; CPU不用此位，暂置为0
DESC_LIMIT_CODE2    equ     1111_0000000000000000b
DESC_LIMIT_DATA2    equ     DESC_LIMIT_CODE2
DESC_LIMIT_VIDEO2   equ     0000_000000000000000b
DESC_P              equ     1_000000000000000b
DESC_DPL_0          equ     00_0000000000000b
DESC_DPL_1          equ     01_0000000000000b
DESC_DPL_2          equ     10_0000000000000b
DESC_DPL_3          equ     11_0000000000000b
DESC_S_CODE         equ     1_000000000000b
DESC_S_DATA        equ     DESC_S_CODE
DESC_S_SYS          equ     0_0000000000000b
DESC_TYPE_CODE      equ     1000_00000000b              ; x=1，c=0，r=0，a=0。代码段可执行、非依从、不可读、已访问位a清零
DESC_TYPE_DATA      equ     0010_00000000b              ; x=0, c=0, w=1, a=0。数据段不可执行、向上扩展、可写、已访问位a清零

DESC_CODE_HIGH4     equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00
DESC_DATA_HIGH4     equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
DESC_VIDEO_HIGH4    equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x0b
; -------------------------------------gdt描述符属性[结束]-----------------------------------------------

; -------------------------------------gdt选择子[开始]--------------------------------------------------
RPL0                equ     00b
RPL1                equ     01b
RPL2                equ     10b
RPL3                equ     11b
TI_GDT              equ     000b
TI_LDT              equ     100b
; -------------------------------------gdt选择子[结束]--------------------------------------------------