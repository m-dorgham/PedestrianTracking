#include "YoloInterface.h"
#include "Properties.h"

YoloInterface::YoloInterface(float thresh, float hier_thresh) : thresh{thresh}, hier_thresh{hier_thresh}
{
    list *options = read_data_cfg("cfg/coco.data");
    char *name_list = option_find_str(options, "names", "data/coco.names");
    labels = get_labels(name_list);

    net = parse_network_cfg("cfg/yolo.cfg");

    load_weights(&net, "data/yolo.weights");

    set_batch_network(&net, 1);
    srand(2222222);
}

YoloInterface::~YoloInterface()
{

}

std::list<cv::Rect> YoloInterface::processFrame(cv::Mat frameRGB, cv::Mat other)
{
    double time;
    int j;
    float nms=.3;

    //convert cv::Mat to darknet image
    IplImage copy = frameRGB;
    image im = ipl_to_image(&copy);
    rgbgr_image(im);
    image sized = letterbox_image(im, net.w, net.h);


    layer l = net.layers[net.n-1];

    box *boxes = (box*) calloc(l.w*l.h*l.n, sizeof(box));
    float **probs = (float**) calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float*) calloc(l.classes + 1, sizeof(float *));
    float **masks = 0;
    if (l.coords > 4){
        masks = (float**) calloc(l.w*l.h*l.n, sizeof(float*));
        for(j = 0; j < l.w*l.h*l.n; ++j) masks[j] = (float*) calloc(l.coords-4, sizeof(float *));
    }

    float *X = sized.data;
    time=what_time_is_it_now();
    network_predict(net, X);

    printf("Predicted in %f seconds.\n", what_time_is_it_now()-time);

    get_region_boxes(l, im.w, im.h, net.w, net.h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);
    if (nms) do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);

    std::list<cv::Rect> rects = getHumansBoundingBoxes(boxes, probs, l.w*l.h*l.n, l.classes);

    free_image(im);
    free_image(sized);
    free(boxes);
    free_ptrs((void **)probs, l.w*l.h*l.n);

    return rects;
}

std::list<cv::Rect> YoloInterface::getHumansBoundingBoxes(box *boxes, float **probs, int total, int classes)
{
    std::list<cv::Rect> rects;
    int i, j;
    for(i = 0; i < total; ++i){
        int class_i = max_index(probs[i], classes);
        float prob = probs[i][class_i];
        if(prob > thresh && strcmp(labels[class_i], "person")==0) {
            printf("%s: %.0f%%\n", labels[class_i], prob*100);

            int left  = (boxes[i].x-boxes[i].w/2.)*Properties::frameWidth;
            int right = (boxes[i].x+boxes[i].w/2.)*Properties::frameWidth;
            int top   = (boxes[i].y-boxes[i].h/2.)*Properties::frameHeight;
            int bot   = (boxes[i].y+boxes[i].h/2.)*Properties::frameHeight;

            if(left < 0) left = 0;
            if(right > Properties::frameWidth-1) right = Properties::frameWidth-1;
            if(top < 0) top = 0;
            if(bot > Properties::frameHeight-1) bot = Properties::frameHeight-1;

            printf("Bounding Box: Left=%d, Top=%d, Width=%d, Height=%d\n", left, top, right-left, bot-top);

            rects.push_back(cv::Rect(left, top, right-left, bot-top));
        }
    }
    return rects;
}

//this function is copied from darknet image.c
image YoloInterface::ipl_to_image(IplImage *src)
{
    int h = src->height;
    int w = src->width;
    int c = src->nChannels;
    image out = make_image(w, h, c);
    ipl_into_image(src, out);
    return out;
}

//this function is copied from darknet image.c
void YoloInterface::ipl_into_image(IplImage *src, image im)
{
    unsigned char *data = (unsigned char *)src->imageData;
    int h = src->height;
    int w = src->width;
    int c = src->nChannels;
    int step = src->widthStep;
    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }
}

