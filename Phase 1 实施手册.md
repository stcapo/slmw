# Phase 1 实施手册

> 启动日期：2026-05-25  
> 当前阶段：第六步，Tuya App 配网/控制验证准备  
> 原则：只做 12V 低压验证；12V 只进 MOSFET 模块和 LED，绝不接入 T5AI-Core。

## 0. 本阶段只做什么

Phase 1 的第一轮闭环最终目标是：

```text
T5AI-Core -> PWM -> YYNMOS-4/NMOS-4 -> 12V LED 灯带
```

但是第一步只验证 `T5AI-Core` 单板：

1. Type-C 供电后板子能亮、能启动。
2. 电脑能识别出 USB 串口。
3. 串口工具能看到启动日志。
4. 万用表能测到板载 `5V` 和 `3V3` 供电点正常。

通过这一步以后，再进入 PWM 单测；不要一开始就把 MOSFET、灯带、Buck 全部接上。

## 1. 准备物料

| 物料 | 本步是否使用 | 说明 |
| --- | --- | --- |
| T5AI-Core 开发板 | 使用 | 主角 |
| Type-C 数据线 | 使用 | 必须是数据线，不只是充电线 |
| 电脑 | 使用 | 当前电脑是 macOS 12.7.6 |
| 万用表 | 使用 | UT61B+、UT61E PRO 或同级都可以 |
| 12V2A 电源 | 不使用 | 下一步再测 |
| YYNMOS-4/NMOS-4 | 不使用 | PWM 跑通后再接 |
| LED 灯带 | 不使用 | MOSFET 接线确认后再接 |
| LM2596 Buck | 不使用 | 后面单独测，不能直接接 T5 |

## 2. 上电前检查

先把桌面清空，只放 `T5AI-Core + Type-C 线 + 万用表`。

检查顺序：

1. 板子下面不要垫金属物。
2. 不要把任何杜邦线、12V 电源线、灯带线接到 T5。
3. Type-C 线先不要插电脑。
4. 肉眼看板子有没有明显断针、歪针、金属屑、烧焦、破损。
5. 如果后续要量电压，万用表黑表笔固定碰 `GND`，红表笔去碰 `5V` 或 `3V3`。

## 3. 电脑识别串口

当前未插 T5 时，本机只看到：

```text
/dev/cu.Bluetooth-Incoming-Port
```

所以插上 T5 后，应该多出来新的 `/dev/cu.*` 串口设备。T5-AI 官方引脚图标注板载 `Dual Channel Serial (CH342F)`：

- `CH1`：串口下载，连接 `P10/P11`
- `CH2`：串口调试，连接 `P0/P1`

操作：

```bash
ls /dev/cu.*
```

记录插线前后的差异。

通过标准：

```text
插线前：只有 Bluetooth 或少数已有串口
插线后：多出 1-2 个 USB/serial/WCH/CH34x 相关串口
```

如果没有新增串口，优先怀疑：

1. Type-C 线只是充电线。
2. 板子没供上电。
3. CH342/CH34x 驱动没有生效。
4. USB 口/转接头异常。

## 4. 串口日志

推荐先用官方网页工具，不急着装复杂开发环境：

```text
https://tuyaopen.ai/tools/
```

使用 Chrome 或 Edge 打开。

串口调试参数先用：

```text
目标设备：T5AI 或自定义
波特率：115200
数据位：8
停止位：1
校验位：无
```

连接后按一下 T5 板上的 `RESET`。如果串口正确，接收窗口通常会出现启动日志。

通过标准：

```text
能连接串口
按 RESET 后有日志滚动
日志里没有持续重启、异常乱码、烧录失败等明显错误
```

如果 115200 没有日志，先不要乱烧录，后续再按官方工具支持的 921600/1500000 等速率排查。

## 5. 万用表测电压

万用表设置：

```text
档位：直流电压 V DC
黑表笔：GND
红表笔：5V 或 3V3
```

预期结果：

| 测点 | 正常范围 | 说明 |
| --- | --- | --- |
| `5V` 对 `GND` | 约 4.75V-5.25V | Type-C 供电 |
| `3V3` 对 `GND` | 约 3.20V-3.40V | 板载 3.3V 电源 |

注意：

- 表笔不要同时碰到相邻两个针脚。
- 不要用电流档去测电压。
- 不确定表笔位置时先拍照标注，再测。

## 6. 第一页记录模板

```text
日期：
T5AI-Core 是否亮灯：
插线后新增串口名称：
串口工具是否能连接：
RESET 后是否有日志：
5V 实测：
3V3 实测：
异常现象：
结论：通过 / 未通过
```

## 8. 2026-05-25 第一次实测记录

```text
日期：2026-05-25
插线前串口：
  /dev/cu.Bluetooth-Incoming-Port
插线后新增串口：
  /dev/cu.usbmodem5AAE1666553
  /dev/cu.usbmodem5AAE1666551
TuyaOpen Tools：
  /dev/cu.usbmodem5AAE1666551 可连接
  工具显示 Serial connected successfully
当前判断：
  USB 数据线、电脑 Web Serial、T5 板载 USB 串口枚举已通过
  但当前收到的是网页工具的环境信息，不等于已经看到 T5 启动日志
待复核：
  RESET 后是否有 T5 启动日志
  /dev/cu.usbmodem5AAE1666553 是否为调试日志口
  5V/3V3 电压测量点和万用表档位
```

## 9. 2026-05-25 第二次实测记录

```text
电池校验：
  万用表测一节电池为 1.601V，万用表基本可用
T5 电压：
  用户复测 5V/3.3V 仍接近 0V
串口：
  /dev/cu.usbmodem5AAE1666551 和 /dev/cu.usbmodem5AAE1666553 均已尝试
  按 RESET 后未看到 T5 启动日志
当前判断：
  仍不能直接判定板子损坏
  因为 USB Dual Serial 已经枚举，说明 USB 数据线、USB 口、CH342F 串口芯片至少已上电
下一步排查：
  1. 确认板上 POWER/PW LED 是否亮
  2. 按官方引脚图重新确认 GND、5V、3V3 的金属测点
  3. 串口日志波特率优先尝试 460800，再尝试 115200
  4. 如确认官方 5V/3V3 引脚仍为 0V 且电源灯不亮，联系卖家换板
```

## 10. 2026-05-25 第三次实测记录

```text
问题原因：
  T5AI-Core 板上电源开关未打开
修正后结果：
  打开电源开关后，RESET 有启动日志
  5V 和 3.3V 测点电压恢复正常
当前结论：
  Phase 1 第一步通过
  可以进入 PWM 空载单测
```

## 10.1 2026-05-25 烧录记录

```text
烧录端口：
  /dev/cu.usbmodem5AAE1666551
日志端口：
  /dev/cu.usbmodem5AAE1666553
烧录结果：
  Write flash success
  CRC check success
  Reboot done
  Flash write success
后置提示：
  tyutool_cli 报 No such option: -B
当前判断：
  Flash 写入和校验已成功，程序大概率已烧入
  -B 报错属于烧录后工具调用参数问题，暂不作为失败处理
下一步：
  打开日志端口 553，确认是否输出 PWM smoke test 日志
```

## 10.2 2026-05-25 PWM 程序运行记录

```text
TuyaOpen Tools：
  选择 T5AI 时，串口波特率固定为 460800
  553 端口按 RESET 能看到 bootloader / l2 boot / app startup / go to tuya 等启动日志
  选择 custom 并改 115200 时，boot 阶段可能乱码
Arduino Serial Monitor：
  553 端口能看到 PWM smoke test 输出
  示例：
    PWM_A P24 duty=90%, PWM_B P25 duty=10%, freq=200Hz
    PWM_A P24 duty=0%, PWM_B P25 duty=100%, freq=200Hz
逻辑分析仪：
  LA1010 已抓到两路周期性数字波形
当前判断：
  T5AI-Core 已成功运行自定义程序
  P24/P25 已有 PWM 输出
  下一步只需确认 CH0/CH1 与 P24/P25 的接线映射、频率约 200Hz、占空比随日志变化
```

## 11. 第二步：PWM 空载单测

目标：让 T5AI-Core 输出两路 3.3V PWM 信号，并用 Kingst LA1010 逻辑分析仪确认频率和占空比。

本步仍然不接：

- 12V2A 电源
- YYNMOS-4/NMOS-4 模块
- LED 灯带
- LM2596 Buck

### 11.1 使用的测试程序

本项目已新增一个 Arduino 冒烟测试程序：

```text
firmware/t5_pwm_smoke_test/t5_pwm_smoke_test.ino
```

测试程序输出：

| 引脚 | 作用 | 频率 | 占空比变化 |
| --- | --- | ---: | --- |
| `P24` | PWM_A | 200Hz | 10% -> 50% -> 90% -> 0% -> 100% |
| `P25` | PWM_B | 200Hz | 90% -> 50% -> 10% -> 100% -> 0% |

选择 200Hz 是为了后续接 YYNMOS/NMOS 模块时不超过其标称 500Hz 上限。这个程序是原型验证用，不代表最终产品 PWM 实现。

### 11.2 Arduino IDE 设置

官方 Arduino-TuyaOpen 框架支持 `TUYA_T5AI`，且支持 macOS。

Arduino IDE 2 开发板管理器地址：

```text
https://github.com/tuya/arduino-tuyaopen/releases/download/global/package_tuya_open_index.json
```

设置步骤：

1. 安装并打开 Arduino IDE 2。
2. 如果 Arduino IDE 提示 `Sign in`，直接跳过。只做本地编译/烧录不需要 Arduino 账号。
3. 打开 `文件 -> 首选项`。
   - macOS 顶部菜单通常是 `Arduino IDE -> Settings...` 或 `Arduino IDE -> Preferences...`
   - 也可以按快捷键 `Command + ,`
   - 中文界面可能显示为 `Arduino IDE -> 设置...` 或 `Arduino IDE -> 首选项...`
4. 在 `其他开发板管理器地址` 填入上面的地址。
5. 打开 `开发板管理器`：
   - Arduino IDE 2 左侧边栏第二个图标，像一块小板子/芯片
   - 或顶部菜单 `工具 -> 开发板 -> 开发板管理器...`
   - 英文界面是 `Tools -> Board -> Boards Manager...`
6. 在开发板管理器搜索 `Tuya Open` 并安装。
7. 开发板选择 `TUYA_T5AI_CORE`；如果菜单没有这个名字，再选 `TUYA_T5AI_BOARD` 或 `TUYA_T5AI`。
8. 关闭 TuyaOpen Tools 网页串口，避免串口被占用。
9. 端口选择下载口。T5AI 有两个串口，能看日志的是调试口，另一个通常是烧录口；如果烧录失败，就换另一个端口。
10. 打开并烧录 `firmware/t5_pwm_smoke_test/t5_pwm_smoke_test.ino`。

常见卡点：

- `Select Other Board and Port` 弹窗只是选择已经安装好的板，不是安装开发板包的地方。
- 如果只看到 `Arduino AVR Boards`、`Arduino ESP32 Boards` 等，说明 Tuya Open 开发板包还没有安装。
- 板名是 `TUYA_T5AI_CORE`，不是 `TUYA_T4AI_CORE`。
- 如果 GitHub 地址搜索不到 `Tuya Open`，把开发板管理器地址换成国内 Gitee 地址：

```text
https://gitee.com/tuya-open/arduino-tuyaopen/releases/download/global/package_tuya_open_index_cn.json
```

烧录会覆盖当前板子的出厂示例固件。当前项目目标是做自研原型验证，可以接受；如果需要保留出厂示例，先记录当前日志和现象。

### 11.3 LA1010 接线

只接逻辑分析仪，不接任何 12V 负载。

```text
LA1010 GND -> T5 GND
LA1010 CH0 -> T5 P24
LA1010 CH1 -> T5 P25
```

LA1010 设置建议：

| 项目 | 设置 |
| --- | --- |
| 采样率 | 1MS/s 起步 |
| 阈值 | 1.8V 或 3.3V 逻辑阈值 |
| 采样时长 | 5s-10s |
| 解码 | 不需要协议解码，只看方波 |

### 11.4 通过标准

| 检查项 | 通过标准 |
| --- | --- |
| 串口日志 | 烧录后串口每 3 秒输出一次 duty 信息 |
| `P24` | 约 200Hz 方波，占空比随日志变化 |
| `P25` | 约 200Hz 方波，占空比与 `P24` 反向变化 |
| 电平 | 低电平接近 0V，高电平约 3.3V |
| 安全 | 测试期间 T5 不重启、不发烫、无异味 |

通过后，下一步才是 `12V 电源单独测量` 和 `MOSFET 模块低压接线前检查`。

## 12. 当前阶段不能做的事

- 不接 12V 电源到 T5。
- 不接 MOSFET 模块。
- 不接 LED 灯带。
- 不调 LM2596 给 T5 供电。
- 不烧录陌生固件。
- 不把万用表电流档并到电源两端。

PWM 空载单测通过后，下一步才是 `12V 电源单测` 和 `MOSFET 模块接线前检查`。

## 13. 第三步：12V2A 电源单独测量

目标：确认 12V2A 成品适配器输出电压正常，并确认输出正负极。此步骤只测电源，不接 T5、不接 MOSFET、不接灯带。

### 13.1 准备

| 物料 | 用途 |
| --- | --- |
| 12V2A 成品适配器 | 被测电源 |
| 万用表 | 测直流电压和极性 |
| 绝缘胶带/纸板 | 防止输出线短路 |

桌面只放 12V 电源和万用表。输出端红黑线或圆头插头不要碰在一起。

### 13.2 万用表设置

```text
黑表笔 -> COM
红表笔 -> V/Ω
档位 -> 直流电压 V DC
手动量程 -> 20V 或 60V 档
自动量程 -> 直接选 V DC
```

不要使用：

- 电流档 `A/mA/uA`
- 电阻档 `Ω`
- 蜂鸣/通断档
- 交流电压 `V~`

### 13.3 如果输出是红黑两根线

1. 先不要插 220V。
2. 把红线和黑线分开，避免两根线碰到。
3. 插上 12V 适配器。
4. 黑表笔碰黑线金属端。
5. 红表笔碰红线金属端。
6. 读万用表数值。

判断：

| 读数 | 含义 |
| --- | --- |
| `+11.4V` 到 `+12.6V` 左右 | 正常；红线是正极，黑线是负极 |
| `-11.4V` 到 `-12.6V` 左右 | 电压正常，但表笔/线极性反了 |
| 接近 `0V` | 未供电、表笔没碰到金属、档位错误或电源异常 |
| 明显不是 12V，如 5V/24V | 电源型号不对，先不要继续 |

### 13.4 如果输出是圆形 DC 插头

常见 5.5mm DC 圆头通常是内正外负，但必须实测确认。

1. 插上 12V 适配器。
2. 黑表笔碰圆头外圈金属。
3. 红表笔轻轻伸进圆孔中心，碰里面的金属触点。
4. 不要让红表笔同时碰到外圈，否则会短路。
5. 读万用表数值。

判断：

| 读数 | 含义 |
| --- | --- |
| `+11.4V` 到 `+12.6V` 左右 | 内芯是正极，外圈是负极 |
| `-11.4V` 到 `-12.6V` 左右 | 内芯是负极，外圈是正极 |
| 接近 `0V` | 没碰到中心金属、适配器未上电或电源异常 |

### 13.5 记录模板

```text
日期：
电源标签标称：
输出形式：红黑线 / DC 圆头
万用表档位：
实测电压：
极性判断：
适配器指示灯是否亮：
结论：通过 / 未通过
```

通过标准：

```text
空载输出约 12V
正负极明确
输出线没有短路
无异味、无异常发热
```

此步骤通过后，下一步进入 `YYNMOS-4/NMOS-4 模块接线前检查`。

## 13.1 2026-05-25 12V 电源实测记录

```text
输出形式：
  圆形 DC 插头
实测电压：
  12.25V
极性：
  内芯正极，外圈负极
状态：
  输出线没有短路
  无异味、无异常发热
结论：
  12V2A 适配器单测通过
```

## 14. 第四步：YYNMOS-4/NMOS-4 接线前检查

目标：确认 MOSFET 模块端子、T5 控制信号和 12V LED 负载接线方向。此步骤先不上电，先看清端子。

### 14.1 模块端子理解

YYNMOS-4/NMOS-4 是低边开关模块，逻辑可以理解成：

```text
12V 电源正极 -> 模块电源正极 -> 模块各路输出+
模块各路输出- -> MOSFET 开关 -> 12V 电源负极
```

控制侧：

```text
T5 PWMx -> 模块 PWMx
T5 GND  -> 模块 GNDx
```

功率侧：

```text
12V 适配器正极 -> 模块 电源正极
12V 适配器负极 -> 模块 电源负极
模块 输出x+ -> LED 负载正极
模块 输出x- -> LED 负载负极
```

### 14.2 接线前必须确认

| 检查项 | 通过标准 |
| --- | --- |
| 模块规格 | 使用 `NMOS-4 (3-5V)` 或 `YYNMOS-4 (3-5V)` |
| 电源输入端 | 能在模块上找到 `电源正极`、`电源负极` 或 `DC+`、`DC-` |
| 输出端 | 能找到 `输出1+ / 输出1-`、`输出2+ / 输出2-` |
| 控制端 | 能找到 `PWM1/GND1`、`PWM2/GND2` |
| LED 负载 | 先用 12V 单色暖白灯带或确认好线序的 12V 负载 |
| T5 状态 | PWM 程序已运行，P24/P25 有 200Hz 输出 |

### 14.2.1 2026-05-25 模块照片确认

用户已提供 YYNMOS-4/NMOS-4 正反面照片，背面丝印可见：

控制侧：

```text
PWM1 GND1
PWM2 GND2
PWM3 GND3
PWM4 GND4
```

功率侧：

```text
DC+
DC-
OUT1+
OUT1-
OUT2+
OUT2-
OUT3+
OUT3-
OUT4+
OUT4-
```

当前第一轮只使用：

```text
PWM1 / GND1
DC+ / DC-
OUT1+ / OUT1-
```

### 14.2.2 没有 DC 母座转接头时的处理

优先推荐购买 `5.5*2.5mm DC 母座转螺丝端子` 或 `5.5*2.5mm DC 母座转红黑线`。如果要马上验证，可以剪开 12V 适配器的低压输出线。

只允许剪：

```text
12V 适配器 -> 圆形 DC 插头 这一侧的低压输出线
```

绝对不剪：

```text
插墙/插排的 220V 输入线
```

剪线步骤：

1. 拔掉 12V 适配器的 220V 插头。
2. 等 10 秒。
3. 在靠近圆形 DC 插头后方约 5cm-10cm 的位置剪断输出线；保留圆头小尾巴，后续需要时还能识别或改造。
4. 把适配器输出侧两根线分开。
5. 每根线剥皮约 5mm-7mm。
6. 先不要接模块，重新插上适配器。
7. 用万用表直流电压档测两根裸线。
8. 读数为 `+12V` 时，红表笔碰到的那根是 `12V+`，黑表笔碰到的是 `12V-`。
9. 拔掉适配器。
10. 给 `12V+` 贴红色胶带或写 `+`，给 `12V-` 贴黑色胶带或写 `-`。

如果剪开后发现不是两根普通导线，而是同轴结构：

```text
中间一根芯线 + 外面一圈铜丝/屏蔽层
```

通常中间芯线是正极，外层铜丝是负极，但仍然必须用万用表复测后再接。

处理后的线头要求：

- 正负两根线不能有散开的铜丝互相碰到。
- 裸铜长度只露出能进端子的 5mm-7mm。
- 不接线时，两根裸线要分开并临时包胶带。
- 插电测量时手不要碰裸铜。

### 14.3 第一轮推荐只接单路

为了降低风险，第一轮只接一路 LED，不接双色温/多路灯带。

推荐顺序：

```text
T5 P24 -> MOSFET PWM1
T5 GND -> MOSFET GND1

12V 正极 -> MOSFET 电源正极
12V 负极 -> MOSFET 电源负极

MOSFET 输出1+ -> 12V 单色灯带正极
MOSFET 输出1- -> 12V 单色灯带负极
```

如果没有剪开圆形 DC 插头，先不要硬接。需要用 DC 母座转接线、鳄鱼夹转接线，或把适配器输出安全转成两根明确的正负线后再接模块端子。

### 14.4 上电顺序

1. T5 先用 Type-C 上电，确认串口输出 PWM 日志。
2. MOSFET 模块先不接 12V，只接 `T5 P24 -> PWM1` 和 `T5 GND -> GND1`。
3. 断开 12V 适配器市电。
4. 把 12V 正负接到 MOSFET 模块电源输入。
5. 把 LED 单路接到 `输出1+ / 输出1-`。
6. 复查没有铜丝短路。
7. 插上 12V 适配器。
8. 观察 LED 是否随 PWM 日志明暗变化。

### 14.5 立即断电条件

出现以下任一情况，立刻拔 12V 适配器：

- LED 不正常爆亮且无法变化
- 模块冒烟、异味、发烫
- 12V 电源指示灯闪烁或熄灭
- T5 重启、电脑串口断开
- 接线端子有火花或铜丝碰线

通过标准：

```text
单路 LED 能随 P24 duty 变化而明暗变化
T5 不重启
MOSFET 模块不异常发热
12V 电源稳定
```

### 14.6 2026-05-27 转接件到货后的实操步骤

本轮目标：

```text
T5AI-Core P24 PWM -> YYNMOS-4 PWM1 -> 12V 单色 LED 灯带明暗变化
```

只使用一路：

```text
PWM1 / GND1
DC+ / DC-
OUT1+ / OUT1-
```

#### 14.6.1 先测 DC 母座转接头

1. 不接 MOSFET，不接 LED。
2. 把 `12V2A 适配器` 插入 `DC 母座转接头`。
3. 万用表直流电压档：

```text
红表笔 -> 转接头 +
黑表笔 -> 转接头 -
```

4. 预期读数：

```text
约 12.25V
```

5. 如果读数是负数，说明转接头标注/线色与实际相反，按万用表实测重新标正负。
6. 测完拔掉 12V 适配器。

#### 14.6.2 接 T5 控制侧

T5 先用 Type-C 上电，确认 Arduino Serial Monitor 还能看到 PWM 日志：

```text
PWM_A P24 duty=...
PWM_B P25 duty=...
```

然后接：

```text
T5 P24 -> MOSFET PWM1
T5 GND -> MOSFET GND1
```

注意：

- 不接 T5 的 `5V`。
- 不接 T5 的 `3V3`。
- 不把 12V 接到 T5。
- 鳄鱼夹夹 T5 排针时要避开相邻针脚，不能让金属夹同时碰两根针。

#### 14.6.3 接 MOSFET 功率侧

确认 12V 适配器仍然拔掉，再接：

```text
DC 母座转接头 + -> MOSFET DC+
DC 母座转接头 - -> MOSFET DC-
```

推荐用螺丝端子压紧裸线或杜邦线金属端，不要只靠手按。

#### 14.6.4 接单色 12V LED

先用 `12V 单色暖白灯带`，不要先接双色温三线灯带。

```text
LED 正极 / +12V / 红线 -> MOSFET OUT1+
LED 负极 / - / 黑线或白线 -> MOSFET OUT1-
```

如果灯带没有红黑线，只看焊盘丝印：

```text
+12V / + -> OUT1+
- / GND -> OUT1-
```

#### 14.6.5 上电前复查

上电前逐项确认：

```text
T5 P24 只接 PWM1
T5 GND 只接 GND1
12V+ 只接 DC+
12V- 只接 DC-
LED+ 只接 OUT1+
LED- 只接 OUT1-
T5 没有接到 12V
端子没有散铜丝短路
```

#### 14.6.6 上电观察

1. T5 保持 Type-C 供电，PWM 程序运行。
2. 插上 12V 适配器。
3. 观察 LED。

预期现象：

```text
LED 每 3 秒左右变化一次亮度
大致会经历：暗 -> 中 -> 亮 -> 灭 -> 全亮
```

因为当前 P24 程序 duty 是：

```text
10% -> 50% -> 90% -> 0% -> 100%
```

#### 14.6.7 如果不亮

先拔掉 12V 适配器，再排查：

```text
1. Arduino Serial Monitor 是否还在输出 PWM_A P24 日志
2. T5 P24 是否确实接到 PWM1
3. T5 GND 是否确实接到 GND1
4. 12V+ / 12V- 是否接到 DC+ / DC-
5. LED 正负是否接反
6. 是否误接到 OUT2/OUT3/OUT4
```

可以用万用表辅助测：

```text
MOSFET DC+ 对 DC- 应约 12V
T5 P24 对 T5 GND 平均电压会随 duty 变化
```

#### 14.6.8 如果异常

出现以下情况立刻拔掉 12V：

```text
冒烟
异味
端子打火
MOSFET 模块迅速发烫
12V 适配器指示灯闪烁/熄灭
T5 重启或 USB 断开
LED 爆亮且无法变化
```

#### 14.6.9 通过后记录

```text
日期：
12V 实测：
使用 LED：
P24 -> PWM1 是否确认：
LED 是否随 3 秒 duty 变化：
MOSFET 温度：
T5 是否重启：
异常：
结论：通过 / 未通过
```

#### 14.6.10 2026-05-27 Step 10 单路闭环实测记录

```text
日期：2026-05-27
当前工具状态：
  剥线钳、剪钳尚未到货
  已有单根母对母线和可夹单个排针的细鳄鱼夹
  12V 单色暖白灯带线头已剪开并露出铜丝，灯带有 +12V 丝印
T5 状态：
  USB 供电后 Arduino Serial Monitor 持续输出 PWM 日志
  P24 duty 循环：10% -> 50% -> 90% -> 0% -> 100%
接线：
  T5 P24 -> YYNMOS-4 PWM1
  T5 GND -> YYNMOS-4 GND1
  DC 转接头红线 -> YYNMOS-4 DC+
  DC 转接头黑线 -> YYNMOS-4 DC-
  12V 单色灯带 +12V/红线 -> YYNMOS-4 OUT1+
  12V 单色灯带负极/黑线 -> YYNMOS-4 OUT1-
结果：
  12V 灯带每 3 秒按 暗 -> 中 -> 亮 -> 灭 -> 全亮 变化
  单路低压 PWM 闭环验证通过
结论：
  Step 10 通过
  下一步进入 P24/P25 双路双色温混光验证
```

### 14.7 第五步准备：双色温灯带线序确认

#### 14.7.1 2026-05-27 双色温灯带线序实测记录

```text
测试方法：
  红线接 12V 电源正极
  黄线 WW 接 12V 电源负极，灯带亮暖白
  白线 CW 接 12V 电源负极，灯带亮冷白
结论：
  该双色温灯带为共阳极结构
  红线 = 公共正极 / +12V 或 +24V
  黄线 = WW 暖白负极通道
  白线 = CW 冷白负极通道
```

#### 14.7.2 下一步双路 MOSFET 接线计划

本步目标：

```text
T5AI-Core P24 -> YYNMOS-4 PWM1 -> 黄线 WW 暖白通道
T5AI-Core P25 -> YYNMOS-4 PWM2 -> 白线 CW 冷白通道
```

推荐接线：

```text
控制侧：
  T5 P24 -> MOSFET PWM1
  T5 P25 -> MOSFET PWM2
  T5 GND -> MOSFET GND1
  T5 GND -> MOSFET GND2

功率输入：
  DC 转接头红线 / 12V+ -> MOSFET DC+
  DC 转接头黑线 / 12V- -> MOSFET DC-

双色温灯带：
  红线公共正极 -> MOSFET OUT1+（或模块 DC+ / 任一路 OUT+，以丝印和实测为准）
  黄线 WW 暖白负极 -> MOSFET OUT1-
  白线 CW 冷白负极 -> MOSFET OUT2-
```

注意：

```text
红线公共正极不能接到 OUT1- 或 OUT2-
黄线/白线不要再直接接电源负极，改由 MOSFET OUT1-/OUT2- 控制
12V 仍然绝不接入 T5AI-Core
```

#### 14.7.3 2026-05-27 双路双色温混光实测记录

```text
接线：
  T5 P24 -> YYNMOS-4 PWM1 -> 黄线 WW 暖白负极
  T5 P25 -> YYNMOS-4 PWM2 -> 白线 CW 冷白负极
  T5 GND -> YYNMOS-4 GND1/GND2
  12V+ -> YYNMOS-4 DC+ / 灯带红线公共正极
  12V- -> YYNMOS-4 DC-
测试程序：
  firmware/t5_pwm_smoke_test/t5_pwm_smoke_test.ino
观察：
  灯光有周期性变化
  初始五档程序下，偏冷/中性/偏暖的肉眼区分不够明显
  T5 未重启，12V 电源稳定，未记录冒烟、异味、打火等异常
判断：
  双路双色温硬件闭环初步通过
  通道方向和色温映射后续可在 App 控制阶段校准
```

#### 14.7.4 2026-05-27 极限冷暖两档测试记录

新增测试程序：

```text
firmware/t5_cct_extreme_test/t5_cct_extreme_test.ino
```

程序只在两档之间切换：

```text
PURE_COLD:
  WW P24 duty = 0%
  CW P25 duty = 100%

PURE_WARM:
  WW P24 duty = 100%
  CW P25 duty = 0%
```

实测结论：

```text
极限冷白 / 极限暖白两档区别明确
双路 MOSFET 控制有效
Step 11 双路双色温硬件闭环通过
```

遗留现象：

```text
冷白通道肉眼能感觉到闪烁
当前 PWM 频率为 200Hz，属于原型阶段低频测试
YYNMOS-4/NMOS-4 标称最大 PWM 频率为 500Hz，因此本阶段先记录问题，不强行作为量产调光体验验收
后续产品化 MOSFET 功率级和正式固件应提升 PWM 频率，目标至少 2kHz 以上
```

## 15. 第六步：Tuya App 配网/控制验证

> 日期：2026-05-28  
> 目标：从“本地程序控制 PWM”推进到“手机 App -> Tuya 云/设备 -> DP 命令 -> PWM -> MOSFET -> 双色温灯带”的完整控制链路。

### 15.1 本步先做什么

本步不是先追求最终产品体验，而是按顺序验证四件事：

```text
1. T5AI-Core 能运行 TuyaIoT 示例/自定义固件
2. 设备能进入 Tuya App 配网/绑定流程
3. App 下发 DP 命令后，T5 串口能收到事件
4. DP 命令最终能控制 P24/P25 两路 PWM，从而控制开关、亮度、色温
```

本步仍然遵守硬件边界：

```text
12V 只进 MOSFET 模块和 LED 灯带
12V 绝不接入 T5AI-Core
T5AI-Core 仍用 USB/Type-C 供电
T5 GND 与 MOSFET 控制侧 GND 保持共地
```

### 15.2 本机可用的 TuyaIoT 示例依据

当前 Arduino-TuyaOpen 包里已有 `TuyaIoT` 示例：

```text
~/Library/Arduino15/packages/tuya_open/hardware/tuya_open/1.2.5/libraries/TuyaIoT/examples/quickStart/quickStart.ino
~/Library/Arduino15/packages/tuya_open/hardware/tuya_open/1.2.5/libraries/TuyaIoT/examples/quickStart/README_zh.md
~/Library/Arduino15/packages/tuya_open/hardware/tuya_open/1.2.5/libraries/TuyaIoT/src/TuyaIoT.h
```

示例关键调用：

```cpp
#include "TuyaIoT.h"

TuyaIoT.setEventCallback(tuyaIoTEventCallback);
TuyaIoT.readBoardLicense(&license);
TuyaIoT.setLicense(license.uuid, license.authkey);
TuyaIoT.begin("PRODUCT_ID", PROJECT_VERSION);
```

当前判断：

```text
配网/云端控制不再使用纯 PWM smoke test 程序
需要切换到基于 TuyaIoT 库的固件
TuyaIoT 固件再把 App 收到的 DP 值映射到 P24/P25 PWM 输出
```

### 15.3 配网前必须准备的信息

配网前至少要确认：

```text
1. Tuya IoT 平台上的产品 Product ID / PID
2. 设备 TuyaOpen 授权信息：UUID 和 AuthKey
3. 手机安装 Tuya Smart / 涂鸦智能 App
4. 手机连接 2.4GHz Wi-Fi，暂不使用 5GHz-only 网络
5. Arduino IDE 仍选择 TUYA_T5AI_CORE
6. 下载口仍优先使用 /dev/cu.usbmodem5AAE1666551
7. 日志口仍优先使用 /dev/cu.usbmodem5AAE1666553
```

关于授权信息：

```text
quickStart 示例会先尝试 TuyaIoT.readBoardLicense(&license)
如果板子已有 TuyaOpen 授权码，串口会打印 uuid/authkey
如果没有板载授权码，则必须在代码里填写 TUYA_DEVICE_UUID 和 TUYA_DEVICE_AUTHKEY
未替换示例里的 uuidxxxxxxxxxxxxxxxx / xxxxxxxx 时，demo 不能真正连接云端
```

### 15.4 今日推荐执行顺序

第一轮只验证“云端/DP链路能不能通”，不要一开始就把灯光算法写复杂。

推荐顺序：

```text
1. 先复制 quickStart 示例到本项目 firmware 目录，建立独立 App 配网测试固件
2. 保留串口日志，先只跑 TuyaIoT 初始化和配网事件
3. 先不急着接 12V，确认 T5 能启动 TuyaIoT 固件且串口无崩溃
4. 确认 Product ID / UUID / AuthKey 是否可用
5. 烧录 App 配网测试固件
6. 打开日志口，记录是否进入绑定/联网/MQTT 状态
7. 手机 App 尝试添加设备
8. 如果 App 能看到设备并绑定成功，再接回 12V 灯带做 DP 控制
```

### 15.5 本阶段 DP 设计草案

第一版 DP 先按常见灯具模型理解，不在没有 PID 模型前写死最终结论。

建议目标：

```text
开关：bool
亮度：value，映射到总亮度 duty
色温：value，映射到 WW/CW 比例
```

第一版 PWM 映射思路：

```text
power = false:
  WW = 0
  CW = 0

power = true:
  total = brightness
  warm_ratio = color_temperature
  WW = total * warm_ratio
  CW = total * (1 - warm_ratio)
```

注意：

```text
P24/P25 与 WW/CW 的方向以后可以通过代码或接线互换
App 色温滑条方向如果反了，不代表硬件失败
```

### 15.6 通过标准

第一轮 App 配网通过标准：

```text
T5 烧录 TuyaIoT 固件成功
串口能看到 TuyaIoT 初始化日志
设备能进入配网/绑定流程
Tuya App 能添加设备
绑定后设备显示在线
App 下发开关 DP 时，串口能看到 DP 接收日志
```

完整 Phase 1 通过标准：

```text
App 开关 -> 灯带开/关
App 亮度 -> 灯带整体亮度变化
App 色温 -> WW/CW 冷暖比例变化
T5 不重启
12V 电源稳定
MOSFET 无异常发热
```

### 15.7 异常排查顺序

如果不能配网，按这个顺序查：

```text
1. 串口是否有 TuyaIoT 初始化日志
2. 是否仍在运行旧 PWM smoke test，而不是 TuyaIoT 固件
3. Product ID 是否正确
4. UUID/AuthKey 是否为真实授权，不是示例占位符
5. 手机是否在 2.4GHz Wi-Fi
6. App 是否选择了正确品类/添加方式
7. 设备是否已绑定过旧账号，需要 remove/resetNetcfg 后重新配网
8. 日志里是否出现 MQTT connected / activate success / bind start 等事件
```

如果 App 能配网但灯不动，按这个顺序查：

```text
1. 串口是否收到 DP_RECEIVE_OBJ 或 DP_RECEIVE_RAW
2. DP ID 是否与产品模型一致
3. DP 类型是否与代码读取方式一致
4. PWM 输出是否仍在 P24/P25
5. 12V 灯带/MOSFET 接线是否保持 Step 11 成功状态
```

### 15.8 实测记录模板

每次反馈后追加一条记录，格式如下：

```text
日期：
测试目标：
使用固件：
Product ID：
UUID/AuthKey 状态：板载读取成功 / 手动填写 / 未确认
烧录结果：
日志口：
串口关键日志：
App 操作：
App 结果：
灯带现象：
异常：
判断：
下一步：
```

### 15.9 2026-05-28 初始记录

```text
当前状态：
  12V 单路灯带闭环已通过
  P24/P25 双路双色温硬件闭环已通过
  极限冷暖两档测试通过
  冷白通道在 200Hz 下可见闪烁，已记录为原型频率风险
今日任务：
  开始 Tuya App 配网/控制验证
执行原则：
  后续每次用户反馈配网/烧录/App/灯带结果后，都追加记录到本节
  先验证配网和 DP 链路，再做亮度/色温体验优化
```

### 15.10 2026-05-28 Tuya App 控制固件准备记录

```text
测试目标：
  准备第一版 Tuya App 配网/控制固件
使用固件：
  firmware/t5_tuya_cct_app/t5_tuya_cct_app.ino
  firmware/t5_tuya_cct_app/appConfig.json
授权信息：
  Product ID 已填写进本地 ino
  UUID/AuthKey 已填写进本地 ino
  手册不记录完整 AuthKey，避免密钥泄露
硬件输出：
  P24 -> WW 暖白 MOSFET 通道
  P25 -> CW 冷白 MOSFET 通道
  PWM 使用 T5 硬件 PWM，频率 200Hz，仍满足当前 MOSFET 模块 500Hz 以内限制
DP 兼容：
  主 DP：开关 20、亮度 22、色温 23
  兼容 DP：开关 1、亮度 2、色温 3
  如果收到其他 DP，串口会打印 Unhandled DP id，后续按真实 App 面板校准
编译结果：
  arduino-cli 编译通过
  FQBN：tuya_open:tuya_open:TUYA_T5AI_CORE
  目标：t5_tuya_cct_app.ino_QIO_1.0.0.bin
下一步：
  1. 打开 firmware/t5_tuya_cct_app/t5_tuya_cct_app.ino
  2. Arduino IDE 选择 TUYA_T5AI_CORE 和下载串口
  3. 先只接 T5 USB，烧录固件并打开日志口
  4. 看到 TuyaIoT 启动日志后，再用 App 配网
  5. App 在线后，再接回 12V/MOSFET/双色温灯带验证开关、亮度、色温
```

### 15.11 2026-05-28 Tuya App 控制固件烧录记录

```text
测试目标：
  烧录第一版 Tuya App 配网/控制固件到 T5AI-Core
使用固件：
  firmware/t5_tuya_cct_app/t5_tuya_cct_app.ino
用户反馈：
  已烧录完成
当前判断：
  进入串口日志与 App 配网验证阶段
下一步：
  1. 先只接 T5 USB，不接 12V 灯带电源
  2. 打开日志串口，波特率 115200
  3. 按 RESET，记录 TuyaIoT 启动日志
  4. 若看到 BIND_START / ACTIVATE / MQTT 等日志，再用 App 添加设备
  5. App 绑定在线后，再接回 12V/MOSFET/灯带做 DP 控制验证
```

### 15.12 2026-05-28 TuyaIoT 配网模式启动记录

```text
测试目标：
  确认 T5AI-Core 烧录后是否进入 Tuya App 配网状态
用户反馈串口关键日志：
  TUYA_EVENT_BIND_START
  netcfg module start type:0x1
  netcfg module start type:0x2
  start ap success:SmartLife-5A63
  ap netcfg server ip:192.168.176.1
  tuya ap using tls + psk
  Start Adv
当前判断：
  TuyaIoT 固件已启动
  设备已进入绑定/配网模式
  BLE 广播已启动
  AP 热点配网已启动，热点名为 SmartLife-5A63
  日志时间显示 01-01 00:00:00 属于未联网前未同步时间，暂不作为异常
下一步：
  使用涂鸦智能 App 添加设备
  优先等待 App 自动发现蓝牙设备
  如自动发现失败，切换 AP 配网并连接 SmartLife-5A63 热点
  配网成功后观察 ACTIVATE_SUCCESSED / MQTT_CONNECTED 日志
```

### 15.13 2026-05-28 BLE 连接记录

```text
测试目标：
  判断 App 是否已经连到 T5AI-Core 的 BLE 配网通道
用户反馈串口关键日志：
  feed watchdog
  BLE_GAP_EVENT_CONNECT
  Ble Connected
  mtu update event
  BLE_GAP_EVENT_SUBSCRIBE
  ble recv req type 0x0000
  ble dev info: state:0, pkg_len:253
  send notify ok
当前判断：
  feed watchdog 为系统喂狗日志，属于正常运行
  手机/App 已经通过 BLE 连接到设备
  App 已请求设备信息，设备已回复
  当前还不是配网成功，尚未看到 ACTIVATE_SUCCESSED / MQTT_CONNECTED
下一步：
  继续在 App 内完成 2.4GHz Wi-Fi 信息下发和绑定
  若 App 卡住，退出本次添加流程后重新自动发现
  若自动发现失败，切换 AP 配网连接 SmartLife-5A63 热点
```

### 15.14 2026-05-28 误选 5GHz Wi-Fi 后配网失败记录

```text
测试目标：
  排查 App 配网时误选 5GHz Wi-Fi 后，重新选择 2.4GHz Wi-Fi 仍添加失败的问题
用户反馈：
  App 内第一次误连 workwifi 的 5G 网络
  串口日志显示设备一直尝试连接 5G Wi-Fi
  用户随后手机重新连接 2.4G Wi-Fi，再次添加设备失败
当前判断：
  设备侧可能保留了上一次下发的错误 SSID/密码或处于半绑定/失败状态
  App 侧也可能残留失败添加的设备记录
  需要先清理设备配网状态和 App 失败记录，再重新下发 2.4GHz Wi-Fi
排查顺序：
  1. App 内删除失败/离线/添加中的设备记录
  2. T5AI-Core 长按内置按键 3 秒以上，触发 TuyaIoT.remove()
  3. 按 RESET 重启，确认重新出现 TUYA_EVENT_BIND_START
  4. 手机确认连接 2.4GHz Wi-Fi 后，再重新添加设备
  5. 若 BLE 自动发现失败，改用 AP 配网并连接 SmartLife-5A63 热点
```

### 15.15 2026-05-28 2.4GHz Wi-Fi 入网与云连接记录

```text
测试目标：
  验证误选 5GHz Wi-Fi 后，重新用 2.4GHz Wi-Fi 完成配网
用户反馈串口关键日志：
  Start LAN initialization
  lan init success
  TUYA_TLS Begin Connect a2.tuyaus.com:443
  handshake finish for a2.tuyaus.com
  TUYA_TLS Success Connect a2.tuyaus.com:443
  http request send
  key:uuid...ver
  tuya_mqtt_start...
当前判断：
  T5AI-Core 已成功连接 2.4GHz Wi-Fi
  已能访问涂鸦云端 a2.tuyaus.com:443
  TLS 握手成功，说明网络、DNS、证书链路基本正常
  设备已进入 MQTT 启动阶段
下一步：
  确认 App 设备显示在线
  确认串口是否出现 TUYA_EVENT_ACTIVATE_SUCCESSED / TUYA_EVENT_MQTT_CONNECTED
  在线后接回 12V/MOSFET/双色温灯带
  从 App 依次测试开关、亮度、色温，并记录 DP_RECEIVE_OBJ 日志
```

### 15.16 2026-05-28 App 下发 DP 与本地 PWM 映射记录

```text
测试目标：
  验证 App 控制命令是否已经通过云端/MQTT 下发到 T5AI-Core，并触发本地灯光逻辑
用户反馈串口关键日志：
  recv message TopicName:smart/device/in/...
  Tuya Event ID:8(TUYA_EVENT_DP_RECEIVE_OBJ)
  TUYA_EVENT_DP_RECEIVE_OBJ count=3
  Receive DP id=1
  Switch DP=on
  Apply light: power=on, bright=89%, cct=50% warm, WW=44%, CW=45%
  Receive DP id=2
  Brightness raw=416, percent=42
  Apply light: power=on, bright=42%, cct=50% warm, WW=21%, CW=21%
  Receive DP id=2
  Brightness raw=1000, percent=100
  Apply light: power=on, bright=100%, cct=50% warm, WW=50%, CW=50%
  Receive DP id=1
  Switch DP=off
  Apply light: power=off, bright=100%, cct=50% warm, WW=0%, CW=0%
  PUBACK
当前判断：
  Wi-Fi 已通过
  云端 MQTT 下行已通过
  App 控制命令已到达设备
  开关 DP id=1 已正确处理并上报
  亮度 DP id=2 已正确处理并上报
  本地 PWM duty 计算已随 App 命令变化
  Unhandled DP id=7 / 15 暂不影响开关和亮度主链路，后续根据产品 DP 定义决定是否处理
阶段结论：
  App -> Tuya Cloud -> MQTT -> T5AI-Core -> DP handler -> PWM duty 计算链路已通过
下一步：
  接回 12V/MOSFET/双色温灯带，确认实际灯光能随 App 开关和亮度变化
  继续寻找/测试色温 DP；若当前调光开关面板没有色温 DP，需要更换/调整产品功能或面板
```

### 15.17 2026-05-28 实物控灯、无色温 DP 与亮度不明显记录

```text
测试目标：
  验证 App 控制是否能驱动 12V + MOSFET + 双色温灯带实物，并分析色温/亮度问题
用户反馈：
  已接回 12V + MOSFET + 双色温灯带
  App 开关可以实际控制灯
  App 亮度可以实际控制灯
  当前 App 页面找不到色温控制
  当前 10% 与 100% 亮度肉眼区别几乎不明显
截图观察：
  当前面板标题为“调光开关 2”
  页面包含：总开关、总亮度、总亮度范围、开关1、开关2
  页面未出现“色温 / 冷暖 / CCT”滑条或控件
当前判断：
  12V/MOSFET/灯带实物控制链路已通过
  当前产品品类/面板是两路调光开关，不是双色温灯具面板
  当前面板下发的主要 DP 是开关和亮度，不提供明确色温 DP
  因此“找不到色温 DP”属于产品模型/面板选择问题，不是硬件接线问题
亮度差异不明显的可能原因：
  1. 当前调光开关面板与双色温灯业务模型不匹配，存在总亮度、分路亮度、亮度范围等额外 DP，固件只处理了部分 DP
  2. 固件当前固定色温为 50% warm，总亮度会被拆成 WW/CW 两路各一半；100% 总亮度实际为 WW 50% + CW 50%
  3. App 的“亮度范围/分路校准”可能对输出做了低端限制或范围映射，导致面板显示百分比不等于最终灯光线性变化
  4. 人眼对亮度不是线性感知，手机相机自动曝光也会压缩亮度差异，但 10% 与 100% 几乎无差异仍需要继续排查
  5. 需要用逻辑分析仪复核 P24/P25 在 App 1%/10%/100% 时的实际 PWM 占空比，判断问题在 App/DP、固件 PWM、还是 MOSFET/灯带响应
阶段结论：
  App 开关和亮度实物控灯可以阶段性 check
  Phase 1 完整目标仍未完成，因为缺少 App 色温控制
推荐路线：
  新建或调整为“照明/灯具/双色温/CCT”类产品和公版面板
  功能定义至少包含：开关、亮度、色温
  固件已有 20/22/23 和 1/2/3 两套 DP 兼容逻辑，换到正确 CCT 产品后优先观察是否收到色温 DP
下一步：
  先不要继续在“调光开关 2”面板里硬找色温
  到涂鸦平台确认当前产品是否可添加“色温”标准 DP
  如果不可添加或面板不显示色温，则创建新的双色温灯具产品
  新 PID/授权信息拿到后更新 ino，重新烧录、remove/reset 后再配网
```

### 15.18 2026-05-28 色温 DP 24 发现记录

```text
测试目标：
  排查 App 修改色温后灯光不变化的问题
用户反馈串口关键日志：
  Receive DP id=1
  Switch DP=on
  Apply light: power=on, bright=88%, cct=14% warm, WW=12%, CW=76%
  Receive DP id=24
  Unhandled DP id=24
  Receive DP id=1
  Switch DP=off
当前判断：
  App/云端已经把色温命令下发到设备
  当前产品的色温 DP 实际为 id=24
  固件当前只把 23 和 3 作为色温 DP 处理，因此 DP 24 被打印为 Unhandled
  “发送修改色温命令灯没变化”的直接原因是固件未处理 DP 24
关于 14%：
  日志中的 cct=14% warm 是固件当前内部色温比例，不是总亮度下限
  当前总亮度 bright=88%，色温 14% warm 会被拆成 WW=12%、CW=76%
  若 App 页面显示某个下限 14%，需要结合具体控件判断；从日志看，业务上更像当前色温比例或面板范围/校准值，而不是硬件故障
修复方案：
  将 DP id=24 加入色温 DP 处理列表
  重新编译烧录后，再移动色温滑条，应出现 CCT raw=... / Apply light... 日志
  若色温方向反了，则在代码中反转 colorTempPercent 映射
```

### 15.19 2026-05-28 固件补充 DP 24 处理记录

```text
测试目标：
  修复色温 DP 24 收到但不处理的问题，并增强 DP 调试日志
修改固件：
  firmware/t5_tuya_cct_app/t5_tuya_cct_app.ino
修改内容：
  新增 DPID_TEMP_ALT2 = 24
  isTempDp() 将 24 识别为色温 DP
  对未处理 DP 增加 type/value 调试输出
  对 DP 读取失败增加错误码和调试输出
编译结果：
  arduino-cli 编译通过
  FQBN：tuya_open:tuya_open:TUYA_T5AI_CORE
  目标：t5_tuya_cct_app.ino_QIO_1.0.0.bin
下一步：
  重新烧录该固件
  App 移动色温滑条时，串口应出现 Receive DP id=24 / CCT raw=... / Apply light...
  如果灯光冷暖方向与 App 相反，再修改固件做方向反转
  继续移动亮度范围/分路控件，观察 DP debug type/value 来识别 DP 7 和 15
```

### 15.20 2026-05-28 App 冷暖值固件 Review 与后续验证方案

```text
测试目标：
  围绕 PWM 精度、色温混合效果、3.3V 电源稳定性与电压兼容性，推进 Phase 1 后续实施
当前固件 Review：
  1. DP 处理：
     开关兼容 20 / 1
     亮度兼容 22 / 2
     色温/冷暖兼容 23 / 3 / 24
     当前日志已证明本产品冷暖值实际下发 DP 24
  2. 亮度/色温换算：
     dpValueToPercent() 支持 0-100 和 0-1000 两类 App 数值
     当前内部只保留百分比，属于 Phase 1 验证可用，但不是高精度调光实现
  3. 混光算法：
     colorTempPercent = 0 表示纯冷
     colorTempPercent = 100 表示纯暖
     WW = brightness * colorTempPercent / 100
     CW = brightness - WW
     该算法保证 WW + CW 约等于总亮度，但不保证不同色温下的视觉亮度完全一致
  4. PWM 输出：
     当前使用 T5 硬件 PWM
     频率 200Hz
     duty 标尺 0-10000
     目前按 1% 粒度输出，即 0/100/200/.../10000
  5. 电气边界：
     T5 输出 3.3V 控制信号
     当前 MOSFET 模块输入规格为 3-5V，最低驱动电流 2mA
     理论兼容，但 3.3V 处在输入范围下沿，需要实测 P24/P25 高电平、电源跌落和 MOSFET 输入响应
当前风险：
  1. App 冷暖值方向可能与固件定义相反，需要用实际灯光确认
  2. 1% 粒度会丢失 App 0-1000 原始值的细节，后续若做平滑调光，需要改成 0-1000 内部标尺
  3. 中间色温视觉亮度可能偏低或偏高，需要建立 WW/CW 校准系数
  4. 200Hz 会有可见闪烁风险，但受当前 MOSFET 模块 500Hz 限制，Phase 1 只做原型验证
  5. DP 7 / 15 可能是亮度范围、通道亮度或面板配置，暂未纳入业务逻辑
后续验证顺序：
  Step A：App 冷暖值 DP 验证
    移动冷暖值滑条到最冷、中间、最暖
    记录 CCT raw、warm percent、WW duty、CW duty
    观察是否对应纯冷/中性/纯暖
    若方向反了，修改固件为 100 - dpValueToPercent(value)
  Step B：PWM 精度验证
    用逻辑分析仪测 P24/P25
    测试 App 亮度 1%、10%、50%、100%
    每个亮度再测冷暖值 0%、50%、100%
    记录频率是否约 200Hz，占空比是否接近串口打印的 WW/CW duty
  Step C：色温混合效果验证
    灯带接 12V 和 MOSFET
    测纯冷、50/50、纯暖三档
    确认红线公共正极、黄线 WW、白线 CW 的方向是否和 App 一致
    若 50/50 视觉亮度不自然，后续增加 WW/CW 校准系数
  Step D：3.3V 稳定性验证
    T5 USB 供电，灯带 12V 供电
    万用表测 T5 3V3 对 GND
    分别在灯灭、100% 冷、100% 暖、50/50 全亮时记录 3V3
    目标：3V3 不应明显跌落，T5 不应重启
  Step E：电压兼容性验证
    用万用表或逻辑分析仪测 P24/P25 高电平
    目标高电平接近 3.3V
    MOSFET 输入端在 3.3V PWM 下应能稳定开关
    MOSFET 模块无异常发热
通过标准：
  App 开关能控制灯灭/亮
  App 亮度变化能在串口、PWM 占空比、实物亮度三层对应
  App 冷暖值变化能在串口、PWM 占空比、实物冷暖三层对应
  3V3 稳定，T5 不重启
  P24/P25 高电平满足 MOSFET 3-5V 输入要求
```

### 15.21 2026-05-28 灯带出口电压异常与暖白不亮记录

```text
测试目标：
  排查色温纯暖时暖白不亮、纯冷时测得电压异常的问题
用户反馈：
  App 亮度 100%，冷暖值 1000：
    在灯带接口另一端铜线测量，暖白与红线相关读数跳动约 3.8V-4.1V，但灯不亮
    冷白相关读数约 0.3V
  App 亮度 100%，冷暖值 0：
    暖白相关读数约 0.9V
    冷白相关读数约 7.7V
  现象：
    冷暖值 0 时只亮冷白
    冷暖值 500 时冷白变弱，暖白不亮
    冷暖值 1000 时全部不亮
当前判断：
  固件层 DP 24 已正常解析，串口 duty 计算正确
  实物问题集中在暖白通道没有形成有效电流回路
  灯带端测到 3.8V-4.1V 或 7.7V 不能直接作为通道正常依据，因为当前是低端 MOSFET 开关，关断或接触不良时负极线可能悬空，万用表会读到不稳定/中间电压
  纯暖时 WW=100%、CW=0%，若全灭，说明暖白回路没有被拉通到 GND
  纯冷时 CW=100% 能亮，说明公共正极、12V 电源、冷白通道至少部分有效
可能原因：
  1. 暖白黄线未真正接到 MOSFET 输出低端，或铜丝/夹子/螺丝端子接触不可靠
  2. P24 到 MOSFET 暖白通道 PWM/IN 的控制线未接通、接错通道、或没有共地
  3. MOSFET 暖白通道损坏或该通道输入/输出端子接法与冷白通道不一致
  4. 灯带接口另一端测量方式没有固定公共参考点，导致读到悬空电压或 PWM 平均电压
  5. P24 硬件 PWM 输出没有实际到达 MOSFET 输入，需要用逻辑分析仪或万用表对 GND 复核
踩坑记录：
  不要只在灯带出口铜线上互相测电压判断通道好坏
  低端 MOSFET 调光时，正确排查要围绕“公共正极是否恒为 12V”和“各负极是否被 MOSFET 拉到 GND”
  万用表测 PWM 会显示平均值，200Hz PWM 下读数可能随 duty 跳动，不等于真实高低电平波形
下一步排查：
  1. 断开 T5 P24/P25 控制线，先用手动方式验证灯带：
     红线接 12V+
     黄线短接电源负极应亮暖白
     白线短接电源负极应亮冷白
  2. 若黄线手动接负极不亮，问题在灯带/接头/黄线
  3. 若黄线手动接负极能亮，把 MOSFET 冷白成功通道和暖白通道互换：
     用 P25 + 当前冷白成功的 MOSFET 通道去驱动黄线
     若黄线亮，说明灯带暖白正常，问题在 P24 或原暖白 MOSFET 通道
     若黄线仍不亮，问题在黄线接触或灯带暖白支路
  4. App 纯暖时测 P24 对 T5 GND / MOSFET GND：
     预期高电平约 3.3V 或逻辑分析仪显示约 100% duty
  5. App 纯暖时测 MOSFET 暖白 IN 对 GND：
     若 P24 有信号但 IN 没信号，控制线/夹子/排针接触问题
  6. App 纯暖时测 MOSFET 暖白输出负端对电源负极：
     导通时应接近 0V
     若仍悬空或高电压，MOSFET 通道未导通或输出接法错误
```

### 15.22 2026-05-28 PWM 放大链路疑似问题记录

```text
测试目标：
  根据用户复测结果，继续定位暖白通道不亮是否由 PWM 放大/MOSFET 驱动链路导致
用户反馈：
  灯带直连测试正常，暖白/冷白支路本身没有问题
  MOSFET 模块的 IN 和 OUT 指示灯均有反应
  用户当前判断问题集中在 PWM 放大链路
当前判断：
  灯带本体可以暂时排除
  App/DP/固件 duty 计算可以暂时排除，因为串口已经输出 WW=100% / CW=0% 等正确状态
  问题集中在 T5 PWM 输出到 MOSFET 输入、MOSFET 通道导通能力、输出接线/负载回路
重要踩坑：
  MOSFET 模块 IN/OUT 指示灯亮，不等于该通道已经能给灯带提供足够负载电流
  指示灯通常只需要很小电流，灯带需要更大的实际电流
  因此“指示灯亮但灯不亮”仍可能是 MOSFET 未完全导通、通道损坏、接触电阻过大、或输出回路接错
下一步锁定方法：
  1. 交换 T5 控制线：
     保持 MOSFET 输出和灯带线不变，只交换 P24/P25 到 MOSFET IN 的控制线
     如果问题跟着 P24 走，重点查 P24 硬件 PWM/3.3V 输出
     如果问题留在原暖白 MOSFET 通道，重点查该 MOSFET 通道或输出接线
  2. 交换 MOSFET 输出通道：
     用当前已能点亮冷白的 MOSFET 通道去驱动黄线 WW
     如果黄线能亮，说明暖白灯带支路正常，原暖白 MOSFET 通道异常
     如果黄线仍不亮，说明黄线接触/输出接法仍需复查
  3. 用逻辑分析仪测 P24/P25：
     App 纯暖：P24 应为 100% duty，P25 应为 0% duty
     App 纯冷：P24 应为 0% duty，P25 应为 100% duty
     App 中间：两路应接近 50% duty
  4. 用万用表测 3.3V 与控制高电平：
     T5 3V3 对 GND 应稳定
     P24/P25 高电平应接近 3.3V
     MOSFET IN 端应能收到同样的高电平
阶段假设：
  若 P25 能稳定驱动冷白，而 P24 无法驱动暖白，优先怀疑 P24 硬件 PWM 输出/接线或对应 MOSFET 通道，而不是 App 色温逻辑
  若两个 T5 控制脚都能驱动同一个 MOSFET 通道，则 MOSFET 模块整体 3.3V 兼容性基本成立
```

### 15.23 2026-05-28 串口模拟指令与本地链路隔离方案

```text
测试目标：
  通过串口命令绕过 App/云端/DP，下发本地 PWM 测试指令，判断问题在云端链路还是本地 PWM/MOSFET/灯带链路
用户思路：
  先用串口模拟发指令
  再查云文档/云 API
  区分云端问题和本地链路问题
当前判断：
  该思路正确，但排查优先级应为：
    1. 串口本地直驱 PWM
    2. App DP 下发
    3. 云 API / 云文档
  因为当前日志已经证明 App/云端能下发 DP 1/2/24，云端不是第一嫌疑
固件修改：
  在 firmware/t5_tuya_cct_app/t5_tuya_cct_app.ino 中新增串口命令入口
  编译结果：BUILD SUCCESS
串口命令：
  help             显示命令说明
  state            打印当前 power/brightness/cct/WW/CW
  on               本地开灯
  off              本地关灯
  b <0-1000>       本地设置亮度原始值
  c <0-1000>       本地设置冷暖原始值，0=冷，1000=暖
  set <p> <b> <c>  本地设置 power、亮度原始值、冷暖原始值
  ww <0-100>       直接驱动 P24 暖白 PWM，占空比百分比，P25=0
  cw <0-100>       直接驱动 P25 冷白 PWM，占空比百分比，P24=0
  mix <ww> <cw>    直接设置 P24/P25 两路占空比
推荐测试：
  1. 不操作 App，只打开串口监视器
  2. 发送 ww 100
     预期：P24=100%、P25=0%，暖白应亮
  3. 发送 cw 100
     预期：P24=0%、P25=100%，冷白应亮
  4. 发送 mix 50 50
     预期：暖白/冷白同时半亮
  5. 发送 set 1 1000 1000
     预期：按混光算法输出纯暖
  6. 发送 set 1 1000 0
     预期：按混光算法输出纯冷
判断规则：
  如果 ww 100 仍不能点亮暖白，则问题不在 App/云/DP，而在 P24、MOSFET 暖白通道、输出接线或 3.3V 驱动能力
  如果 ww 100 能点亮暖白，而 App 纯暖不能点亮，则问题回到 DP 映射/业务逻辑
  如果 cw 100 能亮但 ww 100 不能亮，优先查 P24 到 MOSFET 暖白通道链路
  如果 mix 50 50 两路都能亮，再进入 PWM 精度和色温混合效果验证
```

### 15.24 2026-05-28 P24 到暖白本地链路锁定记录

```text
测试目标：
  在已排除 App/云/DP 后，排查 P24 -> MOSFET 暖白通道 -> 黄线 的本地链路
用户反馈：
  当前可以确定问题集中在 P24 -> MOSFET 暖白通道 -> 黄线
  现象与此前一致：冷白通道可工作，暖白通道不亮或无法被有效驱动
当前判断：
  不再优先排查 Tuya App、云 API、DP 24 或混光算法
  后续重点是把本地链路拆成三段：
    A. P24 控制信号是否正确输出
    B. MOSFET 暖白通道是否被 3.3V 输入有效驱动
    C. MOSFET 输出到黄线是否形成负载电流回路
最短排查顺序：
  1. 使用串口命令 ww 100：
     预期：P24=100%、P25=0%，只亮暖白
     若不亮，继续步骤 2
  2. 测 P24 对 T5 GND：
     万用表应接近 3.3V；逻辑分析仪应接近 100% duty
     若 P24 无高电平，问题在 T5 P24/固件/针脚接触
  3. 测 MOSFET 暖白 IN 对 MOSFET GND：
     应与 P24 相同，接近 3.3V 或 100% duty
     若 P24 有、IN 没有，问题在杜邦线/夹子/排针接触
  4. 把 P24 控制线插到冷白已验证成功的 MOSFET IN 通道：
     若该通道能被 P24 点亮负载，说明 P24 没问题，原暖白 MOSFET 通道/输出接线有问题
     若仍不能，说明 P24 输出或 P24 线材/接触问题
  5. 把 P25 控制线插到原暖白 MOSFET IN 通道：
     若暖白通道能工作，说明 MOSFET 暖白通道没问题，P24 侧问题更大
     若仍不能，说明原暖白 MOSFET 通道或输出接线问题更大
  6. 用冷白已验证成功的 MOSFET 输出通道接黄线：
     若黄线亮，说明黄线/暖白灯带正常，原暖白 MOSFET 输出通道异常
     若黄线不亮，说明黄线接触、端子压接或灯带暖白回路仍需复查
测量注意：
  所有低压控制测量都以 T5 GND/MOSFET GND 为黑表笔参考
  所有灯带负载测量都以 12V 电源负极为黑表笔参考
  不要只看 MOSFET 指示灯；指示灯亮不等于能带动灯带负载
  不要用灯带端悬空线之间的电压判断通道是否导通
下一步记录项：
  ww 100 时 P24 对 GND 电压/占空比
  ww 100 时 MOSFET 暖白 IN 对 GND 电压/占空比
  P24 控制冷白成功通道是否能亮
  P25 控制原暖白通道是否能亮
  冷白成功输出通道接黄线是否能亮
```

### 15.25 2026-05-28 P24/暖白通道交换法实测记录

```text
测试目标：
  用交换法判断问题在 P24、MOSFET 原暖白通道，还是 3.3V 驱动能力
用户反馈：
  黑表笔接 T5 GND，红表笔接 P24，测得约 2.983V
  测 OUT1- 和 DC+，读数约 4.6V-4.7V
  在 power=on、bright=100%、cct=100% warm、WW=100%、CW=0% 状态下：
    将 P24 插到原来能点亮冷白的 MOSFET IN 通道，结果能点亮冷白
    但冷白不是全亮，测得约 7.6V
    将 P25 接到原暖白 MOSFET IN 通道，P25 也不能点亮
当前判断：
  P24 并非完全无输出，因为 P24 能驱动原冷白成功通道点亮
  原暖白 MOSFET 通道或其输出接线高度可疑，因为 P25 接到该通道也不能工作
  P24 高电平 2.983V 接近但略低于 MOSFET 模块标称 3V 输入下限，属于驱动余量不足/边缘状态
  P24 驱动冷白成功通道时仍不是全亮，说明即使能导通，也可能存在 3.3V GPIO 驱动不足、MOSFET 输入负载过重、或模块无法被 3.0V 左右完全驱动的问题
  OUT1- 与 DC+ 只有 4.6V-4.7V，不符合“满导通时灯带应获得接近电源电压”的预期，说明该输出通道未形成有效满导通回路，或测量点/通道对应关系仍需复核
重要结论：
  当前第一嫌疑：原暖白 MOSFET 通道/输出接线/端子
  当前第二嫌疑：该 MOSFET 模块对 T5 约 3.0V 控制高电平的兼容性不足
  App、云、DP 24、混光算法不再作为当前主要嫌疑
下一步排查：
  1. 先测 12V 电源本身：
     在灯亮/不亮两种情况下测 DC+ 对 DC-
     若电源本身不是约 12V，先处理电源压降
  2. 测 P24 空载高电平：
     断开 P24 到 MOSFET IN 的线，串口发 ww 100
     测 P24 对 GND 是否回到约 3.3V
     若空载 3.3V、接入 MOSFET 后降到 2.98V，说明 MOSFET 输入对 T5 GPIO 有负载压力
  3. 手动用 3V3/5V 测 MOSFET 原暖白通道：
     断开 P24/P25 到该 IN 的线
     只把 MOSFET 原暖白 IN 接到 3V3，GND 共地
     若不亮，再短时接到 5V 控制电平
     注意：5V 只能接 MOSFET IN，绝不能接回 T5 GPIO
  4. 判断：
     若 5V IN 也无法点亮原暖白通道，说明该 MOSFET 通道/输出接线基本有问题
     若 5V IN 能全亮但 P24/P25 不能，说明 T5 3.3V GPIO 驱动该模块不充分，需要电平转换/驱动级或更换 3.3V 兼容 MOSFET 模块
     若 3V3 IN 能全亮，说明 T5 P24/P25 接线或输出电压跌落是问题
  5. 临时推进方案：
     使用原冷白成功通道或模块其他未用通道驱动黄线 WW
     若可行，Phase 1 先绕开坏/弱通道继续验证色温混光
```

## 16. 支线：涂鸦 WiFi+BT 5-in-1 成品灯带控制器测试

> 日期：2026-05-26  
> 目的：先独立验证成品灯带控制器的使用方式，不接 T5AI-Core，不接 YYNMOS/NMOS 模块。

### 16.1 当前观察

用户反馈：

```text
控制器不接负载时，Pilot lamp 不常亮
反复插拔电源时，Pilot lamp 偶尔蓝灯闪一下后熄灭
控制器标称输入：DC12-24V
实测当前供电：约 18.5V
```

当前判断：

- 蓝灯闪一下不等于控制器烧坏，可能是上电瞬间、模式状态或配网状态提示。
- 不接负载时指示灯不常亮，不一定异常。
- 该控制器不是 Buck 降压器，输出电压基本等于输入电压。
- 18.5V 不适合接 12V 灯带；可以用于 24V 灯带低亮度验证。

### 16.2 只接 24V CCT/双色温灯带

若 24V 灯带是三线双色温/CCT，常见线序为：

```text
V+  -> 公共正极
WW  -> 暖白负极通道
CW  -> 冷白负极通道
```

接控制器输出端：

```text
灯带 V+  -> 控制器 V+
灯带 WW  -> 控制器 WW
灯带 CW  -> 控制器 CW
```

如果灯带线颜色是红/黄/白，通常先按：

```text
红线 -> V+
黄线 -> WW
白线 -> CW
```

黄线和白线接反一般只会导致冷暖方向反了，后续可以互换；红线不能接错。

### 16.3 上电前检查

1. 控制器输入端只接当前 `18.5V` 电源，不接 T5。
2. 输出端只接 24V 灯带，不接 12V 灯带。
3. 灯带红线或 `+24V/V+` 必须接控制器 `V+`。
4. 另外两根线只接 `WW/CW`，不要接到 `R/G/B`。
5. 线头不能有散铜丝互相碰到。

### 16.4 输出模式设置

该控制器是 5-in-1，需要切到 CCT/Dual White 模式。

操作：

```text
短按 SET -> 切换输出模式
看到 Pilot lamp 黄灯闪烁 -> CCT/Dual White 模式
```

不要长按 SET，长按通常用于配网/重置。

### 16.5 通过标准

```text
24V 灯带能亮
短按 SET 后可切到 CCT/Dual White 模式
手机 App 或控制器状态能控制开关/亮度/色温
控制器无异味、无异常发热
电源无异常叫声、闪烁、过热
```

如果灯不亮：

```text
先断电
确认 V+ 是否接到灯带正极
确认是否在 CCT 模式
确认控制器是否处于 App 关闭状态
确认 18.5V 供电是否仍稳定
```

### 16.6 异常记录：接负载后仍无输出

用户反馈：

```text
换接 24V 灯带后，控制器接上负载仍无明显响应
怀疑 5-in-1 成品控制器已损坏
```

当前处理：

- 暂停反复插拔电源。
- 不再接 12V 灯带。
- 先做控制器输入与输出的万用表验证。

排查顺序：

```text
1. 只测电源空载输出，确认仍为约 18.5V
2. 电源接入控制器 INPUT 后，测控制器 INPUT + / - 是否仍为约 18.5V
3. 控制器接电但不接灯带，测 OUTPUT V+ 与 WW/CW/R/G/B 之间是否有电压变化
4. 短按 SET 切模式，每次切换后再测输出
5. 若 INPUT 正常但所有 OUTPUT 通道始终无响应，控制器损坏或处于不可用状态的概率较高
```

## 17. 支线：NiceMCU WB3S V1 最小 setup 测试

> 日期：2026-05-26  
> 目的：把 NiceMCU WB3S V1 当作便宜备板/对照实验板玩一玩；不改变 T5AI-Core 主路径。

### 17.1 当前原则

第一轮只做：

```text
USB 上电
电脑识别串口
测 3.3V
查看启动日志
确认板子没有异常发热
```

第一轮不做：

```text
不接 12V
不接 MOSFET
不接 LED 灯带
不刷 OpenBeken / LibreTiny / 其他第三方固件
不把 T5 的程序直接套到 WB3S
```

### 17.2 准备物料

| 物料 | 用途 |
| --- | --- |
| NiceMCU WB3S V1 | 被测备板 |
| USB 数据线 | 供电与串口 |
| 电脑 | 识别串口、看日志 |
| 万用表 | 测 `5V`、`3V3`、`GND` |

### 17.3 上电前检查

1. 桌面不要放金属物。
2. 只接 USB，不接外部电源。
3. 看板上是否有 `5V`、`3V3`、`GND` 丝印。
4. 看是否有 `BOOT`、`RST`、`EN` 或类似按键。
5. 看 USB 口附近是否有 CH340/USB 串口芯片。

### 17.4 电脑识别串口

插板前先查：

```bash
ls /dev/cu.*
```

插上 NiceMCU WB3S 后再查一次：

```bash
ls /dev/cu.*
```

通过标准：

```text
多出一个 /dev/cu.usbserial*、/dev/cu.wchusbserial*、/dev/cu.usbmodem* 等串口
```

如果没有新增串口：

- 换 USB 数据线。
- 换 USB 口。
- 检查 macOS 是否需要 CH340/WCH 驱动。
- 看板上电源灯是否亮。

### 17.5 万用表测电压

USB 供电后测：

```text
黑表笔 -> GND
红表笔 -> 5V
红表笔 -> 3V3
```

通过标准：

| 测点 | 正常范围 |
| --- | --- |
| `5V` 对 `GND` | 约 4.75V-5.25V |
| `3V3` 对 `GND` | 约 3.20V-3.40V |

如果 `3V3` 不正常，先停止，不继续串口/刷机。

### 17.6 查看启动日志

用 Arduino Serial Monitor、串口助手或 TuyaOpen Tools 的 custom 串口模式都可以。

先试：

```text
波特率：115200
数据位：8
停止位：1
校验：无
```

操作：

1. 选择新增串口。
2. 打开串口监视器。
3. 按一下板上的 `RST` 或重新插拔 USB。
4. 看是否有启动日志。

如果 115200 没日志或乱码，再试：

```text
9600
74880
921600
1500000
```

当前只要求看到“有启动输出”，不要求理解所有日志。

### 17.7 是否刷机

先不刷。

NiceMCU WB3S V1 可能是第三方 BK7238/BK 系列开发板，不等同于 T5AI-Core。后续如果只是玩，可以再评估：

- OpenBeken：偏智能家居/本地控制玩法。
- LibreTiny：偏 Arduino/PlatformIO 风格开发。

但刷第三方固件会覆盖原厂/卖家固件，且可能需要进入 boot/download 模式。等确认串口、电源、芯片型号后再决定。

### 17.8 记录模板

```text
日期：
板子丝印/型号：
插 USB 后新增串口：
5V 实测：
3V3 实测：
115200 是否有日志：
按 RST 是否有日志变化：
板子是否发热：
结论：通过 / 未通过
```

### 17.9 2026-05-26 NiceMCU 初次日志记录

```text
现象：
  Arduino Serial Monitor 能收到数据，但全部乱码
板上按键：
  无 RST
  有 RF 和 EN
当前判断：
  串口链路已有数据输出
  乱码优先按波特率不匹配处理
  EN 大概率可作为复位/使能按键使用
  RF 暂不长按，避免触发配网/测试/擦除类动作
下一步：
  用不同波特率逐个测试
  每次切换波特率后短按 EN 或重新插拔 USB
```

推荐波特率测试顺序：

```text
115200
9600
74880
38400
57600
921600
1500000
```

操作方式：

1. 关闭 Serial Monitor。
2. 改一个波特率。
3. 打开 Serial Monitor。
4. 短按 `EN`，或者拔插 USB。
5. 看日志是否变成可读英文/数字。

如果所有波特率都乱码：

- 确认串口监视器设置为 `No line ending` 或 `Both NL & CR` 均可尝试，但这通常不影响乱码。
- 确认不是同时被其他串口工具占用。
- 暂不刷机，先拍板子正反面确认芯片、串口芯片和丝印。

### 17.10 2026-05-26 NiceMCU 可读启动日志

```text
操作：
  短按 EN
结果：
  串口出现可读日志
关键日志：
  BK7238_1.0.14
  [I/FAL] Fal(V0.4.0)success
  [I/OTA] RT-Thread OTA package(V0.2.8-beken-1133282d-20220604) initialize success.
  go os_addr(0x10000)
  www.sparkleiot.com
  ready
当前判断：
  NiceMCU WB3S V1 上电正常
  串口通信正常
  EN 可作为复位/重启触发使用
  芯片/固件日志显示 BK7238 + RT-Thread/OTA 出厂固件
  当前无需刷机
```

下一步最小测试：

```text
1. 测 5V/GND 和 3V3/GND
2. 观察板子 1-2 分钟是否明显发热
3. 搜索手机 Wi-Fi/蓝牙列表，看是否出现 Sparkleiot/NiceMCU/BK/Tuya 类设备
4. 不接任何外部负载
```

### 17.11 查看 NiceMCU Wi-Fi/BLE 名称

目标：先尝试通过串口查询当前 Wi-Fi/BLE 信息，不刷机、不改配置。

当前依据：

- 板子日志显示 `BK7238_1.0.14`、`RT-Thread OTA`、`www.sparkleiot.com`、`ready`。
- 同类 NiceMCU XH-WB3S/BK7238 出厂固件可能带 AT 命令入口。
- BK7238 AT 指令通常使用 `AT+命令?` 作为查询命令，`AT+命令=...` 才是设置命令。

操作设置：

```text
串口：NiceMCU 新增串口
波特率：能看到 ready 的那个波特率，当前为 115200
Line ending：Both NL & CR 或 Newline + Carriage Return
```

先发最小命令：

```text
AT
```

若返回 `OK` 或类似响应，再依次尝试只读查询：

```text
AT+GMR
AT+CWMODE?
AT+CIPSTAMAC?
AT+CWSAP?
```

只读原则：

- 可以发送 `AT`。
- 可以发送以 `?` 结尾的查询命令。
- 暂不发送带 `=` 的设置命令，例如 `AT+CWSAP=...`。
- 暂不发送擦除、恢复出厂、配网、OTA、重启类命令。

如果 `AT+CIPSTAMAC?` 返回 MAC 地址，可以用 MAC 地址后 3 字节去手机 Wi-Fi/BLE 扫描列表里匹配设备。

如果 `AT+CWSAP?` 返回 SSID，即可知道板子 SoftAP 名称；如果无返回，说明当前固件可能未开启 SoftAP 或 AT 服务不完整。

### 17.12 2026-05-26 NiceMCU AT 查询结果

```text
AT:
  OK
AT+GMR:
  <ATVER:1.1>
  <SDKVER:3.0.70>
  OK
AT+CWMODE?:
  +WLMODE:1,1
  OK
AT+CIPSTAMAC?:
  +WLMAC:c8:47:8c:01:12:1f
  OK
AT+CWSAP?:
  ERROR
```

当前判断：

- NiceMCU WB3S V1 的出厂固件开放 AT 命令入口。
- 固件版本：`ATVER 1.1`，`SDKVER 3.0.70`。
- Wi-Fi MAC：`c8:47:8c:01:12:1f`。
- 手机 Wi-Fi/BLE 扫描时，可优先查找 MAC 后缀接近 `01:12:1f` 的设备。
- `AT+CWSAP?` 是常见 ESP-AT 风格命令，但该固件返回 `ERROR`，说明当前 Sparkleiot/BK7238 命令集不完全兼容 ESP-AT。

如何学习/查 AT 命令：

1. 先从启动日志提取关键词，例如：

```text
BK7238
Sparkleiot
ATVER
SDKVER
www.sparkleiot.com
```

2. 搜索资料时组合关键词：

```text
BK7238 AT command
Sparkleiot AT command
XH-WB3S AT command
BK7238 ATVER SDKVER
```

3. AT 命令通常有三类：

```text
AT              测试串口是否通
AT+XXX?         查询当前配置
AT+XXX=?        查询命令参数范围
AT+XXX=...      设置配置
```

4. 小白调试原则：

```text
优先发 AT 和带 ? 的查询命令
谨慎发带 = 的设置命令
暂不发 reset/restore/erase/ota/update 类命令
```

5. 不能假设所有 AT 固件都兼容 ESP-AT。`AT+CWSAP?` 报错就是例子：命令风格相似，但厂商命令集可能不同。

### 17.13 2026-05-26 NiceMCU Wi-Fi 扫描与入网记录

```text
AT+CWLAP:
  可扫描周围 Wi-Fi AP，返回 RSSI 与 BSSID/MAC 列表
AT+WSCAN:
  输出格式与 AT+CWLAP 基本一致，当前判断为扫描 Wi-Fi 的等效命令或别名
AT+CWJAP="SSID","password":
  返回 OK
  随后出现 WIFI CONNECTED
  随后出现 WIFI GOT IP
AT+CIFSR:
  +CIFSR:STAIP,"192.168.1.47"
  +CIFSR:STAMAC,"c8:47:8c:01:12:1f"
  OK
```

当前结论：

- NiceMCU WB3S V1 出厂 AT 固件可扫描 Wi-Fi。
- 可连接本地 Wi-Fi 并获取局域网 IP。
- 当前设备 IP 为 `192.168.1.47`。
- 当前设备 Wi-Fi MAC 为 `c8:47:8c:01:12:1f`。
- `AT+CWLAP` 与 `AT+WSCAN` 在当前固件里表现为同类扫描命令。

注意：

- 若 Wi-Fi 密码曾被复制到外部聊天或文档，建议后续更换测试 Wi-Fi 密码。
- 不再继续公开记录真实 Wi-Fi 密码。
