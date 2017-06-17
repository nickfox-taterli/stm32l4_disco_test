# STM32L476 DISCO 开发学习测试

## 概述

- 目前驱动都是比较高效的,部分HAL改成我自己习惯的库格式.
- 无HAL库,全部都是LL库.

## 软件资源

- QSPI + DMA2_Channel 7 Low Priority
- SAI1 + DMA2_Channel 1 Low Priority
- FreeRTOS v9.0.0
- STM32CubeMX F4 LL Library
- 我自己写的部分库

## 使用 

- QSPI 读写支持/擦除支持
- QSPI 映射支持
- SAI + QSPI + DMA 播放音乐,音频文件放在0x90000000,然后播放.
- 如果需要使用FreeRTOS CLI需要在Keil中配置,但是这就占用串口了.

## 进度

- [x] QSPI DMA [N25Q128] 驱动
- [x] SAI DMA [CS43L22] 驱动 (192K/96K/48K/32K/16K/8K)
- [x] SAI DMA [CS43L22] 驱动 (44K/22K/11K)
- [x] DFSDM 驱动
- [x] FreeRTOS CLI
- [x] FreeRTOS CPU使用率统计
- [ ] MEMS基本驱动
- [ ] 段式液晶
- [ ] 按键
- [ ] LED
- [ ] USB HOST


## 更多

https://www.lijingquan.net