# Lab Sens'it

## Prerequesite

1. Locally on your computer: 

Execute: `git clone <YOUR_REPOSITORY>`

2. In your repository, insert in file `README.md`:
```
My Sens'it ID: <YOUR_SENSIT_ID>
```
3. Commit and push your work!

---
---

# PART 1 - Hands on the Sens'it SDK

## Sigfox Cloud

1.  Head over to the [activate](https://buy.sigfox.com/activate).

2. Select your country, enter the device ID & PAC and finish creating your account.

3. Once done, log on the [backend](https://backend.sigfox.com/).

4. Try sending a message by double pressing the button.

5. Click on your device ID and select the **"Messages"** tab. You should now see a Sigfox message.

6. Clone this repository.

## Sens'it development environment

### Linux / Mac OS

1. Download and install [GNU Arm Embedded Toolchain version 7.2.1](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads).

3. Execute: `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`

4. Execute: `brew install dfu-util`

5. Edit: `./sensit-sdk-v2.0.0/sdk/Makefile`:

    - Make sure your `CC`, `BIN_TOOL`, `SIZE_TOOL` paths links to the right folder.
    - Example:
        ```
        CC = /Users/<YOUR_PATH>/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc
        BIN_TOOL  = /Users/<YOUR_PATH>/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-objcopy
        SIZE_TOOL = /Users/<YOUR_PATH>/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-size
        ```

6. In `./sensit-sdk-v2.0.0/sdk/`, execute: `make temperature`

### Windows

1. Download and install [GNU Arm Embedded Toolchain version 7.2.1](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads). **/!\ Make sure to tick the `Add environment path variables` at the end of installation.**

2. Download and unzip [dfu-util-0.9-win64.zip](http://dfu-util.sourceforge.net/releases/).

3. Download [Make for Windows](http://gnuwin32.sourceforge.net/downlinks/make.php).

4. Download [Zadig](https://zadig.akeo.ie/), you can install the STM32 driver once the device is in **bootloader mode** (_cf. 'Program your Sens'it'_ steps 1. to 4.).

5. Edit: `./sensit-sdk-v2.0.0/sdk/Makefile`:

    - Set `LIB_PATH`, `BIN_PATH`, `OBJ_PATH` paths links to the right folder.
    - Example:
        ```
        LIB_PATH = C:/Users/<YOUR_PATH>/sensit-sdk-v2.0.0/sdk/lib
        BIN_PATH = C:/Users/<YOUR_PATH>/sensit-sdk-v2.0.0/sdk/bin
        OBJ_PATH = C:/Users/<YOUR_PATH>/sensit-sdk-v2.0.0/sdk/obj
        ```

6. In `./sensit-sdk-v2.0.0/sdk/` (where the `Makefile` is), execute: `"C:/Users/<YOUR_PATH>/GnuWin32/bin/make.exe" temperature`

Now check on the Sigfox Backend if you received some messages.

## Program your Sens'it

To program your Sens'it you will need to put it in bootloader:

1. Connect your device to your computer.
2. Reset your device. With one of the provided firmwares you can do this with 4 short presses on the button.
3. When the secondary LED starts blinking, do a long press on the button.
4. If both LEDs become white, your device is in **bootloader mode**.
5. Now you can flash:
    - `LINUX/MACOS`: Use the `make prog` command to program your Sens'it.
    - `WINDOWS`: Use the `"C:/Users/<YOUR_PATH>/dfu-util.exe" -a 0 -s 0x08000000:leave -D bin/sensit.bin` command to program your Sens'it.

**Hourray, you just flashed the** `main_TEMPERATURE.c` **firmware.**

Now check on the Sigfox Backend if you received some messages.

## Visualize and decode your messages

1. Head over to the [Sigfox Platform](https://workshop.iotagency.sigfox.com/).
2. Create an account or sign in.
3. Go to the `API` section and keep it open.
4. On the [Sigfox Backend](https://backend.sigfox.com/devicetype/list):
    - Click on your device type **name**.
    - Go to the `CALLBACKS` section.
    - Create a two new callbacks:
        * From step 3. copy and paste the **BIDIR** and **GEOLOC** information.
    - Send a Sigfox message.

Your Sens'it messages must now be appearing on the platform and you should also have the Sigfox geolocation working.

---
---

# PART 2 - Implementing a use case

## Use case

1. Think of a use case with your group.
    - Fill in the blanks [here](https://goo.gl/remJnJ).
    - Describe briefly your use-case idea _(try to keep it business focused if you can)_.

## Implementation

1. Make sure your Sens’it is activated and messages are received on the Sigfox Backend.

2. Use the Sens’it SDK docs.
    - Open index.html in the `./sensit-sdk-v2.0.0/doc/html/` folder.
    
### Program your Sens'it

Implement a new firmware in the `main.c` file (located in `./sensit-sdk-v2.0.0/sdk/src/`).

- To compile use `make main`. This command will also let you verify your code.
- To flash your Sens'it you will need to put it in bootloader:
    1. Connect your device to your computer.
    2. Reset your device. With one of the provided firmwares you can do this with 4 short presses on the button.
    3. When the secondary LED starts blinking, do a long press on the button.
    4. If both LEDs become white, your device is in bootloader.
    5. Then, use the `make prog` command to program your Sens'it.

## Visualize and decode your messages

1. Head over to https://workshop.iotagency.sigfox.com.
2. Create a parser _(use your name as parser name)_ to decode your payloads.
3. Build a dashboard.

## Pitch

1. Download & fill the pitch slides [here](https://goo.gl/E6et8W)
2. Pitch your use case! **(2-3min talk max)**

---
---

# Hint

If you want to return to the **original firmware** of the Sens'it 3, use the following command to program it:
```
dfu-util -a 0 -s 0x08000000:leave -D bin/sensit_discovery_vX.X.X.bin
```
Replace `X.X.X` with the current version of the Discovery firmware available in the `bin` folder.