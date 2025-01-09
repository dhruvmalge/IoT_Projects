import cv2
from flask import Flask, Response
from flask_cors import CORS
import random
import numpy as np  # To handle array operations

app = Flask(__name__)
CORS(app)

cap = cv2.VideoCapture(0)  # Use the default camera (0)
cap.set(3, 520)  # Width
cap.set(4, 520)  # Height

# Load YOLO
yolo_net = cv2.dnn.readNet("yolov3.weights", "yolov3.cfg")

# Set the backend and target to CUDA (GPU)
yolo_net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
yolo_net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA)

layer_names = yolo_net.getLayerNames()
output_layers = [layer_names[i - 1] for i in yolo_net.getUnconnectedOutLayers()]

# Load COCO names (class names)
with open("coco.names", "r") as f:
    classes = [line.strip() for line in f.readlines()]

def generate_frames():
    while True:
        success, frame = cap.read()
        if not success:
            break
        
        blob = cv2.dnn.blobFromImage(frame, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
        yolo_net.setInput(blob)
        outputs = yolo_net.forward(output_layers)
        
        class_ids = []
        confidences = []
        boxes = []
        height, width, channels = frame.shape

        for out in outputs:
            for detection in out:
                scores = detection[5:]
                class_id = np.argmax(scores)
                confidence = scores[class_id]
                if confidence > 0.2:                      
                    center_x = int(detection[0] * width)
                    center_y = int(detection[1] * height)
                    w = int(detection[2] * width)
                    h = int(detection[3] * height)

                    x = int(center_x - w / 2)
                    y = int(center_y - h / 2)

                    boxes.append([x, y, w, h])
                    confidences.append(float(confidence))
                    class_ids.append(class_id)
        
        indices = cv2.dnn.NMSBoxes(boxes, confidences, 0.2, 0.2)         
        if len(indices) > 0:
            for i in indices.flatten():
                x, y, w, h = boxes[i]
                randColor = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
                frame = cv2.rectangle(frame, (x, y), (x + w, y + h), color=randColor, thickness=3)
                
                label = classes[class_ids[i]]
                cv2.putText(frame, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, randColor, 2)

        ret, buffer = cv2.imencode('.jpg', frame)
        if not ret:
            continue
    
        frame = buffer.tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

@app.route('/video_feed.mp4')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def index():
    return '''
    <html>
        <body>
            <h1>Camera Stream</h1>
            <img src="/video_feed" width="640" height="480"/>
        </body>
    </html>
    '''

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

cap.release()
cv2.destroyAllWindows()
