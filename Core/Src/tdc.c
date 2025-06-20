#include "main.h"

/**
 * @brief 飞秒系数，用于计算最终时间
 */
#define Fe9 (4e6/2/1e9)

/**
 * @brief 类型定义简写，提高代码可读性
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

/**
 * @brief 逻辑电平定义
 */
#define OFF 0
#define ON  1

/**
 * @brief GPIO控制宏定义
 */
#define SSN(x) HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, x)
#define SCK(x) HAL_GPIO_WritePin(TDC_SCK_GPIO_Port, TDC_SCK_Pin, x)
#define SI(x)  HAL_GPIO_WritePin(TDC_SI_GPIO_Port, TDC_SI_Pin, x)
#define RTN(x) HAL_GPIO_WritePin(TDC_RTN_GPIO_Port, TDC_RTN_Pin, x)

/**
 * @brief GPIO读取宏定义
 */
#define SO()   HAL_GPIO_ReadPin(TDC_SO_GPIO_Port, TDC_SO_Pin)
#define INT()  HAL_GPIO_ReadPin(TDC_INT_GPIO_Port, TDC_INT_Pin)

/**
 * @brief 简单延时函数
 * @param t 延时参数，影响延时时间
 */
void delay(uint8_t t) {
    volatile uint32_t delay = t * 500;
    while (delay > 0) {
        delay--;
    }
}

/**
 * @brief 发送1位
 * @note 生成时钟上升沿并发送高电平
 */
void send1(void) {
    SCK(1);
    delay(1);
    SI(1);
    delay(1);
    SCK(0);
    delay(1);
    //delay(1);
}

/**
 * @brief 发送0位
 * @note 生成时钟上升沿并发送低电平
 */
void send0(void) {
    SCK(1);
    delay(1);
    SI(0);
    delay(1);
    SCK(0);
    delay(1);
    //delay(1);
}

/**
 * @brief 写入8位数据
 * @param wbuf8 待写入的8位数据
 */
void write8(uint8_t wbuf8) {
    uint8_t cnt;
    uint8_t MSB8 = 0x80;
    
    SSN(0);
    for (cnt = 8; cnt > 0; cnt--) {
        if ((wbuf8 & MSB8) > 0)
            send1();
        else
            send0();
        MSB8 /= 2;
    }
    delay(1);  // 这里没有SPI关闭命令，在测试程序中代码关闭
}

/**
 * @brief 写入32位数据
 * @param wbuf32 待写入的32位数据
 */
void write32(uint32_t wbuf32) {
    uint8_t cnt;
    uint32_t MSB32 = 0x80000000;
    
    SSN(0);
    delay(1);
    for (cnt = 32; cnt > 0; cnt--) {
        if ((wbuf32 & MSB32) > 0)
            send1();
        else
            send0();
        MSB32 /= 2;
    }
    delay(1);
    SSN(1);
}

/**
 * @brief 读取8位数据
 * @return 读取到的8位数据
 */
uint8_t read8(void) {
    uint8_t cnt;
    uint8_t LSB8 = 0x80;
    uint8_t rbuf8 = 0x00;
    
    SSN(0);
    delay(3);
    for (cnt = 8; cnt > 0; cnt--) {
        //rbuf8 <<= 1;
        SCK(1);
        delay(3);

        if (SO())
            rbuf8 |= LSB8;
        LSB8 /= 2;
        delay(1);
        SCK(0);
        delay(10);
    }
    delay(3);
    SSN(1);
    return rbuf8;
}

/**
 * @brief 读取32位数据
 * @return 读取到的32位数据
 */
uint32_t read32(void) {
    uint8_t cnt;
    uint32_t LSB32 = 0x80000000;
    uint32_t rbuf32 = 0x00000000;
    
    SSN(0);
    delay(1);
    for (cnt = 32; cnt > 0; cnt--) {
        //rbuf32 <<=1;
        SCK(1);
        delay(3);
        if (SO())
            rbuf32 |= LSB32;
        LSB32 /= 2;
        delay(1);
        SCK(0);
        delay(3);
    }
    delay(3);
    SSN(1);
    return rbuf32;
}

/**
 * @brief 复位TDC芯片
 */
void reset(void) {
    RTN(1);
    delay(1);
    RTN(0);
    delay(5);
    RTN(1);
    delay(1);
}

/**
 * @brief 初始化TDC芯片
 * @note 配置TDC工作参数，包括测量范围、中断和时钟设置
 */
void TDC_Init() {
    reset();

    write8(0x50);         // power on reset;
    delay(1);
    SSN(1);
    
    //----------------------------------------------------------------------------
    // 测量范围1，用stop1的第一个脉冲减去START的脉冲
    // 注意：测量范围1中，从START开始到最后一个STOP信号的时间间隔不能超过1.8us，否则溢出。
    /**/
    write32(0x80009420);  // 测量范围1，校准陶瓷晶振时间为8个32K周期，244.14us 设置4M上电后一直起振，自动校准，上升沿敏感
    delay(1);
    write32(0x81010100);  // 测量范围1，STOP1-START

    //----------------------------------------------------------------------------
    // 测量范围1，用stop2的第一个脉冲减去START脉冲
    // 注意：测量范围1中，从START开始到最后一个STOP信号的时间间隔不能超过1.8us，否则溢出。
    /*
    SPI_WRITE32(0x80008420);  // 测量范围1，校准陶瓷晶振时间为8个32K周期，244.14us 设置4M上电后一直起振，自动校准，上升沿敏感
    delay(1);
    write32(0x81094800);      // 测量范围1，STOP2-START
    */

    //----------------------------------------------------------------------------
    // 测量范围1，MCU给了START信号后，用STOP2的第一个脉冲减去STOP1的第一个脉冲
    // 注意：测量范围1中，从START开始到最后一个STOP信号的时间间隔不能超过1.8us，否则溢出。
    /*
    write32(0x80008420);  // 测量范围1，校准陶瓷晶振时间为8个32K周期，244.14us 设置4M上电后一直起振，自动校准，上升沿敏感
    delay(1);
    write32(0x81194900);  // 测量范围1，1stSOTPCH2-1stSTOPCH1
    */
    
    //----------------------------------------------------------------------------
    // 测量范围2，用STOP1的第一个脉冲减去START的第二个脉冲
    /*
    write32(0x80009410);
    
    write32(0x80008468);  // 测量范围2中：自动校准，晶振上电后一直起振。
    // spi_write32(0x80008428); // 测量范围2中：自动校准，晶振上电后一直起振。
    delay(1);
    write32(0x81214200);  // 测量范围2，STOP1接收1个脉冲，定义计算方法，用STOP1的第一个脉冲减去START脉冲
    */

    delay(1);
    write32(0x82E00000);  // 开启所有中断源
    delay(1);
    write32(0x83080000);  // 溢出预划分器64us
    delay(1);
    write32(0x84200000);
    delay(1);
    write32(0x85080000);
    delay(1);
    write8(0x70);
    delay(1);
}

/**
 * @brief 获取TDC状态寄存器
 * @return 状态寄存器值
 */
uint32_t TDC_Get_Status_Reg()
{
    uint32_t reg;

    // while(INTN) //判断中断置位否
    //     delay_us(1);

    write8(0xb4);
    delay(1);
    reg = read32();

    return reg;
}

/**
 * @brief TDC测试函数
 * @return 测试寄存器值
 */
uint32_t TDC_Test() {
    uint32_t test_reg;

    SSN(0);
    write32(0x81884200);  // INTN的脉冲和这个有关。
    delay(1);
    write8(0xb5);
    delay(1);
    test_reg = read8();
    SCK(0);
    SSN(1);

    return test_reg;
}

/**
 * @brief 将定点数转换为浮点数
 * @param fixedPoint 32位定点数
 * @return 转换后的浮点数值
 */
float fixed2float(uint32_t fixedPoint) {
    int16_t integerPart = (fixedPoint >> 16) & 0xFFFF;  // 获取高16位整数部分
    uint16_t fractionalPart = fixedPoint & 0xFFFF;      // 获取低16位小数部分

    float result = (float)integerPart + ((float)fractionalPart / 65536.0f);  // 将小数部分除以2^16转换为浮点数

    return result;
}

/**
 * @brief 将TDC测量值转换为纳秒
 * @param val TDC测量原始值
 * @return 转换后的时间，单位为纳秒
 */
float TDC_to_ns(uint32_t val) {
    float val_f = fixed2float(val);
    return val_f / Fe9;
}

/**
 * @brief 进行一次TDC测量
 * @param result 存储测量结果的指针
 * @param timeout 超时时间，单位毫秒
 * @return 0表示测量成功，1表示测量超时
 */
uint8_t TDC_Measure(uint32_t *result, uint32_t timeout) {
    uint32_t t = HAL_GetTick();

    write8(0x70);
    delay(1);
    SSN(1);
    //HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_RESET);
    PULSE_GPIO_Port->BSRR = (uint32_t)PULSE_Pin << 16U;  // 清除PULSE引脚
    for (uint8_t i = 0; i < 1; i++) {}                   // 短延时
    PULSE_GPIO_Port->BSRR = PULSE_Pin;                   // 设置PULSE引脚
    //HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_SET);

    while (INT()) {
        delay(1);
        if (HAL_GetTick() - t > timeout) {
            return 1;  // 测量超时
        }
    }

    delay(1);
    write8(0xB0);      // READ REG0
    delay(1);
    *result = read32();

    return 0;          // 测量成功
}

/**
 * @brief 转换TDC值为时间 (未使用)
 * @param val TDC测量原始值
 * @return 转换后的时间，单位为纳秒
 *
float convert(uint32_t val) {
    unsigned int z, x;
    float sum, num = 2.0;

    z = (val >> 16);
    x = (val & 0xffff);

    for (int i = 0; i < 16; i++) {
        num *= 2.0;
        if (x & 0x8000) {
            sum += (1.0 / num);
        }
        x <<= 1;
    }

    return ((z + sum) * 250.0); // 使用的是4Mhz晶振    显示的时间单位是ns
}
*/
