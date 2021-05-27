/*
 * Title: Our first OS: HellOS
 */        
        .code16                 #告诉编译器这段代码是16位汇编，不会被翻译成机器码
        .section .text          #告诉编译器这段代码加载时要放在文本段，也和机器码无关
        .global main            #告诉编译器对其他文件可见，和机器码无关
main:

        jmp _start              #跳转到 _strat位置
        .byte   144             #将数据接着上面的JMP机器码保存，虽然不会执行到 
        .ascii  "HellOS"        #保存数据              
        .word   512             #BytesPerSector  数据           
        .byte   1               #SectorsPerCluster 数据         
        .word   1               #ReservedForBoot     数据        
        .byte   2               #NumberOfFats     数据          
        .word   224             #RootDirEntries (224 * 32 = 7168 = 14 sectors to read) 数据          
        .word   2880            #LogicalSectors             数据  
        .byte   0xf0            #MediumByte       数据          
        .word   9               #SectorsPerFat    数据          
        .word   18              #SectorsPerTrack   数据         
        .word   2               #Sides            数据          
        .long   0               #HiddenSectors   数据             
        .byte   0               #LargeSectors     数据        
        .byte   0               #DriveNo           数据                          
        .byte   0x29            #Signature (41 for Floppy)数据  
        .long   0x12345678      #VolumeID数据  
        .ascii  "Our First OS"  #VolumeLabel数据  
        .ascii  "FAT12   "      #FileSystem数据  

_start:                         #初始化
        movw $0, %ax            #将立即数0x0000，移到16位寄存器ax
        movw %ax, %ss           #将ax寄存器值移动到ss寄存器中
        movw %ax, %ds           #将ax寄存器值移动到ds寄存器中
        movw %ax, %es           #将ax寄存器值移动到es寄存器中
        movw $string, %si       #将string地址填到si寄存器中
loop:
        movb    $0xe, %ah       #将0xe 写到ah； 这个是打印模式参数
        movb    (%si), %al      #将si中存放地址中的值取出来给al，这个是要打印的值
        cmpb    $0, %al         #比较al中的值 和 0
        je      _end            #如果al中值为0，也就是打印完了，就跳到end
        int     $0x10           #启动中断调用，打印功能
        addw    $1, %si         #si寄存器值加1，也就是下一个字符
        jmp     loop            # 继续循环
_end:
        jmp     _end            #while（1）

string:
        .ascii  "Welcome to HellOS :)"     # 填数据
        .byte '\n'                         # 填数据
        .ascii  "This is a baby HellOS :)" # 填数据

        .fill 0x1fe - (. - main) ,1,0      #填充次数0x1fe -(.- main)次。其中.表示当前地址，
                                           #main是标签main代表的地址，每次填充字节个数1，
                                           #填充内容0x00
        .word   0xaa55                     #添加结束符