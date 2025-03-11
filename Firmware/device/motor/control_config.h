/******
	************************************************************************
	******
	** @project : XDrive_Step
	** @brief   : Stepper motor with multi-function interface and closed loop function. 
	** @brief   : 具有多功能接口和闭环功能的步进电机
	** @author  : unlir (知不知啊)
	** @contacts: QQ.1354077136
	******
	** @address : https://github.com/unlir/XDrive
	******
	************************************************************************
	******
	** {Stepper motor with multi-function interface and closed loop function.}
	** Copyright (c) {2020}  {unlir(知不知啊)}
	** 
	** This program is free software: you can redistribute it and/or modify
	** it under the terms of the GNU General Public License as published by
	** the Free Software Foundation, either version 3 of the License, or
	** (at your option) any later version.
	** 
	** This program is distributed in the hope that it will be useful,
	** but WITHOUT ANY WARRANTY; without even the implied warranty of
	** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	** GNU General Public License for more details.
	** 
	** You should have received a copy of the GNU General Public License
	** along with this program.  If not, see <http://www.gnu.org/licenses/>.
	******
	************************************************************************
******/

/*****
  ** @file     : control_config.c/h
  ** @brief    : 控制配置
  ** @versions : 2.2.3
  ** @time     : 2020/09/15
  ** @reviser  : unli (HeFei China)
  ** @explain  : null
*****/

#ifndef CONTROL_CONFIG_H
#define CONTROL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/********************  硬件配置区  ********************/
#define _Current_Rated_Current  (3000)  /**< 额定电流(mA)*/
#define _Current_Cali_Current   (2000)  /**< 校准电流(mA)*/

/********************  运动参数配置区  ********************/
#define Move_Step_NUM               ((int32_t)(200))  /**< (使用的电机单圈步数)(每步磁场旋转90°)*/
#define Move_Divide_NUM             ((int32_t)(256))  /**< (每步柔性件控制细分量)*/
#define Move_Pulse_NUM              ((int32_t)(Move_Step_NUM * Move_Divide_NUM))  /**< (电机单圈脉冲数)*/
#define _Move_Rated_Speed           ((int32_t)(50 * Move_Pulse_NUM))    /**< (额定转速)(50转每秒)*/
#define _Move_Rated_UpAcc           ((int32_t)(1000 * Move_Pulse_NUM))  /**< (固件额定加速加速度)(1000r/ss)*/
#define _Move_Rated_DownAcc         ((int32_t)(1000 * Move_Pulse_NUM))  /**(固件额定减速加速度)(1000r/ss)*/
#define _Move_Rated_UpCurrentRate   ((int32_t)(20 * _Current_Rated_Current))	/**< (固件额定增流梯度)(20倍额定/s)*/
#define _Move_Rated_DownCurrentRate	((int32_t)(20 * _Current_Rated_Current))	/**< (固件额定减流梯度)(20倍额定/s)*/

/****************************************  控制器频率配置区  ****************************************/
#define CONTROL_FREQ_HZ   (20000)                     /**< 控制频率_hz*/
#define CONTROL_PERIOD_US (1000000 / CONTROL_FREQ_HZ) /**< 控制周期_us*/

/*void Control_Config_Init_Static(void);  // 控制静态配置*/
/*void Control_Config_Init_Dynamic(void); // 控制动态配置*/

/********************  可变硬件配置区  ************************/
extern int32_t Current_Rated_Current; /**< 额定电流(mA)*/
extern int32_t Current_Cali_Current;  /**< 校准电流(mA)*/

/********************  可变运动参数配置区  ********************/
extern int32_t Move_Home_Offset;
extern int32_t Move_Rated_Speed;           /**< (额定转速)(50转每秒)*/
extern int32_t Move_Rated_UpAcc;           /**< (固件额定加速加速度)(1000r/ss)*/
extern int32_t Move_Rated_DownAcc;         /**< (固件额定减速加速度)(1000r/ss)*/
extern int32_t Move_Rated_UpCurrentRate;   /**< (固件额定增流梯度)(20倍额定/s)*/
extern int32_t Move_Rated_DownCurrentRate; /**< (固件额定减流梯度)(20倍额定/s)*/

#ifdef __cplusplus
}
#endif

#endif
