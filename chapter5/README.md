## 保护模式进阶
### 获取内存
通过0x15号BIOS中断实现，分别有三个子功能，子功能号放在EAX（AX）中，注意BIOS中断在实模式下，以下只能在实模式中进行

+ eax=0xe820：获取所有物理内存
+ ax=0xe801：检测低15MB和16MB~4GB
+ ah：最多检测64MB

---

### 0xe820

ARDS结构

|  字节偏移量 | 名称  | 描述  |
|---|---|---|
|  0 |  BaseAddrLow | 基地址的低32位  |
| 4  | BaseAddrHigh  | 基地址的高32位  |
| 8  |  LengthLow | 内存长度的低32位，字节为单位  |
| 12  |  LengthHigh | 内存长度的高32位，字节为单位  |
| 16  |  Type | 本段内存类型  |

每次int0x15都会返回一个这种结构  
Type字段具体含义

|  值 | 描述  |
|---|---|
| 1  | 这段内存可以被操作系统使用  |
| 2 | 内存使用中或者系统保留，os无法使用  |
|  其他 | 未定义  |

调用0xe820需要的参数及其含义

|  寄存器 | 用法  |
|---|---|
| EAX  | 固定输入0xe820  |
| EBX | 使用前必须置为0，该值为当前的ARDS下标  |
|  ES:DI | 该值指向的内存为ARDS结构中的值  |
|  ECX | ARDS结构的大小，固定为20字节  |
|  EDX | 签名标记，固定为0x534d4150  |

返回0xe820中寄存器或标志位含义

|  寄存器或标志 | 用法  |
|---|---|
| CF  | 0表示没出错，1表示出错  |
| EAX | 固定为0x534d4150  |
|  ES:DI | 该值指向的内存为ARDS结构中的值  |
|  ECX | ARDS结构的大小，固定为20字节  |
|  EBX | 下一个ARDS，若返回后为0，那么检测结束  |

注意：在赋值时，EBX、EDX、DI只用赋值一次，其他每次中断，都要重新赋值

---

### 0xe801

低于15MB的内存单位为1KB，高于16MB的内存单位为以64KB记录  
具体寄存器功能说明
|  寄存器或标志 | 用法  |
|---|---|
| AX（调用）  | 固定输入0xe801  |
|---|---|
| AX（返回）  | 1KB为单位，最大为0x3c00  |
| CF  | 0表示没出错，1表示出错  |
| BX | 64KB为单位  |
|  CX | 同AX  |
|  DX | 同BX  |

分为两段内存检测原因：实际物理内存和检测内存如下图
![这是图片](../imgs/chapter5/001.png "内存检测")
由于80286的寻址范围最大为16MB，老式ISA设备会使用15MB以上缓存区，但操作系统不可以使用这段（memory hole），实际的物理内存大小要加1MB

---

### 0xe88
此中断只会显示0~63MB，因为不包含低端1MB大小。  
具体寄存器功能说明
|  寄存器或标志 | 用法  |
|---|---|
| AH（调用）  | 固定输入0x88  |
|---|---|
| AX（返回）  | 1KB为单位，不包括低端1MB  |
| CF  | 0表示没出错，1表示出错  |

### 分页
在没有开启分页情况下，线性地址（基地址+段内偏移）= 物理地址  
页表用于存储线性地址和物理地址的映射关系，一页大小为4KB，一个页表项为4B  
页目录表用于存储页目录项（页表物理地址）  
在32位地址空间中，虚拟地址的高10位用于定位页目录项，中间10位用于定位页表项，最后12位用于页内偏移地址（用于表示4KB之内任意地址）  
注意给出PDE（页目录项）和PTE（页表项）索引后，还需要*4，因为它们都是4B大小。  
具体转换步骤：  
1. 将虚拟地址的高10位乘4，加上页目录表的物理地址，得到页目录项物理地址，页目录项里是页表的物理地址
2. 将虚拟地址的中间10位乘4，加上第一步中得到的页表物理地址，得到页表项物理地址，页表项里是分配的物理页地址
3. 虚拟地址的低12位，加上第2步得到的物理地址

页目录项
![这是图片](../imgs/chapter5/002.png "页目录项")
页表项
![这是图片](../imgs/chapter5/003.png "页表项")
4字节大小，只有12~31为物理地址  
P，Present，存在位。若为1表示该页存在于物理内存  
RW，Read/Write，读写位，1表示可读可写，0表示不可读写  
US，User/Supervisor，普通用户/超级用户，1表示User级，任意级别可访问，0表示Supervisor级，级别为3不可访问
PWT，Page-level Write-Through，页级通写位，用时直接为0  
PCD，Page-level Cache Disabel，页级高速缓存禁止位。1表示启用高速缓存  
A，Acessed，访问位。1表示该页被CPU访问过  
D，Dirty，脏页位，对页面写时，设置为1  
PAT，Page Attribute Table，页属性表位，置为0
G，Global，全局位，1为全局页，将在TLB中一直保存  
AVL，Available，保留位，没用  

启用分页机制  

1. 准备好页目录表和页表
2. 将页目录表地址写入cr3（页目录表基址寄存器）
3. cr0的PG位置1

![这是图片](../imgs/chapter5/003.png "cr3")
只用设置高20位（页目录表低12位必定为0），其他都为0  

### 准备页表

见代码002  
设置虚拟地址空间的0~3GB设置为用户进程，3GB~4GB是操作系统。  
页目录表物理地址位置：0x100000，页目录表占4KB，第一个页表物理地址为0x101000  
各个用户进程的高1GB要指向内核物理地址，即进程768（768*4MB刚好是3GB）~1022（1023指向页目录表本身）的页目录项都一样  
0x00000000～0x3fffffff 是第 1 个 1GB 内存  
0x40000000～0x7fffffff 是第 2 个 1GB 内存  
0x80000000～0xbfffffff 是第 3 个 1GB 内存  
0xc0000000～0xffffffff 是第 4 个 1GB 内存
最高位每变化4位为1GB  

对页表和页目录表是在物理地址下操作，而对gdt，栈等要修改为虚拟地址

0xffc00000～0xffc00fff -> 0x000000101000～0x000000101fff  
出现上述映射的原因：如果高10位全为1即在第1023个页目录项，即11111111110...b = 0xffc...，该目录项的高20位物理地址为0x100000,由于该地址是高10位的虚拟地址索引，因此会被当做页表的物理地址。  
此时若中间10位为0，则索引到第0个页目录项，为第一个页表物理地址，即0x101000。最终12位地址也是0，所以加上不变。最后会索引成上述这样

0xfff00000～0xfff00fff -> 0x000000101000～0x000000101fff  
出现上述映射的原因：同上0xfff会索引到第768个目录项，刚好指向第一个页表

0xfffff000～0xffffffff -> 0x000000100000～0x000000100fff 
出现上述映射的原因：0xfffff会先索引到1023个页目录项，然后又索引到1023个页目录项，最终就是页目录表的物理地址，因此访问页目录项可以使用0xfffffxxx，其中xxx是页目录表内便宜地址，刚好4KB，不用再乘4

总结：

+ 获取页目录表物理地址：0xfffff000
+ 获取页目录项物理地址：0xfffffxxx
+ 获取页表项物理地址：0x3ff（左移22位变成ffc）+ 中间10位<<12 + 低12位

### 快表
在快表中，高20位的虚拟地址对应高20位的物理地址  
TLB的更新方式交给开发者  
一种方式是重新加载CR3，一种是使用invlpg命令

### C编写内核

见003文件夹  
gcc命令  
`-c` 编译、汇编，不进行链接，生成目标文件  
`-o` 输出的文件以指定文件名存储,如果不加-c则会直接生成可执行文件
目标文件也称作待重定位文件，重定位指文件里面的符号没有安排地址，符号地址在与其他目标文件链接成可执行文件后进行定位  
`file xxx` 查看文件状态  
`nm xxx` 查看符号地址  
`ld 源文件 -Ttext 0xc0001500 -e main -o 生成文件` -Ttext表示起始虚拟地址，-e指定程序起始地址，默认是_start  

ELF中目标文件类型分为：待重定位文件、共享目标文件（动态链接库）、可执行文件

条目对应程序（节）头表中的一个程序（节）头里的描述信息  
ELF信息包含在/usr/include/elf.h中  
ELF header结构
```C
#define EI_NIDENT (16)

typedef struct
{
  unsigned char e_ident[EI_NIDENT];     /* Magic number and other info */
  Elf32_Half    e_type;                 /* Object file type */
  Elf32_Half    e_machine;              /* Architecture */
  Elf32_Word    e_version;              /* Object file version */
  Elf32_Addr    e_entry;                /* Entry point virtual address */
  Elf32_Off     e_phoff;                /* Program header table file offset */
  Elf32_Off     e_shoff;                /* Section header table file offset */
  Elf32_Word    e_flags;                /* Processor-specific flags */
  Elf32_Half    e_ehsize;               /* ELF header size in bytes */
  Elf32_Half    e_phentsize;            /* Program header table entry size */
  Elf32_Half    e_shnum;                /* Section header table entry count */
  Elf32_Half    e_shstrndx;             /* Section header string table index */
} Elf32_Ehdr;
```
解释：

+ 前四个字节为固定值：e_ident[0] = 0x7f,e_ident[1]='E',e_ident[2]='L',e_ident[3]='F'
+ e_ident[4] 用于表示elf文件类型，0为不可识别，1为32位elf，2为64位elf
+ e_ident[5] 编码格式，0为非法，1为小端，2为大端
+ e_ident[6] 版本信息，默认为1，0为非法
+ e_ident[7~15] 保留位，默认为0
+ e_type指定文件类型