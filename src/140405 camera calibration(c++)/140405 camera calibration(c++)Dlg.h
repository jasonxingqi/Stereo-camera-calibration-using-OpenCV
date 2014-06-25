/****************************************************************************************************************
 ** Thuy Tuong Nguyen, PhD
 ** University of California, Davis
 ** June 2014
 ** 
 ** About this code:
 ** - For research purpose only. 
 ** - For beginners who are trying to use OpenCV's functions for stereo camera calibration.
 ** - Including 2 main files "140405 camera calibration(c++)Dlg.h" and "140405 camera calibration(c++)Dlg.cpp".
 ** - In default, the code can be opened using Visual Studio 2013, but other Visual Studio versions are alright
 **   with the .h and .cpp files. 
 ** - Chessboard, symmetric circle grid, and asymmetric circle grid in different sizes can be used.
 ** - Able to calibration one view (2 images) and multiple views.
 ** - Calibration results are saved to 2 text files (one for left, one for right image) with intrinsic, 
 **   distortion, rotation, projection, and perspective transformation (Q) matrices.
 ****************************************************************************************************************/


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef _CRT_SECURE_NO_WARNINGS
	# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;


// ---

#define CALIBRATION_PATTERN_CHESSBOARD					0
#define CALIBRATION_PATTERN_SYMMETRIC_CIRCLE_GRID		1
#define CALIBRATION_PATTERN_ASYMMETRIC_CIRCLE_GRID		2
#define CHESSBOARD_WIDTH								8
#define CHESSBOARD_HEIGHT								6
#define SYMMETRIC_CIRCLE_GRID_WIDTH						8
#define SYMMETRIC_CIRCLE_GRID_HEIGHT					9
#define ASYMMETRIC_CIRCLE_GRID_WIDTH					4
#define ASYMMETRIC_CIRCLE_GRID_HEIGHT					11

#define IS_DISPLAY_CORNER_RESULT						false
#define IS_SAVE_CALIBRATION_RESULT						false
#define IS_SAVE_RECTIFIED_IMG							false

#define WINDOW_LEFT_IMG				"Left image"
#define WINDOW_LEFT_IMG_RECTIFY		"Rectified left image"
#define WINDOW_RIGHT_IMG			"Right image"
#define WINDOW_RIGHT_IMG_RECTIFY	"Rectified right image"
#define WINDOW_DISPARITY_IMG		"Disparity image"

#define FILENAME_LEFT_CALIBRATION	"leftCalibration.txt"
#define FILENAME_RIGHT_CALIBRATION	"rightCalibration.txt"


// ---


#pragma once


// CMy140405cameracalibrationcDlg dialog
class CMy140405cameracalibrationcDlg : public CDialogEx
{
// Construction
public:
	CMy140405cameracalibrationcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY140405CAMERACALIBRATIONC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedButtonLoadStereoImg();
	afx_msg void OnBnClickedButtonLoadStereoImgMultiview();
	afx_msg void OnBnClickedButtonCalibrateStereoImg();

	afx_msg void OnBnClickedRadioUseChessboard();
	afx_msg void OnBnClickedRadioUseSymmetricCirlceGrid();
	afx_msg void OnBnClickedRadioUseAsymmetricCircleGrid();
	

	// ---------------------------------------------


private:
	void CreateChessboardObjectPoints(Size boardSize, vector<vector<Point3f>>& objectPoints, int nThView);
	bool DetectChessboardCorners(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName);

	bool DetectSymmetricCircles(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName);

	void CreateAsymmetricCirclesObjectPoints(Size boardSize, vector<vector<Point3f>>& objectPoints, int nThView);
	bool DetectAsymmetricCircles(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName);
	
	void DetectPointsInImages(Mat img1, Mat img2, int nThView);

	double CheckCalibrationQuality(Mat intrinsic1, Mat distortion1, Mat intrinsic2, Mat distortion2, Mat F);
	void SaveCalibrationResult(Mat intrinsic1, Mat distortion1, Mat R1, Mat P1, Mat Q, string filename);

	void InitVariables();
	void InitGUI();
	void GetInputParam();
	void SetOutputParam();


private:
	
	// input var
	int boardWidth, boardHeight;
	bool isDisplayCornerResult, isSaveCalibrationResult, isSaveRectifiedImg;
	int calibrationPattern;

	// output var
	double reprojectionError;

	// common var
	char arrayOfChar[256];
	int nViews = -1;

	// usage var
	Mat leftImg, leftImgRectify;
	Mat rightImg, rightImgRectify;
	vector<vector<Point3f>> objectPoints;
	vector<vector<Point2f>> imagePoints1, imagePoints2;
};
