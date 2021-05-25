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
		<Item Name="BLE_Data.ctl" Type="VI" URL="../BLE_Data.ctl"/>
		<Item Name="BLE_Data_TargetDevice.ctl" Type="VI" URL="../BLE_Data_TargetDevice.ctl"/>
		<Item Name="BLE_DataAquisition.vi" Type="VI" URL="../BLE_DataAquisition.vi"/>
		<Item Name="BLE_Message.ctl" Type="VI" URL="../BLE_Message.ctl"/>
		<Item Name="BLE_Message_Info.ctl" Type="VI" URL="../BLE_Message_Info.ctl"/>
		<Item Name="BLE_Response.ctl" Type="VI" URL="../BLE_Response.ctl"/>
		<Item Name="BLE_Response_Info.ctl" Type="VI" URL="../BLE_Response_Info.ctl"/>
		<Item Name="Main_Data.ctl" Type="VI" URL="../Main_Data.ctl"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="mscorlib" Type="VI" URL="mscorlib">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
