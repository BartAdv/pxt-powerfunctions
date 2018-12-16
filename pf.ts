namespace powerfunctions {
    //% block="On PF message"
    export function onPFMessage(handler: (msg: number) => void) {
        powerfunctions.onPFMessageRaw(() => {
            let msg: number = powerfunctions.pfMessage();
            handler(msg);
        })
    }
}