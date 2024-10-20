import paho.mqtt.client as mqtt
import threading
import zmq

import sys
import cv2
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QVBoxLayout
from PyQt5.QtGui import QImage, QPixmap, QPainter, QColor
from PyQt5.QtCore import QTimer, Qt, QObject, QRunnable, pyqtSignal, pyqtSlot, QThreadPool
import traceback

c = threading.Condition()
flag = [True, True, True, True]

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

class WorkerSignals(QObject):
    finished = pyqtSignal()
    error = pyqtSignal(tuple)
    result = pyqtSignal(object)
    progress = pyqtSignal(int)

class Worker(QRunnable):
    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()

        # Store constructor arguments (re-used for processing)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()

        # Add the callback to our kwargs
        self.kwargs['progress_callback'] = self.signals.progress

    @pyqtSlot()
    def run(self):
        # Retrieve args/kwargs here; and fire processing using them
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            traceback.print_exc()
            exctype, value = sys.exc_info()[:2]
            self.signals.error.emit((exctype, value, traceback.format_exc()))
        else:
            self.signals.result.emit(result)  # Return the result of the processing
        finally:
            self.signals.finished.emit()  # Done

class VideoPlayer(QWidget):
    def __init__(self, video_path, index: int):
        super().__init__()
        self.video_path = video_path
        self.initUI()
        self.index = index

    def initUI(self):
        self.setWindowTitle('Video Player')
        self.setStyleSheet("background-color: black;")
        self.setWindowFlag(Qt.FramelessWindowHint)
        self.label = QLabel(self)
        layout = QVBoxLayout()
        layout.addWidget(self.label)
        self.setLayout(layout)
        self.cap = cv2.VideoCapture(self.video_path)
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_frame)
        self.threadpool = QThreadPool()
        self.timer.start(30)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            height, width, channel = frame.shape
            step = channel * width
            qImg = QImage(frame.data, width, height, step, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(qImg)
            c.acquire()
            if flag[self.index]:
                self.label.setPixmap(self.get_scaled_pixmap(pixmap))
            else:
                self.label.setPixmap(QPixmap())
            c.release()
        else:
            self.cap.set(cv2.CAP_PROP_POS_FRAMES, 0)

    def get_scaled_pixmap(self, pixmap):
        screen_rect = self.screen().geometry()
        screen_width = screen_rect.width()
        screen_height = screen_rect.height()
        pixmap = pixmap.scaled(screen_width, screen_height, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        result = QPixmap(screen_width, screen_height)
        result.fill(QColor('black'))
        painter = QPainter(result)
        x = (screen_width - pixmap.width()) // 2
        y = (screen_height - pixmap.height()) // 2
        painter.drawPixmap(x, y, pixmap)
        painter.end()
        return result

    def zeromq_thread(self, progress_callback):
        print("Starting ZeroMQ thread")
        while True:
            #  Wait for next request from client
            message = socket.recv()
            print(f"Received request: {message}")

            global flag

            if message == b"0":
                c.acquire()
                flag = [True, False, False, False]
                c.release()
            elif message == b"1":
                print("Received message 1")
                c.acquire()
                print("Acquired lock")
                flag = [False, True, False, False]
                print("Set flag")
                c.release()
                print("Released lock")
            elif message == b'2':
                c.acquire()
                flag = [False, False, True, False]
                c.release()
            elif message == b"3":
                c.acquire()
                flag = [False, False, False, True]
                c.release()
            elif message == b"4":
                c.acquire()
                flag = [True, True, True, True]
                c.release()
            else:
                print(message)
                print("Invalid message format, skipping...")
                return

            print(flag)

            socket.send(b"OK")

    def mqWorkerThread(self):
        worker = Worker(self.zeromq_thread)
        self.threadpool.start(worker)


if __name__ =="__main__":
    app = QApplication(sys.argv)

    video_paths = [
        'server/contents/videos/alice.mp4',
        'server/contents/videos/inception.mp4',
        # 'server/contents/videos/sisyphus.mp4',
        # 'server/contents/videos/vertigo.mp4',
    ]
    screens = app.screens()
    print(screens)
    players = []
    for i, video_path in enumerate(video_paths):
        player = VideoPlayer(video_path, i)
        if i+1 < len(screens):
            screen = screens[i+1]
            player.move(screen.geometry().x(), screen.geometry().y())
            player.showFullScreen()
        else:
            player.show()
        if i == 0:
            player.mqWorkerThread()
        players.append(player)

    sys.exit(app.exec_())