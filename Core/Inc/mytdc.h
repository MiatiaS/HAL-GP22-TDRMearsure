//
// Created by 20614 on 25-5-26.
//

#ifndef MYTDC_H
#define MYTDC_H

#include "main.h"

/**
 * @brief 初始化TDC，配置硬件SPI接口
 * @param hspi 使用的SPI句柄指针
 */
void MYTDC_Init(SPI_HandleTypeDef *hspi);

/**
 * @brief 复位TDC芯片
 */
void MYTDC_Reset(void);

/**
 * @brief 获取TDC状态寄存器
 * @return 状态寄存器值
 */
uint32_t MYTDC_Get_Status_Reg(void);

/**
 * @brief 进行一次TDC测量
 * @param result 存储测量结果的指针
 * @param timeout 超时时间，单位毫秒
 * @return 0表示测量成功，1表示测量超时
 */
uint8_t MYTDC_Measure(uint32_t *result, uint32_t timeout);

/**
 * @brief 将TDC测量值转换为纳秒
 * @param val TDC测量原始值
 * @return 转换后的时间，单位为纳秒
 */
float MYTDC_to_ns(uint32_t val);

/**
 * @brief TDC测试函数
 * @return 测试寄存器值
 */
uint32_t MYTDC_Test(void);

#endif //MYTDC_H
