//
// Created by 20614 on 25-5-26.
//

#if 0
#include "mytdc.h"
#include "spi.h"
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

// 全局SPI句柄
static SPI_HandleTypeDef *g_hspi ;

/**
 * @brief 使用硬件SPI发送8位数据
 * @param data 待发送的8位数据
 */
static void SPI_Send8(uint8_t data)
{
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_RESET); // 选中设备
    HAL_SPI_Transmit(&hspi4, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET);
}

/**
 * @brief 使用硬件SPI发送32位数据
 * @param data 待发送的32位数据
 */
static void SPI_Send32(uint32_t data)
{
    uint8_t buffer[4];
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
    
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_RESET); // 选中设备
    HAL_SPI_Transmit(&hspi4, buffer, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
}

/**
 * @brief 使用硬件SPI读取8位数据
 * @return 读取到的8位数据
 */
static uint8_t SPI_Read8(void)
{
    uint8_t txData = 0xFF; // 读取时发送的空数据
    uint8_t rxData = 0;
    
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_RESET); // 选中设备
    HAL_SPI_TransmitReceive(&hspi4, &txData, &rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
    
    return rxData;
}

/**
 * @brief 使用硬件SPI读取32位数据
 * @return 读取到的32位数据
 */
static uint32_t SPI_Read32(void)
{
    uint8_t txData[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // 读取时发送的空数据
    uint8_t rxData[4] = {0};
    
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_RESET); // 选中设备
    HAL_SPI_TransmitReceive(&hspi4, txData, rxData, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
    
    uint32_t result = ((uint32_t)rxData[0] << 24) | 
                       ((uint32_t)rxData[1] << 16) | 
                       ((uint32_t)rxData[2] << 8) | 
                       rxData[3];
    return result;
}

/**
 * @brief 复位TDC芯片
 */
void MYTDC_Reset(void)
{
    HAL_GPIO_WritePin(TDC_RTN_GPIO_Port, TDC_RTN_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TDC_RTN_GPIO_Port, TDC_RTN_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(TDC_RTN_GPIO_Port, TDC_RTN_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

/**
 * @brief 将定点数转换为浮点数
 * @param fixedPoint 32位定点数
 * @return 转换后的浮点数值
 */
static float fixed2float(uint32_t fixedPoint)
{
    int16_t integerPart = (fixedPoint >> 16) & 0xFFFF;  // 获取高16位整数部分
    uint16_t fractionalPart = fixedPoint & 0xFFFF;      // 获取低16位小数部分

    float result = (float)integerPart + ((float)fractionalPart / 65536.0f);  // 将小数部分除以2^16转换为浮点数

    return result;
}

/**
 * @brief 初始化TDC，配置硬件SPI接口
 * @param hspi 使用的SPI句柄指针
 */
void MYTDC_Init(SPI_HandleTypeDef *hspi)
{
    g_hspi = hspi;
    
    // 复位TDC
    MYTDC_Reset();

    // 配置TDC
    SPI_Send8(0x50);         // power on reset;
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
    HAL_Delay(1);
    
    //----------------------------------------------------------------------------
    // 测量范围1，用stop1的第一个脉冲减去START的脉冲
    // 注意：测量范围1中，从START开始到最后一个STOP信号的时间间隔不能超过1.8us，否则溢出。
    SPI_Send32(0x80009420);  // 测量范围1，校准陶瓷晶振时间为8个32K周期，244.14us 设置4M上电后一直起振，自动校准，上升沿敏感
    HAL_Delay(1);
    SPI_Send32(0x81010100);  // 测量范围1，STOP1-START

    HAL_Delay(1);
    SPI_Send32(0x82E00000);  // 开启所有中断源
    HAL_Delay(1);
    SPI_Send32(0x83080000);  // 溢出预划分器64us
    HAL_Delay(1);
    SPI_Send32(0x84200000);
    HAL_Delay(1);
    SPI_Send32(0x85080000);
    HAL_Delay(1);
    SPI_Send8(0x70);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
    HAL_Delay(1);
}

/**
 * @brief 获取TDC状态寄存器
 * @return 状态寄存器值
 */
uint32_t MYTDC_Get_Status_Reg(void)
{
    uint32_t reg;

    SPI_Send8(0xb4);
    reg = SPI_Read32();

    return reg;
}

/**
 * @brief TDC测试函数
 * @return 测试寄存器值
 */
uint32_t MYTDC_Test(void)
{
    uint8_t test_reg;

    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_RESET); // 选中设备
    SPI_Send32(0x81884200);  // INTN的脉冲和这个有关
    HAL_Delay(1);
    SPI_Send8(0xb5);
    HAL_Delay(1);
    test_reg = SPI_Read8();
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备

    return test_reg;
}

/**
 * @brief 将TDC测量值转换为纳秒
 * @param val TDC测量原始值
 * @return 转换后的时间，单位为纳秒
 */
float MYTDC_to_ns(uint32_t val)
{
    float val_f = fixed2float(val);
    return val_f / Fe9;
}

/**
 * @brief 进行一次TDC测量
 * @param result 存储测量结果的指针
 * @param timeout 超时时间，单位毫秒
 * @return 0表示测量成功，1表示测量超时
 */

uint8_t MYTDC_Measure(uint32_t *result, uint32_t timeout)
{
    uint32_t t = HAL_GetTick();

    SPI_Send8(0x70);
    HAL_GPIO_WritePin(TDC_SSN_GPIO_Port, TDC_SSN_Pin, GPIO_PIN_SET); // 释放设备
    HAL_Delay(1);

    PULSE_GPIO_Port->BSRR = (uint32_t)PULSE_Pin << 16U;  // 清除PULSE引脚
    for (uint8_t i = 0; i < 1; i++) {}                   // 短延时
    PULSE_GPIO_Port->BSRR = PULSE_Pin;                   // 设置PULSE引脚

    while (HAL_GPIO_ReadPin(TDC_INT_GPIO_Port, TDC_INT_Pin) == GPIO_PIN_SET)
    {
        if (HAL_GetTick() - t > timeout)
        {
            return 1;  // 测量超时
        }
    }

    HAL_Delay(1);
    SPI_Send8(0xB0);      // READ REG0
    *result = SPI_Read32();

    return 0;          // 测量成功
}

#endif