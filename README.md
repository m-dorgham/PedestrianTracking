# PedestrianTracking

This is a research project developed during my Master studies in University of Hamburg 2017.

The Target is to detect and track any moving human in RGB videos captured by a single stationary camera.

The approach splits into two main phases:

##### 1. Humans detection
I implemented two approaches for detecting people, the first is by using background subtraction and supporting it by a neural network trained to classify humans, I retrained Inception v3 model via tensorflow (https://www.tensorflow.org/tutorials/image_retraining). 

The second detection method is Yolo (https://pjreddie.com/darknet/yolo/) which gave very good detection results.

##### 2. Human tracking
I used Unscented Kalman filter to keep track of the dynamics of the motion each human, and used the Hungarian algorithm to solve the assignment problem.

I used the tracking submodule from Smorodov's Multitarget-tracker (https://github.com/Smorodov/Multitarget-tracker) but I modified the state change function and the initialization of the initial state.

#### Datasets
* PETS 2009 Benchmark Data [S2, View_005] (http://www.cvg.reading.ac.uk/PETS2009/a.html)
* Kinect Tracking Precision (KTP) dataset (http://www.dei.unipd.it/~munaro/KTP-dataset.html)

#### Demo videos
* Background subtraction with Inception model (https://youtu.be/a6h9dLBJgTU)
* Yolo detection (https://youtu.be/v2D3t0t7gWM)

#### My Observations

* Background Subtraction approach (without the classifier) is pretty fast and suitable for real-time applications but the problem is that it is very sensitive to any change in illumination and consequently produce lots of false positives and wrong detections, which make it non-robust approach for most real-world applications. Even after supporting it with a classifier (which usualy takes away the computational effectiveness advantage) the method still performs bad with sudden change in lighting (try to configure the project to export the binary images to see why).

* Yolo on the other hand gives very good detections for humans (in the 2 datasets I tested) and is robust to the change in lighting conditions, however it is very slow if you are to run it on a CPU (takes from 6 to 20 seconds per image depending on the CPU). You can run it on an expensive GPU and get the image done in less than a second, but that might not always be feasible.

* Linear Kalman filter performs very very bad in tracking the motion of humans since people's motion is highly nonlinear, Unscented kalman filter is better suited for this case.

* Unscented Kalman filter is good for keeping track of occluded persons since it keeps predicting their current position -based on their previous dynamics- even when thier detections disappear for some frames.

* Solving the assignment problem by using only the Euclidean distance (Hungarian algorithm) is not efficient and usually leads to mixing the tracking ids when occlusions happen since the distance will be nearly identical for the two colliding objects.

#### Ideas for future enhancement
The main bottleneck in the tracking problem is solving the assignment problem, as a poor solution usually leads to the mixing of the tracks. Therfore considering color information is important and logical to reduce this probelm. A good approach in my opinion should combine both color features and euclidean distance information.

**Project Usage:**
1. install the prerequisites mentioned in the next section.
2. download yolo pre-trained model from this link (https://pjreddie.com/media/files/yolo.weights) and put it under the subdirectory data/yolo/data/ in PedestrianTracking project.
3. configure the config.cfg file in the project root directory with your own paths and desired parameters.
4. optional: instal tensorflow and build the image_labler example as in this link (https://www.tensorflow.org/tutorials/image_retraining), otherwise you can set useClassifier to false in config.cfg and not use it.

**Prerequisites:**
1. CMake >= 3.1
2. opencv v3.2
3. opencv_contrib v3.2

**Third-party software**
1. ConfigFile by Richard J. Wagner
2. Tracking module from Multitarget-tracker project by Smorodov's  
(https://github.com/Smorodov/Multitarget-tracker)


#### License
GNU GPLv3: http://www.gnu.org/licenses/gpl-3.0.txt 
