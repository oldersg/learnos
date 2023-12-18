## 中断
### 中断分类
+ 外部中断（硬件中断）
    
    + 可屏蔽中断
    + 不可屏蔽中断
+ 内部中断

    + 软中断
    + 异常 

---

### 外部中断

使用信号线INTR（INTeRrupt）和NMI（Non Maskable Interrupt）通知CPU  
INTR和NMI区别  
![这是图片](../imgs/chapter7/001.png "INTR和NMI")
INTR属于可屏蔽中断，可以通过设置eflags中的IF位屏蔽。而NMI不行，不可屏蔽中断的中断向量号为2  
中断程序分为上半部分和下半部分。其中上半部分为需要立即执行，属于关中断状态，下半部分属于开中断，可以稍后执行

---

### 内部中断

软中断：由软件主动发起的中断  

+ int 0~255
+ int3 触发中断向量号为3，为调试指令，机器码为0xcc 
    
    + 原理为：调试器将被调试的进程断点起始地址的第1字节备份后，原地修改为0xcc。当执行到0xcc时，触发中断。当中断发生时，会在栈中保存上下文，通过查看此时栈的上下文，从而获得寄存器，内存单元的情况。

+ into 中断向量号为4，溢出指令，查看OF位为1是触发
+ bound 检查数组是否越界，中断向量号为5
+ ud2 未定义指令，中断向量号为6，用于软件测试  

上面几种除了int 0~255外，都可被成为异常。

异常： CPU内部错误引起  
可以无视IF位的中断类型：NMI和内部中断  
异常分类

+ Fault：如Page Fault
+ Trap：int3
+ Abort

总结：异常和不可屏蔽中断的中断向量号由CPU提供，可屏蔽中断由中断代理8259A提供，软中断由软件提供。

---

### 中断描述符表（IDT）

中断描述符为8字节，具体结构如下
![这是图片](../imgs/chapter7/002.png "中断门")
在使用中断描述符进入中断，会将IF置为0，关中断  
IDT的位置可以在内存任意地方，找寻方式使用中断描述表寄存器（IDTR）  
IDTR长度为48位，6字节，其中低16位表示界限（0~0xffff，64KB），高32位表示基地址，但只支持0~255中断号，其中0号为除法错  
加载IDTR指令为：lidt