const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const SerialPort = require('serialport');
const app = express();

const WS_PORT = 8888;
const HTTP_PORT = 8000;

const wsServer = new WebSocket.Server({port: WS_PORT}, ()=>console.log(`WS Server is listening at ${WS_PORT}`));
const port = new SerialPort('/dev/ttyUSB0' , {
    baudRate: 9600,
});

let controller = null;
let monitoring = null;
let cam = null;
let process = null;

var publicDir = require('path').join(__dirname,'/Public');
app.use(express.static(publicDir));


wsServer.on('connection', (ws, req) => {
    ws.id = req.socket.remoteAddress.replace(/^.*:/, '');
    if(ws.id == "10.0.0.2")
    {
        monitoring = ws; 
        console.log(`Monitoring has connected`);
    }    
    else if(ws.id == "10.0.0.3")
    {
        controller = ws; 
        console.log(`Controller has connected`);
    }   
    else if(ws.id == "10.0.0.10")
    {
        cam = ws; 
        console.log(`Camera has connected`);
    }
    else if(ws.id == "10.0.0.1") {
        process = ws;
        console.log(`Process link established`);
    }
 

    ws.on('message', data => {   
        if(ws.id != "10.0.0.10" && ws.id != "10.0.0.1")
            console.log(`${ws.id}: ${data}`);    
        if(ws.id == "10.0.0.10") {
            let base64data = Buffer.from(data).toString('base64');
            if(monitoring !== null && monitoring !== undefined )
            {             
               
                if(monitoring.readyState === monitoring.OPEN && data.length > 5000 )
                {
                    
                    monitoring.send("1#" + base64data);         
                }
                else {
                    monitoring.send("3#" + data);
                }
                
            }
            if(process !== null && process !== undefined && data.length > 5000){
                if(process.readyState === process.OPEN)
                {
                    process.send(base64data);
                }
            }
        }      
        else if(ws.id == "10.0.0.3")
        {
            port.write(data);
            console.log(`Forwarded to microcontroller: ${data}`);
        }
        else if(ws.id == "10.0.0.1") {
            let base64data = "2#" + Buffer.from(data).toString('base64');

            if(monitoring !== null && monitoring !== undefined )
            {             
               
                if(monitoring.readyState === monitoring.OPEN)
                {
                    
                    monitoring.send(base64data);       
                }
                
            }
        }
    });
});
app.get('/client',(req,res)=>res.sendFile(path.resolve(__dirname, 'Public/client.html')));
app.listen(HTTP_PORT, ()=> console.log(`HTTP server listening at ${HTTP_PORT}`));