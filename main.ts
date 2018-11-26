// Lego Power Functions Infrared Control for PXT
// Based on https://github.com/jurriaan/Arduino-PowerFunctions
// and on https://github.com/philipphenkel/pxt-powerfunctions

// temporarily, till it's exposed for CPX
let irLed = pins.TX
// TODO: figure out how to handle this better
let _toggle = 0

enum PowerFunctionsChannel {
    //% block="1"
    One = 0,
    //% block="2"
    Two = 1,
    //% block="3"
    Three = 2,
    //% block="4"
    Four = 3
}

enum PowerFunctionsDirection {
    //% block="forward"
    Forward = 1,
    //% block="backward"
    Backward = -1
}

enum PowerFunctionsOutput {
    //% block="red"
    Red = 0,
    //% block="blue"
    Blue = 1
}

enum PowerFunctionsMotor {
    //% block="red | channel 1"
    Red1 = 0,
    //% block="red | channel 2"
    Red2 = 1,
    //% block="red | channel 3"
    Red3 = 2,
    //% block="red | channel 4"
    Red4 = 3,
    //% block="blue | channel 1"
    Blue1 = 4,
    //% block="blue | channel 2"
    Blue2 = 5,
    //% block="blue | channel 3"
    Blue3 = 6,
    //% block="blue | channel 4"
    Blue4 = 7
}

enum PowerFunctionsCommand {
    //% block="float"
    Float = 0,
    //% block="forward"
    Forward = 1,
    //% block="backward"
    Backward = 2,
    //% block="brake"
    Brake = 3
}

//% weight=99 color=#0fbc11 icon="\uf0e4" block="Power Functions"
namespace powerfunctions {

    const COMBO_DIRECT_MODE = 0x01
    const SINGLE_PIN_CONTINUOUS = 0x2
    const SINGLE_PIN_TIMEOUT = 0x3
    const SINGLE_OUTPUT = 0x4
    const SINGLE_EXT = 0x6
    const ESCAPE = 0x4

    function getOutput(motor: PowerFunctionsMotor): PowerFunctionsOutput {
        switch (motor) {
            case PowerFunctionsMotor.Red1:
            case PowerFunctionsMotor.Red2:
            case PowerFunctionsMotor.Red3:
            case PowerFunctionsMotor.Red4:
                return PowerFunctionsOutput.Red
            case PowerFunctionsMotor.Blue1:
            case PowerFunctionsMotor.Blue2:
            case PowerFunctionsMotor.Blue3:
            case PowerFunctionsMotor.Blue4:
                return PowerFunctionsOutput.Blue;
        }
    }

    function getChannel(motor: PowerFunctionsMotor) {
        switch (motor) {
            case PowerFunctionsMotor.Red1: return PowerFunctionsChannel.One;
            case PowerFunctionsMotor.Red2: return PowerFunctionsChannel.Two;
            case PowerFunctionsMotor.Red3: return PowerFunctionsChannel.Three;
            case PowerFunctionsMotor.Red4: return PowerFunctionsChannel.Four;

            case PowerFunctionsMotor.Blue1: return PowerFunctionsChannel.One;
            case PowerFunctionsMotor.Blue2: return PowerFunctionsChannel.Two;
            case PowerFunctionsMotor.Blue3: return PowerFunctionsChannel.Three;
            case PowerFunctionsMotor.Blue4: return PowerFunctionsChannel.Four;
        }
    }

    function sendSinglePwm(motor: PowerFunctionsMotor, pwm: number) {
        let output = getOutput(motor)
        let channel = getChannel(motor)
        transport.send(channel, _toggle | channel, SINGLE_OUTPUT | output, pwm)
        transport.toggle()
    }

    /**
    * Move a motor forward.
    */
    //% blockId=pf_move_forward
    //% block="move forward | with motor %motor at speed %speed"
    //% weight=100
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
    //% speed.min=1 speed.max=7
    function moveForward(motor: PowerFunctionsMotor, speed: number) {
        speed = Math.max(-7, Math.min(7, speed))
        sendSinglePwm(motor, speed);
    }

    /**
     * Move a motor backward.
     */
    //% blockId=pf_move_backward
    //% block="move backward | with motor %motor at speed %speed"
    //% weight=90
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
    //% speed.min=1 speed.max=7
    function moveBackward(motor: PowerFunctionsMotor, speed: number): void {
        speed = Math.max(-7, Math.min(7, speed))
        sendSinglePwm(motor, 0xf + 1 - speed);
    }
}

namespace transport {

    const CYCLE = 1000000 / 38000
    const HALF_PERIOD = CYCLE / 2
    const START_STOP_PAUSE = 39 * CYCLE
    const LOW_PAUSE = 10 * 1000000 / 38000
    const HIGH_PAUSE = 21 * 1000000 / 38000

    function sendBit(): void {
        for (let i = 0; i < 6; i++) {
            irLed.digitalWrite(true)
            control.waitMicros(HALF_PERIOD)
            irLed.digitalWrite(false)
            control.waitMicros(HALF_PERIOD)
        }
    }

    function sendStartStopBit(): void {
        sendBit()
        control.waitMicros(START_STOP_PAUSE)
    }

    function pause(channel: PowerFunctionsChannel, count: number): void {
        let pause = 0;

        if (count == 0) {
            pause = 4 - (channel + 1);
        } else if (count < 3) { // 1, 2
            pause = 5;
        } else {  // 3, 4, 5
            pause = 5 + (channel + 1) * 2
        }
        control.waitMicros(pause * 77)
    }

    export function send(channel: PowerFunctionsChannel, nib1: number, nib2: number, nib3: number): void {
        let message = nib1 << 12 | nib2 << 8 | nib3 << 4 | (0xf ^ nib1 ^ nib2 ^ nib3);

        for (let i = 0; i < 6; i++) {
            pause(channel, i)
            sendStartStopBit()
            for (let j = 0; j < 16; j++) {
                sendBit()
                control.waitMicros((0x8000 & (message << j)) != 0 ? HIGH_PAUSE : LOW_PAUSE);
            }
            sendStartStopBit()
        }
    }

    export function toggle(): void {
        _toggle ^= 0x8
    }

}
