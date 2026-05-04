#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
this func cleans the original P2 file by removing the comment lines
it opens the users original file and read it line by line and checks
- whether the line begins with a #
if the line does begin with a # then the func skips it
    else writes the line into a new output file - clean.pgm
*/
void removeComments(const char *input,const char *output) {
    FILE *in = fopen(input,"r");//opening original pgm file for reading
    FILE *out = fopen(output,"w");//opening for writing
    char line[10000];//buffer to store each line of the file in a new clean manner

    //checking if the file doesnt exists or open
    if (!in) {
        printf("Can not open input file");//letting the user know
        return;}//exiting the program
    if (!out) {
        printf("Can not open output file");//letting the user know
        return;}//exiting the program

    while(fgets(line, sizeof(line),in)) {//reading the file line by line
    int i=0;//variable for reading the lines
    while(line[i]==' ' ||line[i]=='\t'){i++;}//skipping spaces and tabs
    if(line[i]=='#')//if the first char of a line is # then skipping it going to the next line
        continue;
    fputs(line,out);//storing the clean lines in a new file
    }
    fclose(in);//closing input file
    fclose(out);//closing output file
}

/*
this func reads the P2 file and loads all pixel values into the memory.
it reads the header - magic string, width, height and maximum grayscale values
it creates a array of row pointers and stores all pixel values

*/
//saving a new p2 file with no #
int **loadP2(const char *path,int *w, int *h,int *maxval) {
    FILE *fp=fopen(path,"r");//opening the cleaned P2 file
    int r,c;// variables for loop
    if(!fp){//checking if the files opens and exists
        fprintf(stderr,"Can not open file: %s",path);//letting the user know if not
        return NULL;//returning null, exiting program
    }

    char magic[3];//buffer for magic string P2
    fscanf(fp,"%2s",magic);//reading magic from the file
    fscanf(fp,"%d %d",w,h);//reading width and height from the file
    fscanf(fp,"%d",maxval);//reading the max gray value from the file

    int width=*w;//storing the width
    int height=*h;//storing the height

    //allocating array of row pointers
    int **pixels= malloc((size_t)height *sizeof(int*));
    if(!pixels){//checking allocation
        fclose(fp);
        fprintf(stderr,"malloc failed for row pointers\n");
        return NULL;
    }

    //allocating a big contiguous block for all pixels
    int *data = malloc((size_t)width *(size_t)height *sizeof(int));
    if(!data){//checking allocation
        free(pixels);
        fclose(fp);
        fprintf(stderr,"malloc failed for pixel data\n");
        return NULL;
    }

    //setting each row pointer to point into the huge block
    for(r=0;r<height;r++) {pixels[r]= data+(size_t)r *width;}

    //reading the pixel values into the block using the row pointers
    for(r=0;r<height; r++){
        for(c=0;c<width;c++){
            if(fscanf(fp,"%d",&pixels[r][c])!= 1){
                fprintf(stderr,"Error in pixel reading");
                free(data);
                free(pixels);
                fclose(fp);
                return NULL;
            }}
    }
    fclose(fp);//closing file
    return pixels;//returning int** - row pointers
}
/*
this func writes an image stored in a 2D int array into a new P@ image file
it opens the output file for writing, if successful - it prints the header
then it loops through every row and col of the pixel and writes each value into the file making it a calid PGM image
*/

void saveP2(const char *path,int **px, int w, int h, int maxval){
    FILE *fp=fopen(path,"w");//opening the output file
    int r,c;//variables for loop
    if (!fp){//checking is files exists and opens
        fprintf(stderr,"Can not open file for writing");//letting the user know if it doesn't
        return;
    }

    fprintf(fp,"P2\n%d %d\n%d\n",w,h,maxval);//writing the header
    for(r=0;r<h;r++){//looping thru and writing the pixels row wise
        for (c=0;c<w;c++){fprintf(fp,"%d ",px[r][c]);}//writing the pixel values and a space
    fprintf(fp,"\n");//adding a newline after each row
    }
    fclose(fp);                             // close the file
}


int **scaleImage(int **pixels, int width, int height, float scale, int maxval, int *newW, int *newH);
int **avgfilter(int **pixels, int width, int height, int maxval);
int **medianfilter(int **pixels, int width, int height, int maxval);
int **cannyFilter(int **pixels, int width, int height, int maxval);
int **gaussianBlur(int **pixels, int width, int height);
int **thresholdEdges(int **img, int width, int height);
int **nonMaxSuppression(int **grad, int **dir, int width, int height);
void sobel(int **img, int width, int height,int **grad, int **dir);
int **sobelFilter(int **pixels, int width, int height, int maxval);
int **prewittFilter(int **pixels, int width, int height, int maxval);
int **computeLBP(int **pixels, int width, int height, int outMax);
int **meanfilter(int **pixels, int width, int height, int maxval);


int main() {
    char filename[256];//storing user input file name
    char cleanfile[] = "clean.pgm";//new clean file with no comments
    int width,height,maxval;//width, height and maxval of the file
    int **pixels = NULL;//pointer variable to the array
    int choice,newW,newH;//variable to save user choice, new height, new width

    printf("Enter P2 PGM filename (test.pgm): ");//asking the user to enter the file name
    scanf("%255s",filename);//saving the file name

    removeComments(filename,cleanfile);//calling remove Comments func to remove the comments from the file
    pixels = loadP2(cleanfile,&width,&height,&maxval);//calling loadP2 fucn to store the pixel values
    if (!pixels) return 1;//exiting if any error

    printf("Image loaded: %d x %d\n", width, height);

    while (1) {
        choice = MENU();

        if (choice == 1) //if user choice = 1 then
        {
        float scale;//variable to store user input
        printf("Enter scale (2, 0.5 etc): ");//asking user to enter scale
        scanf("%f", &scale);//getting the scale
        int **scaled = scaleImage(pixels, width, height, scale, maxval,&newW, &newH);//calling scale image func to get the zoom/shrinked image
        }

    else if(choice ==2)//if user enters 2
        {
        int ops;//variable to get user input
        printf("\n1. Average Filter\n2. Mean Filter\n3. Median Filter\nEnter a choice: ");//asking the user for the type of filter they want
        scanf("%d", &ops);//getting the user input
        if(ops==1){avgfilter(pixels,width,height,maxval);}//if user enters 1 then, calling avg filter func
        else if(ops==2){meanfilter(pixels,width,height,maxval);}
        else if (ops==3){medianfilter(pixels,width,height,maxval);}//is user enters3 then , calling the median filter func
        else{printf("Invalid input");}//else printing invalid input
        }

    else if (choice == 3)//is the user enter 3 then
        {
        int ops;//variable for user input
        printf("\n1. Canny Edge Filter\n2. Sobel  Edge Filter\n3. Prewitt Edge Filter\nEnter a choice : ");//asking user for the filter
        scanf("%d", &ops);//getting the user input
        if(ops==1){cannyFilter(pixels, width, height, maxval);}//if the user enters 1 then calling canny func
        else if(ops==2){sobelFilter(pixels, width, height, maxval);}//if the user enters 2 then calling sobel filter func
        else if (ops==3){prewittFilter(pixels,width,height,maxval);}//if the user enter 3, then calling prewitt filter func
        else{printf("Invalid input");}//else printing invalid input
        }

    else if (choice==4)//if the user enters 4 then, calling lbp func
        {
            int **lbp = computeLBP(pixels, width, height, 255);
        }

    else if (choice==0)
        {
            printf("THANK YOU!");
            break;
        }

    else {printf("Invalid Input");}//else printing invalid input
    }

    return 0;
}

int MENU(){
    int choice;
    printf("\n1. Zoom/Shrink Image\n2. Apply Filter\n3. Edge Detection\n4. Compute LBP\n0. Exit\nEnter choice: ");//displaying the menu
    scanf("%d",&choice);//getting user choice
    return choice;
    }

//keeping values within range (lo, hi) for edge replication
static int clamp(int v, int lo, int hi)
{
    if(v<lo) return lo;//if value v is less than lo then returning lo
    if(v>hi) return hi;//if v is more than hi then returning hi
    return v;//else returning v
}

/*
this func generates a LBP image from the input pixel matrix. for every pixel -
it compares the intensity of the eight surrounding neighbour.
each comparison produces a bit (1 or 0), forming a 8 bit LPB code
this code represents the local texture
*/
int **computeLBP(int **pixels, int width, int height, int outMax)
{
    int r,c,k;//loop variables
    //defining the 8 positions around the center pixel in clockwise order
    int dr[8] ={-1,-1,-1,0,1,1,1,0};
    int dc[8] = {-1,0,1,1,1,0,-1,-1};
    int t;//variable for cleaning us errors
    //allocating memory for the output
    int **out= malloc(height*sizeof(int*));
    if(!out) return NULL;//if allocations fails, return null
    for(r=0; r<height; r++){//lopping through
        out[r] =malloc(width*sizeof(int));
        if(!out[r]) {//if row allocations fails
            for(t=0; t< r; t++) free( out[t]);//free rows created before
            free(out);
            return NULL;
        }}
    //comparing the lbp values for each pixel
    for(r=0; r<height;r++){//loop through height
        for(c=0; c<width; c++){//loop through width
            int center =pixels[r][c];//center pixel
            unsigned char code=0;//holding 8 bit lbp value
            for(k=0; k<8; k++) {//loop through all 8 neighbours
                int rr = clamp(r+dr[k], 0, height-1);//doing edge boundary
                int cc = clamp(c+dc[k], 0, width -1);
                int neighbor =pixels[rr][cc];
                code <<=1;//shifting previous bit left
                if (neighbor>=center)//threshold rule - if neighbor >= center , then 1 bit */
                    code|=1;//set the lowest bit to 1
            }
            out[r][c]=(int)code;//storing lpb code
        }}

    FILE *fp =fopen("lbp.pgm","w");//saving the result as lbp.pgm
    if (fp){
        int writeMax=255;//max intensity in pgm
        fprintf(fp,"P2\n%d %d\n%d\n",width,height,writeMax);//printing the width, height and max value
        for(r=0; r< height; r++){//looping through
            for(c=0; c <width; c++){//looping through
                fprintf(fp,"%d",out[r][c]);//writing the pixel values
            }
            fprintf(fp,"\n");//ending the row
        }fclose(fp);//closing the file
        printf("\nSaved file - lbp.pgm");
    }

    else
        {printf("\nCouldnt save lbp.pgm");}//if error then letting the user know
    return out;
}


/* this func performs edge detection on the grayscale image
it 1st allocaed an output matrix, then applies 2 - 3x3 kernels i. Gx for detecting the vertical edges
and ii. Gy for horizontal edges.
for every pixel it multiplies the surrounding 3x3 neighbours with each kernel to compute the horizontal and vertical gradients.
these gradients are combined to estimate edge strength - which is then stored in the output image file
*/
int **prewittFilter(int **pixels, int width, int height, int maxval)
{
    int r,c,i,j;// loop variables
    int Gx[3][3] ={{-1,0,1},{-1,0,1},{-1,0,1}};//horizontal edge detection
    int Gy[3][3] ={{1,1,1},{0,0,0},{-1,-1,-1}};//vertical edge detection

    /* allocate output */
    int k;
    int **out = malloc(height * sizeof(int *));//allocating memory for the filtered file
    if (!out) return NULL;// returning null if allocation fails
    for(r=0; r<height; r++){//looping through height
        out[r] = malloc(width * sizeof(int));//freeing all rows is allocation fails
        if (!out[r]){
            for(k=0; k<r;k++) free(out[k]);
            free(out);
            return NULL;
        }
    }
    //setting the borders to 0 to continue with prewitt
    for(c=0; c<width;c++) {out[0][c]=0; out[height-1][c]=0;}//top and bottom row
    for(r=0; r<height;r++) {out[r][0]=0; out[r][width-1]=0;}//right and left row

    //prewitt
    for(r=1;r<height- 1;r++){
        for(c=1;c<width-1;c++){
            int gx=0; //x gradient
            int gy=0;//y gradient
            //getting 3x3 block
            for (i=-1;i<=1; i++){
                for(j=-1; j<=1; j++){
                    int p= pixels[r+i][c+j];
                    gx+= p*Gx[i+1][j+1];
                    gy+= p*Gy[i+1][j+1];
                }}

            int mag= abs(gx)+abs(gy);//approx gradient magnitude
            if (mag>maxval)mag =maxval;
            out[r][c] = mag;
        }}

    FILE *fp =fopen("prewitt.pgm","w");//saving the new file as prewitt.pgm
    if (fp){
        fprintf(fp,"P2\n%d %d\n%d\n",width,height,maxval);//adding the height, width and maxval in the file
        for(r=0;r<height; r++){
            for(c=0;c<width;c++){fprintf(fp,"%d",out[r][c]);}//writing the pixel value
            fprintf(fp,"\n");//going to the next row
        }
        printf("\nSaved file - prewitt.pgm");
        fclose(fp);
    }

    else
        {printf("\nCouldnt save prewitt.pgm");}//is unable to save the file, letting the use know
    return out;
}


/*
this func detects edges in the image using the sobel operation.
it applies Gx and Gy gradient kernels to estimate edge strength in both directions.
this fun gives more weight to the central pixel.
the combined gradient magnitude forms the output image.
*/
int **sobelFilter(int **pixels, int width, int height, int maxval)
{
    int r,c,i,j;//loop variables
    int Gx[3][3] ={{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}};//horizontal gradient, left to right
    int Gy[3][3] ={{ 1, 2, 1},{ 0, 0, 0},{-1,-2,-1}};//vertical gradient
    int **out = malloc(height * sizeof(int*));//allocating row pointers
    for(r= 0; r<height; r++){out[r] =malloc(width*sizeof(int));}//allocating each row
    //applying sobel filter
    for(r=1; r<height-1; r++){//loop through rows
        for(c=1;c<width-1; c++){//looping through cols
            int gx=0,gy=0;//accumulating gradient
            //setting 3x3 block
            for(i=-1; i<=1;i++){//looping through neighbour rows
                for(j=-1;j<=1;j++) {//looping through col
                    int pixel = pixels[r+i][c+j];//neighbour pixels
                    gx += pixel*Gx[i+1][j+1];//multiplying by Gx 3x3 block
                    gy += pixel*Gy[i+1][j+1];// multiplying by Gy 3x3x block
                }}
            int mag =abs(gx)+abs(gy);//gradient magnitude
            if(mag>255)mag =255;//clamping to max gray scale
            out[r][c]=mag;//storing the magnitude in the output
        }}

    FILE *fp =fopen("sobel.pgm","w");//saving the sobel filter image
    if(fp){
    fprintf(fp,"P2\n%d %d\n%d\n",width,height,maxval);//writing the header - width, height and maxval
    //writing the pixel values row by row
    for(r=0; r<height;r++){//looping through rows
        for(c=0;c<width;c++)
            fprintf(fp,"%d",out[r][c]);//writing each pixel
        fprintf(fp,"\n");//going to the next line after each row
    }
    printf("\nSaved file - sobel.pgm");
    fclose(fp);}//closing the output file

    else
        {printf("\nCouldnt save sobel.pgm");}//if error
    return out;
}


int **cannyFilter(int **pixels, int width, int height, int maxval)
{
    int r;//variable for loop
    int **blur =gaussianBlur(pixels,width,height);//calling gaussian blur func
    int **grad =malloc(height*sizeof(int*));//allocating rows for the gradient magnitude
    int **dir  =malloc(height*sizeof(int*));//allocating rows for gradient directions
    for(r=0; r<height;r++){//
        grad[r] =malloc(width*sizeof(int));//allocating each row for gradient
        dir[r]  =malloc(width*sizeof(int));//allocating each row for direction
    }

    sobel(blur,width,height,grad,dir);//calling sobel func
    int **thin =nonMaxSuppression(grad,dir,width,height);//calling nonMaxSuppression func for thinning edges
    int **edges=thresholdEdges(thin,width,height);//calling thresholdEdges func

    saveP2("cannyFilter.pgm",edges,width,height,maxval);//writing the P2 file
    printf("\nSaved file - cannyFilter.pgm");

    return edges;//returning the final edge image
}


/*
this func takes a grayscale image and applies 3x3 gaussian blur filter to it. for each pixel,
it multiplies the surrounding 3x3 neighbourhood by a gaussian kernel,
sums the result and then divides by 16 to avg them.
*/
int **gaussianBlur(int **pixels, int width, int height)
{
    int r,c,dr,dc;//loop variables
    //3x3 block for guassian blue
    float kernel[3][3]={{1,2,1},{2,4,2},{1,2,1}};//top row, center row and bottom row
    float norm = 16.0;//sum of all 3x3 block/kernel values
    //allocating for new output image
    int **out = malloc(height*sizeof(int*));//rows
    for(r=0; r<height;r++)//looping through each row
        out[r] =malloc(width*sizeof(int));//each col

    for(r=0; r<height;r++){
        for(c=0;c< width;c++){
            float sum=0;//getting the sum
            //applying the guassian kernel around
            for(dr=-1; dr<=1;dr++) {//kernel row offset
                for(dc=-1; dc<=1; dc++) {//kernal col offset
                    int rr =r+dr;//neighbour row index
                    int cc =c+dc;//neighbour col
                    if(rr>=0 && rr<height && cc>=0 && cc<width)//skipping pixels outside the the image, checking the borders
                        sum += pixels[rr][cc]*kernel[dr+1][dc+1];
                }}
            out[r][c]=(int)(sum/norm);//storing the blur pixel values
        }}

    return out;//returning the blue image
}

/*
this func applies the sobel operator to an image to detect edges.
it uses 3x3 kernels- Gx and Gy to computer approx in the x and y direction around each pixel.
it calculates gx and gy and stores a simple gradient magnitude abs(gx) + abs(gy) in grad.
*/
void sobel(int **img, int width, int height,int **grad, int **dir)
{
    int r,c;//loop variables
    int Gx[3][3] = {{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}};//horizontal gradient
    int Gy[3][3] = {{ 1, 2, 1},{ 0, 0, 0},{-1,-2,-1}};//vertical gradient
    //initial gradient[][] and dir[][] at 0
    for(r=0;r<height;r++)
        for(c=0;c<width; c++){
            grad[r][c]=0;//gradient magnitude
            dir[r][c]=0;//gradient direction
            }

    //applying sobel filter
    for(r=1; r<height-1;r++){//skipping first and last row
        for(c=1;c<width-1;c++){//skipping first and last col
            int gx=0, gy=0,i,j;//reseting the accumulators
            for(i=-1;i<=1; i++){//kernel row offset
                for(j=-1; j<=1;j++){//kernel col offset
                    gx +=img[r+i][c+j]*Gx[i+1][j+1];//applying Gx
                    gy +=img[r+i][c+j]*Gy[i+1][j+1];//applying Gy
                    }}
            grad[r][c]=abs(gx)+abs(gy);//simple magnitude
            dir[r][c]=(abs(gy)>abs(gx))?1:0;//0=horizontal, 1=vertical
            }}
}


/*
this func thins out the edges by keeping only the local maxima along the edge direction/
it takes grad (gradient magnitude) and dir (direction) and allocates an output image,
and for each non border pixel checks its neighbour along the gradient direction.
it produces a thinner and cleaner edge by checking the magnitude of the current pixel.
*/
int **nonMaxSuppression(int **grad, int **dir, int width, int height)
{
    int r,c;//loop variables
    int **out =malloc(height*sizeof(int*));//allocating rows
    for(r=0;r<height;r++)
        out[r] =malloc(width*sizeof(int));//cols

        //applying non maximum suppression
    for(r=1; r<height-1; r++){//skipping top and bottom borders
        for(c=1; c<width-1;c++){//skipping left and right borders
            int g=grad[r][c];//current gradient magnitude
            //checking gradient direction
            if (dir[r][c]==0){//if horizontal (left to right)
                    //checking if pixel is greater than neighbour
                    if(g>=grad[r][c-1] && g>=grad[r][c+1])
                        {out[r][c]=g;}//left and right neighbour - keep local max
                    else
                    {out[r][c]=0;}//suppress it
                    }
            else
            {// if vertical
                if(g>=grad[r-1][c] && g>=grad[r+1][c]) {out[r][c]=g;}//top and bottom - keep
                else {out[r][c]=0;}//suppress
            }}}
    return out;//returning the image
}


/*
this fun classifies the edge into strong, weak or non edges using 2 thresholds - high and low.
it allocates an output image and initializes it to 0. for each non boarder pixel, if the
value in image > or = high - it marks it as strong
if between low and high - then only keeps it an as edge
all pixels remain 0.
*/
int **thresholdEdges(int **img, int width, int height)
{
    int r,c;//loop variables
    int high=50;//setting high threshold for strong edges
    int low=20;//setting low threshold for weak edges

    //allocating output matric
    int **out =malloc(height*sizeof(int*));//rows
    for(r=0; r<height;r++)
        out[r] =malloc(width*sizeof(int));//cols
        //initializing output image to all zeros
    for(r=0;r<height;r++)
        for(c=0;c<width;c++)
            out[r][c]=0;

    //applying threshold
    for (r=1; r<height-1; r++){//skiping borders
        for(c=1; c<width-1; c++){
                //if strong edge pixel then
            if(img[r][c]>=high){out[r][c]=255;}//keeping it
            else if (img[r][c]>=low) {//if weak edge pixel then keeping only if at least one direct neighbour is strong
                if (img[r-1][c]>=high || img[r+1][c]>=high || img[r][c-1]>=high || img[r][c+1]>=high)//checking for strong top, bottom, left or right neighbour
                    out[r][c] = 255;//changing weak to strong
            }}}

    return out;//returning the final edge image
}


/*
the func applies 3x3 median filter to a grayscale. it removes the noise, while
preserving edges better than a normal blur
*/
int **medianfilter(int **pixels, int width, int height, int maxval)
{
    int r,c,dr,dc,i,j;//variables for loop
    //allocating output matrix
    int **out =malloc(height*sizeof(int*));//rows
    for(r=0; r<height; r++)
        out[r] =malloc(width*sizeof(int));//cols
    int median[9];//array to store 9 neighbours

    //applying median filter
    for(r=0; r<height;r++){//looping thru rows
        for(c=0;c<width; c++){//cols
                int count=0;//total neighbours
            for(dr=-1; dr<=1; dr++){//row offset
                for(dc=-1; dc<=1; dc++){//col offset
                    int rr =r+dr;//neighbour row
                    int cc =c+dc;//neighbour col
                    //checking the boundries
                    if(rr>=0 && rr<height && cc>=0 && cc<width){median[count++] = pixels[rr][cc];}
                }}

                //sorting the collected values
            for(i=0; i<count-1; i++){
                for(j=i+1; j<count; j++){
                    if(median[j] < median[i]) {//swapping - smallest to largest
                        int temp = median[i];
                        median[i] = median[j];
                        median[j] = temp;
                    }}}
            out[r][c] = median[count/2];//getting the median - the middle elements
        }}

    FILE *fp = fopen("medianfilter.pgm","w");//opening a new file of median filter
    if(fp){//if the files opens
    fprintf(fp,"P2\n%d %d\n%d\n",width,height,maxval);//writing the header - wight, height and maxval in the file
    for(r=0; r<height;r++){//writing the pixel values
        for(c=0; c<width;c++)
            fprintf(fp,"%d",out[r][c]);
        fprintf(fp,"\n");//adding new line after each row
    }
    printf("Saved file - medianfilter.pgm");
    fclose(fp);}//closing the file

    else
        {printf("Couldn't save medianfilter.pgm");}//letting the user know if fails

    return out;//returning the median filter image
}

/*
this func applies 4x4 mean (avg) filter to a grayscale image. a mean filter smooths
the image by averaging pixel values in a neighbourhood, reducing noise but also blurring edges
*/
//4x4 mean filter
int **meanfilter(int **pixels, int width, int height, int maxval)
{
    int r,c,dr,dc;//loop variables
    //allocating output matrix
    int **out =malloc(height*sizeof(int*));//row pointers
    for(r=0; r<height; r++)
        out[r] =malloc(width*sizeof(int));//each row

    //applying 4x4 mean filter
    for(r=0; r<height; r++){//loop through row
        for(c=0; c<width; c++){//loop through col
            int sum=0;//sum of values
            int count=0;//num of valid pixels
            //4x4 neighborhood
            for(dr=0; dr<4; dr++){// row offset
                for(dc=0; dc<4; dc++){//col offset 0 to 3
                    int rr = r+dr;//neighbor row
                    int cc = c+dc;//neighbor column
                    //check image boundries
                    if(rr>=0 && rr<height && cc>=0 && cc<width){
                        sum += pixels[rr][cc];//adding valid pixel
                        count++;//incrementing count
                    }}}

            ////calculating mean
            if (count>0){out[r][c] = sum/count;}//if count less than 0
            else {out[r][c] = pixels[r][c];}
        }}

    FILE *fp = fopen("mean4x4.pgm","w");//opening a new file for the mean filter
    if(fp){//is files opens
        fprintf(fp,"P2\n%d %d\n%d\n",width,height,maxval);//writing the header - file type, width, height, maxval
        for(r=0; r<height; r++){//writing each pixel row by row
            for(c=0; c<width; c++)
                fprintf(fp,"%d",out[r][c]);//writing each pixel
            fprintf(fp,"\n");//adding a new line after each row
        }
        printf("\nSaved file - mean4x4.pgm");
        fclose(fp);//closing the new file
    }

    else
        {printf("\nCouldnt save mean4x4.pgm\n");}//if opening file fails, then letting the user know

    return out;//returning the new mean filter image
}


/*
this func applies 3x3 avg filter to a grayscale image. this filter smooths the image
by averaging each pixel with its neighbour. it reduces noise but also blurs the edges
*/
int **avgfilter(int **pixels, int width, int height, int maxval)
{
    int r,c,dr,dc;//variables for loop
    //allocating new image
    int **out =malloc(height*sizeof(int*));//each row pointer
    for(r=0; r<height;r++)
        out[r] =malloc(width*sizeof(int));//each row

    //applying filter
    for(r=0; r<height;r++){//loop thru row
        for(c=0;c<width;c++){//col
            int sum=0;//sum of neighbour values
            int count=0;//num of valid neighbours

            //3x3 neighborhood
            for(dr=-1; dr<=1; dr++){//ror offset
                for(dc=-1; dc<=1;dc++){//col offset
                    int rr = r+dr;//row index
                    int cc = c+dc;//col index
                    if(rr>=0 && rr<height && cc>=0 && cc<width){//checking boundaries
                        sum+= pixels[rr][cc];//adding neighbour values
                        count++;//incrementing count by 1
                    }}}
            out[r][c] = sum/count;//calculating avg
        }}

    FILE *fp = fopen("avgfilter.pgm","w");//opening new file for the avg filtered image
    if(fp)
    {//if the files opens correctly
    fprintf(fp,"P2\n%d %d\n%d\n",width,height,maxval);//writing the header - file type, width, height, maxval
    for(r=0; r<height; r++){//writing each pixel row wise
        for(c=0; c<width;c++)
            fprintf(fp,"%d",out[r][c]);//writing each pixel
        fprintf(fp,"\n");//adding new line after each now
    }
    printf("\nSaved file - avgfilter,pgm");
    fclose(fp);}//closing the file

    else
        {printf("\nCouldnt save avgfilter.pgm");}//letting the user know if any error

    return out;//returning the new image
}


/*
this func resizes a grayscale image by a given factor.
the scale value can be greater than 1 = zoom image
or less than 1 = shrink image
*/
int **scaleImage(int **pixels, int width, int height, float scale,int maxval, int *newW, int *newH){
    int r,c;//loop variables
    *newW = (int)(width*scale);//new image width
    *newH = (int)(height*scale);//new image height
    if(*newW<1) *newW=1;//setting minimum width to 1
    if(*newH<1) *newH=1;//setting minimum height to 1
    //allocating output for the scaled image
    int **out =malloc((*newH)*sizeof(int*));//allocating row pointers
    for(r=0; r<*newH; r++)
        out[r] =malloc((*newW)*sizeof(int));//each row
    //creating new scaled image
    for(r=0; r<*newH; r++){//loop through each row
        for(c=0; c<*newW; c++){//col
            int src_r = (int)(r/scale);//original row index
            int src_c = (int)(c/scale);//original col index
            if (src_r>=height) src_r =height-1;//bottom border
            if (src_c>=width)  src_c =width-1;//right border
            out[r][c] = pixels[src_r][src_c];//copying the nearest pixel
        }}
    printf("\nSaved file - scaled.pgm");
    saveP2("scaled.pgm",out,*newW,*newH,maxval);//saving the new scaled image

    return out;//returning the new scaled image
}
