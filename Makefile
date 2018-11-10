CC			=	gcc
CFLAGS			= 	-DGATEWAY_CODE -DRASPBERRY_COMPILE -std=gnu99
MONGOOSE		=	libraries/mongoose
MODBUS 			=	Modbus
INC			= 	-IRemoteControl -IDrivers -ICommon -I$(MONGOOSE) -I$(MODBUS) -IUtils
LIBS			=	-lpthread -lbcm2835 -lm


debimeter_app: debimeter.o RemoteControlManager.o RemoteControl_Proxy.o  ADS1115Driver.o INA219BDriver.o SerialPortDriver.o ApplicationManager.o RTCManager.o IPManager.o RavenEyenDriver.o ConfigurationManager.o mongoose.o ModbusTCPSlave.o ModbusCore.o ModbusMaster_Core.o ModbusRTUMaster.o ModbusASCIIMaster.o TotalVolumeCalculator.o
	$(CC) $(CFLAGS)  $^ -o debimeter_app $(LIBS)

debimeter.o: debimeter.c
	$(CC) $(CFLAGS) $(INC) -c debimeter.c $(LIBS)


RemoteControlManager.o: RemoteControl/RemoteControlManager.c
	$(CC) $(CFLAGS) $(INC) -c RemoteControl/RemoteControlManager.c $< $(LIBS)

RemoteControl_Proxy.o: RemoteControl/RemoteControl_Proxy.c
	$(CC) $(CFLAGS) $(INC) -c RemoteControl/RemoteControl_Proxy.c $< $(LIBS)

INA219BDriver.o: Drivers/INA219BDriver.c
	$(CC) $(CFLAGS) $(INC) -c Drivers/INA219BDriver.c $< $(LIBS)

ADS1115Driver.o: Drivers/ADS1115Driver.c
	$(CC) $(CFLAGS) $(INC) -c Drivers/ADS1115Driver.c $< $(LIBS)

SerialPortDriver.o: Drivers/SerialPortDriver.c
	$(CC) $(CFLAGS) $(INC) -c Drivers/SerialPortDriver.c $< $(LIBS)

ConfigurationManager.o: ConfigurationManager.c
	$(CC) $(CLAGS) $(INC) -c ConfigurationManager.c $< $(LIBS)

mongoose.o: $(MONGOOSE)/mongoose.c $(MONGOOSE)/mongoose.h
	$(CC) $(CFLAGS) $(INC) -c $(MONGOOSE)/mongoose.c $<

ModbusCore.o: $(MODBUS)/ModbusCore.c
	$(CC) $(CFLAGS) $(INC) -c $(MODBUS)/ModbusCore.c

ModbusTCPSlave.o: $(MODBUS)/ModbusTCPSlave.c
	$(CC) $(CFLAGS) $(INC) -c $(MODBUS)/ModbusTCPSlave.c

ModbusMaster_Core.o: $(MODBUS)/ModbusMaster_Core.c
	$(CC) $(CFLGAS) $(INC) -c $(MODBUS)/ModbusMaster_Core.c $< $(LIBS)

ModbusRTUMaster.o: $(MODBUS)/ModbusRTUMaster.c
		$(CC) $(CFLGAS) $(INC) -c $(MODBUS)/ModbusRTUMaster.c $< $(LIBS)

ModbusASCIIMaster.o: $(MODBUS)/ModbusASCIIMaster.c
	$(CC) $(CFLGAS) $(INC) -c $(MODBUS)/ModbusASCIIMaster.c $< $(LIBS)

RTCManager.o: Utils/RTCManager.c
	$(CC) $(CFLAGS) $(INC) -c Utils/RTCManager.c $<

TotalVolumeCalculator.o: Utils/TotalVolumeCalculator.c
	$(CC) $(CFLAGS) $(INC) -c Utils/TotalVolumeCalculator.c $<

IPManager.o: Utils/IPManager.c
	$(CC) $(CFLAGS) $(INC) -c Utils/IPManager.c $<

RavenEyenDriver.o: Drivers/RavenEyenDriver.c
	$(CC) $(CFLAGS) $(INC) -c Drivers/RavenEyenDriver.c $< $(LIBS)

ApplicationManager.o:	ApplicationManager.c
	$(CC) $(CFLAGS) $(INC) -c ApplicationManager.c $< $(LIBS)

clean:
	rm -rf *.o debimeter
