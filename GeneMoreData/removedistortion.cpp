
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <iostream>
#include <dirent.h>
#include <string>
#include <unistd.h>

#define PI 3.14159265

using namespace cv;
using namespace std;

#define CLIP2(minv, maxv, value) (min(maxv, max(minv, value))) // choose the between value
CvMat* cvmGetCol(CvMat * src, int iCol){
    int i;
    int nRow = src->rows;
    CvMat *curCol = cvCreateMat(nRow, 1, src->type);
    for (i = 0; i<nRow; i++)
        cvmSet(curCol, i , 0, cvmGet(src, i , iCol));
    return curCol;
}
/*
Mat * rotateImage (const Mat& source, double angle){
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat* rot_mat;
    cv2DRotationMatrix(src_center, angle, 1.0, rot_mat);
    Mat dst;
    cvWarpAffine(source, dst, rot_mat, source.size());
    return dst;
}
*/

int main (int argc, char *argv[])
{
    //IplImage *img_in = 0, *img_affine = 0, *img_scene=0, *img_interp=0, *img_tmp=0, *tmp_image=0;
    //uchar *data_in;//=(uchar*)malloc(sizeof(IplImage));
    //uchar *data_affine;//=(uchar*)malloc(sizeof(IplImage));
    //uchar *data_scene;//=(uchar*)malloc(sizeof(IplImage));
    //uchar *data_interp;//=(uchar*)malloc(sizeof(IplImage));
    uchar *data_in, *data_affine, *data_scene, *data_interp;
    int height, width, step, channels;
    
    //int i, j, k;
    //double distp_x, distp_y;
    int  curpi, curpj;

    if(argc<3){
        printf("Usage: main filefolder degree\n\7");
        exit(0);
    }

// X_c is the camera image, X_a is the affinely rectified image, X is the ture image
// X_a = H_1*X_c; X_a=H_2*X
// load the distorted image X_c

    DIR *dir;
    struct dirent *ent;
    dir = opendir(argv[1]);
    string filepath, dirpath;
    dirpath = argv[1];
    int counter_image =0;
    while ((ent = readdir(dir)) != NULL) {

    	
        //printf("%s\n", ent->d_name);
        //uchar *data_in=(uchar*)malloc(sizeof(IplImage));
    	//uchar *data_affine=(uchar*)malloc(sizeof(IplImage));
    	//uchar *data_scene=(uchar*)malloc(sizeof(IplImage));
    	//uchar *data_interp=(uchar*)malloc(sizeof(IplImage));
    	//uchar *data_in= new uchar;//malloc(sizeof(IplImage));
    	//uchar *data_affine= new uchar;//malloc(sizeof(IplImage));
    	//uchar *data_scene= new uchar;//malloc(sizeof(IplImage));
    	//uchar *data_interp= new uchar;//malloc(sizeof(IplImage));
        counter_image++;
        cout << "Num of the Processing image : " << counter_image << endl;
        ostringstream convert;
        convert << counter_image;
        string filename_i = convert.str();
        string if_file = ent->d_name;
        if (if_file == ".." || if_file == "."){
            //cout << "test ---" << endl;
            continue;
        }
        printf("%s\n", ent->d_name);

chdir(argv[1]);
IplImage* img_in = cvLoadImage(ent->d_name);
if(!img_in){
    printf("Could not load image file: %s\n", argv[1]);
    exit(0);
}

    // tmp image
    
    int height_tmp, width_tmp;
    width  = img_in->width;
    height = img_in->height;
    height_tmp = height*2;
    width_tmp  = width*2;
    channels = img_in->nChannels;
    IplImage* tmp_image = cvCreateImage(cvSize(width_tmp, height_tmp), IPL_DEPTH_8U, channels);
    cvZero(tmp_image);
    
    /*
    char* input_2 = argv[2];
    int input_2_int = atoi(input_2);
    if(input_2_int == 0) {
    */
        cout << "Put original image into the LEFT-TOP of 2x size image. And Success!" << endl;
        for(int i = 0; i<height; i++) {
            for(int j =0; j <width; j++) {
                cvSet2D(tmp_image, i, j, cvGet2D(img_in, i ,j));
            }
        }
    /*} else {
    height_tmp = height*6;
    width_tmp  = width*6;
    channels = img_in->nChannels;
    tmp_image = cvCreateImage(cvSize(width_tmp, height_tmp), IPL_DEPTH_8U, channels);
    cvZero(tmp_image);

        cout << "Put original image into the RIGHT-TOP of 2x size image. And Success!" << endl;
        for(int i = 0; i<height; i++) {
            for(int j =0; j <width; j++) {
                cvSet2D(tmp_image, i+height, j+width, cvGet2D(img_in, i ,j));
            }
        }
    }
    */

    img_in = tmp_image;
    
    // tmp image

height=img_in->height;
width=img_in->width;
step=img_in->widthStep;
channels=img_in->nChannels;
data_in=(uchar *)img_in->imageData;
printf("Processing a %d x %d image with %d channels\n", height, width, channels);
CvMat *check = cvCreateMat(height, width, CV_64FC1);
cvZero(check);

// allocate the output image and initialize
IplImage* img_affine = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, channels);
IplImage* img_scene = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, channels);
IplImage* img_tmp = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, channels);
cvZero(img_tmp);
cvZero(img_affine);
cvZero(img_scene);
data_affine = (uchar*)img_affine->imageData;
data_scene = (uchar*)img_scene->imageData;

// remove projective distortion
CvMat * vetex = cvCreateMat(3,4,CV_64FC1);

// get the coordinate values of four vetexes of a square
// up-left, up-right, down-left, down-right
/*
for(i=0; i<4; i++)
{
    printf("input the %d the point's coordinate (x', y')", i);
    scanf("%lf %lf",&distp_x,&distp_y);

    //double tmpxp[i];
    //double tmpyp[i];
    //distp_x = tmpxp[i];
    //distp_y = tmpyp[i];
    
      //Mat set function:
      //cvmSet(destination(mat),row,col,src(double value))
      
    cvmSet(vetex,0,i,distp_x);
    cvmSet(vetex,1,i,distp_y);
    cvmSet(vetex,2,i,1);
    printf("\n");
    printf("%lf,%lf",distp_x,distp_y);
    //printf("%f",cvGet2D(vetex,0,0))
    cout <<endl;
    for(int x =0; x< 3;x++)
    {
        for(int y=0; y< 4;y++)
        {
            printf("%lf ",cvmGet(vetex,x,y));

        }
        cout<<endl;
    }


}
*/

    char* input_2 = argv[2];
    int input_2_int = atoi(input_2);
    string affine_name_t;
    string scene_name_t;
    if(input_2_int == 30) {
        //double vetexdata[] = {1124,1324,1124,1324,1537,1532,1727,1732,1,1,1,1};
        //CvMat  tmp_vetex = cvMat(3,4,CV_64FC1, vetexdata);
        //vetex = (&tmp_vetex);
        cvmSet(vetex,0,0,1124);cvmSet(vetex,1,0,1537);cvmSet(vetex,2,0,1); 
        cvmSet(vetex,0,1,1324);cvmSet(vetex,1,1,1532);cvmSet(vetex,2,1,1);
        cvmSet(vetex,0,2,1124);cvmSet(vetex,1,2,1727);cvmSet(vetex,2,2,1);
        cvmSet(vetex,0,3,1324);cvmSet(vetex,1,3,1732);cvmSet(vetex,2,3,1);
        for(int x =0; x< 3;x++)
        {
            for(int y=0; y< 4;y++)
            {
                printf("%lf ",cvmGet(vetex,x,y));
    
            }
            cout<<endl;
        }
        affine_name_t.append("affine_");
        affine_name_t.append("30_");
        affine_name_t.append(filename_i);
        affine_name_t.append(".jpg");
        scene_name_t.append("scene_");
        scene_name_t.append("30_");
        scene_name_t.append(filename_i);
        scene_name_t.append(".jpg");

    }
    if(input_2_int == 45) {
        //double vetexdata[] = {1124,1324,1124,1324,1537,1532,1727,1732,1,1,1,1};
        //CvMat  tmp_vetex = cvMat(3,4,CV_64FC1, vetexdata);
        //vetex = (&tmp_vetex);
        cvmSet(vetex,0,0,1074);cvmSet(vetex,1,0,1540);cvmSet(vetex,2,0,1); 
        cvmSet(vetex,0,1,1374);cvmSet(vetex,1,1,1532);cvmSet(vetex,2,1,1);
        cvmSet(vetex,0,2,1074);cvmSet(vetex,1,2,1724);cvmSet(vetex,2,2,1);
        cvmSet(vetex,0,3,1374);cvmSet(vetex,1,3,1732);cvmSet(vetex,2,3,1);
        for(int x =0; x< 3;x++)
        {
            for(int y=0; y< 4;y++)
            {
                printf("%lf ",cvmGet(vetex,x,y));
    
            }
            cout<<endl;
        }
        affine_name_t.append("affine_");
        affine_name_t.append("45_");
        affine_name_t.append(filename_i);
        affine_name_t.append(".jpg");
        scene_name_t.append("scene_");
        scene_name_t.append("45_");
        scene_name_t.append(filename_i);
        scene_name_t.append(".jpg");

    }
    if(input_2_int == 60) {
        //double vetexdata[] = {1124,1324,1124,1324,1537,1532,1727,1732,1,1,1,1};
        //CvMat  tmp_vetex = cvMat(3,4,CV_64FC1, vetexdata);
        //vetex = (&tmp_vetex);
        cvmSet(vetex,0,0,1024);cvmSet(vetex,1,0,1542);cvmSet(vetex,2,0,1); 
        cvmSet(vetex,0,1,1424);cvmSet(vetex,1,1,1532);cvmSet(vetex,2,1,1);
        cvmSet(vetex,0,2,1024);cvmSet(vetex,1,2,1722);cvmSet(vetex,2,2,1);
        cvmSet(vetex,0,3,1424);cvmSet(vetex,1,3,1732);cvmSet(vetex,2,3,1);
        for(int x =0; x< 3;x++)
        {
            for(int y=0; y< 4;y++)
            {
                printf("%lf ",cvmGet(vetex,x,y));
    
            }
            cout<<endl;
        }
        affine_name_t.append("affine_");
        affine_name_t.append("60_");
        affine_name_t.append(filename_i);
        affine_name_t.append(".jpg");
        scene_name_t.append("scene_");
        scene_name_t.append("60_");
        scene_name_t.append(filename_i);
        scene_name_t.append(".jpg");

    }
    if(input_2_int == 75) {
        //double vetexdata[] = {1124,1324,1124,1324,1537,1532,1727,1732,1,1,1,1};
        //CvMat  tmp_vetex = cvMat(3,4,CV_64FC1, vetexdata);
        //vetex = (&tmp_vetex);
        cvmSet(vetex,0,0,924);cvmSet(vetex,1,0,1545);cvmSet(vetex,2,0,1); 
        cvmSet(vetex,0,1,1524);cvmSet(vetex,1,1,1532);cvmSet(vetex,2,1,1);
        cvmSet(vetex,0,2,924);cvmSet(vetex,1,2,1719);cvmSet(vetex,2,2,1);
        cvmSet(vetex,0,3,1524);cvmSet(vetex,1,3,1732);cvmSet(vetex,2,3,1);
        for(int x =0; x< 3;x++)
        {
            for(int y=0; y< 4;y++)
            {
                printf("%lf ",cvmGet(vetex,x,y));
    
            }
            cout<<endl;
        }
        affine_name_t.append("affine_");
        affine_name_t.append("75_");
        affine_name_t.append(filename_i);
        affine_name_t.append(".jpg");
        scene_name_t.append("scene_");
        scene_name_t.append("75_");
        scene_name_t.append(filename_i);
        scene_name_t.append(".jpg");

    }

    const char * affine_name = affine_name_t.c_str();
    const char * scene_name = scene_name_t.c_str();


// ************* Affine Rectification ****************
CvMat *l1 = cvCreateMat(3,1,CV_64FC1);
CvMat *l2 = cvCreateMat(3,1,CV_64FC1);
CvMat *m1 = cvCreateMat(3,1,CV_64FC1);
CvMat *m2 = cvCreateMat(3,1,CV_64FC1);

CvMat *v1 = cvCreateMat(3,1,CV_64FC1);
CvMat *v2 = cvCreateMat(3,1,CV_64FC1);

CvMat *vanishL = cvCreateMat(3,1,CV_64FC1);

// p28 result2.4: l = X x X'
cvCrossProduct(cvmGetCol(vetex,0), cvmGetCol(vetex,1), l1);
cvCrossProduct(cvmGetCol(vetex,2), cvmGetCol(vetex,3), l2);
cvCrossProduct(cvmGetCol(vetex,0), cvmGetCol(vetex,2), m1);
cvCrossProduct(cvmGetCol(vetex,1), cvmGetCol(vetex,3), m2);
// p27 result2.2: X = l x l'
cvCrossProduct(l1, l2, v1);
cvCrossProduct(m1, m2, v2);

cvCrossProduct(v1, v2, vanishL);

// normalize vanishing line
// in order to map the distorted image back to the image window
// the result of following code is vanishL(x,y,1)
double scale = 1.0;
cvmSet(vanishL,0,0,cvmGet(vanishL,0,0)/cvmGet(vanishL,2,0)*scale);
cvmSet(vanishL,1,0,cvmGet(vanishL,1,0)/cvmGet(vanishL,2,0)*scale);
cvmSet(vanishL,2,0,1.0*scale);


/*
     [1 0 0]
  H1=[0 1 0]
     [x y 1] ->vanishL
  */
double H1data[] = {1,0,0,0,1,0,cvmGet(vanishL,0,0),cvmGet(vanishL,1,0),cvmGet(vanishL,2,0)};
CvMat H1 = cvMat(3,3,CV_64FC1, H1data);

// transform X_a = H_1 * X_c, where X_a is the affinely rectified image.
CvMat * ptx = cvCreateMat(3,1,CV_64FC1);
CvMat * ptxp = cvCreateMat(3,1,CV_64FC1);

cvmSet(&H1,2,2,1.0);
for (int i=0; i<height; i++){    // y - ver
    for(int j=0; j<width; j++){    // x - ver
        // set X_c
        cvmSet(ptxp,0,0,(double)j);
        cvmSet(ptxp,1,0,(double)i);
        cvmSet(ptxp,2,0,1.0);

        // compute X_a
        cvMatMul(&H1, ptxp, ptx);    // cvMatMul(src1, src2, dst)  , dst = src1*src2
        curpi = CLIP2(0, height-1, (int)(cvmGet(ptx,1,0)/cvmGet(ptx,2,0)));
        curpj = CLIP2(0, width-1, (int)(cvmGet(ptx,0,0)/cvmGet(ptx,2,0)));

        cvSet2D(img_affine, curpi, curpj, cvGet2D(img_in,i,j));
        cvmSet(check, curpi,curpj,1);
    }
}

//  output reconstructed affine image
/*if (img_interp != NULL) {
	cvReleaseImage(&img_interp);
}*/
IplImage* img_interp = cvCloneImage(img_affine);
data_interp = (uchar *)img_interp->imageData;

// interpolation
double count;
for(int i=1; i<height-1; i++){      // y - ver
    for (int j=1; j<width -1; j++){     // x - ver
        if(cvmGet(check,i,j)== 0){
            count = (cvmGet(check,i-1,j)==1)+(cvmGet(check,i+1,j)==1)+
                    (cvmGet(check,i,j-1)==1)+(cvmGet(check,i,j+1)==1)+
                    (cvmGet(check,i-1,j-1)==1)+(cvmGet(check,i-1,j+1)==1)+
                    (cvmGet(check,i+1,j-1)==1)+(cvmGet(check,i+1,j+1)==1);
            if(count != 0){
                for(int k=0; k <channels; k++)
                    data_interp[i*step+j*channels+k] = (int)((data_affine[(i-1)*step+j    *channels+k]+data_affine[(i+1)*step+j    *channels+k]+
                                                              data_affine[i    *step+(j-1)*channels+k]+data_affine[i    *step+(j+1)*channels+k]+
                                                              data_affine[(i-1)*step+(j-1)*channels+k]+data_affine[(i-1)*step+(j+1)*channels+k]+
                                                              data_affine[(i+1)*step+(j-1)*channels+k]+data_affine[(i+1)*step+(j+1)*channels+k]
                                                              )/count);
            }
        }
    }
}
//if (img_affine != NULL) {
	cvReleaseImage(&img_affine);
//
img_affine = cvCloneImage(img_interp);
chdir("..");
if(!cvSaveImage(affine_name, img_affine))
    printf("Could not save file\n");

chdir(argv[1]);

    cout << "affine finished" << endl;
// ********************* Metric Rectification ********************
// transform points by H1
CvMat *pt = cvCreateMat(3,1,CV_64FC1);
for(int i=0;i<4;i++)
{
    cvMatMul(&H1, cvmGetCol(vetex,i), pt);
    cvmSet(vetex,0,i,(int)(cvmGet(pt,0,0)/cvmGet(pt,2,0)));
    cvmSet(vetex,1,i,(int)(cvmGet(pt,1,0)/cvmGet(pt,2,0)));
    cvmSet(vetex,2,i,1.0);
}

cvCrossProduct(cvmGetCol(vetex,0), cvmGetCol(vetex,1),l1);
cvCrossProduct(cvmGetCol(vetex,0), cvmGetCol(vetex,2),m1);
cvCrossProduct(cvmGetCol(vetex,0), cvmGetCol(vetex,3),l2);
cvCrossProduct(cvmGetCol(vetex,2), cvmGetCol(vetex,1),m2);

double l11, l12, m11, m12, l21, l22, m21, m22;
l11 = cvmGet(l1,0,0); l12 = cvmGet(l1,1,0);
l21 = cvmGet(l2,0,0); l22 = cvmGet(l2,1,0);
m11 = cvmGet(m1,0,0); m12 = cvmGet(m1,1,0);
m21 = cvmGet(m2,0,0); m22 = cvmGet(m2,1,0);

// M*x = b-----p56 (l1*m1,l1*m2+l2m1,l2*m2)(s11,s12,s22)T=0
/*
            [s11 s12 0](m1)
  (l1,l2,l3)[s21 s22 0](m2)=0
            [0   0   0](m3)
  */
double Mdata[] = {l11*m11, l11*m12 + l12*m11, l21*m21, l21*m22 + l22*m21};
double bdata[] = {-l12*m12, -l22*m22};
CvMat M = cvMat(2,2,CV_64FC1, Mdata);
CvMat b = cvMat(2,1,CV_64FC1, bdata);
CvMat *x = cvCreateMat(2,1,CV_64FC1);
cvSolve(&M, &b, x);

// Set matrix S
double Sdata[] = {cvmGet(x,0,0), cvmGet(x,1,0), cvmGet(x,1,0), 1.0};
CvMat S = cvMat(2,2,CV_64FC1, Sdata);

cout <<"S="<<endl;

for(int x =0; x< 2;x++)
{
    for(int y=0; y< 2;y++)
    {

        printf("%lf ", cvmGet(&S,x,y));
    }
    cout<<endl;
}

// SVD S = UDV_T
CvMat* U = cvCreateMat(2,2,CV_64FC1);
CvMat* D = cvCreateMat(2,2,CV_64FC1);
CvMat* V = cvCreateMat(2,2,CV_64FC1);
cvSVD(&S, D, U, V, CV_SVD_U_T|CV_SVD_V_T);

// The flags cause U and V to be returned transposed (does not work well without the transpose flags).
// Therefore, in OpenCV , S = U^T D V
CvMat* U_T = cvCreateMat(2,2,CV_64FC1);
CvMat* sqrtD = cvCreateMat(2,2,CV_64FC1);
CvMat* A = cvCreateMat(2,2,CV_64FC1);
cvPow(D, sqrtD, 0.5);
cvTranspose(U, U_T);
cvMatMul(U_T, sqrtD, A);
cvMatMul(A, V, A);

// Set H2
double t[] = {0, 0};
double H2data[] = {cvmGet(A,0,0), cvmGet(A,0,1),t[0], cvmGet(A, 1, 0), cvmGet(A, 1, 1), t[1], 0, 0, 1};
CvMat H2 = cvMat(3,3,CV_64FC1, H2data);
CvMat *invH2 = cvCreateMat(3,3,CV_64FC1);
cvInvert(&H2, invH2);

cvZero(check);

for(int i=0; i<height ; i++){     // y - ver
    for (int j=0; j < width; j++){    // x - hor
        // set X_a
        cvmSet(ptxp,0,0,(double)j);
        cvmSet(ptxp,1,0,(double)i);
        cvmSet(ptxp,2,0,1.0);
        // compute X
        cvMatMul(invH2, ptxp, ptx);
        curpi = CLIP2(0, height-1, (int)(cvmGet(ptx,1,0)/cvmGet(ptx,2,0)));
        curpj = CLIP2(0, width-1, (int)(cvmGet(ptx,0,0)/cvmGet(ptx,2,0)));

        cvSet2D(img_scene,curpi,curpj,cvGet2D(img_affine,i,j));
        cvmSet(check,curpi,curpj,1);
    }
}

// output reconstructed scene image
//if (img_interp != NULL) {
	cvReleaseImage(&img_interp);
//}
img_interp =  cvCloneImage(img_scene);
data_interp = (uchar *)img_interp->imageData;

// interpolation
for(int i=1; i<height-1;i++){    // y - ver
    for (int j=1; j<width -1; j++){     // x - hor
        if (cvmGet(check,i,j)== 0){
            count = (cvmGet(check, i-1, j)==1)+(cvmGet(check,i+1,j)==1)+(cvmGet(check,i,j-1)==1)+(cvmGet(check,i,j+1)==1);
            if(count !=0){
                for (int k=0; k<channels; k++){
                    data_interp[i*step+j*channels+k]=(int)((data_scene[(i-1)*step+j*channels+k]+
                                                            data_scene[(i+1)*step+j*channels+k]+
                                                            data_scene[i*step+(j-1)*channels+k]+
                                                            data_scene[i*step+(j+1)*channels+k]
                                                            )/count);
                }
            }
        }
    }
}
//if (img_scene != NULL) {
	cvReleaseImage(&img_scene);
//}
img_scene = cvCloneImage(img_interp);
chdir("..");
if(!cvSaveImage(scene_name,img_interp))
    printf("Could not save file\n");
    /*
    double theta = (15*2*CV_PI/180);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int x_ = i*cos(theta) + j*sin(theta);
            int y_ = j*cos(theta) - i*sin(theta);
            if (x_ < 0 || x_> height-1 || y_ < 0 || y_ > width-1){
                continue;
            }
            cvSet2D(img_tmp,x_,y_, cvGet2D(img_scene,i,j) );
        }
    }
    

    
        float m[6];
        CvMat M2 = cvMat(2,3,CV_64FC1, m);

        double factor = 1;

        m[0] = (float) (factor * cos(theta));
        m[1] = (float) (factor * sin(theta));
        m[3] = -m[1];
        m[4] = m[0];
        m[2] = img_scene->width*0.5f;
        m[5] = img_scene->height*0.5f;

        cvGetQuadrangleSubPix(img_scene, img_tmp, &M2);

        //cvWaitKey(1);

    
    cvSaveImage("rotate.jpg", img_tmp);
    */

chdir(argv[1]);
// release the image


//if (img_in !=NULL){
cout << "release img_in" << endl;
cvReleaseImage(&img_in);

//}
//if (img_affine != NULL){
cout << "release img_affine" << endl;
cvReleaseImage(&img_affine);

//}
//if (img_scene != NULL){
cout << "release img_scene" << endl;
cvReleaseImage(&img_scene);

//}
//if (img_interp != NULL){
cout << "release img_interp" << endl;
cvReleaseImage(&img_interp);

//}
//if (img_tmp != NULL){
cout << "release img_tmp" << endl;
cvReleaseImage(&img_tmp);
//}
//if (tmp_image != NULL){
cout << "release tmp_image" << endl;
//cvReleaseImage(&tmp_image);
//}

//cvReleaseImage(&data_in);
//cvReleaseImage(&data_affine);
//cvReleaseImage(&data_scene);
//cvReleaseImage(&data_interp);
//free((void*)data_in);
//free((void*)data_affine);
//free((void*)data_scene);
//free((void*)data_interp);
//delete  data_in;
//delete  data_interp;
//delete  data_scene;
//delete  data_affine;
//return 0;
}


return 0;
}

