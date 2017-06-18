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
- [x] MEMS基本驱动
- [ ] 段式液晶
- [ ] 按键
- [x] LED
- [ ] USB HOST
- [ ] MFX
- [ ] 各种片内外设
- [ ] 串口波特率自动识别

## 指令

- [x] help:打印所有存在的指令.FreeRTOS自带.
- [x] task-stats:查看任务状态,CLI肯定写R的,这是CLI自带的,然后修改过下.
- [x] run-time-stats:统计哪个任务更占资源,目前CPU空闲都是99%
- [x] query-heap:看看还有对少堆内存.
- [x] playback:是否从DFSDM录音然后从CS43L22播放
- [x] led:控制板载两个LED
- [ ] 播放QSPI的音乐...

## 更多

https://www.lijingquan.net