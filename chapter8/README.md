## makefile
### 基本知识
linux中有atime，mtime，ctime使用`stat filename`  
makefile通过mtime（**只改变数据**）的方式来更新  
```make
1:2
	echo "hello makefile"
```
在依次创建了1和2文件后，会输出echo "hello makefile"  
使用@符号取消命令行输出，可见**test1文件夹**  

---
### 伪指令
使用`.PHONY:clean`用法添加伪指令  
常见的伪指令用法如下：
| 名称  | 功能  |
|---|---|
| all  | 完成模块所有打包  |
| test  | 测试makefile流程  |
|  clean | 搭配rm使用  |

---
### 变量
定义变量：`obj = test1.o test2.o`  
使用：`$(obj)`  
可见**test2文件夹**  
系统变量使用  
命令相关
| 名称  | 功能  |
|---|---|
| CC  | C语言编译器，默认为"cc"  |
| CXX  | C++编译参数，默认为"g++"  |
|  CPP | C预处理器，默认为"$(CC) -E"  |

参数相关
| 名称  | 功能  |
|---|---|
| CFLAGS  | C语言编译参数  |
| CXXFLAGS  | C++编译参数  |
|  CPPFLAGS | C预处理器参数  |
|  LDFLAGS | 链接器参数  |

---

### 隐式规则
注释：`#`  
换行：`\`  
有时不需要添加固定的命令依然能生成目标文件

---

### 神奇的用法
| 名称  | 功能  |
|---|---|
| $@  | 目标文件的集合  |
| $<  | 依赖文件的第一个  |
|  $^ | 所有依赖文件  |
|  $? | 所有比目标文件mtime更加新的依赖文件集合  |
可见**test3**

## 实现assert

