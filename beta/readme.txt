欢迎所有用户反馈系统漏洞。
请将漏洞爆发时的运行时截图（或照片）、简短叙述、可能原因、解决方案（如果有）一并发送到开发者邮箱。
邮箱：tianhehechu@qq.com
欢迎加入开发者交流QQ群【NNOS开发者交流群】757581301

年年软件
2019.4.27
----------------------------------------------------------------------
/*系统级命令*/
sysinfo 查看系统信息
version 查看版本号
mem或free 查看内存信息
cls或clear 清空控制台
ls 查看当前目录文件列表
type或cat 查看文件内容 示例：cat test.txt
langmode 切换语言模式（存在漏洞） 示例：langmode 3
（0：英文  3：中文）
vim 使用控制台编辑器查看文件内容 示例：vim test.txt
notepad 使用文本编辑器打开文件 示例：notepad test.txt
run 运行可执行程序 示例：run hello05.nex
（注：运行可执行程序可省略run和.nex，直接使用文件名）
del/rm 删除文件 示例：del anbox.nex

/*应用级命令*/
任意键：正常结束当前应用程序（无退出按键者）
Shift + F4 强制结束当前应用程序
Shift + F2 打开新的命令行窗口
TAB 窗口切换
F11 底层窗口显示到最前端
鼠标左键：切换窗口、关闭窗口、确定
鼠标右键：无
timers 打开计时器
picture 图片查看器 示例：picture cheng.jpg
music 音乐播放器 示例：music goodbye.mml
（其他音乐文件：ltstar.mml、fate.mml、happy.mml）
（注意：若在虚拟机中使用音乐播放器，请使用VMware，在QEMU中无法听到声音。）
caltor 计算器 示例 caltor 1+1

/*应用程序列表*/
hello05.nex //Hello word C语言示例程序
helwin04.nex //C语言窗口示例程序
vim //控制台文本编辑器
notepad //文本编辑器
snake //贪吃蛇（未完成）
sushu //C语言显示10000以内素数测试程序
timers //计时器
beep //蜂鸣发声器测试程序
color02 //调色板测试程序
crack07 //DS寄存器病毒攻防测试程序
anbox //非矩形窗口测试程序
lineball //幻彩线球
alienwar //外星人战争游戏，左右键移动，空格发射炮弹
picture //照片查看器，支持jpg、bmp （按Q键退出）
music //音乐播放器（勿使用鼠标关闭，有漏洞，按Q键退出）
----------------------------------------------------------------------
本系统已引入996ICU协议，任何使用者与本系统有关的任何行为不应违反此协议。







