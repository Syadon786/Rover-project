const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const Gpio = require("onoff").Gpio;
const app = express();

const WS_PORT = 8888;
const HTTP_PORT = 8000;

const wsServer = new WebSocket.Server({port: WS_PORT}, ()=>console.log(`WS Server is listening at ${WS_PORT}`));

let connectedClients = [];

var publicDir = require('path').join(__dirname,'/Public');
app.use(express.static(publicDir));


wsServer.on('connection', (ws, req) => {
    ws.id = req.socket.remoteAddress.replace(/^.*:/, '');
    console.log(`${ws.id} has connected`);
    connectedClients.push(ws); 
    ws.on('message', data => {
        connectedClients.forEach((ws, i) => {
            if(ws.readyState === ws.OPEN) {             
                let base64data = Buffer.from(data).toString('base64');
                ws.send(base64data);         
            } 
            else
            {
                connectedClients.splice(i, 1);
            }
        })
    });
});
app.get('/client',(req,res)=>res.sendFile(path.resolve(__dirname, 'Public/client.html')));
app.listen(HTTP_PORT, ()=> console.log(`HTTP server listening at ${HTTP_PORT}`));