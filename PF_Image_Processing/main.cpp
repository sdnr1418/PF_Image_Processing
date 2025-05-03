#include <iostream>
#include<fstream>
#include<vector>
#include<cstring>
#include<cmath>
#include <algorithm>

using namespace std;

struct Image {
    char ImageFileName[100];
    vector<vector<int>> ImageData;
    int cols, rows, maxGray;
    vector<char> comment;

    bool imageLoaded;
    bool imageModified;

    void changeBrightness(double factor) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] *= factor;
                if (ImageData[r][c] > maxGray)
                    ImageData[r][c] = maxGray;
            }
    }
    int loadImage(char ImageName[]) {

        ifstream FCIN(ImageName);

        if (!FCIN.is_open())
            return -1;

        char MagicNumber[5];
        char Comment[100];

        FCIN.getline(MagicNumber, 4);
        FCIN.getline(Comment, 100);
        FCIN >> cols >> rows >> maxGray;

        ImageData.clear();
        ImageData.resize(rows, vector<int>(cols, 0));

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                FCIN >> ImageData[r][c];

        if (FCIN.fail())
            return -2;

        FCIN.close();
        imageLoaded = true;
        imageModified = false;
        strcpy_s(ImageFileName, ImageName);
        return 0;
    }
    int saveImage(char ImageName[]) {
        ofstream FCOUT(ImageName);
        if (!FCOUT.is_open())
            return -1;

        FCOUT << "P2\n# This is a comment\n"
            << cols << " " << rows << endl << maxGray << endl;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++)
                FCOUT << ImageData[r][c] << " ";
            FCOUT << endl;
        }
        FCOUT.close();
        imageModified = false;
        return 0;
    }
    void horizontalFlipImage() {
        for (int r = 0; r < rows / 2; r++) {
            for (int c = 0; c < cols; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[rows - 1 - r][c];
                ImageData[rows - 1 - r][c] = T;
            }
        }
    }
    void imageRotation(double angleDegrees) {
        const double M_PI = acos(-1.0);
        double angleRadians = angleDegrees * M_PI / 180.0;

        vector<vector<int>> rotatedImage(rows, vector<int>(cols, 0));


        int centerX = cols / 2;
        int centerY = rows / 2;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int rotatedX = static_cast<int>(cos(angleRadians) * (c - centerX) - sin(angleRadians) * (r - centerY) + centerX);
                int rotatedY = static_cast<int>(sin(angleRadians) * (c - centerX) + cos(angleRadians) * (r - centerY) + centerY);

                if (rotatedX >= 0 && rotatedX < cols && rotatedY >= 0 && rotatedY < rows) {
                    rotatedImage[r][c] = ImageData[rotatedY][rotatedX];
                }
                else {
                    rotatedImage[r][c] = 255;
                }
            }
        }

        ImageData = rotatedImage;
    }
    void verticalFlipImage() {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols / 2; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[r][cols - 1 - c];
                ImageData[r][cols - 1 - c] = T;
            }
        }
    }
    void imageCropping(int st_x, int st_y, int cw, int ch) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (r > ch || r < st_y || c < st_x || c > cw) {
                    ImageData[r][c] = 255;
                }
            }
        }
    }
    void combineImages(const Image& otherImage, bool sideBySide = true) {
        // Check if dimensions match for side-by-side combination
        if (sideBySide && rows == otherImage.rows) {
            int newCols = cols + otherImage.cols;
            vector<vector<int>> combinedImage(rows, vector<int>(newCols, 0));

            // Copy pixels from the current image
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    combinedImage[r][c] = ImageData[r][c];
                }
            }

            // Copy pixels from the other image
            for (int r = 0; r < otherImage.rows; r++) {
                for (int c = 0; c < otherImage.cols; c++) {
                    combinedImage[r][cols + c] = otherImage.ImageData[r][c];
                }
            }

            // Update image dimensions and data
            cols = newCols;
            ImageData = combinedImage;
        }
        // Check if dimensions match for top-to-bottom combination
        else if (!sideBySide && cols == otherImage.cols) {
            int newRows = rows + otherImage.rows;
            vector<vector<int>> combinedImage(newRows, vector<int>(cols, 0));

            // Copy pixels from the current image
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    combinedImage[r][c] = ImageData[r][c];
                }
            }

            // Copy pixels from the other image
            for (int r = 0; r < otherImage.rows; r++) {
                for (int c = 0; c < otherImage.cols; c++) {
                    combinedImage[rows + r][c] = otherImage.ImageData[r][c];
                }
            }

            // Update image dimensions and data
            rows = newRows;
            ImageData = combinedImage;
        }
        else {
            cout << "Invalid combination. Dimensions do not match." << endl;
        }
    }
    void toBinary() {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (ImageData[r][c] > 127) {
                    ImageData[r][c] = maxGray;
                }
                else {
                    ImageData[r][c] = 0;
                }
            }
        }
    }
    void applyMedianFilter() {
        vector<vector<int>> filteredImage(rows, vector<int>(cols, 0));

        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                vector<int> neighborhood;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        neighborhood.push_back(ImageData[r + i][c + j]);
                    }
                }
                sort(neighborhood.begin(), neighborhood.end());
                filteredImage[r][c] = neighborhood[4];  // 3x3 median filter
            }
        }

        ImageData = filteredImage;
    }
    void translateImage(int offsetX, int offsetY) {
        vector<vector<int>> translatedImage(rows, vector<int>(cols, 0));

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int newR = r + offsetY;
                int newC = c - offsetX;

                if (newR >= 0 && newR < rows && newC >= 0 && newC < cols) {
                    translatedImage[r][c] = ImageData[newR][newC];
                }
                else {
                    translatedImage[r][c] = 255;
                }
            }
        }

        ImageData = translatedImage;
    }
    void scaleImage(double scaleFactor) {
        int newRows = static_cast<int>(rows * scaleFactor);
        int newCols = static_cast<int>(cols * scaleFactor);

        vector<vector<int>> scaledImage(newRows, vector<int>(newCols, 0));

        for (int r = 0; r < newRows; r++) {
            for (int c = 0; c < newCols; c++) {
                int originalR = static_cast<int>(r / scaleFactor);
                int originalC = static_cast<int>(c / scaleFactor);

                scaledImage[r][c] = ImageData[originalR][originalC];
            }
        }

        rows = newRows;
        cols = newCols;
        ImageData = scaledImage;
    }
    void reSize(float factX, float factY) {
        int new_r = rows * factY;
        int new_c = cols * factX;

        vector<vector<int>> ImageResize(new_r, vector<int>(new_c));

        for (int r = 0; r < new_r; r++) {
            for (int c = 0; c < new_c; c++) {
                ImageResize[r][c] = ImageData[int(r / factY)][int(c / factX)];
            }
        }
        rows = new_r;
        cols = new_c;
        ImageData = ImageResize;
    }
    void applyMeanFilter() {
        vector<vector<int>> filteredImage(rows, vector<int>(cols, 0));

        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                int sum = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        sum += ImageData[r + i][c + j];
                    }
                }
                filteredImage[r][c] = sum / 9;  // 3x3 mean filter
            }
        }

        ImageData = filteredImage;
    }
    void adjustSharpness(double factor) {
        vector<vector<int>> sharpenedImage(rows, vector<int>(cols, 0));

        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                int sharpPixel = static_cast<int>(5 * ImageData[r][c] -
                    ImageData[r - 1][c] -
                    ImageData[r + 1][c] -
                    ImageData[r][c - 1] -
                    ImageData[r][c + 1]);
                sharpenedImage[r][c] = max(0, min(255, sharpPixel));
            }
        }

        ImageData = sharpenedImage;
    }
    void linearContrastStretching() {
        vector<int> flattenedData;
        for (const auto& row : ImageData) {
            flattenedData.insert(flattenedData.end(), row.begin(), row.end());
        }

        int minIntensity = *min_element(flattenedData.begin(), flattenedData.end());
        int maxIntensity = *max_element(flattenedData.begin(), flattenedData.end());

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] = static_cast<int>(255.0 *
                    (ImageData[r][c] - minIntensity) / (maxIntensity - minIntensity));
            }
        }
    }
    void computeImageDerivative(const vector<vector<int>>& derivativeMask) {
        int maskSize = derivativeMask.size();
        int maskRadius = maskSize / 2;

        vector<vector<int>> derivativeImage(rows, vector<int>(cols, 0));

        for (int r = maskRadius; r < rows - maskRadius; r++) {
            for (int c = maskRadius; c < cols - maskRadius; c++) {
                int sum = 0;
                for (int i = -maskRadius; i <= maskRadius; i++) {
                    for (int j = -maskRadius; j <= maskRadius; j++) {
                        sum += derivativeMask[i + maskRadius][j +
                            maskRadius] * ImageData[r + i][c + j];
                    }
                }
                derivativeImage[r][c] = sum;
            }
        }

        ImageData = derivativeImage;
    }
    void applyLinearFilter(const char filterFileName[]) {
        ifstream filterFile(filterFileName);
        if (!filterFile.is_open()) {
            cout << "Error: Could not open filter file." << endl;
            return;
        }

        int filterSize;
        filterFile >> filterSize;

        vector<vector<int>> filterMatrix(filterSize, vector<int>(filterSize, 0));

        for (int i = 0; i < filterSize; i++) {
            for (int j = 0; j < filterSize; j++) {
                filterFile >> filterMatrix[i][j];
            }
        }

        filterFile.close();

        int filterRadius = filterSize / 2;

        vector<vector<int>> filteredImage(rows, vector<int>(cols, 0));

        for (int r = filterRadius; r < rows - filterRadius; r++) {
            for (int c = filterRadius; c < cols - filterRadius; c++) {
                int sum = 0;
                for (int i = -filterRadius; i <= filterRadius; i++) {
                    for (int j = -filterRadius; j <= filterRadius; j++) {
                        sum += filterMatrix[i + filterRadius][j +
                            filterRadius] * ImageData[r + i][c + j];
                    }
                }
                filteredImage[r][c] = sum;
            }
        }

        ImageData = filteredImage;
    }
    void enhanceImageWithFilter() {
        vector<vector<int>> enhancementFilter = { {-1, -1, -1},
                                                 {-1,  9, -1},
                                                 {-1, -1, -1} };
        applyFilter(enhancementFilter);
    }
    void applyFilter(const vector<vector<int>>& filter) {
        int filterSize = filter.size();
        int filterRadius = filterSize / 2;

        vector<vector<int>> enhancedImage(rows, vector<int>(cols, 0));

        for (int r = filterRadius; r < rows - filterRadius; r++) {
            for (int c = filterRadius; c < cols - filterRadius; c++) {
                int sum = 0;
                for (int i = -filterRadius; i <= filterRadius; i++) {
                    for (int j = -filterRadius; j <= filterRadius; j++) {
                        sum += filter[i + filterRadius][j + filterRadius] * ImageData[r + i][c + j];
                    }
                }
                enhancedImage[r][c] = max(0, min(255, sum));
            }
        }

        ImageData = enhancedImage;
    }

};

struct Menu {
    vector<string> menuItems;

    Menu(char menuFile[]) {  //constructor which creates object Menu and initilalizes it with the load menu function
        loadMenu(menuFile);
    }

    int loadMenu(char menuFile[]) {
        ifstream IN;
        IN.open(menuFile);  //open file
        if (!IN.is_open())  //if file not open then return -1
            return -1;
        char menuItem[100], TotalItems[10];

        int Choices;

        IN.getline(TotalItems, 8);  //getline and store in char array "TotalItems"
        Choices = atoi(TotalItems);     //set choice the integer value of TotalItems
        for (int i = 1; i <= Choices; i++) {       //start from the second line and iterate till the end
            IN.getline(menuItem, 99);       //line is read and stored in char array menuItem
            menuItems.push_back(menuItem);  //the vector array is appended inserting menuItem from the end
        }
        IN.close();
        return Choices;
    }

    int presentMenu() {
        int userChoice;
        int totalChoices = menuItems.size();    //stores size of vector array menuItems in totalChoices


        do {
            int k = 1;
            for (int i = 0; i < totalChoices; i++) {        //using this loop the whole menu is displayed on the screen
                if (menuItems[i][0] != '*') {
                    cout << k << "\t" << menuItems[i] << endl;
                    k++;
                }
            }
            cout << " Enter Your Choice (1 - " << k - 1 << " ): ";
            cin >> userChoice;
        } while (userChoice < 1 || userChoice > totalChoices);      //keep displaying the menu until the user enter the correct option from the menu
        return userChoice;
    }

};

int main() {
    char MenuFile[] = "MainMenu.txt"; //MenuFile saves MainMenu file name
    Image images[2]; //declaring array of type<Image>, size --> 2
    int activeImage = 0;
    int errorCode = 0;
    int userChoice;
    //    int TotalChoices = loadMenu("MainMenu.txt");
    int totalChoices;

    Menu menu(MenuFile);        //declaring menu of type<Menu>
    totalChoices = menu.menuItems.size();

    do {
        userChoice = menu.presentMenu();

        //load image
        if (1 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }
        } //load image

        //save image
        else if (2 == userChoice) {     //save image
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //brightness
        else if (3 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            float bri_fact;
            cout << "Enter brightness factor: ";
            cin >> bri_fact;
            images[activeImage].changeBrightness(bri_fact);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //linear contrast stretching method
        else if (4 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].linearContrastStretching();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //sharpness
        else if (5 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            float sharp_fact;
            cout << "Enter sharpness factor: ";
            cin >> sharp_fact;
            images[activeImage].adjustSharpness(sharp_fact);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //to binary
        else if (6 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].toBinary();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //resize
        else if (7 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            float xfactor, yfactor;
            cout << "Enter Factor for x: ";
            cin >> xfactor;
            cout << "Enter Factor for y: ";
            cin >> yfactor;

            images[activeImage].reSize(xfactor, yfactor);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //imageRotate
        else if (8 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            double theta;
            cout << "Enter angle theta: ";
            cin >> theta;

            images[activeImage].imageRotation(theta);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //flip horz
        else if (9 == userChoice) {         //flip image horizontal
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].horizontalFlipImage();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //flip vert
        else if (10 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].verticalFlipImage();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //imageCropping
        else if (11 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            int st_x, st_y, end_x, end_y;

            cout << "Enter start X: ";
            cin >> st_x;
            cout << "Enter end X: ";
            cin >> end_x;
            cout << "Enter start Y: ";
            cin >> st_y;
            cout << "Enter end Y: ";
            cin >> end_y;

            images[activeImage].imageCropping(st_x, st_y, end_x, end_y);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //combining images
        else if (12 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            char img_name[100];
            cout << "Enter Image Name to combine with: ";
            cin.ignore();
            cin.getline(img_name, 100);

            errorCode = images[activeImage + 1].loadImage(img_name);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            bool is_side = true;
            char input;
            cout << "Side by Side? (y/n): ";
            cin >> input;
            if (input == 'N' || input == 'n') {
                is_side = false;
            }

            images[activeImage].combineImages(images[activeImage + 1], is_side);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //Mean filter
        else if (13 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].applyMeanFilter();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //Median Filter
        else if (14 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].applyMedianFilter();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //Linear filter with file
        else if (15 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            char filter_name[100];

            cout << "Enter filter file name: ";
            cin.ignore();
            cin.getline(filter_name, 100);


            images[activeImage].applyLinearFilter(filter_name);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //enhace image with filter
        else if (16 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            images[activeImage].enhanceImageWithFilter();

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //image derivative
        else if (17 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            vector<vector<int>> derivativeMask = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
            images[activeImage].computeImageDerivative(derivativeMask);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //edge detection
        else if (18 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            vector<vector<int>> edgeMask = { {0, 1, 0}, {1, -4, 1}, {0, 1, 0} };
            images[activeImage].computeImageDerivative(edgeMask);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //image translation
        else if (19 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            int offsetX, offsetY;
            cout << "Enter X offset for translation: ";
            cin >> offsetX;
            cout << "Enter Y offset for translation: ";
            cin >> offsetY;
            images[activeImage].translateImage(offsetX, offsetY);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }

        }

        //scalling Image
        else if (20 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name to load: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            float scale_fact;
            cout << "Enter Scalling Factor: ";
            cin >> scale_fact;

            images[activeImage].scaleImage(scale_fact);

            cout << "Specify File Name to save new file: ";
            cin >> ImageFileName;
            errorCode = images[activeImage].saveImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Saved as " << ImageFileName << endl;
            }
            else {
                cout << "Save Error: Code " << errorCode << endl;
            }
        }

        //exit
        else if (21 == userChoice) {

        }

    } while (userChoice != totalChoices);
    return 0;
}