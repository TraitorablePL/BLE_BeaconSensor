package com.example.ble_clientandroid;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_ACCESS_COARSE_LOCATION = 2;
    private static final long SCAN_PERIOD = 3000;
    private static boolean mScanning = false;;

    private ListView devicesList;
    private Button scanButton;
    private Toolbar appToolbar;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothManager bluetoothManager;
    private BluetoothLeScanner bluetoothLeScanner;

    private Handler scanDelayHandler = new Handler();

    private ArrayAdapter<String> listAdapter;
    private ArrayList<BluetoothDevice> scannedDevices;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        appToolbar = findViewById(R.id.toolbar);
        appToolbar.setTitleTextColor(ContextCompat.getColor(getApplicationContext(),R.color.colorPrimaryText));
        setSupportActionBar(appToolbar);
        getSupportActionBar().setDisplayShowTitleEnabled(true);

        scanDelayHandler = new Handler();
        scannedDevices = new ArrayList<BluetoothDevice>();

        bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();

        listAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        devicesList = findViewById(R.id.scannedDevicesList);
        devicesList.setAdapter(listAdapter);

        checkBluetoothState();

        scanButton = findViewById(R.id.scanButton);
        checkLocationPermission();
    }

    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            if(!scannedDevices.contains(result.getDevice())){
                scannedDevices.add(result.getDevice());
                if(result.getDevice().getName() == null)
                    listAdapter.add("Unknown device\n" +  result.getDevice().getAddress());
                else
                    listAdapter.add(result.getDevice().getName() + "\n" +  result.getDevice().getAddress());
                listAdapter.notifyDataSetChanged();
            }
        }
    };

    public void scanDevices(View view) {
        if (bluetoothAdapter != null && bluetoothAdapter.isEnabled() && checkLocationPermission()){
            scannedDevices.clear();
            listAdapter.clear();

            if (!mScanning) {
                Toast.makeText(getApplicationContext(), "Scanning for devices...", Toast.LENGTH_SHORT).show();
                scanDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mScanning = false;
                        bluetoothLeScanner.stopScan(leScanCallback);
                        scanButton.setEnabled(true);
                    }
                }, SCAN_PERIOD);
                mScanning = true;
                bluetoothLeScanner.startScan(leScanCallback);
                scanButton.setEnabled(false);
            }
            else {
                mScanning = false;
                bluetoothLeScanner.stopScan(leScanCallback);
                scanButton.setEnabled(true);
            }
        }
        else{
            checkBluetoothState();
        }
    }

    private boolean checkLocationPermission(){
        if(ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    REQUEST_ACCESS_COARSE_LOCATION);
            return false;
        }
        else{
            return true;
        }
    }

    private void checkBluetoothState(){
        if(bluetoothAdapter == null){
            Toast.makeText(this, "Bluetooth is not supported on your device", Toast.LENGTH_SHORT).show();
        }
        else if(!bluetoothAdapter.isEnabled()){
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    protected void onActivityResult(int reqCode, int resultCode, Intent data){
        super.onActivityResult(reqCode, resultCode, data);

        if(reqCode == REQUEST_ENABLE_BT){
            checkBluetoothState();
        }
    }

    @Override
    public void onRequestPermissionsResult (int requestCode, String[] permissions, int[] grantResults){
        super.onRequestPermissionsResult(requestCode,permissions, grantResults);
        switch (requestCode){
            case REQUEST_ACCESS_COARSE_LOCATION:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    Toast.makeText(this, "Access coarse location allowed. You can scan BLE devices", Toast.LENGTH_SHORT).show();
                }
                else{
                    Toast.makeText(this, "Access coarse location forbidden. You can't scan BLE devices", Toast.LENGTH_SHORT).show();
                }
                break;
        }
    }
}