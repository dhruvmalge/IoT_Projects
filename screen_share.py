import io
import time
import pyautogui
import numpy as np
import cv2
from flask import Flask, Response, render_template_string
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

streaming = True

def capture_screen():
    screenshot = pyautogui.screenshot()
    img = np.array(screenshot)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    ret, jpeg = cv2.imencode('.jpg', img)
    return jpeg.tobytes()

def generate_frames_screen():
    global streaming
    while streaming:
        frame = capture_screen()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')
        time.sleep(0.1)

@app.route('/screen_feed.mp4')
def video_feed():
    return Response(generate_frames_screen(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/screen_share')
def index():
    return render_template_string('''
        <html>
            <body>
                <h1>Screen Share Stream</h1>
                <img src="/video_feed" width="100%">
                <br>
                <form action="/stop_stream" method="post">
                    <button type="submit">Stop Screen Share</button>
                </form>
            </body>
        </html>
    ''')

@app.route('/stop_stream', methods=['POST'])
def stop_stream():
    global streaming
    streaming = False
    return '<h1>Screen Share Stopped</h1><p><a href="/">Go back</a></p>'

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
