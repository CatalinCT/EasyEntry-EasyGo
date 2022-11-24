from gpiozero import InputDevice, LED
from time import sleep
from imutils.video import VideoStream
from imutils.video import FPS
import face_recognition
import imutils
import pickle
import time
import cv2

currentname = "unknown"
encodingsP = "encodings.pickle"
acces = LED(21)
no_acces = LED(20)
enable = InputDevice(16)

pic_0 = cv2.imread('/photo/Loading_0_.jpg',cv2.IMREAD_COLOR)
pic_25 = cv2.imread('/photo/Loading_25_.jpg',cv2.IMREAD_COLOR)
pic_50 = cv2.imread('/photo/Loading_50_.jpg',cv2.IMREAD_COLOR)
pic_75 = cv2.imread('/photo/Loading_75_.jpg',cv2.IMREAD_COLOR)
pic_srv = cv2.imread('/photo/SERVICE_MODE.jpg',cv2.IMREAD_COLOR)

def face_ID():
    aux = 0
    currentname = "unknown"
    print("[INFO] loading encodings + face detector...")
    data = pickle.loads(open(encodingsP, "rb").read())
    vs = VideoStream(usePiCamera=True).start()
    time.sleep(1.0)
    fps = FPS().start()
    while True:
        frame = vs.read()
        frame = imutils.resize(frame, width=150)
        boxes = face_recognition.face_locations(frame)
        encodings = face_recognition.face_encodings(frame, boxes)
        names = []
        aux = aux + 1

        for encoding in encodings:
            matches = face_recognition.compare_faces(data["encodings"], encoding)
            name = "Unknown"  # if face is not recognized, then print Unknown

            if True in matches:
                matchedIdxs = [i for (i, b) in enumerate(matches) if b]
                counts = {}
                for i in matchedIdxs:
                    name = data["names"][i]
                    counts[name] = counts.get(name, 0) + 1

                name = max(counts, key=counts.get)

                # If someone in your dataset is identified, print their name on the screen
                if currentname != name:
                    currentname = name
                    print(currentname)
                    acces.on()
                    print('Acces')

            # update the list of names
            names.append(name)

        # loop over the recognized faces
        for ((top, right, bottom, left), name) in zip(boxes, names):
            # draw the predicted face name on the image - color is in BGR
            cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 225), 2)
            y = top - 15 if top - 15 > 15 else top + 15
            cv2.putText(frame, name, (left, y), cv2.FONT_HERSHEY_SIMPLEX, .8, (0, 255, 255), 2)

        # display the image to our screen
        frame = imutils.resize(frame, width=1050)
        cv2.namedWindow("Facial", cv2.WND_PROP_FULLSCREEN)
        cv2.setWindowProperty("Facial", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
        cv2.imshow("Facial", frame)
        key = cv2.waitKey(1) & 0xFF

        if (key == ord("q")):
            break

        fps.update()

    fps.stop()
    print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
    print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

    cv2.destroyAllWindows()
    vs.stop()

def afisare(pic,time):
    pic = imutils.resize(pic, width=1050)
    #cv2.namedWindow('img',cv2.WND_PROP_FULLSCREEN)
    #cv2.setWindowProperty('img',cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)
    cv2.imshow('img',pic)
    cv2.waitKey(time)
    #cv2.destroyAllWindows()

if __name__ == "__main__":
    
    print("Start EasyEntry-EasyGo")
    #time.sleep(60)
    #print("Timer STOP, RUN START")
    afisare(pic_0,8000)
    afisare(pic_25,2000)
    afisare(pic_50,2000)
    afisare(pic_75,2000)
    afisare(pic_srv,3000)
    cv2.destroyAllWindows()
    face_ID()
    if(currentname == "Catalin")
        pic_current = cv2.imread('/photo/Welcome_Catalin.jpg', cv2.IMREAD_COLOR)
        afisare(pic_current,0)
    #if key == ord("q"):
    #    break
