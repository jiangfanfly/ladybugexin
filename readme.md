
# ladybugexin 检校
ladybugexin实现导出ladybug全景相机内参(等距投影+畸变参数)和外参的功能。服务于pano-slam系统。
## VS源代码
需先配置第三方依赖库：ladybug、Eigen、ceres   
工程文件在VSsln下

## 可执行程序

运行环境：win10    
运行ladybugexin.exe
参数使用  
1. 不带输出参数，默认工作目录下创建  
.\ladybugexin.exe   PATH_TO_PGR 1  
example: .\ladybugexin.exe  F:\ladybug\Ladybug-000000.pgr 1  

2. 带输出参数，指定工作目录下创建  
.\ladybugexin.exe   PATH_TO_PGR  PATH_TO_OUTPUT 1  
example: .\ladybugexin.exe  F:\ladybug\Ladybug-000000.pgr F:\a 1  


最后一个参数   
1：从pgr文件导出图像 到 Img0 Img1 ...  
			0：不导出  

导出结果为 CalibrationResult，Img0 Img1 ...  timestamp.txt

-------------------
# 测试用数据 
ladybug导出的pgr文件，例如：  
Ladybug-000000.pgr  
