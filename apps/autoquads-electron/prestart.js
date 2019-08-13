const fs = require('fs-extra');
const path = require('path')
let files = [];
let latestBuildsFolderPath = '../../build/node-addon/latest-builds/';
let electronComponentsFolderPath = '../components-electron/';

fs.readdirSync(latestBuildsFolderPath).forEach(file => {
    let ext = path.extname(file)
    if (ext === '.node') {
        files.push(file);
    }
});

files.sort(function (a, b) {
    return a.localeCompare(b);
});

let latestModulePath = latestBuildsFolderPath + files[files.length - 1];

fs.copy(latestModulePath, './autoquads.node', function (err) {
    if (err){
        console.log('An error occured while copying latest node-addon build.')
        return console.error(err)
    }
    console.log('Copy "autoquads.node" completed!')
});

fs.copy(electronComponentsFolderPath, './components', function (err) {
    if (err){
        console.log('An error occured while copying electron components.')
        return console.error(err)
    }
    console.log('Copy "electron-components" completed!')
});