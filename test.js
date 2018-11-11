const dgram = require('dgram');
const socket = dgram.createSocket('udp4');

const mbedPort = 8042;
const mbedAddress = '192.168.4.1';

var currentTime = Date.now();
var prevTime = currentTime;

var maxSpeeds = [100, 100, 100, 100, 15000, 20, 1600];
var minSpeeds = [-100, -100, -100, -100, 500, 0, 1050];
var speedsSteps = [0, 0, 0, 0, 10, 0, 10];
var speedDirectionsUp = [true, true, true, true, true, true];
var feedback = {};
const commandBuffer = Buffer.alloc(17);
const commandObject =  {
    speeds: [0, 0, 0, 0, 0, 0, 1500],
    fieldID: 'A',
    robotID: 'B',
    shouldSendAck: false,
};

function updateCommandBuffer() {
    const speeds = commandObject.speeds;

    let i;

    for (i = 0; i < speeds.length; i++) {
        commandBuffer.writeInt16LE(speeds[i], 2 * i);
    }

    commandBuffer.writeUInt8(commandObject.fieldID.charCodeAt(0), 2 * i);
    commandBuffer.writeUInt8(commandObject.robotID.charCodeAt(0), 2 * i + 1);
    commandBuffer.writeUInt8(commandObject.shouldSendAck ? 1 : 0, 2 * i + 2);

    commandObject.shouldSendAck = false;
}

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

    feedback = {
        speed1: msg.readInt16LE(0),
        speed2: msg.readInt16LE(2),
        speed3: msg.readInt16LE(4),
        speed4: msg.readInt16LE(6),
        speed5: msg.readInt16LE(8),
        speed6: msg.readInt16LE(10),
        ball1: msg.readUInt8(12),
        ball2: msg.readUInt8(13),
        isSpeedChanged: msg.readUInt8(14),
        refereeCommand: String.fromCharCode(msg.readUInt8(15)),
        time: msg.readInt32LE(16),
    };

    if (feedback.refereeCommand === 'P') {
        commandObject.shouldSendAck = true;
    }

    console.log(('0' + (currentTime - prevTime)).slice(-2), feedback);

    prevTime = currentTime;
});

socket.on('listening', () => {
    const address = socket.address();
    console.log(`socket listening ${address.address}:${address.port}`);

    var value = 0;
    const speeds = commandObject.speeds;

    setInterval(function () {
        for (let i = 0; i < speeds.length; i++) {
            let newSpeed = speeds[i] + (speedDirectionsUp[i] ? speedsSteps[i] : - speedsSteps[i])

            if (speedDirectionsUp[i] && newSpeed > maxSpeeds[i]) {
                newSpeed = maxSpeeds[i];
                speedDirectionsUp[i] = false;
            } else if (!speedDirectionsUp[i] && newSpeed < minSpeeds[i]) {
                newSpeed = minSpeeds[i];
                speedDirectionsUp[i] = true;
            }

            speeds[i] = newSpeed;
        }

        speeds[0] = 0;
        speeds[1] = 0;
        speeds[2] = 0;
        speeds[3] = 0;
        speeds[4] = 0;
        speeds[5] = 0;
        speeds[6] = 1500;

        updateCommandBuffer();
        //console.log(commandObject);
        //console.log(commandBuffer);
        socket.send(commandBuffer, 0, commandBuffer.length, mbedPort, mbedAddress);

        //pipeMotorSpeed = Math.sin(value) * 1000;
        //value += 0.005;
    }, 200);
});

socket.bind(8042);