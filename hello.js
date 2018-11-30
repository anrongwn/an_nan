var addon = require('bindings')('hello');
const moment = require('moment');

//var obj = new addon.anObject(10);
var obj = addon(233);

//
console.log(obj.plusOne());

obj.fcb((data) => {
    console.log(data);
});


obj.async_fcb(100, (err, data) => {

    if (err === null)
        console.log(`async callback result : ${data}`);
    else
        console.log(`async callback error code = ${err}`);
});
console.log('async_fcb ok');


let cmd = {
    id: 1,
    param: {
        value1: 'wangjr',
        value2:'jianrong'
    }
}

cmd.id = 100;
if (cmd.param !== undefined) {
    cmd.param.value1 = 'wad';
}
JSON.stringify(cmd);
//cmd.param2 = 'sd';
if (cmd.param2 === undefined) {
    console.log('cmd.param2 === undefined');
}
let scmd = JSON.stringify(cmd);
console.log(scmd);

let ocmd = JSON.parse(scmd);
console.log(ocmd);
console.log(typeof (ocmd));

//stdio
const cp = require('child_process');
const path = require('path');

let win32_cmd = path.join(__dirname, '/bin/node_win32.exe');
console.log(win32_cmd);

const defaults = {
    encoding: 'utf8',
    timeout: 0,
    maxBuffer: 200 * 1024,
    killSignal: 'SIGTERM',
    windowsHide : false,
    cwd: null,
    env: null
};

/**
let child = cp.exec('./bin/node_win32.exe', defaults, (error, stdout, stderr) => {
    //console.log(child);
    console.log(`exec info ${error}, ${stdout}, ${stderr}`);
});
 */
let child = cp.spawn('./src/node_win32/Release/node_win32.exe', ['wangjr', 'nodejs']);
const util = require('util');

child.on('error', (err) => {
    console.log(err);
    process.exit(1);
})

child.stdin.on('error', (error) => {
    console.log(`child.stdin error : ${error}`);
    process.exit(3);
})

/** 
child.stdin.on('finish', () => {
    console.log(`child.stdin finish`);
})
*/

child.stdout.on('data', (data) => {
    console.log(`recv child data: ${data}`);
})

child.on('close', (code, signal) => {
    console.log(`child close ${code}, ${signal}`);
    process.exit(1);
})

child.on('exit', (code, signal) => {
    console.log(`child exit ${child.pid}, ${code}, ${signal}`);
    //process.exit(1);
})


/*
child.stdin.write(message.length.toString(), (error) => {
    console.log(`write message len error, ${error}`);
})
*/
function fix2number(n) {
    return [0,n].join('').slice(-2);
}
function fix3number(n) {
    return [0,n].join('').slice(-3);
}
function getTime(format) {
    var curdate = new Date();
    if (format == undefined) return curDate;
    format = format.replace(/Y/i, curdate.getFullYear());
    format = format.replace(/M/i, fix2number(curdate.getMonth() + 1));
    format = format.replace(/d/i, fix2number(curdate.getDate()));
    format = format.replace(/H/i, fix2number(curdate.getHours()));
    format = format.replace(/m/i, fix2number(curdate.getMinutes()));
    format = format.replace(/s/i, fix2number(curdate.getSeconds()));
    format = format.replace(/ms/i, fix3number(curdate.getMilliseconds()));

    curdate = null;
    return format;
}

let reqsid = 0;
const send_count = 200000+1;

let intervalid = setInterval(() => {
    //let message = getTime('Y-M-d H:m:s.ms'); //curDate.toLocaleString();
    let message = moment().format('YYYY-MM-DD HH:mm:ss.SSS');
    message += ' hello win32...>>>';
    message += (reqsid += 1);

    if (reqsid === send_count) {
        message = '@EOT@EOT'; //exit code
        //clearInterval(intervalid);
    }

    if (reqsid <= send_count) {
        let message_buf = Buffer.from(message, 'ascii');

        //console.log();
        let message_len = Buffer.alloc(4, 0, 'ascii');
        message_len.writeInt32LE(message.length);
        message = null;

        //write message len
        child.stdin.write(message_len, (error) => {
            if (error === undefined) {
                //console.log(`write message_len  : ${message_buf.byteLength}'${message_len}', ${message_len.byteLength}`);
            } else {
                console.log(`write message_len error, ${error}`);
            }
            message_len = null;
        })

        //write message
        child.stdin.write(message_buf, (error) => {
            if (error === undefined) {
                console.log(`write message  : ${message_buf}`);
                
            } else {
                console.log(`write message error, ${error}`);
            }
            message_buf = null;
        })

    }

    if (reqsid > send_count) {
        return;
    }
}, 1);

process.on('SIGINT', () => {
    console.log(`app Received SIGINT.  process:${process.pid} exit(3).`);

    /*
    let message = '%EOT%EOT'; //exit code
    
    let message_buf = Buffer.from(message, 'ascii');

    child.stdin.write(message_buf, (error) => {
        if (error === undefined) {
            console.log(`write message  : ${message_buf}`);
        } else {
            console.log(`write message error, ${error}`);
        }
    });
    */

    child.kill('SIGTERM');

    process.exit(3);
});