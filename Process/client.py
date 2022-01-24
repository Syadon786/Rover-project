# pip3 install websockets
# pip3 install pillow
# pip3 install opencv3

import websockets as wb
import cv2 as cv
import numpy as np
import asyncio
import io
import PIL.Image as Image
import base64
import threading
from time import sleep

import config


async def listen():
    url = "ws://10.0.0.1:8888"
    async with wb.connect(url) as websockets:
        while True:
            msg = await websockets.recv()
            await decodeByteStream(msg)
            await send(websockets)


async def decodeByteStream(data):
    b64 = base64.b64decode(data)
    buf = io.BytesIO(b64)
    img = cv.cvtColor(np.array(Image.open(buf)), cv.COLOR_RGB2BGR)
    config.images.append(img)


async def send(websocket):
    if (len(config.processed) > 0):
        img = config.processed.pop(0)
        if (len(config.processed) > 10):
            config.processed = []
        _, im_arr = cv.imencode('.jpg', img)
        im_bytes = im_arr.tobytes()
        await websocket.send(im_bytes)


def processImage():
    face_cascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')
    stop_cascade = cv.CascadeClassifier("StopSign_HAAR_Cascade.xml")
    yield_cascade = cv.CascadeClassifier("YieldSign_HAAR_Cascade.xml")
    while True:
        if (len(config.images) > 0):
            img = config.images.pop(0)
            if (len(config.images) > 10):
                config.images = []
            gray = cv.cvtColor(img, cv.COLOR_RGB2GRAY)
            faces_rect = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=3, minSize=(100, 100))
            stop_rect = stop_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=4)
            yield_rect = yield_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=4)

            if (len(faces_rect) > 0):
                for (x, y, w, h) in faces_rect:
                    cv.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), thickness=2)
            if (len(stop_rect) > 0):
                for (x, y, w, h) in stop_rect:
                    cv.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
                    cv.putText(img, "Stop", (x, y - 10),
                               cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4)
            if (len(yield_rect) > 0):
                for (x, y, w, h) in yield_rect:
                    cv.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
                    cv.putText(img, "Yield", (x, y - 10),
                               cv.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4)
            config.processed.append(img)
            cv.waitKey(20)


def startThreads(processImage):
    thread = threading.Thread(target=processImage)
    thread.start()


def main():
    startThreads(processImage)
    loop = asyncio.get_event_loop()
    loop.run_until_complete(listen())
    loop.close()


if __name__ == "__main__":
    main()
