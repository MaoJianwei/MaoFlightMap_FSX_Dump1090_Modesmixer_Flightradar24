#include "maoflightmap.h"
#include <QtWidgets/QApplication>

#include "IPCuser.h"

int main(int argc, char *argv[])
{
	// 打包部署应用时，运行如下命令，准备好所有依赖库
	// C:\Qt\Qt5.9.6\5.9.6\msvc2015\bin\windeployqt.exe --release --force --compiler-runtime --no-quick-import --no-translations ./MaoFlightMap.exe


	if (false)
	{
		DWORD dwResult;

		int altitude;
		int radioAltitude;

		long long latitudeL;
		long long longitudeL;

		char lat;
		double latitude;
		int latDegree;
		int latMinute;
		int latSecond;

		char lon;
		double longitude;
		int lonDegree;
		int lonMinute;
		int lonSecond;
		char planeModel[24];
		short gForce;
		char ICAO[5] = { 0 };

		if (FSUIPC_Open(SIM_ANY, &dwResult))
		{
			while (true)
			{
				//机型
				if (!FSUIPC_Read(0x3500, 24, planeModel, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error altitude");

				//最近机场的ICAO
				if (!FSUIPC_Read(0x0658, 4, ICAO, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error altitude");

				//G-force(FSX)
				if (!FSUIPC_Read(0x11BA, 2, &gForce, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error altitude");
				double aa = gForce / 625.0;

				//海拔
				if (!FSUIPC_Read(0x0574, 4, &altitude, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error altitude");
				altitude *= 3.28084;

				//无线电高度（离地高度）
				if (!FSUIPC_Read(0x31E4, 4, &radioAltitude, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error RadioAlt");
				radioAltitude = radioAltitude * 3.28084 / 65535;



				//纬度、经度
				if (!FSUIPC_Read(0x0560, 8, &latitudeL, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error latitude");

				if (!FSUIPC_Read(0x0568, 8, &longitudeL, &dwResult) ||
					!FSUIPC_Process(&dwResult))
					printf("error longitude");


				latitude = latitudeL * 90.0 / (10001750.0 * 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow
				latDegree = latitude;
				latMinute = (latitude - latDegree) * 60;
				latSecond = ((latitude - latDegree) * 60 - latMinute) * 60;
				lat = latitude > 0 ? 'N' : 'S';

				longitude = longitudeL * 360.0 / (65536.0 * 65536.0	* 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow
				lonDegree = longitude;
				lonMinute = (longitude - lonDegree) * 60;
				lonSecond = ((longitude - lonDegree) * 60 - lonMinute) * 60;
				lon = longitude > 0 ? 'E' : 'W';

				printf("%c,%d,%d,%d\t%c,%d,%d,%d\t%d\t%d\n",
					lat, latDegree, latMinute, latSecond,
					lon, lonDegree, lonMinute, lonSecond,
					altitude,
					radioAltitude);
			}

		}
		else
			printf("connect to FSUIPC fail!");

		FSUIPC_Close(); // Closing when it wasn't open is okay, so this is safe here
		return 0;
	}


	QApplication a(argc, argv);
	MaoFlightMap w;
	w.show();
	return a.exec();
}
