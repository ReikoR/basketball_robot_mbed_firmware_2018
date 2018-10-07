const gulp = require('gulp');
const spawn = require('child_process').spawn;
const fs = require('fs');
const path = require('path');
const drivelist = require('drivelist');

function getDrives(callback) {
    const validDisks = [];

    drivelist.list(function(error, disks) {
        if (error) {
            console.log(error);
        }

        console.log(disks);

        disks.forEach(function (disk) {
            if (/mbed/gi.exec(disk.description)) {
                validDisks.push(disk);
            }
        });

        callback(validDisks);
    });
}

function copyBin(callback) {
    getDrives(function (drives) {
        console.log('---');
        console.log(drives);

        if (drives.length > 0 && drives[0].mountpoints.length > 0) {
            console.log(drives[0].mountpoints);

            const targetPath = drives[0].mountpoints[0].path + '//mbed_firmware.bin';

            let direcory = 'BUILD/LPC1768/GCC_ARM/';

            let fileNames = fs.readdirSync(direcory);

            let binFileNames = fileNames.filter(function (fileName) {
                return fileName.match(/.*\.bin/ig);
            });

            console.log('binFileNames', binFileNames);

            if (binFileNames.length === 1) {
                copyFile(direcory + binFileNames[0], targetPath, callback);

            } else if (binFileNames.length === 1) {
                console.log('No *.bin files found in', direcory);
                callback();

            } else {
                console.log(binFileNames.length, '*.bin files found in', direcory);
                callback();
            }

        } else {
            console.log('No drives found');

            callback();
        }
    });
}

gulp.task('compile', function(callback) {
    //https://docs.mbed.com/docs/mbed-os-handbook/en/latest/dev_tools/build_profiles/
    const child = spawn('mbed', [
        'compile',
        '-t', 'GCC_ARM',
        '-m', 'LPC1768',
        '--profile', 'debug'
    ]);

    child.stdout.on('data', (data) => {
        console.log(data.toString());
    });

    child.stderr.on('data', (data) => {
        console.log(`stderr: ${data}`);
    });

    child.on('close', (code) => {
        //console.log(`child process exited with code ${code}`);
        callback(code === 0 ? null : "Compile failed");
    });
});

gulp.task('program', function(callback) {
    copyBin(callback);
});

gulp.task('compile-program', ['compile'], function(callback) {
    copyBin(callback);
});

gulp.task('run', ['compile-program']);

/*function copyFile(source, target, cb) {
    var cbCalled = false;

    var rd = fs.createReadStream(source, {
        encoding: "binary"
    });

    rd.on("error", function(err) {
        console.log(err);
        done(err);
    });

    var wr = fs.createWriteStream(target, {
        encoding: "binary"
    });

    wr.on("error", function(err) {
        console.log(err);
        done(err);
    });

    wr.on("close", function(ex) {
        done();
    });

    rd.pipe(wr);

    function done(err) {
        if (!cbCalled) {
            cb(err);
            cbCalled = true;
        }
    }
}*/

/*function copyFile(source, target, cb) {
    fs.readFile(source, {encoding: "binary"}, function (err, data) {
        if (err) {
            cb(err);
        } else {
            fs.writeFile(target, data, {encoding: "binary"}, function (err) {
                if (err) {
                    cb(err);
                } else {
                    cb();
                }
            });
        }
    });
}*/

function copyFile(source, target, cb) {
    //console.log('Copying from', source, 'to', target);
    console.log('Copying from', path.normalize(source), 'to', path.normalize(target));

    const child = spawn('copy', [
        path.normalize(source), path.normalize(target),
        '/Y'
    ], {shell: true});

    child.stdout.on('data', (data) => {
        console.log(data.toString());
    });

    child.stderr.on('data', (data) => {
        console.log(`stderr: ${data}`);
    });

    child.on('error', (error) => {
        console.log(error);
    });

    child.on('close', (code) => {
        //console.log(code);
        cb(code === 0 ? null : "Copy failed");
    });
}