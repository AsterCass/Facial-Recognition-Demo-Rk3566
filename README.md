## 应用于 RK3566 的人脸识别程序

### 注意

瑞芯微的其他RK系列类似，只是这里以RK3566举例

开发板使用[泰山派](https://wiki.lckfb.com/zh-hans/tspi-rk3566/)进行测试，
SDK也是使用泰山派的，但是本质上大同小异，确定基础GUN版本和架构，基本都能通用

### 编译

修改`toolchain.cmake`相关内容，
执行`mkdir build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake .. && cmake --build . -j $(nproc)`