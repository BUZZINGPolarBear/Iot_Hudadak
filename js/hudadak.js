var awsIot = require('aws-iot-device-sdk');

var device = awsIot.device({
    keyPath: configKey.keyPath,
   certPath: configKey.certPath,
     caPath: configKey.caPath,
   clientId: "mainESP32",
       host: configKey.host
 });