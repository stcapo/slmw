Phase 2 目标：
一套低压 DC 智能灯控中间层 PCB，优先覆盖 12V/24V 恒压灯带，支持 Wi-Fi/Zigbee 两个 BOM 版本。

主控：
Wi-Fi：T3-3S WB3S

核心电路：
12/24V 输入保护
3.3V Buck
2-4 路低边 MOSFET
MOSFET gate 驱动/缓冲
配网按键
状态 LED
UART/烧录/LA 测试点

它们是“同类候选”，但不是完全并列、也不是随便互换。两者尺寸一样，都是 16mm × 24mm × 2.8mm 的 3S 类邮票孔模组，但芯片、引脚语义、PWM能力、内存、WiFi/BLE代际都不同。

项目	WB3S	T3-3S	对我们影响
芯片	BK7231T	Tuya T3	T3 更新
主频	120MHz	320MHz	T3 余量大
Flash	2MB	4MB	T3 更适合 TuyaOS/OTA
SRAM	256KB	640KB	T3 更稳
WiFi	802.11 b/g/n	802.11 b/g/n/ax	T3 支持 WiFi 6
蓝牙	BLE 4.2	BLE 5.4	T3 配网/兼容性更好
PWM	多个 PWM 引脚	明确 5 路 32-bit PWM，规格书写明适合高品质 LED 控制	T3 更贴合我们的核心问题
工作电压	3.0-3.6V	2.0-3.6V	T3 电源容忍度更高，但仍按 3.3V 设计
IO驱动	典型 6mA，最大 15mA	典型 6mA，最大 20mA	T3 稍强，但仍建议加 MOSFET 驱动级
天线增益	1.5dBi	2.02dBi	T3 略好
工作温度	-40 到 85℃	-40 到 85℃	打平
尺寸	16×24×2.8mm	16×24×2.8mm	打平
硬性要求判断：

如果只是“能联网、能控制灯”，WB3S 也能做。
但结合我们的真实需求：硬件 PWM、解决频闪/亮度异常、Phase 2 一体化 PCB、后续量产、保留涂鸦生态，T3-3S 明显更合适。

最关键证据是 T3-3S 规格书明确写了：

5×PWM
多达五路的 32 位 PWM 输出
非常适合高品质的 LED 控制
这正好对应我们当前痛点：软件 PWM 不稳定 + MOSFET 驱动异常 + 色温/亮度不准。

但注意一个坑：T3-3S 和 WB3S 不是完全 pin-to-pin 兼容。

例如：

WB3S Pin 1 = CEN，T3-3S Pin 1 = NC
WB3S Pin 3 = EN，T3-3S Pin 3 = CEN
WB3S Pin 10 = PWM1，T3-3S Pin 10 = 普通 GPIO P19，不是硬件 PWM
所以 Phase 2 不要说“WB3S/T3-3S 完全兼容封装”。更准确是：

机械尺寸相同，可以做同一类封装思路，但电气引脚要按 T3-3S 主设计；如要兼容 WB3S，需要跳线/0Ω电阻/不同 BOM。

我建议 Phase 2 这样定：

WiFi 主线：T3-3S
PWM 输出：优先用 Pin 5 / Pin 6
T3-3S 上对应：P24 / P32，均支持硬件 PWM
用于：WW / CW 两路 MOSFET 驱动
不要再沿用 Phase 1 的 P24/P25 思维。那是 T5AI-Core 开发板阶段的引脚名；到了 T3-3S，重新定义：

DP 1 = 开关
DP 2 = 亮度
DP 24 = 色温
PWM_WW = T3-3S P24
PWM_CW = T3-3S P32
最终一句话：

T3-3S 是新设计主线；WB3S 只作为老方案兼容/备选，不建议作为我们 Phase 2 首版 PCB 主控。
