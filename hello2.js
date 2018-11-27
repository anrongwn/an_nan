
//stdio
const cp = require('child_process');
const util = require('util');
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

/*
child.stdout.on('data', (data) => {
    console.log(`recv child data: ${data}`);
})
*/

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

const send_message_len = async function (message_len) {
    return new Promise((resolve, reject) => {
        child.stdin.write(message_len, (error) => {
            if (error === undefined) {
                console.log(`write message_len  : '${message_len}'`);
                resolve('write_len_ok');
            } else {
                console.log(`write message_len error, ${error}`);
                reject('write_len_error');
            }
        });
    });
};

const send_message_data = async function (message) {
    return new Promise((resolve, reject) => {
        child.stdin.write(message, (error) => {
            if (error === undefined) {
                console.log(`write message  : ${message}`);
                resolve('write_message_ok');
            } else {
                console.log(`write message error, ${error}`);
                reject('write_message_error');
            }
        });
    });
};

let reqsid = 0;
const send_count = (200000 + 1);

const interval_cb = async function () {
    let curDate = new Date();
    let message = getTime('Y-M-d H:m:s.ms'); //curDate.toLocaleString();
    message += ' hello win32...>>>';
    message += (reqsid+=1).toString();

    if (reqsid === send_count) {
        message = '@EOT@EOT'; //exit code
        //clearInterval(intervalid);
    }

    if (reqsid <= send_count) {
        let message_buf = Buffer.from(message, 'ascii');
        let message_len = Buffer.alloc(4, 0, 'ascii');
        message_len.writeInt32LE(message.length);
        message = null;
        curDate = null;

        let v = await send_message_len(message_len);
        console.log(`sent_message_len result : ${v}`);

        let v1 = await send_message_data(message_buf);
        console.log(`send_message_data result : ${v1}`);
        message_buf = null;
    }
    

    if (reqsid > send_count) {
        return;
    }
}

//开始发送
interval_cb();

/*
let intervalid = setInterval(() => {
    return;
}, 1000);
*/
process.on('SIGINT', () => {
    console.log(`app Received SIGINT.  process:${process.pid} exit(3).`);

    child.kill('SIGTERM');

    process.exit(3);
});

child.stdout.on('data', (data) => {
    console.log(`recv child data: ${data}`);

    //再次发送
    interval_cb();
})
