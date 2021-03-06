#include "commun.h"


using namespace cv;

Webcam::Webcam()
{

    key = 0;
    windowName1 = "binairisation";
    windowName2 = "flux original";
    h=240;
    s=0;
    v=0;
    ds = 20;
    dh = 100;
    sommeX= 0;
    sommeY= 0;
    nbPixels=0;
    CvPoint positionObj = cvPoint(-1, -1);

}

Webcam ::~Webcam()
{
    //destructor
}

int Webcam::initWindow(const char *name)
{
    // Définition de la fenêtre + redimensionnement :
    cvNamedWindow(name, 0);
    cvResizeWindow(name, 640,480);
    return 0;
}

int Webcam::binairisation()
{
    // Ouverture du flux video :
    capture = cvCreateCameraCapture(0);
    // Init Fen :
    initWindow(windowName1);
    initWindow(windowName2);


    // Verification si l'ouverture du flux est ok :
    if (!capture)
    {
        cout << "Erreur ouverture flux video." << endl;
        return 1;
    }


    // Tant que l'utilisateur n'appuie pas sur la touche 'q' :
    while(key!='q' && key!='Q')
    {

        //récupération du flux video dans la var image
        image = cvQueryFrame(capture);



        //création d'un masque pour isoler
        mask=cvCreateImage(cvGetSize(image), image-> depth, 1);

         // conversion du flux RGB en HSV pour travailler sur la saturation et éviter les problèmes du à la brillance de l'image
        hsv=cvCloneImage(image);
        cvCvtColor(image, hsv, CV_BGR2HSV);

        //création d'un masque pour isoler


        //cout << "H="<< h <<" S=" << s <<" =V" << v <<endl;

        cvInRangeS(hsv,cvScalar(s - ds -1, h - dh, 0),cvScalar(s+ ds -1, h + dh, 255), mask);

        //isolation de l'objetr
        kernel=cvCreateStructuringElementEx(5,5,2,2, CV_SHAPE_ELLIPSE);
        cvDilate(mask, mask, kernel, 1);
        cvErode(mask, mask, kernel, 1);

        // suivi  barycentre objet
        positionObj=calculBarycentre();
        tracking(positionObj);

        cout << " X="<< positionObj.x << " Y=" << positionObj.y << endl;



        // Affichage de l'image dans la fenetre :
        cvShowImage(windowName1, mask);
        cvShowImage(windowName2, hsv);





        // On attend 10 ms :
        key = cvWaitKey(10);




    }

    //libération de la mémoire

    cvReleaseCapture(&capture);
    cvDestroyWindow(windowName1);
    cvDestroyWindow(windowName2);

    return 0;
}


CvPoint Webcam :: calculBarycentre () {

    sommeX=0;
    sommeY=0;
    nbPixels=0;

    for (x=0; x< mask-> width; x++) {
        for (y=0; y< mask->height; y++) {

            if (((uchar*)(mask->imageData + y*mask-> widthStep)) [x] == 255) {
                        sommeX +=x;
                        sommeY +=y;
                        nbPixels ++;
            }
        }
    }

    if(nbPixels > 0)
        return cvPoint ((int)(sommeX/nbPixels),(int)(sommeY/nbPixels));
    else
        return cvPoint(-1, -1);

}

int Webcam :: tracking(CvPoint barycentre) {

    int objectNextStepX, objectNextStepY;
    CvPoint positionAct=barycentre;


    //s'il y a assez de pixel on calcul la prochaine position du cercle
    if (nbPixels > 10) {


        //si le barycentre est or de l'image on ne change pas sa position
        if (positionAct.x == 1 || positionAct.y == -1) {
            positionAct.x = positionAct.x;
            positionAct.y = positionAct.y;
        }

        // change pas à pas la position de l'object vers la position désiriée
        if(abs( positionAct.x - barycentre.x) > 5 ) {
            objectNextStepX = max(5, min (100, abs(positionAct.x - barycentre.x)/2));
            positionAct.x += (-1)* sign(positionAct.x - barycentre.x) * objectNextStepX;
        }

         if(abs( positionAct.y - barycentre.y) > 5 ) {
            objectNextStepY = max(5, min (100, abs(positionAct.y - barycentre.y)/2));
            positionAct.y += (-1)* sign(positionAct.y - barycentre.y) * objectNextStepY;
        }


    }

    else {

        positionAct.x = -1;
        positionAct.y = -1;
    }

    if (nbPixels >10) {
        cvDrawCircle(image, positionAct, 15, CV_RGB(255,0,0), -1);
    }

    initWindow("color tracking");
    cvShowImage("color tracking", image);

    return 0;
}










