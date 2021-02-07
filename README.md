
# Data aquisition via Bluetooth Low Energy
Applications created in this repository provide complex solution for data aquisition from server with sensors, via Bluetooth Low Energy protocol to the PC client using Nordic Dongle or Android Smartphone using dedicated app.

# Used equipment:
- Nordic nRF52840 dongle with S140 7.2.0 Softdevice, SDK 17.0.2
- Waveshare nRF51822 development kit with S130 2.0.1 Softdevice, SDK 12.3.0
- J-Link Edu Programmer

# Repository structure:

Checkout on branch **Engineering_thesis_final** to get state where thesis related features are finished and stable. Changes on **master** might include newer features and be instable.
- [BLE_ClientAndroid](https://github.com/TraitorablePL/BLE_BeaconSensor/tree/master/BLE_ClientAndroid "BLE_ClientAndroid") -- subfolder for developing **Android Client Application** with Android Studio
- [BLE_ClientDevice](https://github.com/TraitorablePL/BLE_BeaconSensor/tree/master/BLE_ClientDevice "BLE_ClientDevice")  -- subfolder for **Central device** working with Nordic USB Dongle nRF52840
- [BLE_ServerDevice](https://github.com/TraitorablePL/BLE_BeaconSensor/tree/master/BLE_ServerDevice "BLE_ServerDevice") -- subfolder for **Peripheral device** working with nRF51822 Custom Beacon Board
- [BLE_Tests](https://github.com/TraitorablePL/BLE_BeaconSensor/tree/master/BLE_Tests "BLE_Tests") -- subfolder for acquistion data related Python scripts
# Environment setup for development:

 1. [Download](https://code.visualstudio.com/Download) code editor environment (Visual Studio Code recommended)
 2. [Download](https://git-scm.com/downloads) and install **Git with Git Bash**
 3. [Download](https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058) and setup **make** package for Git Bash
 4. [Download](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) **GCC ARM Embedded Toolchain**
 5. [Download](https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download#infotabs) **Nordic SDKs** with **Softdevices** (listed above in **Used equipment** section)
 6. Fork repo and  clone it in Git Bash `git clone https://github.com/*your_github_nickname*/BLE_BeaconSensor.git`
 7. Copy and paste :
	 - SDK folders: **components**, **modules**,  **integration** and **external** inside **BLE_ClientDevice** from SDK 17.0.2
	 - SDK folders: **components** and **external** inside **BLE_ServerDevice** from SDK 12.3.0
 8.  Set Git Bash as default terminal in VS Code:
	 - Press **Ctrl + Shift + P**, type **Default Shell** and select Git Bash
 9.  Inside added terminal try to compile one of the projects by entering its directory and using `make`. If there are errors related with ARM GCC, change to version appropriate folder, where you have installed toolchain.
> Cannot find: 'C:/Program Files (x86)/GNU Tools ARM Embedded/92019-q4-major/bin/arm-none-eabi-gcc'. 
> Please set values in:
> "C:/Users/Trait/Repos/BLE_BeaconSensor/BLE_ClientDevice/components/toolchain/gcc/Makefile.windows"`
 10. [Download](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Connect-for-desktop) and install **nRF Connect for Desktop** with J-Link drivers. Inside app check to install **Programmer** Tool.
 11. After make succesfully compiled necessary files, connect your J-Link programmer and open **Programmer** inside **nRF Connect**. (For board connection and J-Link header pinout look into **/BLE_ServerDevice/docs** folder)
 12. Select your device and drag and drop hex files from **_build** directory created after running **make** and **Softdevice** hex from downloaded SDK. **Softdevice** needs to be flashed only once and after that device can be flashed only with compiled hex application.
