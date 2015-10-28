main.cpp: 测试记录管理模块的正确性

在test_RM_FileScan()函数中:
首先新建文件，打开文件，然后插入32byte大小的数据，数据定义为：
struct Rec{
int b; float f; char a[24];}

测试插入记录、删除记录、更新记录与获取属性值满足特定条件的记录。
