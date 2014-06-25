#include "stdafx.h"
#include "140405 camera calibration(c++).h"
#include "140405 camera calibration(c++)Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMy140405cameracalibrationcDlg::CMy140405cameracalibrationcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy140405cameracalibrationcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy140405cameracalibrationcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy140405cameracalibrationcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMy140405cameracalibrationcDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_STEREO_IMG, &CMy140405cameracalibrationcDlg::OnBnClickedButtonLoadStereoImg)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATE_STEREO_IMG, &CMy140405cameracalibrationcDlg::OnBnClickedButtonCalibrateStereoImg)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_STEREO_IMG_MULTIVIEW, &CMy140405cameracalibrationcDlg::OnBnClickedButtonLoadStereoImgMultiview)
	ON_BN_CLICKED(IDC_RADIO_USE_CHESSBOARD, &CMy140405cameracalibrationcDlg::OnBnClickedRadioUseChessboard)
	ON_BN_CLICKED(IDC_RADIO_USE_ASYMMETRIC_CIRCLE_GRID, &CMy140405cameracalibrationcDlg::OnBnClickedRadioUseAsymmetricCircleGrid)
	ON_BN_CLICKED(IDC_RADIO_USE_SYMMETRIC_CIRLCE_GRID, &CMy140405cameracalibrationcDlg::OnBnClickedRadioUseSymmetricCirlceGrid)
END_MESSAGE_MAP()


BOOL CMy140405cameracalibrationcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitVariables();
	InitGUI();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMy140405cameracalibrationcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CMy140405cameracalibrationcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ===========================================================================================


void CMy140405cameracalibrationcDlg::OnDestroy() {
	CDialogEx::OnDestroy();
	destroyAllWindows();

	leftImg.release();		leftImgRectify.release();
	rightImg.release();		rightImgRectify.release();

	vector<vector<Point3f>>().swap(objectPoints); // objectPoints.clear() just clears values, not release memory
	vector<vector<Point2f>>().swap(imagePoints1); 	
	vector<vector<Point2f>>().swap(imagePoints2);
}


void CMy140405cameracalibrationcDlg::OnBnClickedOk()
{
	OnDestroy();
	CDialogEx::OnOK();
}


void CMy140405cameracalibrationcDlg::OnBnClickedRadioUseChessboard()
{
	CString s;
	s.Format(L"%2d", CHESSBOARD_WIDTH);		this->GetDlgItem(IDC_EDIT_BOARD_WIDTH)->SetWindowTextW((LPCTSTR)s);
	s.Format(L"%2d", CHESSBOARD_HEIGHT);	this->GetDlgItem(IDC_EDIT_BOARD_HEIGHT)->SetWindowTextW((LPCTSTR)s);

	((CButton*)this->GetDlgItem(IDC_RADIO_USE_SYMMETRIC_CIRLCE_GRID))->SetCheck(false);
	((CButton*)this->GetDlgItem(IDC_RADIO_USE_ASYMMETRIC_CIRCLE_GRID))->SetCheck(false);
}


void CMy140405cameracalibrationcDlg::OnBnClickedRadioUseSymmetricCirlceGrid()
{
	CString s;
	s.Format(L"%2d", SYMMETRIC_CIRCLE_GRID_WIDTH);		this->GetDlgItem(IDC_EDIT_BOARD_WIDTH)->SetWindowTextW((LPCTSTR)s);
	s.Format(L"%2d", SYMMETRIC_CIRCLE_GRID_HEIGHT);		this->GetDlgItem(IDC_EDIT_BOARD_HEIGHT)->SetWindowTextW((LPCTSTR)s);

	((CButton*)this->GetDlgItem(IDC_RADIO_USE_CHESSBOARD))->SetCheck(false);
	((CButton*)this->GetDlgItem(IDC_RADIO_USE_ASYMMETRIC_CIRCLE_GRID))->SetCheck(false);
}


void CMy140405cameracalibrationcDlg::OnBnClickedRadioUseAsymmetricCircleGrid()
{
	CString s;
	s.Format(L"%2d", ASYMMETRIC_CIRCLE_GRID_WIDTH);		this->GetDlgItem(IDC_EDIT_BOARD_WIDTH)->SetWindowTextW((LPCTSTR)s);
	s.Format(L"%2d", ASYMMETRIC_CIRCLE_GRID_HEIGHT);	this->GetDlgItem(IDC_EDIT_BOARD_HEIGHT)->SetWindowTextW((LPCTSTR)s);

	((CButton*)this->GetDlgItem(IDC_RADIO_USE_CHESSBOARD))->SetCheck(false);
	((CButton*)this->GetDlgItem(IDC_RADIO_USE_SYMMETRIC_CIRLCE_GRID))->SetCheck(false);
}


void CMy140405cameracalibrationcDlg::OnBnClickedButtonLoadStereoImg()
{
	GetInputParam();
	CFileDialog dlg(TRUE, _T("*.txt"), _T(""), OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, _T("Left and right images (*.txt)|*.txt|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK) {
		CStdioFile imgFile;
		imgFile.Open((LPCTSTR)dlg.GetPathName(), CFile::modeRead);

		try {
			// load a pair of left and right images
			CString text;
			imgFile.ReadString(text);	WideCharToMultiByte(CP_ACP, 0, text, -1, arrayOfChar, 256, NULL, NULL);		leftImg = imread(arrayOfChar, CV_LOAD_IMAGE_COLOR);		imshow(WINDOW_LEFT_IMG, leftImg);
			imgFile.ReadString(text);	WideCharToMultiByte(CP_ACP, 0, text, -1, arrayOfChar, 256, NULL, NULL);		rightImg = imread(arrayOfChar, CV_LOAD_IMAGE_COLOR);	imshow(WINDOW_RIGHT_IMG, rightImg);
			imgFile.Close();


			// ---- detect chessboard corners (or grid-circles) HEREIN
			nViews = 1;
			GetInputParam();
			vector<vector<Point3f>>().swap(objectPoints);		objectPoints.resize(nViews);
			vector<vector<Point2f>>().swap(imagePoints1);		imagePoints1.resize(nViews);
			vector<vector<Point2f>>().swap(imagePoints2);		imagePoints2.resize(nViews);
			DetectPointsInImages(leftImg, rightImg, nViews - 1);
		}
		catch (Exception e) {
			imgFile.Close();
			MessageBox(L"This is one-view mode. Please load a correct data file!");
		}
	}
}


void CMy140405cameracalibrationcDlg::OnBnClickedButtonLoadStereoImgMultiview()
{
	GetInputParam();
	CFileDialog dlg(TRUE, _T("*.txt"), _T(""), OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, _T("Multiview stereo images (*.txt)|*.txt|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK) {
		CStdioFile imgFile;
		imgFile.Open((LPCTSTR)dlg.GetPathName(), CFile::modeRead);

		// read the first line for the number of views
		CString text;
		imgFile.ReadString(text);
		nViews = (int)wcstod(text, NULL);
		if (nViews == 0) {
			MessageBox(L"This is multi-view mode. Please load a correct data file!");
		}
		else {
			// load pairs of left and right images
			GetInputParam();
			vector<vector<Point3f>>().swap(objectPoints);		objectPoints.resize(nViews);
			vector<vector<Point2f>>().swap(imagePoints1);		imagePoints1.resize(nViews);
			vector<vector<Point2f>>().swap(imagePoints2);		imagePoints2.resize(nViews);
			for (int i = 0; i < nViews; i++) {
				imgFile.ReadString(text);	WideCharToMultiByte(CP_ACP, 0, text, -1, arrayOfChar, 256, NULL, NULL);		leftImg = imread(arrayOfChar, CV_LOAD_IMAGE_COLOR);
				imgFile.ReadString(text);	WideCharToMultiByte(CP_ACP, 0, text, -1, arrayOfChar, 256, NULL, NULL);		rightImg = imread(arrayOfChar, CV_LOAD_IMAGE_COLOR);
				DetectPointsInImages(leftImg, rightImg, i); // ---- detect chessboard corners (or grid-circles) HEREIN
			}
		}
		imgFile.Close();
	}
}


void CMy140405cameracalibrationcDlg::OnBnClickedButtonCalibrateStereoImg()
{
	GetInputParam();
	Size imgSize = leftImg.size();	


	// ---- find intrinsic and distortion parameters of 2 cameras
	Mat intrinsic1 = Mat::eye(3, 3, CV_64F);//		intrinsic1.at<double>(0, 0) = 1.0; // use CV_CALIB_FIX_ASPECT_RATIO
	Mat intrinsic2 = Mat::eye(3, 3, CV_64F);//		intrinsic2.at<double>(0, 0) = 1.0; // use CV_CALIB_FIX_ASPECT_RATIO
	Mat distortion1, distortion2;
	Mat R; // rotation matrix between the 1st and the 2nd camera coordinate systems
	Mat T; // translation vector between the coordinate systems of the cameras
	Mat E; // essential matrix																					// <-- NOT USED
	Mat F; // fundamental matrix																				// <-- USED for calibration quality check
	double rms = stereoCalibrate(objectPoints, imagePoints1, imagePoints2,
								intrinsic1, distortion1, intrinsic2, distortion2, 
								imgSize, 
								R, T, E, F,
								cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5), 
								CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH + CV_CALIB_FIX_PRINCIPAL_POINT
								);
				// minimizes the total re-projection error for all the points in all the available views from both cameras


	// ---- check calibration quality (optional)
	reprojectionError = CheckCalibrationQuality(intrinsic1, distortion1, intrinsic2, distortion2, F);


	// ---- find extrinsic parameters for image rectification
	Mat R1, R2; // 3x3 rectification transform (rotation matrix) for the 1st and the 2nd camera
	Mat P1, P2; // 3x4 projection matrix in the new (rectified) coordinate systems for the 1st and 2nd camera 
	Mat Q; // 4x4 disparity-to-depth mapping matrix	(perspective transformation matrix)
	stereoRectify(intrinsic1, distortion1, intrinsic2, distortion2,
				imgSize, 
				R, T, 
				R1, R2, P1, P2, Q);

	if (isSaveCalibrationResult) {
		SaveCalibrationResult(intrinsic1, distortion1, R1, P1, Q, FILENAME_LEFT_CALIBRATION);	// Q is saved 2 times
		SaveCalibrationResult(intrinsic2, distortion2, R2, P2, Q, FILENAME_RIGHT_CALIBRATION);  // Q is saved for purpose of using reprojectImageTo3D in the 'future'
	}


	// ---- precompute maps for cvRemap()
	Mat mapx1, mapy1, mapx2, mapy2;
	initUndistortRectifyMap(intrinsic1, distortion1, R1, P1, imgSize, CV_16SC2, mapx1, mapy1);
	initUndistortRectifyMap(intrinsic2, distortion2, R2, P2, imgSize, CV_16SC2, mapx2, mapy2);


	// ---- rectify 2 images using maps
	remap(leftImg, leftImgRectify, mapx1, mapy1, CV_INTER_LINEAR);			imshow(WINDOW_LEFT_IMG_RECTIFY, leftImgRectify);
	remap(rightImg, rightImgRectify, mapx2, mapy2, CV_INTER_LINEAR);		imshow(WINDOW_RIGHT_IMG_RECTIFY, rightImgRectify);

	if (isSaveRectifiedImg) {
		imwrite("leftImgRectify.bmp", leftImgRectify);
		imwrite("rightImgRectify.bmp", rightImgRectify);
	}


	// ---- release all matrices	
	//vector<vector<Point3f>>().swap(objectPoints); 
	//vector<vector<Point2f>>().swap(imagePoints1);
	//vector<vector<Point2f>>().swap(imagePoints2);
	intrinsic1.release();		distortion1.release();
	intrinsic2.release();		distortion2.release();
	R.release();
	T.release();
	E.release();
	F.release();
	R1.release();				P1.release();
	R2.release();				P2.release();
	Q.release();
	mapx1.release();			mapy1.release();
	mapx2.release();			mapy2.release();

	SetOutputParam();
}


// ===========================================================================================


void CMy140405cameracalibrationcDlg::CreateChessboardObjectPoints(Size boardSize, vector<vector<Point3f>>& objectPoints, int nThView)  {
	int squareSize = 1; // inch	
	for (int i = 0; i < boardSize.height; i++)
		for (int j = 0; j < boardSize.width; j++)
			objectPoints[nThView].push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
}


void CMy140405cameracalibrationcDlg::CreateAsymmetricCirclesObjectPoints(Size boardSize, vector<vector<Point3f>>& objectPoints, int nThView)  {
	//int squareSize = 1; // inch	
	int squareSize = 0.85;
	for (int i = 0; i < boardSize.height; i++)
		for (int j = 0; j < boardSize.width; j++)
			objectPoints[nThView].push_back(Point3f(float((2*j + i%2)*squareSize), float(i*squareSize), 0));
}


bool CMy140405cameracalibrationcDlg::DetectChessboardCorners(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName) {	
	vector<Point2f>& corners = imagePoints[nThView];
	//bool isFound = findChessboardCorners(img, boardSize, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	bool isFound = findChessboardCorners(img, boardSize, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
	
	if (isFound) {
		// find corners in subpixel
		Mat imgGray;
		cvtColor(img, imgGray, COLOR_BGR2GRAY);
		cornerSubPix(imgGray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		imgGray.release();

		// display corner detection result
		if (isDisplayResult) {
			drawChessboardCorners(img, boardSize, corners, isFound);
			imshow(displayName, img);
		}
	}
	return isFound;
}


bool CMy140405cameracalibrationcDlg::DetectSymmetricCircles(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName) {
	vector<Point2f>& circles = imagePoints[nThView];

	Mat imgGray;
	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	SimpleBlobDetector::Params params;
	params.maxArea = 1000;
	params.minArea = 100;
	params.minDistBetweenBlobs = 7;

	Ptr<FeatureDetector> blobDetector = new SimpleBlobDetector(params);

	bool isFound = findCirclesGrid(imgGray, boardSize, circles, CALIB_CB_SYMMETRIC_GRID, blobDetector);

	imgGray.release();


	// display corner detection result
	if (isDisplayResult && isFound) {
		drawChessboardCorners(img, boardSize, circles, isFound);
		imshow(displayName, img);
	}
	return isFound;
}


bool CMy140405cameracalibrationcDlg::DetectAsymmetricCircles(Mat img, Size boardSize, vector<vector<Point2f>>& imagePoints, int nThView, bool isDisplayResult, char* displayName) {
	vector<Point2f>& circles = imagePoints[nThView];

	Mat imgGray;
	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	SimpleBlobDetector::Params params;
	params.maxArea = 1000;
	params.minArea = 100;
	params.minDistBetweenBlobs = 7;

	Ptr<FeatureDetector> blobDetector = new SimpleBlobDetector(params);

	bool isFound = findCirclesGrid(imgGray, boardSize, circles, CALIB_CB_ASYMMETRIC_GRID, blobDetector); // need opencv_features2d248.lib
	//bool isFound = findCirclesGrid(imgGray, boardSize, circles, CALIB_CB_CLUSTERING, blobDetector);

	imgGray.release();
	

	// display corner detection result
	if (isDisplayResult && isFound) {
		drawChessboardCorners(img, boardSize, circles, isFound);
		imshow(displayName, img);
	}
	return isFound;
}


void CMy140405cameracalibrationcDlg::DetectPointsInImages(Mat img1, Mat img2, int nThView) {
	Size boardSize = Size(boardWidth, boardHeight);	

	if (calibrationPattern == CALIBRATION_PATTERN_CHESSBOARD) {
		CreateChessboardObjectPoints(boardSize, objectPoints, nThView);

		bool isFound = DetectChessboardCorners(img1, boardSize, imagePoints1, nThView, isDisplayCornerResult, WINDOW_LEFT_IMG);
		if (isFound)
			DetectChessboardCorners(img2, boardSize, imagePoints2, nThView, isDisplayCornerResult, WINDOW_RIGHT_IMG);
		else
			MessageBox(L"Please specify correct numbers of chessboard column and row!");
	}
	else 
	{
		if (calibrationPattern == CALIBRATION_PATTERN_SYMMETRIC_CIRCLE_GRID) {
			CreateChessboardObjectPoints(boardSize, objectPoints, nThView);

			bool isFound = DetectSymmetricCircles(img1, boardSize, imagePoints1, nThView, isDisplayCornerResult, WINDOW_LEFT_IMG);
			if (isFound)
				DetectSymmetricCircles(img2, boardSize, imagePoints2, nThView, isDisplayCornerResult, WINDOW_RIGHT_IMG);
			else
				MessageBox(L"Please specify correct numbers of circles in the symmetric grid!");
		}
		else
		{
			if (calibrationPattern == CALIBRATION_PATTERN_ASYMMETRIC_CIRCLE_GRID) {
				CreateAsymmetricCirclesObjectPoints(boardSize, objectPoints, nThView);

				bool isFound = DetectAsymmetricCircles(img1, boardSize, imagePoints1, nThView, isDisplayCornerResult, WINDOW_LEFT_IMG);
				if (isFound)
					DetectAsymmetricCircles(img2, boardSize, imagePoints2, nThView, isDisplayCornerResult, WINDOW_RIGHT_IMG);
				else
					MessageBox(L"Please specify correct numbers of circles in the asymmetric grid!");
			}
		}
	}
}


double CMy140405cameracalibrationcDlg::CheckCalibrationQuality(Mat intrinsic1, Mat distortion1, Mat intrinsic2, Mat distortion2, Mat F) {
	// cuz the output fundamental matrix implicitly includes all the output information, 
	// we can check the quality of calibration using the epipolar geometry constraint: intrinsic2^t * F * intrinsic1 = 0
	double totalError = 0;
	int nPointsInBoard = boardWidth * boardHeight;

	for (int i = 0; i < nViews; i++)
	{
		// left img
		vector<Vec3f> epiline1;
		Mat imgPoint1 = Mat(imagePoints1[i]);
		undistortPoints(imgPoint1, imgPoint1, intrinsic1, distortion1, Mat(), intrinsic1);
		computeCorrespondEpilines(imgPoint1, 1, F, epiline1);

		// right img
		vector<Vec3f> epiline2;
		Mat imgPoint2 = Mat(imagePoints2[i]);
		undistortPoints(imgPoint2, imgPoint2, intrinsic2, distortion2, Mat(), intrinsic2);
		computeCorrespondEpilines(imgPoint2, 2, F, epiline2);

		// calculate error
		for (int j = 0; j < nPointsInBoard; j++) {
			Point2f point1 = imagePoints1[i][j];
			Point2f point2 = imagePoints2[i][j];
			totalError += fabs(point1.x*epiline2[j][0] + point1.y*epiline2[j][1] + epiline2[j][2]) +	// = point1 x epiline2 = (1x3) x (3x1)
						  fabs(point2.x*epiline1[j][0] + point2.y*epiline1[j][1] + epiline1[j][2]);		// = point2 x epiline1 = (1x3) x (3x1)
		}
	}
	return totalError / (nPointsInBoard * nViews);
}


void CMy140405cameracalibrationcDlg::SaveCalibrationResult(Mat intrinsic, Mat distortion, Mat R, Mat P, Mat Q, string filename) {
	FileStorage fs(filename, FileStorage::WRITE);
	fs << "nViews"		<< nViews;
	fs << "intrinsic"	<< intrinsic;
	fs << "distortion"	<< distortion;
	fs << "rotation"	<< R;
	fs << "projection"	<< P;
	fs << "Q"			<< Q;
	fs.release();
}


// ===========================================================================================


void CMy140405cameracalibrationcDlg::InitVariables() {	
	calibrationPattern = CALIBRATION_PATTERN_CHESSBOARD; // <--- modify the default board here
	if (calibrationPattern == CALIBRATION_PATTERN_CHESSBOARD) {
		boardWidth	= CHESSBOARD_WIDTH;
		boardHeight = CHESSBOARD_HEIGHT;
	}
	else if (calibrationPattern == CALIBRATION_PATTERN_SYMMETRIC_CIRCLE_GRID) {
		boardWidth	= SYMMETRIC_CIRCLE_GRID_WIDTH;
		boardHeight = SYMMETRIC_CIRCLE_GRID_HEIGHT;
	}
	else if (calibrationPattern == CALIBRATION_PATTERN_ASYMMETRIC_CIRCLE_GRID) {
		boardWidth	= ASYMMETRIC_CIRCLE_GRID_WIDTH;
		boardHeight = ASYMMETRIC_CIRCLE_GRID_HEIGHT;
	}	

	isDisplayCornerResult	= IS_DISPLAY_CORNER_RESULT;
	isSaveCalibrationResult = IS_SAVE_CALIBRATION_RESULT;
	isSaveRectifiedImg		= IS_SAVE_RECTIFIED_IMG;
}


void CMy140405cameracalibrationcDlg::InitGUI() {
	if (calibrationPattern == CALIBRATION_PATTERN_CHESSBOARD) 						((CButton*)this->GetDlgItem(IDC_RADIO_USE_CHESSBOARD))->SetCheck(true);
	else if (calibrationPattern == CALIBRATION_PATTERN_SYMMETRIC_CIRCLE_GRID) 		((CButton*)this->GetDlgItem(IDC_RADIO_USE_SYMMETRIC_CIRLCE_GRID))->SetCheck(true);
	else if (calibrationPattern == CALIBRATION_PATTERN_ASYMMETRIC_CIRCLE_GRID) 		((CButton*)this->GetDlgItem(IDC_RADIO_USE_ASYMMETRIC_CIRCLE_GRID))->SetCheck(true);
	CString s;
	s.Format(L"%2d", boardWidth);		this->GetDlgItem(IDC_EDIT_BOARD_WIDTH)->SetWindowTextW((LPCTSTR)s);
	s.Format(L"%2d", boardHeight);		this->GetDlgItem(IDC_EDIT_BOARD_HEIGHT)->SetWindowTextW((LPCTSTR)s);

	((CButton*)this->GetDlgItem(IDC_CHECK_IS_DISPLAY_CORNER_RESULT))->SetCheck(IS_DISPLAY_CORNER_RESULT);
	((CButton*)this->GetDlgItem(IDC_CHECK_IS_SAVE_CALIBRATION_RESULT))->SetCheck(IS_SAVE_CALIBRATION_RESULT);	
	((CButton*)this->GetDlgItem(IDC_CHECK_IS_SAVE_RECTIFIED_IMG))->SetCheck(IS_SAVE_RECTIFIED_IMG);
}


void CMy140405cameracalibrationcDlg::GetInputParam() {
	CString strBoardWidth, strBoardHeight;
	this->GetDlgItem(IDC_EDIT_BOARD_WIDTH)->GetWindowTextW(strBoardWidth);
	this->GetDlgItem(IDC_EDIT_BOARD_HEIGHT)->GetWindowTextW(strBoardHeight);
	if (!strBoardWidth.IsEmpty())		boardWidth = (int)wcstod(strBoardWidth, NULL);
	if (!strBoardHeight.IsEmpty())		boardHeight = (int)wcstod(strBoardHeight, NULL);

	if (((CButton*)this->GetDlgItem(IDC_RADIO_USE_CHESSBOARD))->GetCheck() == 1 ? true : false)
		calibrationPattern = CALIBRATION_PATTERN_CHESSBOARD;
	else if (((CButton*)this->GetDlgItem(IDC_RADIO_USE_SYMMETRIC_CIRLCE_GRID))->GetCheck() == 1 ? true : false)
		calibrationPattern = CALIBRATION_PATTERN_SYMMETRIC_CIRCLE_GRID;
	else if (((CButton*)this->GetDlgItem(IDC_RADIO_USE_ASYMMETRIC_CIRCLE_GRID))->GetCheck() == 1 ? true : false)
		calibrationPattern = CALIBRATION_PATTERN_ASYMMETRIC_CIRCLE_GRID;

	isDisplayCornerResult	= (((CButton*)this->GetDlgItem(IDC_CHECK_IS_DISPLAY_CORNER_RESULT))->GetCheck() == 1 ? true : false);
	isSaveCalibrationResult = (((CButton*)this->GetDlgItem(IDC_CHECK_IS_SAVE_CALIBRATION_RESULT))->GetCheck() == 1 ? true : false);
	isSaveRectifiedImg		= (((CButton*)this->GetDlgItem(IDC_CHECK_IS_SAVE_RECTIFIED_IMG))->GetCheck() == 1 ? true : false);
}


void CMy140405cameracalibrationcDlg::SetOutputParam() {
	CString s;

	s.Format(L"%.3f", reprojectionError);		this->GetDlgItem(IDC_STATIC_REPROJECTION_ERROR)->SetWindowTextW((LPCTSTR)s);
}