# PointCloudBP

![PointCloud效果展示](https://github.com/daviddych/PointCloudBP/blob/master/PointCloud%E6%95%88%E6%9E%9C%E5%B1%95%E7%A4%BA.png)


load, show and process 3D point cloud while from las or txt file format

/data 文件夹下面是测试数据，
/bin/x64文件夹下面是编译好的可执行程序PointCloudBP.exe(Release版) 和 PointCloudBPd.exe(Debug版本)


单击PointCloudBP.exe启动后，通过“文件|打开”可以读取点云文件； “文件|清除”清空场景



```
操作简介
```
"鼠标滚轮"： 向下远移视点，向上近移视点
"按下鼠标左键并拖拽"： 场景旋转
"按下鼠标右键并拖拽"： 场景平移
按键使用： w,x,a,d--旋转;   r--重置场景旋转和平移
"Ctrl + 鼠标左键" : 拾取并在状态栏显示点坐标<x, y, z> 
“ctrl + 鼠标右键”，显示/隐藏拾取的点