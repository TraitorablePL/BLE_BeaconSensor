<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="20008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="BLE.lvlib" Type="Library" URL="../BLE.lvlib"/>
		<Item Name="BLE_Action.ctl" Type="VI" URL="../BLE_Action.ctl"/>
		<Item Name="BLE_Data.ctl" Type="VI" URL="../BLE_Data.ctl"/>
		<Item Name="BLE_Data_Packet.ctl" Type="VI" URL="../BLE_Data_Packet.ctl"/>
		<Item Name="BLE_Data_TargetDevice.ctl" Type="VI" URL="../BLE_Data_TargetDevice.ctl"/>
		<Item Name="BLE_DataAquisition.vi" Type="VI" URL="../BLE_DataAquisition.vi"/>
		<Item Name="BLE_Request.ctl" Type="VI" URL="../BLE_Request.ctl"/>
		<Item Name="BLE_Request_Command.ctl" Type="VI" URL="../BLE_Request_Command.ctl"/>
		<Item Name="Controller_Request.ctl" Type="VI" URL="../Controller_Request.ctl"/>
		<Item Name="Controller_Request_Command.ctl" Type="VI" URL="../Controller_Request_Command.ctl"/>
		<Item Name="Controller_Response.ctl" Type="VI" URL="../Controller_Response.ctl"/>
		<Item Name="Controller_Response_Command.ctl" Type="VI" URL="../Controller_Response_Command.ctl"/>
		<Item Name="LittleEndianToFloat.vi" Type="VI" URL="../LittleEndianToFloat.vi"/>
		<Item Name="LittleEndianToInt16.vi" Type="VI" URL="../LittleEndianToInt16.vi"/>
		<Item Name="Main_Data.ctl" Type="VI" URL="../Main_Data.ctl"/>
		<Item Name="Main_Data_ACQ.ctl" Type="VI" URL="../Main_Data_ACQ.ctl"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="mscorlib" Type="VI" URL="mscorlib">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
