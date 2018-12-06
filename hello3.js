
//stdio
const cp = require('child_process');
const util = require('util');
const path = require('path');
const moment = require('moment');

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

const send_message_len = async function (message_len) {
    child.stdin.write(message_len, (error) => {
        if (error === undefined) {
            console.log(`write message_len  : '${message_len}'`);
            return ('write_len_ok');
        } else {
            console.log(`write message_len error, ${error}`);
            throw('write_len_error');
        }
    });
 
};

const send_message_data = async function (message) {
    child.stdin.write(message, (error) => {
        if (error === undefined) {
            console.log(`write message  : ${message}`);
            return ('write_message_ok');
        } else {
            console.log(`write message error, ${error}`);
            throw('write_message_error');
        }
    });

};

let reqsid = 0;
const send_count = (200000 + 1);

const interval_cb = async function () {
    let cmd = {
        cmd: 'wfsopen',
        timestamp: new Date().getTime(),
        timeout: (reqsid += 1),
        data: {
            cmdid: 0,
            servicename: 'CardReader',
            param: {
                spversionreq:'3.03'
            }
        }
        //result:{}
    };
    /*
    let message = moment().format('YYYY-MM-DD HH:mm:ss.SSS');
    message += ' hello win32...>>>';
    message += (reqsid += 1);
    */
    
    let message = JSON.stringify(cmd);
    
    if (reqsid === send_count) {
        message = '@EOT@EOT'; //exit code
        //clearInterval(intervalid);
    }

    if (reqsid <= send_count) {
        let message_buf = Buffer.from(message, 'ascii');
        let message_len = Buffer.alloc(4, 0, 'ascii');
        message_len.writeInt32LE(message.length);

        try {
            let v = await send_message_len(message_len);
            let v1 = await send_message_data(message_buf);
        }
        catch (err) {
            console.log(err);
        }
        
        //clear
        message = null;
        message_buf = null;
        message_len = null;
        cmd = null;
    }
    

    if (reqsid > send_count) {
        return 0;
    }
}

//开始发送
interval_cb();

process.on('SIGINT', () => {
    console.log(`app Received SIGINT.  process:${process.pid} exit(3).`);

    child.kill('SIGTERM');

    process.exit(3);
});

child.stdout.on('data', (data) => {
    
    
    console.log(`recv child data: ${data}`);
    /*
    let cmdresp = JSON.parse(data);
    console.log(`recv child resp: ${JSON.stringify(cmdresp)}`)
    console.log(`cmd=${cmdresp.cmd},rc=${cmdresp.response.rc}`);
    */

    //再次发送
    interval_cb();
})
