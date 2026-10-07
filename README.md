# Pose Estimation: YOLOV8S Pose

## Build up the ENV
1. The env is using Docker from [RZ/V2H AI SDK 8.0](https://www.renesas.com/en/document/sws/rzv2h-ai-sdk-v800-source-code)
2. [Installing DRP-AI TVM1 with Docker (Mera2)](https://github.com/renesas-rz/rzv_drp-ai_tvm/tree/Release-2026-06-30/setup#installing-drp-ai-tvm1-with-docker-mera2)

## Build the application
(Note: In docker env...)
1. Please refer to [Application Example for RZ/V2H and RZ/V2N](https://github.com/renesas-rz/rzv_drp-ai_tvm/blob/Release-2026-06-30/apps/build_appV2H.md).  Or you can follow below execution.

   ```bash
   cp -af app_hand_landmarker_cam/ $TVM_ROOT/how-to/sample_app_v2h/
   cd $TVM_ROOT/how-to/sample_app_v2h/app_hand_landmarker_cam/src
   cp $TVM_ROOT/how-to/sample_app_v2h/app_deeplabv3_cam/src/CMakeLists.txt CMakeLists.txt
   mkdir build
   cd build
   
   cmake -DCMAKE_TOOLCHAIN_FILE=$TVM_ROOT/apps/toolchain/runtime.cmake -DAPP_NAME=app_hand_landmarker ..
   sed -i -e 's/INPUT_CAM_TYPE 0/INPUT_CAM_TYPE 1/g' ../define.h # Not executed when using a USB camera.
   make -j$(nproc)
   ```

2. The `app_hand_landmarker` application binary is generated.

## AI models

1. [hand_landmark_256.onnx](./hand_landmark_256.onnx) downloaded from [How_to_download_ONNX_models.md](https://github.com/renesas-rz/rzv_drp-ai_tvm/blob/Release-2026-06-30/docs/model_list/how_to_convert/How_to_download_ONNX_models.md).

2. Compile model
You can using [compile_onnx_model_quant_handlandmark.py](./compile_onnx_model_quant_handlandmark.py) in this git repository
or follow below commmand to modify compile_onnx_model_quant.py in docker

```bash
cd $TVM_ROOT/tutorials
git checkout compile_onnx_model_quant.py
sed -i \
  -e 's/F\.center_crop(img, 224)/F.center_crop(img, 256)/g' \
  -e 's/mean    = \[0\.485, 0\.456, 0\.406\]/mean    = [0.0, 0.0, 0.0]/g' \
  -e 's/stdev   = \[0\.229, 0\.224, 0\.225\]/stdev   = [1.0, 1.0, 1.0]/g' \
  -e 's/config\.shape_in     = \[1, 480, 640, 3\]/config.shape_in     = [1, 480, 640, 2]/g' \
  -e 's/config\.format_in    = drpai_param\.FORMAT\.BGR/config.format_in    = drpai_param.FORMAT.YUYV_422/g' \
  -e 's/cof_add = \[-m\*r for m in mean\]/cof_add = [0.0, 0.0, 0.0]/g' \
  -e 's/cof_mul = \[1\/(s\*r) for s in stdev\]/cof_mul = [1\/255.0] \* 3/g' \
  -e '/config\.ops = \[/a\        op.Crop(80, 0, 480, 480),' \
  compile_onnx_model_quant.py
```

* This is for using a USB camera.


### Run compile_onnx_model_quant_handlandmark.py
Note: !! Before compile, make sure compile_onnx_model_quant_handlandmark.py place in $TVM_ROOT/tutorials/

```bash
python3 compile_onnx_model_quant_handlandmark.py \
 hand_landmark_256.onnx \
 -o hand_landmark_256 \
 -t $SDK \
 -d $TRANSLATOR \
 -c $QUANTIZER \
 -s 1,3,256,256 \
 -v 100 \
 --images [YOUR QUANTIZE IMAGE PATH]
```

* About [YOUR QUANTIZE IMAGE PATH]:
* You can use below python to take 80~150 .jpg includes: 
1. left and right hand
2. hand size from close and far distances
3. Various hand angles and gestures
4. Different backgrounds
5. No hands picture

```
import cv2, os
os.makedirs("calib", exist_ok=True)
cap = cv2.VideoCapture(0)
cap.set(3, 640); cap.set(4, 480)
n = 0
while True:
    ok, f = cap.read()
    cv2.imshow("calib", f)
    k = cv2.waitKey(1) & 0xFF
    if k == ord(" "):
        cv2.imwrite(f"calib/{n:03d}.jpg", f); n += 1
    if k == ord("q"): break
```

## Setup the Execution Environment  

### 1. Copy and archive files

```bash
cd $TVM_ROOT/how-to/sample_app_v2h/app_hand_landmarker_cam
mkdir sample_hand_landmarker_cam
cp $TVM_ROOT/obj/build_runtime/v2h/lib/* sample_hand_landmarker_cam/
cp $TVM_ROOT/how-to/sample_app_v2h/app_hand_landmarker_cam/src/build/app_hand_landmarker sample_hand_landmarker_cam/
cp -r $TVM_ROOT/tutorials/hand_landmark_256/ sample_hand_landmarker_cam/
tar cvfz sample_hand_landmarker.tar.gz sample_hand_landmarker_cam/
```

## Run the application

### 1. Connecting Camera and Display

- Camera:
  - Use a MIPI camera:
    - Please refer to the [e-con Systems product page](https://www.e-consystems.com/renesas/sony-starvis-imx462-ultra-low-light-camera-for-renesas-rz-v2h.asp) for information on obtaining e-CAM22_CURZH
    - Please connect e-con Systems e-CAM22_CURZH to the MIPI connector (CN7) on the EVK board  
    <img src=./app_hand_landmarker_cam/img/connect_e-cam22_curzh_to_rzv2h_evk.png width=700>
  - Use a USB camera:
    - Please connect USB camera as shown below on the EVK board
      <table>
        <tr>
          <th>RZ/V2H EVK</th>
          <th>RZ/V2N EVK</th>
        </tr>
        <tr>
          <td><img src=./app_hand_landmarker_cam/img/hw_conf_v2h.png width=600></td>
          <td><img src=./app_hand_landmarker_cam/img/hw_conf_v2n.png width=600></td>
        </tr>
      </table>
- Display: Please connect to the HDMI port on the EVK board

### 2. **(On RZ/V Board)** Copy and Try it

For example, as follows.
  ```sh
  scp <yourhost>:sample_hand_landmarker.tar.gz .
  tar xvfz sample_hand_landmarker.tar.gz
  cd sample_hand_landmarker_cam/
  su
  export LD_LIBRARY_PATH=.
  /root/gstreamer_cam_test_CAM0_CN7.sh 1920x1080 # Execute only when using MIPI camera
  ./app_hand_landmarker
  exit  # After terminating the application.
  ```

  > **Note1:** For RZ/V2H and RZ/V2N AI SDK v6.00 and later, you need to switch to the root user with the `su` command when running an application.  
  This is because when you run an application from a weston-terminal, you are switched to the "weston" user, which does not have permission to run the `/dev/xxx` device used in the application.
  
  > **Note2:** The chmod +x <filename> command is necessary if the *.tar.gz file or the application file does not have execution permission.

### 3. Following window shows up on HDMI screen

<img src=./app_hand_landmarker_cam/img/application_result_on_hdmi_hand_landmarker.jpg width=480>

On application window, following information is displayed.

- Camera capture
- Object Detection result (Bounding boxes, class name and score.)  
- Processing times
  - Total AI Time: Processing time taken for AI inference and its pre/post-processes. \[msec\]
  - Inference: Processing time taken for AI inference. \[msec\]
  - PreProcess: Processing time taken for AI pre-processes. \[msec\]
  - PostProcess: Processing time taken for AI post-processes. \[msec\]

### 4. How Terminate Application

To terminate the application, press `Super(Window) + Tab` keys to display the Linux console terminal of RZ/V2H or RZ/V2N Evaluation Board Kit and press `Enter` key on there.

### 5. Logs

The `<timestamp>_app_hand_landmarker_cam.log` file is to be generated under the `logs` folder and is to be recorded the text logs of AI inference results and AI processing time and rate.

```txt
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] ************************************************
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info]   RZ/V2H DRP-AI Sample Application
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info]   Model : mediapipe HandLandmarker | hand_landmark_256
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info]   Input : XXXX Camera
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] ************************************************
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] [START] Start DRP-AI inference...
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] Inference ----------- No. 1
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] PreRuntime In Addr (Physical):
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] raw[0]=0.xxxxxx raw[1]=0.xxxxxx lm0=(0.xxxxxx, 0.xxxxxx, 0) lm8=(0.xxxxxx, 0.xxxxxx, 0.xxxxxx)
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] Hand score : 0.00015842915 , handedness : 0.2775879
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] Total AI Time  : xx.x [ms]
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] PreProcess     : xx.x [ms]
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] Inference      : xx.x [ms]
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] PostProcess: x.x [ms]

[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] [START] Start DRP-AI Inference...
[XXXX-XX-XX XX:XX:XX.XXX] [logger] [info] Inference ----------- No. 2
```
