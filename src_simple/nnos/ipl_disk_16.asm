[BITS 16]
[ORG 0x7C00]            ; 引导扇区加载位置

start:
    cli                 ; 关闭中断
    xor ax, ax
    mov ds, ax          ; 设置数据段寄存器
    mov ss, ax
    mov sp, 0x7C00      ; 设置栈指针

    ; 打印启动消息
    mov si, boot_msg
    call print_string

    ; 从硬盘读取第一个扇区（MBR）
    mov dl, 0x80        ; 0x80 表示第一个硬盘
    mov ax, 0x0201      ; AH=02 为读扇区命令，AL=01 表示读取 1 个扇区
    mov ch, 0x00        ; 磁头号
    mov dh, 0x00        ; 起始扇区号
    mov cl, 0x01        ; 第一个柱面
    mov bx, buffer      ; 将数据放入 buffer 位置
    int 0x13            ; BIOS 磁盘服务调用
    jc disk_error       ; 如果出错，跳转到错误处理

    ; 验证分区表并找到活动分区
    mov si, buffer + 0x1BE  ; 分区表起始位置
    mov cx, 4               ; 最多检查 4 个分区表项
find_active_partition:
    cmp byte [si + 4], 0x80 ; 检查是否为活动分区
    je load_vbr             ; 如果找到活动分区，跳转加载 VBR
    add si, 0x10            ; 向后移动到下一个分区表项
    loop find_active_partition
    jmp no_active_partition ; 没有活动分区则跳转到错误处理

load_vbr:
    ; 读取活动分区的 VBR
    mov dx, [si + 8]        ; 分区起始 LBA 地址
    mov cx, [si + 10]       ; 分区大小（可选，暂不使用）
    mov ax, 0x0201          ; 读取一个扇区
    mov bx, 0x7E00          ; 将 VBR 放置在 0x7E00
    int 0x13                ; BIOS 磁盘服务调用
    jc disk_error           ; 如果出错，跳转到错误处理

    ; 跳转到 VBR 位置执行
    jmp 0x0000:0x7E00

disk_error:
    mov si, error_msg
    call print_string
    hlt

no_active_partition:
    mov si, no_active_msg
    call print_string
    hlt

print_string:
    mov ah, 0x0E            ; BIOS 功能调用打印字符
.next_char:
    lodsb                   ; 加载 SI 中的下一个字符
    or al, al               ; 检查字符是否为 0
    jz .done                ; 如果是 0，字符串结束
    int 0x10                ; 显示字符
    jmp .next_char
.done:
    ret

boot_msg db 'Loading NNOS from HDD...', 0
error_msg db 'Disk read error.', 0
no_active_msg db 'No active partition found.', 0
buffer times 512 db 0       ; 缓冲区，用于读取扇区

times 510-($-$$) db 0       ; 填充到 510 字节
dw 0xAA55                   ; 引导扇区标记
