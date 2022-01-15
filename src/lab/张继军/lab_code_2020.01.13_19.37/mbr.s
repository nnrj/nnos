#=====================================================================================
#
# MBR ( Master Boot Record )
#
# Author : zhangjijun
# History:
# 0.01 - 2005-08-13 19:34 采用传统的CHS磁盘调用
# 0.02 - 2005-08-23 19:44 优先采用扩展磁盘调用
#
# Build :   as --32 mbr.s -o mbr.o
#           objcopy -O binary -j .text mbr.o mbr.bin
#  
#====================================================================================
#
# MBR( Master Boot Record )主引导记录包含两部分的内容，前446字节为启动代码及数据，而
# 从446(0x1BE)开始则是分区表，分区表由四个分区项组成，每个分区项数据为16字节，记录了
# 启动时需要的分区参数。
#
# 在CPU上电之后，若由硬盘启动，则BIOS将硬盘的主引导记录（位于0柱面、0磁道、1扇区）读
# 入7C00处，然后将控制权交给主引导代码。主引导代码的任务包括：
# (1) 扫描分区表，找到一个激活(可引导)分区；
# (2) 找到激活分区的起始扇区；
# (3) 将激活分区的引导扇区装载到内存7C00处；
# (4) 将控制权交给引导扇区代码；
#  
# 如果主引导代码无法完成上述任务，它将显示以下错误信息之一：
# No active partition.
# Invalid partition table.
# Error loading operating system.
# Missing operating system.
#
#====================================================================================
# FAT16分区尺寸与LBA
#====================================================================================
# LBA   HeadsPerCylinder SectorsPerTrack  Maximum Size for Boot Partition
# Disabled 64     32     1GB
# Enabled  255     63     4GB
#
# 为了适应超过8G的硬盘，Windows2000忽略了Start CHS和End CHS，而使用StartLBA和TotalSector  
# 来确定分区在整个磁盘中的位置和大小。
#  
#====================================================================================
#     分区表项结构(16字节)
#====================================================================================
#
# typedef struct _PARTITION_ENTRY
# {
#  UCHAR BootIndicator#  // 能否启动标志
#  UCHAR StartHead#   // 该分区起始磁头号
#  UCHAR StartSector#  // 起始柱面号高2位：6位起始扇区号
#  UCHAR StartCylinder#  // 起始柱面号低8位
#  UCHAR PartitionType#  // 分区类型
#  UCHAR EndHead#   // 该分区终止磁头号
#  UCHAR EndSector#   // 终止柱面号高2位：6位终止扇区号
#  UCHAR EndCylinder#  // 终止柱面号低8位
#  ULONG StartLBA#   // 起始扇区号
#  ULONG TotalSector#  // 分区尺寸（总扇区数）
# }PARTITION_ENTRY,*PPARTITION_ENTRY#
#
#====================================================================================
#    主引导记录(MBR)结构
#====================================================================================
# typedef struct _MASTER_BOOT_RECORD
# {
#  UCHAR    BootCode[446]#
#  PARTITION_ENTRY  Partition[4]#
#  USHORT    Signature#
# }MASTER_BOOT_RECORD,*PMASTER_BOOT_RECORD#
#
#====================================================================================
.intel_syntax
.globl _ENTRY_POINT
.section .text
.code16                        # 生成16位代码而不是32位代码
#.org                   0x0600  # 指定程序被装入内存的起始位置
#=========================================================================
#
# 宏和常量定义
#
#====================================================================

.set ACTIVE_FLAG     ,  0x80    # 激活(可引导)分区标志
.set NOT_ACTIVE_FLAG ,  0x00    # 不激活标志
.set MBR_MOVE_ADDR   ,  0x0600  # MBR先移动自身到该位置然后再运行
.set BOOT_SIGNATURE  ,  0xAA55  # 启动标志
.set SEC_SIG_OFF     ,  0x01FE  # 启动扇区的标志位置

#====================================================================
# 分区表项结构偏移
#====================================================================
.set BootIndicator   ,  0       # 能否启动标志
.set StartHead       ,  1       # 该分区起始磁头号
.set StartSector     ,  2       # 起始柱面号高2位：6位起始扇区号
.set StartCylinder   ,  3       # 起始柱面号低8位
.set PartitionType   ,  4       # 分区类型
.set EndHead         ,  5       # 该分区终止磁头号
.set EndSector       ,  6       # 终止柱面号高2位：6位终止扇区号
.set EndCylinder     ,  7       # 终止柱面号低8位
.set StartLBA        ,  8       # 起始扇区号
.set TotalSector     ,  12      # 分区尺寸（总扇区数）

#====================================================================
# 常用的分区类型
#====================================================================
.set PA_EMPTY        ,  0x00    # 空分区
.set PA_FAT12        ,  0x01    # FAT12 ( < 32680 sectors )
.set PA_FAT16        ,  0x04    # FAT16 ( 32680 - 65535 sectors )
.set PA_EXTENDED     ,  0x05    # DOS EXTENDED
.set PA_BIGDOS_FAT16 ,  0x06    # FAT16 ( 33MB - 4GB )
.set PA_NTFS         ,  0x07    # NTFS
.set PA_FAT32        ,  0x0B    # FAT32
.set PA_FAT32_LBA    ,  0x0C    # FAT32 LBA
.set PA_FAT16_LBA    ,  0x0E    # FAT16 LBA
.set PA_EXTENDED_LBA ,  0x0F    # LBA EXTENDED
.set PA_DYNAMIC_DISK ,  0x42    # Dynamic Disk Volume
#====================================================================
# 主引导记录的入口
#====================================================================
_ENTRY_POINT:                   # 初始化相关寄存器及标志位
         CLI                    # 先关掉中断
         CLD                    # 方向为向前递增
         XOR    AX,AX           # AX = 0
         MOV    DS,AX           # 设置数据段寄存器 DS:SI
         MOV    ES,AX           # 设置附加段寄存器 ES:DI
         MOV    SS,AX           # 设置堆栈段寄存器
         MOV    BP,0x7C00       # 设置基址寄存器
         MOV    SP,BP           # 设置堆栈栈顶
  # 将MBR代码移动到0600H处
         MOV    SI,BP           # SI = 7C00H
         MOV    DI,0x600        # DI = 0600H
         MOV    CX,512          # 待移动的字节数
         REP    MOVSB 
         JMP    0:.RealStart
# 真正开始   
.RealStart:
 # 保存引导驱动器号
         MOV    byte ptr [ DriveNumber ] , DL
 #====================================================================  
 # 检查是否支持磁盘中断INT 13H的扩展
 #====================================================================
         MOV    AH,0x41
         MOV    BX,0x055AA
         INT    0x13
         JC     .LookupActive                            # 如果失败，进位标志为1
         MOV    BYTE ptr [DiskExtension],0x01            # 设置支持磁盘扩展标志
.LookupActive:
 # 查找激活分区
         MOV    BP,offset PartitionTable                 # 指向分区表
         MOV    BL,4                                     # 最多4个分区
#检查激活分区 
.CheckNext: 
         CMP    BYTE ptr [BP+BootIndicator],ACTIVE_FLAG  # 检查该分区是否激活
         JZ     .FoundActive                             # 找到激活分区
         CMP    BYTE ptr [BP+BootIndicator],NOT_ACTIVE_FLAG  # 检查该分区是否激活 
         JNZ    .InvalidTable                            # 无效值
         ADD    BP,0x10                                   # 指向下一个分区表项
         DEC    BL
         JZ     .NoActive                                # 没有找到激活分区
         JMP    .CheckNext
#找到了激活分区
.FoundActive:
 # 保存分区表项
         MOV    AL,4
         SUB    AL,BL                                   # AL = 4-BL = 第一个激活分区表项索引(0-3)
         MOV    BYTE ptr [ActivePartition],AL               # 保存激活分区表项索引
         MOV    DI,BP                                   # DI = 激活分区项
         MOV    DH,BYTE ptr [BP+StartHead]                   # 该分区起始磁头号
         MOV    CL,BYTE ptr [BP+StartSector]                 # 起始柱面号高2位：6位起始扇区号
         MOV    CH,BYTE ptr [BP+StartCylinder]               # 起始柱面号低8位
  # 保存起始扇区号
         MOV    AX,WORD ptr [BP+StartLBA+2]
         MOV    WORD ptr [DAP_SECTOR_LOW+2],AX 
         MOV    AX,WORD ptr[BP+StartLBA]  
         MOV    WORD ptr [DAP_SECTOR_LOW],AX 
 # 检查确信只有一个激活分区 
.Recheck:     
         ADD    BP,0x10                                  # 指向下一个分区项
         DEC    BL
         JZ     .LoadBootSector                          # 装载该分区的引导扇区
         CMP    BYTE ptr [BP+BootIndicator],NOT_ACTIVE_FLAG # 检查该分区是否激活 
         JNZ    .InvalidTable                           # 无效值
         JMP    .Recheck
# 装载激活分区的引导扇区 
.LoadBootSector:
  # 设置驱动器号
         MOV    DL,BYTE ptr [DriveNumber]
  # 检查是否支持扩展磁盘调用
         CMP    BYTE ptr [DiskExtension],0x01
         JNZ    .NoDiskExtension
  # 使用扩展磁盘调用读取引导扇区
 # 
 # INT 13H 
 #  AH = 42H
 #  DL = Drive Number
 # DS:SI = 指向磁盘地址包的指针
 #
         MOV    SI,offset DAP_PACKET_SIZE
         MOV    AH,0x42
         INT    0x13
         JC     .ErrorLoadOS
         JMP    .CheckBootSector
.NoDiskExtension: 
 #====================================================================
 # 
 # INT 13H 
 #  AH = 2        柱面号：0 - 1023
 #  AL = 要读取的扇区数     磁头号：0 - 255   
 #  CH = 柱面号低8位     扇区号：1 - 63 
 #  CL = 柱面号高2位 : 6位扇区号  
 #  DH = 磁头号
 #  DL = 驱动器号
 #  ES:BX = 缓冲区
 #
 #====================================================================
 # 读取引导扇区 
         MOV    BX,0x7C00
         MOV    AX,0x0201
         INT    0x13
         JC     .ErrorLoadOS
 # 检查引导扇区是否合法 
.CheckBootSector: 
 # 装载引导扇区成功，检查引导标志
         MOV    BX,0x7C00
         CMP    WORD ptr [BX+SEC_SIG_OFF], BOOT_SIGNATURE  # 检查引导标志
         JNZ    .MissingOS     
 # 准备跳转到激活扇区的引导扇区代码
 # DL = 磁盘驱动器号
 # DH = 激活分区号
 # DI = 激活分区项
         MOV    DL,BYTE ptr [DriveNumber]
         MOV    DH,BYTE ptr [ActivePartition]
         JMP    0:0x7C00
# 没有激活分区
.NoActive: 
         MOV    SI,offset MsgNoActive
         CALL   ShowMessage
         JMP    .Hang
 # 无效分区表
.InvalidTable:
         MOV    SI,offset MsgPartitionTable
         CALL   ShowMessage
         JMP    .Hang
 # 装载引导扇区失败
.ErrorLoadOS:
         MOV    SI,offset MsgLoadingOS
         CALL   ShowMessage
         JMP    .Hang
 # 引导扇区不合法
.MissingOS:
         MOV    SI,offset MsgMissingOS
         CALL   ShowMessage
.Hang:
         JMP    .Hang		 
#====================================================================
#
# 显示一个字符串
# 输入：
#   DS:SI  = 字符串的起始地址(以NULL结束)
#   
#====================================================================
ShowMessage:
         LODSB      # AL = DS:[SI] SI = SI+1
         OR     AL,AL  # 检测是否遇到NULL字符串
         JZ     .ShowEnd
         MOV    AH,0x0E
         MOV    BX,0x07
         INT    0x10
         JMP    ShowMessage
.ShowEnd:
         RET 
#====================================================================
# 调试例程
#====================================================================
.ifdef  DEBUG
#====================================================================
#
# 显示一个字符
# 输入： AL = 待显示字符
#
#====================================================================
PrintChar:
         PUSH   AX
         PUSH   BX
         MOV    AH,0x0E
         MOV    BX,7
         INT    0x10
         POP    BX
         POP    AX
         RET
#====================================================================
#
# 显示16进制的值(将一个BYTE变为两个ASCII字符打印出来，用于调试)
# 输入： AL = 待显示的字节
#
#====================================================================
PrintByte:
         PUSH   BX
         MOV    BH,AL
  # 显示高4位
         SHR    AL,4
         AND    AL,0x0F
         ADD    AL,0x30
         CMP    AL,0x39
         JLE    .PrintIt
         ADD    AL,0x07
.PrintIt: 
         CALL   PrintChar
 # 显示低4位 
         MOV    AL,BH
         AND    AL,0x0F
         ADD    AL,0x30
         CMP    AL,0x39
         JLE    .PrintItAgain
         ADD    AL,0x07        
.PrintItAgain: 
         CALL   PrintChar
         POP    BX
         RET
.endif  #DEBUG
#====================================================================
# 数据区
#====================================================================
MsgNoActive:         .ascii "No active partition.\0"
MsgPartitionTable:   .ascii "Invalid partition table.\0"   
MsgLoadingOS:        .ascii "Error loading operating system.\0"
MsgMissingOS:        .ascii "Missing operating system.\0"

#====================================================================
#临时数据
#====================================================================
DriveNumber:         .byte  0x80   # 启动磁盘启动器号
ActivePartition:     .byte  0x80   # 激活分区表索引(0-3) 
DiskExtension:       .byte  0x00   # 是否支持磁盘扩展调用
 
#====================================================================  
# 扩展磁盘服务所使用的地址包
#====================================================================
DAP_PACKET_SIZE:     .byte  0x10   # 包的大小为16字节
DAP_RESERVED1:       .byte  0x00   # 保留字节
DAP_READ_SECTORS:    .byte  0x01   # 要处理的扇区数(1 - 127 )
DAP_RESERVED2:       .byte  0x00   # 保留字节
DAP_BUFFER_OFF:      .word  0x7C00 # 缓冲区偏移
DAP_BUFFER_SEG:      .word  0x0000 # 缓冲区段地址
DAP_SECTOR_LOW:      .long  0x0000 # 起始扇区号的低32位
DAP_SECTOR_HIGH:     .long  0x0000 # 起始扇区号的高32位
#====================================================================
# 填充字节
.org  0x1b8           
#                     .space 95    
#====================================================================

#====================================================================
# 标志字节
#====================================================================
UniqueMbrSignature:  .long  0x4B43414A
UnknownWord:         .word  0x00   
#====================================================================
# 分区表(偏移为446)
#====================================================================
PartitionTable:      .byte  0x80
                     .byte  0x01
                     .byte  0x01
                     .byte  0x00
                     .byte  0x0C
                     .byte  0x0FE
                     .byte  0x0BF
                     .byte  0x0FC
                     .long  0x0000003F
                     .long  4194241 
                     .space 48  
#====================================================================
# 扇区最后的标记字节(NASM不支持重复ORG)
#====================================================================
BootSignature:       .word 0xAA55
#====================================================================
# 代码结束
#====================================================================

  
 
 

 


