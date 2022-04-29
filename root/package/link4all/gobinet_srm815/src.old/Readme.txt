使用方法
1.编译gobinet驱动
PC端按如下方法:
make
insmod GobiNet.ko
嵌入式平台可修改makefile后编译
2.编译拨号工具
cd meig-cm/
make 
嵌入式平台同样要修改Makefile后编译
3.拨号
./meig-cm -s [APN名称]
如移动卡拨号:
./meig-cm -s cmnet

