
#if 0

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Move_Step_NUM   ((int)(200))
#define Move_Divide_NUM ((int)(256))
#define Move_Pulse_NUM  ((int)(Move_Step_NUM * Move_Divide_NUM))

#define CALI_Encode_Res 16384U // 81.92 * 200
#define CALI_Gather_Encode_Res ((int)(CALI_Encode_Res / Move_Step_NUM))

typedef enum {
    // 无错误
    CALI_No_Error = 0x00, // 数据无错误
    //原始数据出错
    CALI_Error_Average_Dir, // 平均值方向错误
    CALI_Error_Average_Continuity, // 平均值连续性错误
    CALI_Error_PhaseStep, // 阶跃次数错误
    // 解析数据出错
    CALI_Error_Analysis_Quantity, //解析数据量错误
} CALI_Error;

unsigned int count; // 用于各个计数
int sub_data; // 用于各个算差
int rcd_x, rcd_y;
unsigned char dir;
CALI_Error  error_code;
unsigned int error_data;
unsigned int result_num;

unsigned short coder_data_f[Move_Step_NUM + 1] = {
    14401, 14516, 14584, 14672, 14742, 
    14839, 14911, 15000, 15073, 15168, 
    15241, 15331, 15403, 15498, 15572, 
    15661, 15734, 15828, 15904, 15993, 
    16065, 16157, 16232, 16321, 11, 
    101,   175,   264,   338,   430, 
    504,   593,   666,   760,   835, 
    923,   996,   1090,  1166,  1255, 
    1327,  1420,  1496,  1584,  1657, 
    1751,  1823,  1912,  1986,  2079, 
    2151,  2240,  2315,  2405,  2480, 
    2569,  2642,  2733,  2809,  2897, 
    2971,  3063,  3137,  3226,  3300, 
    3391,  3464,  3552,  3627,  3718, 
    3791,  3877,  3951,  4043,  4116, 
    4202,  4275,  4366,  4440,  4526, 
    4599,  4690,  4763,  4851,  4925, 
    5016,  5089,  5176,  5251,  5343, 
    5415,  5502,  5577,  5668,  5741, 
    5829,  5901,  5992,  6067,  6154, 
    6225,  6317,  6392,  6478,  6551, 
    6643,  6717,  6804,  6878,  6970, 
    7044,  7131,  7205,  7297,  7372, 
    7459,  7532,  7624,  7700,  7787, 
    7860,  7952,  8027,  8115,  8189, 
    8281,  8355,  8444,  8517,  8610, 
    8684,  8773,  8846,  8940,  9015, 
    9104,  9176,  9269,  9346,  9435, 
    9506,  9599,  9674,  9764,  9834, 
    9929,  10000, 10091, 10162, 10257, 
    10327, 10419, 10489, 10583, 10655, 
    10747, 10815, 10911, 10983, 11075, 
    11143, 11240, 11312, 11405, 11473, 
    11569, 11640, 11732, 11802, 11898, 
    11968, 12058, 12128, 12225, 12295, 
    12384, 12453, 12549, 12620, 12710, 
    12779, 12874, 12946, 13037, 13107, 
    13202, 13273, 13364, 13435, 13530, 
    13600, 13690, 13762, 13858, 13928, 
    14019, 14088, 14183, 14256, 14346, 
    14413, 
};

unsigned short coder_data_r[Move_Step_NUM + 1] = {
    14450, 14524, 14614, 14685, 14776, 
    14852, 14940, 15013, 15104, 15179, 
    15268, 15342, 15433, 15508, 15596, 
    15672, 15762, 15837, 15924, 16001, 
    16090, 16166, 16253, 16329, 33, 
    110,   197,   273,   360,   439, 
    527,   603,   691,   768,   856, 
    933,   1021,  1098,  1185,  1263, 
    1351,  1429,  1515,  1592,  1679, 
    1759,  1845,  1919,  2008,  2086, 
    2172,  2246,  2336,  2412,  2499, 
    2575,  2664,  2740,  2827,  2904, 
    2992,  3069,  3155,  3230,  3318, 
    3396,  3482,  3556,  3644,  3721, 
    3808,  3881,  3969,  4045,  4131, 
    4206,  4294,  4370,  4455,  4531, 
    4619,  4695,  4781,  4856,  4944, 
    5022,  5108,  5182,  5270,  5348, 
    5434,  5509,  5597,  5674,  5760, 
    5836,  5923,  5998,  6084,  6161, 
    6247,  6323,  6410,  6484,  6571, 
    6650,  6736,  6811,  6899,  6976, 
    7063,  7139,  7228,  7304,  7391, 
    7468,  7556,  7631,  7719,  7796, 
    7884,  7961,  8047,  8124,  8211, 
    8289,  8376,  8452,  8538,  8617, 
    8705,  8781,  8869,  8946,  9034, 
    9112,  9200,  9277,  9365,  9443, 
    9533,  9609,  9698,  9774,  9864, 
    9941,  10029, 10102, 10196, 10269, 
    10359, 10433, 10525, 10597, 10688, 
    10762, 10854, 10927, 11019, 11091, 
    11183, 11257, 11348, 11418, 11512, 
    11586, 11676, 11746, 11839, 11912, 
    12002, 12073, 12165, 12238, 12326, 
    12398, 12492, 12563, 12651, 12724, 
    12817, 12890, 12978, 13050, 13142, 
    13217, 13306, 13377, 13470, 13544, 
    13634, 13706, 13798, 13871, 13961, 
    14034, 14124, 14196, 14288, 14361, 
    14447, 
};

int CycleSub(int a, int b, int cyc)
{
    int sub_data;

    sub_data = a - b;
    if (sub_data > (cyc >> 1))
        sub_data -= cyc;
    if (sub_data < (-cyc >> 1))
        sub_data += cyc;
    return sub_data;
}

unsigned int CycleRem(unsigned int a,unsigned int b)
{
    return (a + b) % b;
}

int CycleAverage(int a, int b, int cyc)
{
    int sub_data;
    int ave_data;

    sub_data = a - b;
    ave_data = (a + b) >> 1;

    if (abs(sub_data) > (cyc >> 1)) {
        if (ave_data >= (cyc >> 1))
            ave_data -= (cyc >> 1);
        else
            ave_data += (cyc >> 1);
    }
    return ave_data;
}

void Calibration_Data_Check()
{
    int sub_data = 0;

    for (unsigned int count = 0; count < Move_Step_NUM + 1; count++) {
        coder_data_f[count] = (unsigned short)CycleAverage(
            (int)coder_data_f[count], 
            (int)coder_data_r[count], 
            CALI_Encode_Res
        );
    }

    // 平均值数据检查
    sub_data = CycleSub(
        (int)coder_data_f[0],
        (int)coder_data_f[Move_Step_NUM - 1],
        CALI_Encode_Res
    );

    if (sub_data == 0) {
        error_code = CALI_Error_Average_Dir; 
        return;
    }

    if (sub_data > 0) {
        dir = true;
    }

    if (sub_data < 0) {
        dir = false;
    }

    for (unsigned int count = 1; count < Move_Step_NUM; count++) {
        sub_data = CycleSub(
            (int)coder_data_f[count], 
            (int)coder_data_f[count - 1], 
            CALI_Encode_Res
        );

        // 两次数据差过大
        if (abs(sub_data) > (CALI_Gather_Encode_Res * 3 / 2)) {
            error_code = CALI_Error_Average_Continuity;
            error_data = count;
            return;
        }

        // 两次数据差过小
        if (abs(sub_data) < (CALI_Gather_Encode_Res * 1 / 2)) {
            error_code = CALI_Error_Average_Continuity; 
            error_data = count; 
            return;
        }

        if (sub_data == 0) {
            error_code = CALI_Error_Average_Dir;
            error_data = count;
            return;
        }

        if ((sub_data > 0) && (!dir)) {
            error_code = CALI_Error_Average_Dir;
            error_data = count;
            return;
        }

        if ((sub_data < 0) && (dir)) {
            error_code = CALI_Error_Average_Dir;
            error_data = count;
            return;
        }
    }

    unsigned int step_num = 0;

    if (dir) {
        for (unsigned int count = 0; count < Move_Step_NUM; count++) {
            sub_data = (int)coder_data_f[CycleRem(count + 1, Move_Step_NUM)] - 
                        (int)coder_data_f[CycleRem(count, Move_Step_NUM)];

            if (sub_data < 0) {
                step_num++;
                rcd_x = count; // 使用区间前标
                rcd_y = (CALI_Encode_Res - 1) - coder_data_f[CycleRem(rcd_x, Move_Step_NUM)];

                printf("rcd_x:%d\n", rcd_x);
                printf("rcd_y:%d\n", rcd_y);
            }
        }

        if (step_num != 1) {
            error_code = CALI_Error_PhaseStep;
            printf("STEP NUM:%d\n", step_num);
            return;
        }
    } else {
        for (unsigned int count = 0; count < Move_Step_NUM; count++) {
            sub_data = (int)coder_data_f[CycleRem(count + 1, Move_Step_NUM)] - 
                        (int)coder_data_f[CycleRem(count, Move_Step_NUM)];
            
            if (sub_data > 0) {
                step_num++;
                rcd_x = count; // 使用区间前标
                rcd_y = (CALI_Encode_Res - 1) - coder_data_f[CycleRem(rcd_x + 1, Move_Step_NUM)];

                printf("rcd_x:%d\n", rcd_x);
                printf("rcd_y:%d\n", rcd_y);
            }
        }

        if (step_num != 1) {
            error_code = CALI_Error_PhaseStep;
            printf("STEP NUM:%d\n", step_num);
            return;
        }
    }
}

void calibration_loop()
{
    int data_i32;
    unsigned short data_u16;

    if (error_code == CALI_No_Error) {
        int32_t step_x, step_y;
        result_num = 0;

        // rom_data_clear(&_quick_cali); // 擦除数据区
        // rom_data_begin(&_quick_cali); // 开始写数据区

        if (dir) {
            for (step_x = rcd_x; step_x < rcd_x + Move_Step_NUM + 1; step_x++) {
                data_i32 = CycleSub(
                    coder_data_f[CycleRem(step_x+1, Move_Step_NUM)],
                    coder_data_f[CycleRem(step_x,   Move_Step_NUM)],
                    CALI_Encode_Res
                );

                if (step_x == rcd_x) { // 开始边缘
                    for (step_y = rcd_y; step_y < data_i32; step_y++) {
                        data_u16 = CycleRem(
                            Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                } else if (step_x == rcd_x + Move_Step_NUM) { // 结束边缘
                    for (step_y = 0; step_y < rcd_y; step_y++) {
                        data_u16 = CycleRem(
                            Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                } else { // 中间
                    for (step_y = 0; step_y < data_i32; step_y++) {
                        data_u16 = CycleRem(
                            Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                }
            }
        } else {
            for (step_x = rcd_x + Move_Step_NUM; step_x > rcd_x - 1; step_x--) {
                data_i32 = CycleSub(
                    coder_data_f[CycleRem(step_x, Move_Step_NUM)],
                    coder_data_f[CycleRem(step_x + 1, Move_Step_NUM)],
                    CALI_Encode_Res
                );

                if (step_x == rcd_x + Move_Step_NUM) { // 开始边缘
                    for(step_y = rcd_y; step_y < data_i32; step_y++){
                        data_u16 = CycleRem(
                            Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                } else if(step_x == rcd_x) { // 结束边缘
                    for (step_y = 0; step_y < rcd_y; step_y++) {
                        data_u16 = CycleRem(
                            Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                } else { // 中间
                    for (step_y = 0; step_y < data_i32; step_y++) {
                        data_u16 = CycleRem(
                            Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / data_i32,
                            Move_Pulse_NUM
                        );

                        // rom_write_data16(&_quick_cali, &data_u16, 1);
                        result_num++;
                    }
                }
            }
        }

        // rom_data_end(&_quick_cali); // 结束写数据区
        
        if (result_num != CALI_Encode_Res)
            error_code = CALI_Error_Analysis_Quantity;
        else
            printf("MT6816 CAL SUCCESS\n");
    }
}

int main()
{
    // int a = CycleSub(1000, 800, 16384);
    // printf("CycleSub:%d\n", a);

    // int _a = CycleSub(82, 16400, 16400);
    // printf("CycleSub:%d\n", _a);

    // int b = CycleRem(10, 200);
    // printf("CycleRem:%d\n", b);

    Calibration_Data_Check();

    /*--------------------
    CycleSub:200
    CycleSub:82
    CycleRem:10
    rcd_x:199
    rcd_y:81
    --------------------*/

    error_code = CALI_No_Error;
    calibration_loop();

    return 0;
}

#endif
