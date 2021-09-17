#pip install websockets
#pip install pillow 

import websockets as wb
import cv2 as cv
import numpy as np
import asyncio
import PIL.Image as Image
import io
import base64
import threading
import keyboard
from time import sleep

#Saját
import config

async def listen():
    url = "ws://192.168.1.75:8888";
    counter = 0
    async with wb.connect(url) as websockets: 
                    while config.processing:
                        if(counter >= 2):
                            counter = 0
                        else:
                            msg = await websockets.recv()
                            counter += 1
                            print("Kaptam")
                            await decodeByteStream(msg)

async def decodeByteStream(data):
            b64 = base64.b64decode(data)
            buf = io.BytesIO(b64)
            img = Image.open(buf)
            print("Dekódoltam")
            config.images.append(cv.cvtColor(np.array(img), cv.COLOR_RGB2BGR))

def processImage():
    config.haar_cascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')
    while config.processing:
        if(len(config.images) > 0):
            print(f"Feldolgoztam a képet")
           
           #Remove Images
            img = config.images.pop(0)
            if(len(config.images) > 10):
                config.images.remove(config.images[0])
                config.images.remove(config.images[0])

            gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY) 
            faces_rect = config.haar_cascade.detectMultiScale(gray, scaleFactor = 1.1, minNeighbors = 3, minSize = (100, 100))

            if(len(faces_rect) > 0):
                for (x,y,w,h) in faces_rect:
                    cv.rectangle(img, (x,y), (x+w, y+h), (0,255,0), thickness = 2)    

            cv.imshow('Frame', img)
            cv.waitKey(20)

def quitPrg():
    while config.processing:
        if keyboard.is_pressed('q'):
            config.processing = False
            sleep(300)
            config.images = []
            cv.destroyAllWindows()


def startThreads(processImage, quitPrg):
    thread = threading.Thread(target= processImage)
    thread.start()
    thread2 = threading.Thread(target = quitPrg)
    thread2.start()

def main():
    startThreads(processImage, quitPrg)
    loop = asyncio.get_event_loop()
    loop.run_until_complete(listen())
    loop.close()
    print("Ended")

if __name__ == "__main__":
    main()
