/*
 * Title: Our first OS: HellOS
 */        
        .code16                 #���߱�������δ�����16λ��࣬���ᱻ����ɻ�����
        .section .text          #���߱�������δ������ʱҪ�����ı��Σ�Ҳ�ͻ������޹�
        .global main            #���߱������������ļ��ɼ����ͻ������޹�
main:

        jmp _start              #��ת�� _stratλ��
        .byte   144             #�����ݽ��������JMP�����뱣�棬��Ȼ����ִ�е� 
        .ascii  "HellOS"        #��������              
        .word   512             #BytesPerSector  ����           
        .byte   1               #SectorsPerCluster ����         
        .word   1               #ReservedForBoot     ����        
        .byte   2               #NumberOfFats     ����          
        .word   224             #RootDirEntries (224 * 32 = 7168 = 14 sectors to read) ����          
        .word   2880            #LogicalSectors             ����  
        .byte   0xf0            #MediumByte       ����          
        .word   9               #SectorsPerFat    ����          
        .word   18              #SectorsPerTrack   ����         
        .word   2               #Sides            ����          
        .long   0               #HiddenSectors   ����             
        .byte   0               #LargeSectors     ����        
        .byte   0               #DriveNo           ����                          
        .byte   0x29            #Signature (41 for Floppy)����  
        .long   0x12345678      #VolumeID����  
        .ascii  "Our First OS"  #VolumeLabel����  
        .ascii  "FAT12   "      #FileSystem����  

_start:                         #��ʼ��
        movw $0, %ax            #��������0x0000���Ƶ�16λ�Ĵ���ax
        movw %ax, %ss           #��ax�Ĵ���ֵ�ƶ���ss�Ĵ�����
        movw %ax, %ds           #��ax�Ĵ���ֵ�ƶ���ds�Ĵ�����
        movw %ax, %es           #��ax�Ĵ���ֵ�ƶ���es�Ĵ�����
        movw $string, %si       #��string��ַ�si�Ĵ�����
loop:
        movb    $0xe, %ah       #��0xe д��ah�� ����Ǵ�ӡģʽ����
        movb    (%si), %al      #��si�д�ŵ�ַ�е�ֵȡ������al�������Ҫ��ӡ��ֵ
        cmpb    $0, %al         #�Ƚ�al�е�ֵ �� 0
        je      _end            #���al��ֵΪ0��Ҳ���Ǵ�ӡ���ˣ�������end
        int     $0x10           #�����жϵ��ã���ӡ����
        addw    $1, %si         #si�Ĵ���ֵ��1��Ҳ������һ���ַ�
        jmp     loop            # ����ѭ��
_end:
        jmp     _end            #while��1��

string:
        .ascii  "Welcome to HellOS :)"     # ������
        .byte '\n'                         # ������
        .ascii  "This is a baby HellOS :)" # ������

        .fill 0x1fe - (. - main) ,1,0      #������0x1fe -(.- main)�Ρ�����.��ʾ��ǰ��ַ��
                                           #main�Ǳ�ǩmain����ĵ�ַ��ÿ������ֽڸ���1��
                                           #�������0x00
        .word   0xaa55                     #��ӽ�����