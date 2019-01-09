/* WORK IN PROGRESS
FFT in C "hack"
This version assumes the original HDL (i.e. no changes to datapath and channel select vector is in C)

Each loop, read all ADC data from NM0 only
Pull out relevant channel to perform FFT on and store in a local buffer
Once we have enough samples for FFT computation (depends on FFT size), compute FFT on data in local buffer

FFT data is streamed out to the GUI over SPI as before.
The data structure of adc_data_buf is as follows:
adc_data_buf[0] = header
adc_data_buf[1] = ch0 (control)
adc_data_buf[2] = ch1 (stim?)
adc_data_buf[3] = mag LSB
adc_data_buf[4] = mag MSB
adc_data_buf[5] = counter
Each SPI transaction includes a single sample of data and two samples of the N-pt FFT and their corresponding magnitudes
*/

#include <m2sxxx.h>
#include "mss_uart.h"
#include <stdio.h>
#include "mss_spi.h"
#include <mss_hpdma.h>
#include "mss_gpio.h"
#include "mss_timer.h"
#include <stdbool.h>
#include "accelerometer.h"
#include <../Mario_Libero_MSS_CM3_hw_platform/CMSIS/mss_assert.h>
#include <stdlib.h>

#include "arm_math.h"
#include "arm_const_structs.h"

// ring buffer for command bytes received from GUI
#define RINGBUF_SIZE 64
static volatile char ringbuf[RINGBUF_SIZE];
static volatile unsigned int rbwrptr = 0;
static unsigned int rbrdptr = 0;

#define BYTES_IN_RINGBUF() ((rbwrptr - rbrdptr) & (RINGBUF_SIZE-1))

// NM registers
#define NM_CHIP_ID_REG 0x00
#define NM_STATUS_REG 0x01
#define NM_PWR_CONFIG_REG 0x02
#define NM_TEST_SEL_REG 0x03
#define NM_REC_ENABLE0_REG 0x04
#define NM_REC_ENABLE1_REG 0x05
#define NM_REC_ENABLE2_REG 0x06
#define NM_REC_ENABLE3_REG 0x07
#define NM_IMP_EN0_REG 0x08
#define NM_IMP_EN1_REG 0x09
#define NM_IMP_EN2_REG 0x0A
#define NM_IMP_EN3_REG 0x0B
#define NM_REC_CONFIG_REG 0x0C
#define NM_SYS_CONFIG_REG 0x0D
#define RESERVED1 0x0E
#define NM_SCRATCH_REG 0x0F

#define NM_STIM_CFG0_REG 0x10
#define NM_STIM_CFG1_REG 0x11
#define NM_STIM_CFG2_REG 0x12
#define NM_STIM_CFG3_REG 0x13
#define NM_STIM_CFG4_REG 0x14
#define NM_STIM_CFG5_REG 0x15
#define NM_STIM_CFG6_REG 0x16
#define NM_STIM_CFG7_REG 0x17
#define NM_STIM_CFG8_REG 0x18
#define NM_STIM_CFG9_REG 0x19
#define NM_STIM_CFG10_REG 0x1A
#define NM_STIM_CFG11_REG 0x1B
#define NM_STIM_CFG12_REG 0x1C
#define NM_STIM_CFG13_REG 0x1D
#define NM_STIM_CFG14_REG 0x1E
#define NM_STIM_CFG15_REG 0x1F


// SF2 "registers"
#define REG_BASE 0x30000000

int *status = (int*)(REG_BASE+0);
int *rst = (int*)(REG_BASE+4);
int *dbg = (int*)(REG_BASE+8);
int *n0d1 = (int*)(REG_BASE+0x10);
int *n0d2 = (int*)(REG_BASE+0x14);
int *n0ack = (int*)(REG_BASE+0x18);
int *n0adc = (int*)(REG_BASE+0x1c);
int *n1d1 = (int*)(REG_BASE+0x20);
int *n1d2 = (int*)(REG_BASE+0x24);
int *n1ack = (int*)(REG_BASE+0x28);
int *n1adc = (int*)(REG_BASE+0x2c);

// function for reading in bytes received over SPI from Nordic
void spi_rx_handler();

// size of SPI data buffers, in halfwords. Includes a header and 3 channels of accelerometer
#define NUM_ENABLED_CHANNELS 2
#define DATA_BUF_SIZE 6
#define NUM_DATA_BUF 8

static volatile uint16_t adc_data_buf[NUM_DATA_BUF][DATA_BUF_SIZE];
static volatile uint16_t dummy;
static volatile uint16_t temp_buf[DATA_BUF_SIZE];
// Also need a separate buffer for register data, since buf1 and buf2 can't be overwritten.
static uint16_t reg_data_buf[DATA_BUF_SIZE];
// vector for indicating which channels are enabled for transmitting
static uint8_t sel_data_vector[128];

//static uint8_t * bp;	//pointer for accel data
uint8_t mpu6050_irq_active = 0;

static bool stream_en = 0; // for streaming mode
static bool stream_on = 0;
static bool stream_off = 0;

static bool artifact_en = 0; // whether we should do artifact cancellation or not
static bool artifact_on = 0;
static bool artifact_off = 0;

static uint16_t counter = 0;

static bool stim_flag = 0; // true if there was stim on current adc sample
static int chan = 1;
static uint8_t buf_save = 0;
static uint8_t buf_good = NUM_DATA_BUF - 1;
static uint8_t buf_tx = 1;
static uint8_t stim_count = 0;
static uint16_t step = 0;

static uint8_t spi_rx_buf[6];
static volatile unsigned int gui_request_counter = 0;

static int reg_val;
static int reg_addr;
static int reg_data;

static int stim_nm0 = 0; // stim on NM0
static int stim_nm1 = 0; // stim on NM1

static uint32_t stim_rep = 0;

static int crc_count = 0;
static bool rec_enable_write = false;

#define RST_START   (1 << 8)
#define NM0_START   (1 << 12)
#define NM1_START   (1 << 13)
#define RST_BUSY    (1 << 16)
#define NM0_DATA    (1 << 20)
#define NM1_DATA    (1 << 21)
#define NM0_TXBUSY  (1 << 24)
#define NM1_TXBUSY  (1 << 25)
#define NM0_ACK (1 << 28)
#define NM1_ACK (1 << 29)
#define RST_MODE_AM 0x3
#define RST_MODE_NM 0x2
#define OP_READ 0
#define OP_WRITE 1

int stat = 0;
bool stat_flag = false;

// definitions for FFT computations
uint32_t fftSize = 512;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 0;

#define FFT_BUFFERING 4 // used for buffering data in order to do FFT in-place while with overlapping windows

arm_cfft_instance_q15 *S_CFFT; // default to 512 length FFT

static q15_t **fft_data_buf;
static q31_t *mag_data_buf;

uint16_t fft_i = 0;
uint16_t fft_i2 = 0;
uint16_t mag_i = 0;

uint16_t cl_count = 0;
uint16_t rand_min = 0, rand_max = 0;

// Closed loop stuff
uint16_t dead_len = 0;
bool rand_mode = 0;
bool fake_stim = 0;
bool derivative = 0;
bool derivativeB = 0;
bool cl0_en = 0;
bool cl1_en = 0;
bool ctrl_dir = 0;
int16_t ctrl_thresh = 0;
uint16_t ch_order = 2;
uint16_t fbin_min = 0;
uint16_t fbin_max = 0;
uint16_t bitrev_fmin = 0;
uint16_t bitrev_fmax = 0;
uint16_t bitrev_bins[2048];
uint16_t num_bins = 0;
uint16_t bitrev_bins2[2048];
uint16_t num_bins2 = 0;
uint8_t fft_size_sel = 0;

uint8_t ch_ctrl;
uint8_t ch_stim;

uint8_t fft_fsm = 0;
uint8_t to_write1, to_write2, to_send, to_calc; // for FSM to keep track of where we're writing, what we're sending, and what we're calculating over

// bit reversal table
uint16_t bitrevtable[2048] = {
0,1024,512,1536,256,1280,768,1792,128,1152,640,1664,384,1408,896,1920,64,1088,576,1600,320,1344,832,1856,192,1216,704,1728,448,1472,960,1984,32,1056,544,1568,288,1312,800,1824,160,1184,672,1696,416,1440,928,1952,96,1120,608,1632,352,1376,864,1888,224,1248,736,1760,480,1504,992,2016,16,1040,528,1552,272,1296,784,1808,144,1168,656,1680,400,1424,912,1936,80,1104,592,1616,336,1360,848,1872,208,1232,720,1744,464,1488,976,2000,48,1072,560,1584,304,1328,816,1840,176,1200,688,1712,432,1456,944,1968,112,1136,624,1648,368,1392,880,1904,240,1264,752,1776,496,1520,1008,2032,8,1032,520,1544,264,1288,776,1800,136,1160,648,1672,392,1416,904,1928,72,1096,584,1608,328,1352,840,1864,200,1224,712,1736,456,1480,968,1992,40,1064,552,1576,296,1320,808,1832,168,1192,680,1704,424,1448,936,1960,104,1128,616,1640,360,1384,872,1896,232,1256,744,1768,488,1512,1000,2024,24,1048,536,1560,280,1304,792,1816,152,1176,664,1688,408,1432,920,1944,88,1112,600,1624,344,1368,856,1880,216,1240,728,1752,472,1496,984,2008,56,1080,568,1592,312,1336,824,1848,184,1208,696,1720,440,1464,952,1976,120,1144,632,1656,376,1400,888,1912,248,1272,760,1784,504,1528,1016,2040,4,1028,516,1540,260,1284,772,1796,132,1156,644,1668,388,1412,900,1924,68,1092,580,1604,324,1348,836,1860,196,1220,708,1732,452,1476,964,1988,36,1060,548,1572,292,1316,804,1828,164,1188,676,1700,420,1444,932,1956,100,1124,612,1636,356,1380,868,1892,228,1252,740,1764,484,1508,996,2020,20,1044,532,1556,276,1300,788,1812,148,1172,660,1684,404,1428,916,1940,84,1108,596,1620,340,1364,852,1876,212,1236,724,1748,468,1492,980,2004,52,1076,564,1588,308,1332,820,1844,180,1204,692,1716,436,1460,948,1972,116,1140,628,1652,372,1396,884,1908,244,1268,756,1780,500,1524,1012,2036,12,1036,524,1548,268,1292,780,1804,140,1164,652,1676,396,1420,908,1932,76,1100,588,1612,332,1356,844,1868,204,1228,716,1740,460,1484,972,1996,44,1068,556,1580,300,1324,812,1836,172,1196,684,1708,428,1452,940,1964,108,1132,620,1644,364,1388,876,1900,236,1260,748,1772,492,1516,1004,2028,28,1052,540,1564,284,1308,796,1820,156,1180,668,1692,412,1436,924,1948,92,1116,604,1628,348,1372,860,1884,220,1244,732,1756,476,1500,988,2012,60,1084,572,1596,316,1340,828,1852,188,1212,700,1724,444,1468,956,1980,124,1148,636,1660,380,1404,892,1916,252,1276,764,1788,508,1532,1020,2044,2,1026,514,1538,258,1282,770,1794,130,1154,642,1666,386,1410,898,1922,66,1090,578,1602,322,1346,834,1858,194,1218,706,1730,450,1474,962,1986,34,1058,546,1570,290,1314,802,1826,162,1186,674,1698,418,1442,930,1954,98,1122,610,1634,354,1378,866,1890,226,1250,738,1762,482,1506,994,2018,18,1042,530,1554,274,1298,786,1810,146,1170,658,1682,402,1426,914,1938,82,1106,594,1618,338,1362,850,1874,210,1234,722,1746,466,1490,978,2002,50,1074,562,1586,306,1330,818,1842,178,1202,690,1714,434,1458,946,1970,114,1138,626,1650,370,1394,882,1906,242,1266,754,1778,498,1522,1010,2034,10,1034,522,1546,266,1290,778,1802,138,1162,650,1674,394,1418,906,1930,74,1098,586,1610,330,1354,842,1866,202,1226,714,1738,458,1482,970,1994,42,1066,554,1578,298,1322,810,1834,170,1194,682,1706,426,1450,938,1962,106,1130,618,1642,362,1386,874,1898,234,1258,746,1770,490,1514,1002,2026,26,1050,538,1562,282,1306,794,1818,154,1178,666,1690,410,1434,922,1946,90,1114,602,1626,346,1370,858,1882,218,1242,730,1754,474,1498,986,2010,58,1082,570,1594,314,1338,826,1850,186,1210,698,1722,442,1466,954,1978,122,1146,634,1658,378,1402,890,1914,250,1274,762,1786,506,1530,1018,2042,6,1030,518,1542,262,1286,774,1798,134,1158,646,1670,390,1414,902,1926,70,1094,582,1606,326,1350,838,1862,198,1222,710,1734,454,1478,966,1990,38,1062,550,1574,294,1318,806,1830,166,1190,678,1702,422,1446,934,1958,102,1126,614,1638,358,1382,870,1894,230,1254,742,1766,486,1510,998,2022,22,1046,534,1558,278,1302,790,1814,150,1174,662,1686,406,1430,918,1942,86,1110,598,1622,342,1366,854,1878,214,1238,726,1750,470,1494,982,2006,54,1078,566,1590,310,1334,822,1846,182,1206,694,1718,438,1462,950,1974,118,1142,630,1654,374,1398,886,1910,246,1270,758,1782,502,1526,1014,2038,14,1038,526,1550,270,1294,782,1806,142,1166,654,1678,398,1422,910,1934,78,1102,590,1614,334,1358,846,1870,206,1230,718,1742,462,1486,974,1998,46,1070,558,1582,302,1326,814,1838,174,1198,686,1710,430,1454,942,1966,110,1134,622,1646,366,1390,878,1902,238,1262,750,1774,494,1518,1006,2030,30,1054,542,1566,286,1310,798,1822,158,1182,670,1694,414,1438,926,1950,94,1118,606,1630,350,1374,862,1886,222,1246,734,1758,478,1502,990,2014,62,1086,574,1598,318,1342,830,1854,190,1214,702,1726,446,1470,958,1982,126,1150,638,1662,382,1406,894,1918,254,1278,766,1790,510,1534,1022,2046,1,1025,513,1537,257,1281,769,1793,129,1153,641,1665,385,1409,897,1921,65,1089,577,1601,321,1345,833,1857,193,1217,705,1729,449,1473,961,1985,33,1057,545,1569,289,1313,801,1825,161,1185,673,1697,417,1441,929,1953,97,1121,609,1633,353,1377,865,1889,225,1249,737,1761,481,1505,993,2017,17,1041,529,1553,273,1297,785,1809,145,1169,657,1681,401,1425,913,1937,81,1105,593,1617,337,1361,849,1873,209,1233,721,1745,465,1489,977,2001,49,1073,561,1585,305,1329,817,1841,177,1201,689,1713,433,1457,945,1969,113,1137,625,1649,369,1393,881,1905,241,1265,753,1777,497,1521,1009,2033,9,1033,521,1545,265,1289,777,1801,137,1161,649,1673,393,1417,905,1929,73,1097,585,1609,329,1353,841,1865,201,1225,713,1737,457,1481,969,1993,41,1065,553,1577,297,1321,809,1833,169,1193,681,1705,425,1449,937,1961,105,1129,617,1641,361,1385,873,1897,233,1257,745,1769,489,1513,1001,2025,25,1049,537,1561,281,1305,793,1817,153,1177,665,1689,409,1433,921,1945,89,1113,601,1625,345,1369,857,1881,217,1241,729,1753,473,1497,985,2009,57,1081,569,1593,313,1337,825,1849,185,1209,697,1721,441,1465,953,1977,121,1145,633,1657,377,1401,889,1913,249,1273,761,1785,505,1529,1017,2041,5,1029,517,1541,261,1285,773,1797,133,1157,645,1669,389,1413,901,1925,69,1093,581,1605,325,1349,837,1861,197,1221,709,1733,453,1477,965,1989,37,1061,549,1573,293,1317,805,1829,165,1189,677,1701,421,1445,933,1957,101,1125,613,1637,357,1381,869,1893,229,1253,741,1765,485,1509,997,2021,21,1045,533,1557,277,1301,789,1813,149,1173,661,1685,405,1429,917,1941,85,1109,597,1621,341,1365,853,1877,213,1237,725,1749,469,1493,981,2005,53,1077,565,1589,309,1333,821,1845,181,1205,693,1717,437,1461,949,1973,117,1141,629,1653,373,1397,885,1909,245,1269,757,1781,501,1525,1013,2037,13,1037,525,1549,269,1293,781,1805,141,1165,653,1677,397,1421,909,1933,77,1101,589,1613,333,1357,845,1869,205,1229,717,1741,461,1485,973,1997,45,1069,557,1581,301,1325,813,1837,173,1197,685,1709,429,1453,941,1965,109,1133,621,1645,365,1389,877,1901,237,1261,749,1773,493,1517,1005,2029,29,1053,541,1565,285,1309,797,1821,157,1181,669,1693,413,1437,925,1949,93,1117,605,1629,349,1373,861,1885,221,1245,733,1757,477,1501,989,2013,61,1085,573,1597,317,1341,829,1853,189,1213,701,1725,445,1469,957,1981,125,1149,637,1661,381,1405,893,1917,253,1277,765,1789,509,1533,1021,2045,3,1027,515,1539,259,1283,771,1795,131,1155,643,1667,387,1411,899,1923,67,1091,579,1603,323,1347,835,1859,195,1219,707,1731,451,1475,963,1987,35,1059,547,1571,291,1315,803,1827,163,1187,675,1699,419,1443,931,1955,99,1123,611,1635,355,1379,867,1891,227,1251,739,1763,483,1507,995,2019,19,1043,531,1555,275,1299,787,1811,147,1171,659,1683,403,1427,915,1939,83,1107,595,1619,339,1363,851,1875,211,1235,723,1747,467,1491,979,2003,51,1075,563,1587,307,1331,819,1843,179,1203,691,1715,435,1459,947,1971,115,1139,627,1651,371,1395,883,1907,243,1267,755,1779,499,1523,1011,2035,11,1035,523,1547,267,1291,779,1803,139,1163,651,1675,395,1419,907,1931,75,1099,587,1611,331,1355,843,1867,203,1227,715,1739,459,1483,971,1995,43,1067,555,1579,299,1323,811,1835,171,1195,683,1707,427,1451,939,1963,107,1131,619,1643,363,1387,875,1899,235,1259,747,1771,491,1515,1003,2027,27,1051,539,1563,283,1307,795,1819,155,1179,667,1691,411,1435,923,1947,91,1115,603,1627,347,1371,859,1883,219,1243,731,1755,475,1499,987,2011,59,1083,571,1595,315,1339,827,1851,187,1211,699,1723,443,1467,955,1979,123,1147,635,1659,379,1403,891,1915,251,1275,763,1787,507,1531,1019,2043,7,1031,519,1543,263,1287,775,1799,135,1159,647,1671,391,1415,903,1927,71,1095,583,1607,327,1351,839,1863,199,1223,711,1735,455,1479,967,1991,39,1063,551,1575,295,1319,807,1831,167,1191,679,1703,423,1447,935,1959,103,1127,615,1639,359,1383,871,1895,231,1255,743,1767,487,1511,999,2023,23,1047,535,1559,279,1303,791,1815,151,1175,663,1687,407,1431,919,1943,87,1111,599,1623,343,1367,855,1879,215,1239,727,1751,471,1495,983,2007,55,1079,567,1591,311,1335,823,1847,183,1207,695,1719,439,1463,951,1975,119,1143,631,1655,375,1399,887,1911,247,1271,759,1783,503,1527,1015,2039,15,1039,527,1551,271,1295,783,1807,143,1167,655,1679,399,1423,911,1935,79,1103,591,1615,335,1359,847,1871,207,1231,719,1743,463,1487,975,1999,47,1071,559,1583,303,1327,815,1839,175,1199,687,1711,431,1455,943,1967,111,1135,623,1647,367,1391,879,1903,239,1263,751,1775,495,1519,1007,2031,31,1055,543,1567,287,1311,799,1823,159,1183,671,1695,415,1439,927,1951,95,1119,607,1631,351,1375,863,1887,223,1247,735,1759,479,1503,991,2015,63,1087,575,1599,319,1343,831,1855,191,1215,703,1727,447,1471,959,1983,127,1151,639,1663,383,1407,895,1919,255,1279,767,1791,511,1535,1023,2047
};
uint8_t bitrev_factor = 3; // allows use of single table for all FFT sizes
uint16_t bitrevtable_i1 = 0, bitrevtable_i2 = 0; // index for bitrevtable for current sample
int32_t power = 0;
int32_t power2 = 0;
int32_t prev_power = 0;
int32_t prev_power2 = 0;
int32_t delta = 0;
int32_t delta2 = 0;

uint16_t fbin_max2 = 0;
uint16_t fbin_min2 = 0;
bool ctrl_en2 = 0;
bool ctrl_dir2 = 0;
int16_t ctrl_thresh2 = 0;
bool ctrl_comb = 0;
q15_t mean = 0;

void NMstim(int command, int nmid)
{
    switch (nmid)
    {
        case 0:
            *n0d2 = command;
            *status = 0x1010;					// set
            while((*status) & NM0_TXBUSY);
            break;
        case 1:
            *n1d2 = command;
            *status = 0x2020;					// set
            while((*status) & NM1_TXBUSY);
            break;
        default:
            break;
    }
}

void fft_cl(int16_t sample)
{
	int k;
	q15_t fft_sample;

	if (cl_count > 0)
	{
		cl_count--;
	}

	if (cl0_en || cl1_en)
	{
		if (!rand_mode)
		{
			// calculate bitrevtable index for both bins you are about to write (since they're not sequential inside bitrevtable)
			bitrevtable_i1 = bitrevtable[mag_i << bitrev_factor];

			// FSM for writing to and sending from 4 separate buffers. Allows performing FFT on overlapping windows (e.g. 512 pt FFT every 256 samples)
			// fft_i now has to go from 0 to 2*fftSize in increments of 4
			switch(fft_fsm)
			{
			case 0:
				to_write1 = 0;
				to_write2 = 3;
				to_send = 1;
				to_calc = 3;
				break;
			case 1:
				to_write1 = 0;
				to_write2 = 2;
				to_send = 3;
				to_calc = 0;
				break;
			case 2:
				to_write1 = 1;
				to_write2 = 2;
				to_send = 0;
				to_calc = 2;
				break;
			case 3:
				to_write1 = 1;
				to_write2 = 3;
				to_send = 2;
				to_calc = 1;
				break;
			}
			// write data sample to the appropriate buffers

			// de-mean samples
			if (fft_i == 0)
			{
				mean = sample;
			}

			fft_sample = (q15_t) (sample - mean) << 3;
			fft_data_buf[to_write1][fft_i] = fft_sample;
			fft_data_buf[to_write1][fft_i+1] = 0;
			fft_data_buf[to_write2][fft_i2] = fft_sample;
			fft_data_buf[to_write2][fft_i2+1] = 0;

			// load samples to send to the appropriate buffer
			adc_data_buf[buf_tx][3] = (uint16_t) (mag_data_buf[bitrevtable_i1] & 0xFFFF);
			adc_data_buf[buf_tx][4] = (uint16_t) ((mag_data_buf[bitrevtable_i1] >> 16) & 0xFFFF);

			// calculate FFT on appropriate buffer
			if (fft_i == (fftSize<<1) - 2 || fft_i2 == (fftSize<<1) - 2) // if we have fftSize/2 new samples
			{

				adc_data_buf[buf_tx][ch_order] |= 0x8000;
				fft_fsm = (fft_fsm + 1) & 3;
				// calculate FFT on new chunk
				arm_cfft_q15(S_CFFT, fft_data_buf[to_calc], ifftFlag, doBitReverse);
				arm_cmplx_mag_squared_q15(fft_data_buf[to_calc], mag_data_buf, fftSize);
				prev_power = power;
				power = 0;
				for (k=0; k<num_bins; k++)
				{
					power += ((int32_t) (mag_data_buf[bitrev_bins[k]]));
				}
				prev_power2 = power2;
				power2 = 0;
				for (k=0; k<num_bins2; k++)
				{
					power2 += ((int32_t) (mag_data_buf[bitrev_bins2[k]]));
				}
				if (derivative)
				{
					delta = power - prev_power;
				}
				else
				{
					delta = power;
				}

				if (derivativeB)
				{
					delta2 = power2 - prev_power2;
				}
				else
				{
					delta2 = power2;
				}
			}

			fft_i = (fft_i + 2) & ((fftSize<<1) - 1);
			fft_i2 = (fft_i2 + 2) & ((fftSize<<1) - 1);
			mag_i = (mag_i + 1) & ((fftSize>>1)-1);

			if (cl_count == 0)
			{
				if (ctrl_comb)
				{
					if (ctrl_dir & ctrl_dir2)
					{
						if ((delta > ctrl_thresh) || (delta2 > ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else if (ctrl_dir & !ctrl_dir2)
					{
						if ((delta > ctrl_thresh) || (delta2 < ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else if (!ctrl_dir & ctrl_dir2)
					{
						if ((delta < ctrl_thresh) || (delta2 > ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else
					{
						if ((delta < ctrl_thresh) || (delta2 < ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
				}
				else
				{
					if (ctrl_dir & ctrl_dir2)
					{
						if ((delta > ctrl_thresh) && (delta2 > ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else if (ctrl_dir & !ctrl_dir2)
					{
						if ((delta > ctrl_thresh) && (delta2 < ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else if (!ctrl_dir & ctrl_dir2)
					{
						if ((delta < ctrl_thresh) && (delta2 > ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
					else
					{
						if ((delta < ctrl_thresh) && (delta2 < ctrl_thresh2))
						{
							cl_count = dead_len;
							if (!fake_stim)
							{
								if (cl0_en)
								{
									NMstim(0x09 | (1 << 10), 0);
								}
								else if (cl1_en)
								{
									NMstim(0x09 | (1 << 10), 1);
								}
							}
							else
							{
								if (ch_order == 2)
								{
									adc_data_buf[buf_tx][1] |= 0x8000;
								}
								else
								{
									adc_data_buf[buf_tx][2] |= 0x8000;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if (cl_count == 0)
			{
				cl_count = (rand() % (rand_max-rand_min))+rand_min;
				adc_data_buf[buf_tx][2] &= 0x8000;
				if (cl0_en)
				{
					NMstim(0x09 | (1 << 10), 0);
				}
				else if (cl1_en)
				{
					NMstim(0x09 | (1 << 10), 1);
				}
			}
		}
	}
}

void writeNMreg(int nmid, int nm_addr, int data)
{
	int i;

	switch (nmid){
	case 0:
		// need to update the channel select vector if writing to NM_REC_ENABLE reg
		if (nm_addr == NM_REC_ENABLE0_REG)
		{
			// channels 0-15
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i] = 1;
				}
				else
				{
					sel_data_vector[i] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE1_REG)
		{
			// channels 16-31
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+16] = 1;
				}
				else
				{
					sel_data_vector[i+16] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE2_REG)
		{
			// channels 32-47
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+32] = 1;
				}
				else
				{
					sel_data_vector[i+32] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE3_REG)
		{
			// channels 48-63
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+48] = 1;
				}
				else
				{
					sel_data_vector[i+48] = 0;
				}
			}
		}

		*n0d2 = (nm_addr << 16) | data;
		*n0d1 = OP_WRITE;						// select register operation (1 = write, 0 = read
		*status = NM0_START;					// set
		while((*status) & NM0_TXBUSY);
		break;
	case 1:
		// need to update the channel select vector if writing to NM_REC_ENABLE reg
		if (nm_addr == NM_REC_ENABLE0_REG)
		{
			// channels 64-79
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+64] = 1;
				}
				else
				{
					sel_data_vector[i+64] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE1_REG)
		{
			// channels 80-95
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+80] = 1;
				}
				else
				{
					sel_data_vector[i+80] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE2_REG)
		{
			// channels 96-111
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+96] = 1;
				}
				else
				{
					sel_data_vector[i+96] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE3_REG)
		{
			// channels 112-127
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+112] = 1;
				}
				else
				{
					sel_data_vector[i+112] = 0;
				}
			}
		}

		*n1d2 = (nm_addr << 16) | data;
		*n1d1 = OP_WRITE;						// select register operation (1 = write, 0 = read
		*status = NM1_START;					// set
		while((*status) & NM1_TXBUSY);
		break;
	default:
		break;
	}
}


int readNMreg(int nmid, int addr)
{
	int ret, tmp, crcok, i;
	switch (nmid){
		case 0:
			*n0d2 = addr << 16;
			*n0d1 = OP_READ;
			*status = NM0_START;
			while(!(*status & NM0_ACK));
			for(i = 0; i < 4; i++)
			{
				tmp = *n0ack;
				if (i == 1)
					crcok = (tmp == addr);
				if (i == 2)
					ret = tmp;
				if (i == 3)
					crcok &= (tmp & 1);
			}
			return crcok ? ret : -1;
			break;
		case 1:
			*n1d2 = addr << 16;
			*n1d1 = OP_READ;
			*status = NM1_START;
			while(!(*status & NM1_ACK));
			for(i = 0; i < 4; i++)
			{
				tmp = *n1ack;
				if (i == 1)
					crcok = (tmp == addr);
				if (i == 2)
					ret = tmp;
				if (i == 3)
					crcok &= (tmp & 1);
			}
			return crcok ? ret : -1;
			break;
		default:
			return 0;
			break;
		}
}

void NMreset(int nmid)
{
	switch (nmid){
	case 0:
		*n0d2 = (1 << 10) | 1;
		*n0d1 = 0;
		*status = NM0_START | (1 << 4);					// set to command send mode
		while((*status) & NM0_TXBUSY);
		break;
	case 1:
		*n1d2 = (1 << 10) | 1;
		*n1d1 = 0;
		*status = NM1_START | (1 << 5);					// set to command send mode
		while((*status) & NM1_TXBUSY);
		break;
	default:
		break;
	}
}


int main() {

	srand(0);

	SYSREG->SOFT_RST_CR |= SYSREG_FPGA_SOFTRESET_MASK;
	SYSREG->SOFT_RST_CR &= ~SYSREG_FPGA_SOFTRESET_MASK;

	// Initialize SPI_0
	MSS_SPI_init(&g_mss_spi0);
	MSS_SPI_configure_master_mode(&g_mss_spi0,
			MSS_SPI_SLAVE_0,
			MSS_SPI_MODE0,
//			12u, // changed to 12 so that we get 2.5MHz from the new 30MHz apb clock
			8u, // 20MHz/8 = 2.5MHz clock (max nordic is 8MHz)
//			6u,
            // 26u,
			MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE); //needed for transmitting in blocks

//	accel_init();

	int i = 0, j = 0;
	uint8_t buf_idx;
	uint16_t interp;

    writeNMreg(0, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
    writeNMreg(1, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
    writeNMreg(0, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
    writeNMreg(1, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en

    writeNMreg(0, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
    writeNMreg(1, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
    writeNMreg(0, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
    writeNMreg(1, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1

    // set first 96 channels to enabled. saves time from enabling them from the GUI
    writeNMreg(0, NM_REC_ENABLE0_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE1_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE2_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE3_REG, 0xFFFF);

    writeNMreg(1, NM_REC_ENABLE0_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE1_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE2_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE3_REG, 0xFFFF);

    writeNMreg(0, NM_REC_ENABLE0_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE1_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE2_REG, 0xFFFF);
    writeNMreg(0, NM_REC_ENABLE3_REG, 0xFFFF);

    writeNMreg(1, NM_REC_ENABLE0_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE1_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE2_REG, 0xFFFF);
    writeNMreg(1, NM_REC_ENABLE3_REG, 0xFFFF);

    // // set first 2 channels to enabled. saves time from enabling them from the GUI
    // writeNMreg(0, NM_REC_ENABLE0_REG, 0x0000);
    // writeNMreg(0, NM_REC_ENABLE1_REG, 0x3000);
    // writeNMreg(0, NM_REC_ENABLE2_REG, 0x0000);
    // writeNMreg(0, NM_REC_ENABLE3_REG, 0x0000);
 
    // writeNMreg(1, NM_REC_ENABLE0_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE1_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE2_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE3_REG, 0x0000);
 
    // writeNMreg(0, NM_REC_ENABLE0_REG, 0x0000);
    // writeNMreg(0, NM_REC_ENABLE1_REG, 0x3000);
    // writeNMreg(0, NM_REC_ENABLE2_REG, 0x0000);
    // writeNMreg(0, NM_REC_ENABLE3_REG, 0x0000);
 
    // writeNMreg(1, NM_REC_ENABLE0_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE1_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE2_REG, 0x0000);
    // writeNMreg(1, NM_REC_ENABLE3_REG, 0x0000);



    writeNMreg(0, NM_SYS_CONFIG_REG, 0x4000);
    writeNMreg(0, NM_SYS_CONFIG_REG, 0x4000);
    writeNMreg(1, NM_SYS_CONFIG_REG, 0x4000);
    writeNMreg(1, NM_SYS_CONFIG_REG, 0x4000);

	int data_ack_req = 0;

    for (i=0; i<DATA_BUF_SIZE; i++)
	{
        reg_data_buf[i] = 0;
	}

	for (i = 0; i<NUM_DATA_BUF; i++)
	{
		for (j = 0; j<DATA_BUF_SIZE; j++)
		{
			adc_data_buf[i][j] = 0;
		}
	}

    for (i=0; i<2048; i++)
    {
    	bitrev_bins[i] = 0;
    }
    // put the headers in, since they'll be unchanged
    for (i=0; i<NUM_DATA_BUF; i++)
    {
    	adc_data_buf[i][0] = 0xAA00;
    }

	// fft allocation
    fft_data_buf = malloc(FFT_BUFFERING * sizeof(q15_t*));
    ASSERT(fft_data_buf);
	for (i=0; i<FFT_BUFFERING; i++)
	{
		fft_data_buf[i] = malloc(fftSize*2 * sizeof(q15_t*));
		ASSERT(fft_data_buf[i]);
	}
	mag_data_buf = malloc(fftSize * sizeof(q31_t*));
	ASSERT(mag_data_buf);

    uint16_t flags, crcflag, crcflag0, crcflag1 = 0;
    int ret, tmp, crcok;
    char buf[5];
	while(1)
	{
		if (BYTES_IN_RINGBUF() >= 5)
		{
			for(i = 0; i < 5; i++)
			{
				buf[i] = ringbuf[rbrdptr];
				rbrdptr = (rbrdptr+1) % RINGBUF_SIZE;
			}
			if (buf[0] == 0xff)
			{
                data_ack_req = buf[3] & 0x3;

                stream_on = (buf[4] >> 5) & 0x1;
                stream_off = (buf[4] >> 4) & 0x1;
                if (stream_on)
                {
                    stream_en = 1;
                }
                if (stream_off)
                {
                    stream_en = 0;
                    stim_rep = 0;
                }

                artifact_on = (buf[3] >> 7) & 0x1;
                artifact_off = (buf[3] >> 6) & 0x1;
                if (artifact_on)
                {
                    artifact_en = 1;
                    buf_save = 0;
                    buf_good = NUM_DATA_BUF-1;
                    buf_tx = 1;
                }
                if (artifact_off)
                {
                    artifact_en = 0;
                }

                if ((buf[3] >> 5) & 0x01)
                {
                	stim_rep = (buf[1] << 8 | buf[2]) - 1;
                    stim_nm0 = (buf[3] >> 3) & 0x01;
                    stim_nm1 = (buf[3] >> 4) & 0x01;

                    if (stim_nm0)
                    {
                        NMstim(0x09 | (1 << 10), 0);
                        if (stim_rep > 0)
                        {
                            *n0d2 = 0x01 << 16;
                            *n0d1 = OP_READ;
                            stat = *status;
                            stat_flag = true;
                            *status = NM0_START;
                        }
                    }
                    else if (stim_nm1)
                    {
                        NMstim(0x09 | (1 << 10), 1);
                        if (stim_rep > 0)
                        {
                            *n1d2 = 0x01 << 16;
                            *n1d1 = OP_READ;
                            stat = *status;
                            stat_flag = true;
                            *status = NM1_START;
                        }
                    }
                }
			}
			else if (buf[0] == 0xDD)
			{
				dead_len = (buf[1] << 8 | buf[2]);
				cl_count = 0;
				rand_mode = (buf[4] >> 4) & 0x01;
				fake_stim = buf[3] & 0x1;
				derivative = (buf[3] >> 1) & 0x01;
				derivativeB = (buf[3] >> 2) & 0x01;
				fft_size_sel = (buf[4] >> 5) & 0x7;

				// change fftSize variable, redefine new CFFT structure, and reallocate all FFT arrays
				switch (fft_size_sel)
				{
				case 0:
					fftSize = 16;
					bitrev_factor = 7;
					S_CFFT = &arm_cfft_sR_q15_len16;
					break;
				case 1:
					fftSize = 32;
					bitrev_factor = 6;
					S_CFFT = &arm_cfft_sR_q15_len32;
					break;
				case 2:
					fftSize = 64;
					bitrev_factor = 5;
					S_CFFT = &arm_cfft_sR_q15_len64;
					break;
				case 3:
					fftSize = 128;
					bitrev_factor = 4;
					S_CFFT = &arm_cfft_sR_q15_len128;
					break;
				case 4:
					fftSize = 256;
					bitrev_factor = 3;
					S_CFFT = &arm_cfft_sR_q15_len256;
					break;
				case 5:
					fftSize = 512;
					bitrev_factor = 2;
					S_CFFT = &arm_cfft_sR_q15_len512;
					break;
				case 6:
					fftSize = 1024;
					bitrev_factor = 1;
					S_CFFT = &arm_cfft_sR_q15_len1024;
					break;
				case 7:
					fftSize = 2048;
					bitrev_factor = 0;
					S_CFFT = &arm_cfft_sR_q15_len2048;
					break;
				default:
					fftSize = 256;
					bitrev_factor = 3;
					S_CFFT = &arm_cfft_sR_q15_len256;
				}
				// reallocate buffers with new size
				for (i=0; i<FFT_BUFFERING; i++)
				{
					free(fft_data_buf[i]);
					fft_data_buf[i] = malloc(fftSize*2 * sizeof(q15_t*));
					ASSERT(fft_data_buf[i]);
				}
				free(mag_data_buf);
				mag_data_buf = malloc(fftSize * sizeof(q31_t*));
				ASSERT(mag_data_buf);

				if ((buf[4] >> 3) & 0x01)
				{
					cl1_en = 0;
					// clear fft_data_buf and mag_data_buf whenever we reset
					for (j=0; j<FFT_BUFFERING; j++)
					{
						for (i=0; i<2*fftSize; i++)
						{
							fft_data_buf[j][i] = 0;
						}
					}
					for (i=0; i<fftSize; i++)
					{
						mag_data_buf[i] = 0;
					}
				}
				if ((buf[4] >> 2) & 0x01)
				{
					cl1_en = 1;
					fft_i = 0;
					fft_i2 = fftSize;
					fft_fsm = 0;
					mag_i = 0;
				}
				if ((buf[4] >> 1) & 0x01)
				{
					cl0_en = 0;
					// clear fft_data_buf and mag_data_buf whenever we reset
					for (j=0; j<FFT_BUFFERING; j++)
					{
						for (i=0; i<2*fftSize; i++)
						{
							fft_data_buf[j][i] = 0;
						}
					}
					for (i=0; i<fftSize; i++)
					{
						mag_data_buf[i] = 0;
					}
				}
				if ((buf[4] >> 0) & 0x01)
				{
					cl0_en = 1;
					fft_i = 0;
					fft_i2 = fftSize;
					mag_i = 0;
					fft_fsm = 0;
				}

				num_bins = fbin_max - fbin_min + 1;

				for (i=0; i<num_bins; i++)
				{
					bitrev_bins[i] = bitrevtable[(fbin_min+i) << bitrev_factor];
				}

				num_bins2 = fbin_max2 - fbin_min2 + 1;

				for (i=0; i<num_bins2; i++)
				{
					bitrev_bins2[i] = bitrevtable[(fbin_min2+i) << bitrev_factor];
				}
			}
			else if (buf[0] == 0xEE)
			{
				ctrl_dir = (buf[2] >> 7) & 0x01;
				ctrl_thresh = ((buf[2] & 0x7F) << 8) | buf[3];
				if ((buf[1] >> 7) & 0x01)
				{
					ctrl_thresh = -ctrl_thresh;
				}
				if ((buf[4] >> 7) & 0x01)
				{
					ch_order = 1;
				}
				else
				{
					ch_order = 2;
				}
                ch_ctrl = buf[1] & 0x7F;
                ch_stim = buf[4] & 0x7F;
			}
			else if (buf[0] == 0xCC)
			{
				fbin_max = ((buf[1] & 0x03) << 8) | buf[2];
				fbin_min = ((buf[3] & 0x03) << 8) | buf[4];
			}
			else if (buf[0] == 0xAB)
			{
				fbin_max2 = ((buf[1] & 0x03) << 8) | buf[2];
				fbin_min2 = ((buf[3] & 0x03) << 8) | buf[4];
			}
			else if (buf[0] == 0xCD)
			{
				ctrl_en2 = (buf[1] >> 7) & 0x01;
				ctrl_dir2 = (buf[2] >> 7) & 0x01;
				ctrl_thresh2 = ((buf[2] & 0x7F) << 8) | buf[3];
				if (buf[1] & 0x01)
				{
					ctrl_thresh2 = -ctrl_thresh2;
				}
				ctrl_comb = (buf[4] >> 7) & 0x01;
			}
			else if (buf[0] == 0xBB)
			{
				rand_max = (buf[1] << 8) | buf[2];
				rand_min = (buf[3] << 8) | buf[4];
			}
			else if ((unsigned char)buf[0] <= 0x4f)
			{
				reg_val = buf[4] + ((uint32_t)buf[3] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[1] << 24);
				reg_addr = (reg_val>>16) & 0xFFFF;
				reg_data = reg_val & 0xFFFF;
				*(int*)(REG_BASE+buf[0]) = reg_val;
				while((*status) & (NM0_TXBUSY | NM1_TXBUSY));

				if (rec_enable_write)
				{
					rec_enable_write = false;

					if (buf[0] == 0x24)
					{
						// n1d2, need to check for rec_enable
						if (reg_addr == NM_REC_ENABLE0_REG)
						{
							// channels 64-79
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+64] = 1;
								}
								else
								{
									sel_data_vector[i+64] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE1_REG)
						{
							// channels 80-95
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+80] = 1;
								}
								else
								{
									sel_data_vector[i+80] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE2_REG)
						{
							// channels 96-111
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+96] = 1;
								}
								else
								{
									sel_data_vector[i+96] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE3_REG)
						{
							// channels 112-127
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+112] = 1;
								}
								else
								{
									sel_data_vector[i+112] = 0;
								}
							}
						}
					}

					else if (buf[0] == 0x14)
					{
						// n0d2, need to check for rec_enable
						if (reg_addr == NM_REC_ENABLE0_REG)
						{
							// channels 0-15
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i] = 1;
								}
								else
								{
									sel_data_vector[i] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE1_REG)
						{
							// channels 16-31
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+16] = 1;
								}
								else
								{
									sel_data_vector[i+16] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE2_REG)
						{
							// channels 32-47
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+32] = 1;
								}
								else
								{
									sel_data_vector[i+32] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE3_REG)
						{
							// channels 48-63
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+48] = 1;
								}
								else
								{
									sel_data_vector[i+48] = 0;
								}
							}
						}
					}
				}

				if (((buf[0] == 0x10) || (buf[0] == 0x20)) && (reg_data == 1))
				{
					rec_enable_write = true;
				}
			}
		}
		if (!stat_flag)
		{
			stat = *status;
		}
		stat_flag = false;

		gui_request_counter++;
		// poll SPI slave
		if (gui_request_counter>2000)
		{
			reg_data_buf[0] = 0x0; // header byte indicates no data
			MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
			MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE - 1, spi_rx_buf, sizeof(spi_rx_buf));
			MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
			spi_rx_handler();
		}

		// Data packet format in FIFO: ALARM and CRC_ERROR in word 0, data in words 1-64, CRC in word 65
		// DO THIS ON TIMER INTERRUPT
		// check for incoming ADC data
        if ((stat & NM0_DATA)) {
            chan = 1;
            stim_flag = false;

            // read in data to temporary buffer, keeping track of stim flag
            // read NM0 adc data
            for(i = 0; i < 66; i++)
			{
				if (i == 0)
				{
					flags = *n0adc;
				}
				else if (i == 65)
				{
					crcflag0 = *n0adc;
				}
				else
				{
					dummy = *n0adc;
                    if (i-1 == ch_stim)
                    {
                        adc_data_buf[buf_save][chan] = dummy;
                        chan++;
                    }
                    if (i-1 == ch_ctrl)
                    {
                        adc_data_buf[buf_save][chan] = dummy;
                        chan++;
                    }
                    if (!stim_flag && (dummy & 0x8000))
                    {
                        stim_flag = true;
                    }
				}
			}
			// read NM1 adc data
			for(i = 0; i < 66; i++)
			{
				if (i == 0)
					flags = *n1adc;
				else if (i == 65)
					crcflag1 = *n1adc;
				else
				{
					dummy = *n1adc;
                    if (i+63 == ch_stim)
                    {
                        adc_data_buf[buf_save][chan] = dummy;
                        chan++;
                    }
                    if (i+63 == ch_ctrl)
                    {
                        adc_data_buf[buf_save][chan] = dummy;
                        chan++;
                    }
                    if (!stim_flag && (dummy & 0x8000))
                    {
                        stim_flag = true;
                    }
				}
			}

			// check if we need to do the interpolation
			if (artifact_en)
			{
				if (stim_flag)
				{
					// we have stim, can't do anything yet, just keep track of how many consecutive samples artifact is
					stim_count++;
				}
				else
				{
					// no stim flag, need to check if this is the end of a stim artifact
					if (stim_count > 0)
					{
						// there was previous stim, meaning we now overwrite with interpolated data
						for (chan=1; chan<=NUM_ENABLED_CHANNELS; chan++)
						{
							step = (adc_data_buf[buf_save][chan] - adc_data_buf[buf_good][chan])/(stim_count + 1);
							if (chan != ch_order)
							{
								for (i=1; i<=stim_count; i++)
								{
									buf_idx = (buf_good+i) & (NUM_DATA_BUF-1);
									adc_data_buf[buf_idx][chan] = (adc_data_buf[buf_good][chan] + (step*i)) | 0x8000;
								}
							}
							else
							{
								for (i=1; i<=stim_count; i++)
								{
									buf_idx = (buf_good+i) & (NUM_DATA_BUF-1);
									adc_data_buf[buf_idx][chan] = (adc_data_buf[buf_good][chan] + (step*i));
								}
							}
						}
						stim_count = 0;
					}
					buf_good = buf_save;
				}
			}

			// send to closed loop algorithm and transmit
			if ((crcflag0 & 0x1) && stream_en)
			{
				counter++;	//counter for fake data
				fft_cl((int16_t) (adc_data_buf[buf_tx][ch_order] & 0x7FFF));
				adc_data_buf[buf_tx][DATA_BUF_SIZE-1] = counter;

				MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
				MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) adc_data_buf[buf_tx]) + 1, 2*DATA_BUF_SIZE - 1, spi_rx_buf, sizeof(spi_rx_buf));
				MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
				spi_rx_handler();
			}
			else
			{
				crc_count++;
			}

			buf_save = (buf_save+1) & (NUM_DATA_BUF-1);
			buf_tx = (buf_tx+1) & (NUM_DATA_BUF-1);
		}

        if (stim_rep > 0) // if we still have a stim train to start
		{
        	if (stat & NM0_ACK) // stim is happening on NM0
			{
        		for(i = 0; i < 4; i++)
				{
					tmp = *n0ack;
					if (i == 1)
						crcok = (tmp == 0x01);
					if (i == 2)
						ret = tmp;
					if (i == 3)
						crcok &= (tmp & 1);
				}
        		if (crcok & !(ret & (1 << 15))) // previous stim ended, so start a new train
        		{
        			stim_rep--;
        			NMstim(0x09 | (1 << 10), 0);
        			if (stim_rep > 0)
        			{
        				*n0d2 = 0x01 << 16;
						*n0d1 = OP_READ;
						stat = *status;
						stat_flag = true;
						*status = NM0_START;
        			}
        		}
        		else
        		{
        			*n0d2 = 0x01 << 16;
					*n0d1 = OP_READ;
					stat = *status;
					stat_flag = true;
					*status = NM0_START;
        		}
			}
            else if (stat & NM1_ACK) // stim is happening on NM1
            {
                for(i = 0; i < 4; i++)
                {
                    tmp = *n1ack;
                    if (i == 1)
                        crcok = (tmp == 0x01);
                    if (i == 2)
                        ret = tmp;
                    if (i == 3)
                        crcok &= (tmp & 1);
                }
                if (crcok & !(ret & (1 << 15))) // previous stim ended, so start a new train
                {
                    stim_rep--;
                    NMstim(0x09 | (1 << 10), 1);
                    if (stim_rep > 0)
                    {
                        *n1d2 = 0x01 << 16;
                        *n1d1 = OP_READ;
                        stat = *status;
                        stat_flag = true;
                        *status = NM1_START;
                    }
                }
                else
                {
                    *n1d2 = 0x01 << 16;
                    *n1d1 = OP_READ;
                    stat = *status;
                    stat_flag = true;
                    *status = NM1_START;
                }
            }
        }
        else // not doing stim, so continue with register operations
        {
            if (stat & NM0_ACK)
			{
				reg_data_buf[0] = 0xFF00; // header byte
				for(i = 0; i < 4; i++)
				{
					if (i == 0)
						flags = *n0ack;
					else if (i == 3)
						crcflag = *n0ack;
					else
						reg_data_buf[i] = *n0ack;
				}
				if (crcflag & 0x1)
				{
					if (data_ack_req & 0x01)
					{
						MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE - 1, spi_rx_buf, sizeof(spi_rx_buf) );
						MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						spi_rx_handler();
						data_ack_req &= ~0x01;
					}
				}
			}

			if (stat & NM1_ACK)
			{
				reg_data_buf[0] = 0xFF00; // header byte
				for(i = 0; i < 4; i++)
				{
					if (i == 0)
						flags = *n1ack;
					else if (i == 3)
						crcflag = *n1ack;
					else
						reg_data_buf[i] = *n1ack;
				}
				if (crcflag & 0x1)
				{
					if (data_ack_req & 0x02)
					{
						MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE - 1, spi_rx_buf, sizeof(spi_rx_buf) );
						MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						spi_rx_handler();
						data_ack_req &= ~0x02;
					}
				}
			}
        }
	}
	return 0;
}

void spi_rx_handler()
{
	// check if there is meaningful data on SPI rx (indicated by non-zero header byte)
	// if so, throw it in ring buffer.
	gui_request_counter=0;
	int i;
	if (spi_rx_buf[0] == 0xAA) {
 		for(i = 0; i < 5; i++)
		{
			if ((rbwrptr+1)%RINGBUF_SIZE != rbrdptr)
			{
				ringbuf[rbwrptr] = spi_rx_buf[i+1];
				rbwrptr = (rbwrptr+1) % RINGBUF_SIZE;
			}
		}
	}
}

void GPIO5_IRQHandler()
{
	// The MPU6050 data ready signal triggers this interrupt.
	// Since the only flags available are data ready and FIFO overflow we set a timer to emulate a "mostly full" flag.
	MSS_GPIO_clear_irq(MSS_GPIO_5);
	// When this timer expires, Timer1_IRQHandler() sets a flag. In while(1) if the flag is set we check the MPU6050 FIFO.
	MSS_TIM1_start();
	MSS_TIM1_load_immediate(MPU6050_FIFO_DELAY);
}

void Timer1_IRQHandler()
{
	MSS_TIM1_clear_irq();
	mpu6050_irq_active = 1;
}
