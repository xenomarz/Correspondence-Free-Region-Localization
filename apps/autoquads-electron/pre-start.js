const fs = require('fs');
const path = require('path')
let files = [];
let relativeBasePath = '../../build/node-addon/latest-builds/';

fs.readdirSync(relativeBasePath).forEach(file => {
    let ext = path.extname(file)
    if (ext === '.node') {
        files.push(file);
    }
});

files.sort(function (a, b) {
    return a.localeCompare(b);
});

let latestModule = relativeBasePath + files[files.length - 1];

fs.createReadStream(latestModule).pipe(fs.createWriteStream('./autoquads.node'));