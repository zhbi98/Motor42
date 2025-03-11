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

//Oneself
#include "control_config.h"

int32_t Current_Rated_Current = _Current_Rated_Current; /**< 额定电流(mA)*/
int32_t Current_Cali_Current  = _Current_Cali_Current;  /**< 校准电流(mA)*/

int32_t Move_Home_Offset = ((int32_t)(0));
int32_t Move_Rated_Speed = _Move_Rated_Speed;                     /**< (额定转速)(50转每秒)*/
int32_t Move_Rated_UpAcc = _Move_Rated_UpAcc;                     /**< (固件额定加速加速度)(1000r/ss)*/
int32_t Move_Rated_DownAcc = _Move_Rated_DownAcc;                 /**< (固件额定减速加速度)(1000r/ss)*/
int32_t Move_Rated_UpCurrentRate = _Move_Rated_UpCurrentRate;     /**< (固件额定增流梯度)(20倍额定/s)*/
int32_t Move_Rated_DownCurrentRate = _Move_Rated_DownCurrentRate; /**< (固件额定减流梯度)(20倍额定/s)*/

/**
 * @brief  控制静态配置
 * @param  NULL
 * @retval NULL
**/
void Control_Config_Init_Static(void)
{
}

/**
 * @brief  控制动态配置 
 * @param  NULL
 * @retval NULL
**/
void Control_Config_Init_Dynamic(void)
{
}
