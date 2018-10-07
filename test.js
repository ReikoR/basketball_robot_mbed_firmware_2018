const dgram = require('dgram');
const socket = dgram.createSocket('udp4');

const mbedPort = 8042;
const mbedAddress = '192.168.4.1';

var currentTime = Date.now();
var prevTime = currentTime;

var speeds = [0, 0, 0, 0, 0];
var maxSpeeds = [100, 100, 100, 100, 1000];
var minSpeeds = [-100, -100, -100, -100, -1000];
var speedsSteps = [1, 2, 3, 4, 5];
var speedDirectionsUp = [true, true, true, true, true];

function clone(obj) {
    var cloned = {};

    for (let key in obj) {
        cloned[key] = obj[key];
    }

    return cloned;
}

socket.on('error', (err) => {
    console.log(`socket error:\n${err.stack}`);
    socket.close();
});

socket.on('message', (msg, rinfo) => {
    //console.log(`socket got: ${msg} from ${rinfo.address}:${rinfo.port}`);

    currentTime = Date.now();
    console.log(('0' + (currentTime - prevTime)).slice(-2),
        msg.readInt16LE(0),
        msg.readInt16LE(2),
        msg.readInt16LE(4),
        msg.readInt16LE(6),
        msg.readInt16LE(8),
        msg.readUInt8(10),
        msg.readUInt8(11),
        msg.readUInt16LE(12),
        msg.readUInt8(14),
        msg.readInt32LE(15)
    );

    prevTime = currentTime;
});

socket.on('listening', () => {
    const address = socket.address();
    console.log(`socket listening ${address.address}:${address.port}`);

    var value = 0;

    setInterval(function () {
        const command = new Int16Array(5);

        for (let i = 0; i < speeds.length; i++) {
            let newSpeed = speeds[i] + (speedDirectionsUp[i] ? speedsSteps[i] : - speedsSteps[i])

            if (speedDirectionsUp[i] && newSpeed > maxSpeeds[i]) {
                newSpeed = maxSpeeds[i];
                speedDirectionsUp[i] = false;
            } else if (!speedDirectionsUp[i] && newSpeed < maxSpeeds[i]) {
                newSpeed = minSpeeds[i];
                speedDirectionsUp[i] = true;
            }

            speeds[i] = newSpeed;

            command[i] = speeds[i];
        }

        /*command[0] = 0;
        command[1] = 0;
        command[2] = 0;
        command[3] = 0;
        command[4] = 0;*/

        var message = new Buffer.from(command.buffer);
        socket.send(message, 0, message.length, mbedPort, mbedAddress);

        //pipeMotorSpeed = Math.sin(value) * 1000;
        //value += 0.005;
    }, 100);
});

socket.bind(8042);