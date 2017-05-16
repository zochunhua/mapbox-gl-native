const child_process = require('child_process');

const submodulePinsToMaster = child_process.execSync('git -C mapbox-gl-js branch -a --contains `git -C mapbox-gl-js rev-parse HEAD`').toString().split('\n').indexOf('  remotes/origin/master') >= 0;
if (!submodulePinsToMaster) {
    fail("mapbox-gl-js submodule pin is not yet merged to master");
}
