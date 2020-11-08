package com.example.ble_clientandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.util.List;

public class GATTActivity extends AppCompatActivity {

    private BluetoothDevice bluetoothDevice = MainActivity.bluetoothDevice;
    private BluetoothGatt bluetoothGatt;

    private List<BluetoothGattService> gattServices;

    private int connectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public final static String ACTION_GATT_CONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gatt);

        if(connectionState == STATE_DISCONNECTED){
            bluetoothGatt = bluetoothDevice.connectGatt(this, true, gattCallback);
        }
    }

    private final BluetoothGattCallback gattCallback =
            new BluetoothGattCallback() {
                @Override
                public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                                    int newState) {

                    if (newState == BluetoothProfile.STATE_CONNECTED) {

                        Intent intent = new Intent(ACTION_GATT_CONNECTED);
                        connectionState = STATE_CONNECTED;
                        bluetoothGatt.discoverServices();
                        Log.i("BLE_ANDROID", "Connected to GATT server.");
                        Log.i("BLE_ANDROID", "Attempting to start service discovery:");
                        sendBroadcast(intent);
                    } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {

                        Intent intent = new Intent(ACTION_GATT_DISCONNECTED);
                        connectionState = STATE_DISCONNECTED;
                        Log.i("BLE_ANDROID", "Disconnected from GATT server.");
                        sendBroadcast(intent);
                    }

                }

                @Override
                // New services discovered
                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        Intent intent = new Intent(ACTION_GATT_SERVICES_DISCOVERED);
                        sendBroadcast(intent);

                        gattServices = bluetoothGatt.getServices();

                        Log.i("BLE_ANDROID", "Services discovered.");
                        Log.i("BLE_ANDROID", "Services available: " + String.valueOf(gattServices.size()));
                    }
                }
            };

    public void onClickReturn(View view){
        bluetoothGatt.disconnect();
        bluetoothGatt.close();
        bluetoothGatt = null;
        finish();
    }
}