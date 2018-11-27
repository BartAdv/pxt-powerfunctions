// Auto-generated. Do not edit.
declare namespace powerfunctions {

    /**
     * Move a motor forward.
     */
    //% blockId=pf_move_forward
    //% block="move forward | with motor %motor at speed %speed"
    //% weight=100
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
    //% speed.min=1 speed.max=7 shim=powerfunctions::moveForward
    function moveForward(motor: PowerFunctionsMotor, speed: uint8): void;

    /**
     * Move a motor backward.
     */
    //% blockId=pf_move_backward
    //% block="move backward | with motor %motor at speed %speed"
    //% weight=90
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
    //% speed.min=1 speed.max=7 shim=powerfunctions::moveBackward
    function moveBackward(motor: PowerFunctionsMotor, speed: uint8): void;

    /**
     * Brake then float.
     * The motor's power is quickly reversed and thus the motor will stop abruptly.
     */
    //% blockId=powerfunctions_brake
    //% block="brake| motor %motor"
    //% weight=80
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false" shim=powerfunctions::brake
    function brake(motor: PowerFunctionsMotor): void;

    /**
     * Float a motor to stop.
     * The motor's power is switched off and thus the motor will roll to a stop.
     */
    //% blockId=pf_leeway
    //% block="float | motor %motor | to stop"
    //% weight=70
    //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false" shim=powerfunctions::leeway
    function leeway(motor: PowerFunctionsMotor): void;
}

// Auto-generated. Do not edit. Really.
