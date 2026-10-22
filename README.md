# 022_why_play_haruhikage

基于 STM32F103C8T6 的蜂鸣器音乐播放项目，使用 TIM PWM 输出单音主旋律。当前曲目为《春日影》，包含用户提供的简谱中括号内前奏，以及其后可见的主旋律片段，约 37 秒，播放一次后停止。

这是独立的 STM32 自主练习项目，不是 RoboMaster 电控组的正式考核任务。

## 功能

- TIM3_CH1（PA6）输出可变频率 PWM
- 通过改变 PWM 频率控制音高
- 通过 SysTick 计时控制音符时值
- 使用歌曲数据表描述“频率 + 时值”，播放器逻辑与具体歌曲分离
- 支持多首歌曲登记，通过启动索引选择
- 单次播放，不循环

## 硬件接线

目标板：STM32F103C8T6 最小系统板

| 器件 | 连接 |
|---|---|
| 无源蜂鸣器正极 | 5V |
| 无源蜂鸣器负极 | 2N2222 集电极 C |
| 2N2222 发射极 E | GND |
| 2N2222 基极 B | PA6，串联 1kΩ |
| STM32 GND | 5V 电源 GND，必须共地 |

电磁式蜂鸣器建议反向并联 1N4148 续流二极管：负极接 5V，正极接集电极。

注意：TMB12A05 是有源蜂鸣器，只能发固定音调，不能用于本项目的旋律播放。

## 软件配置

- STM32CubeMX + STM32Cube HAL
- STM32F103C8Tx
- 系统时钟：72 MHz
- TIM3 计数频率：1 MHz
  - PSC = 71
  - 初始 ARR = 999
- 定时器 PWM Mode 1，CH Polarity High
- Auto-reload Preload Disable
- 音符频率通过动态修改 ARR 实现，占空比保持约 50%

## 编译

需要 `arm-none-eabi-gcc` 和 `mingw32-make`：

```powershell
mingw32-make -j12
```

编译产物：

```text
build/022_why_play_haruhikage.elf
build/022_why_play_haruhikage.hex
build/022_why_play_haruhikage.bin
```

`.vscode/launch.json` 提供 DAPLink、ST-Link 和 J-Link 三种调试配置。

## 目录结构

```text
022_why_play_haruhikage/
├─ Core/                 # 应用代码和 CubeMX 生成代码
├─ Drivers/              # STM32 HAL / CMSIS
├─ .vscode/              # VS Code 构建与调试配置
├─ 022_why_play_haruhikage.ioc
├─ Makefile
├─ STM32F103C8Tx_FLASH.ld
└─ startup_stm32f103xb.s
```

主要音乐逻辑位于 `Core/Src/main.c` 的 `USER CODE` 区域内，重新生成 CubeMX 代码时不会覆盖。

## 音乐数据格式

每个音符由频率和十六分音符时值组成：

```c
typedef struct
{
    uint16_t freq_hz;
    uint16_t duration_16;
} MusicNote;
```

常用时值宏：

- `LEN_16TH`：十六分音符
- `LEN_8TH`：八分音符
- `LEN_D8TH`：附点八分音符
- `LEN_4TH`：四分音符
- `LEN_D4TH`：附点四分音符
- `LEN_2ND`：二分音符

音高为 `0` 时表示休止符。新增歌曲时只需填写音符数组和 `Song` 结构，播放器无需修改。

## 说明

该项目是单音蜂鸣器，无法还原和弦、伴奏和鼓点；曲谱数据仅保留主旋律。
