#pragma once

#include "winbio_ctl.h"
#include "setupapi_ctl.h"
#include "winsrv_ctl.h"

#define FRAME_WIDTH         128
#define FRAME_HEIGHT        128
#define FRAME_SIZE          (FRAME_WIDTH * FRAME_HEIGHT)

#define Synaptic_FRAME_WIDTH  144
#define Synaptic_FRAME_HEIGHT 56
#define Synaptic_FRAME_SIZE   (Synaptic_FRAME_WIDTH * Synaptic_FRAME_HEIGHT)

static DEVICE_DATA deviceData;

namespace DongleCaptureImage {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;

	using namespace std;

	// driver level
	static PWINBIO_CAPTURE_DATA data;

	// service level
	static WINBIO_UNIT_ID unitId = NULL;
	static PWINBIO_BIR sample = NULL;
	static SIZE_T sampleSize = 0;
	static WINBIO_REJECT_DETAIL rejectDetail = 0;

	// image file control
	static vector<string> fileNameVector = vector<string>();
	static vector<string> filePathVector = vector<string>();

	/// <summary>
	/// GUI 的摘要
	/// </summary>
	public ref class GUI : public System::Windows::Forms::Form
	{
	public:
		GUI(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//
			image_path = Application::StartupPath + "\\Fingerprint Image\\";
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~GUI()
		{
			if (components)
			{
				delete components;
			}
		}
	protected:
	private: System::Windows::Forms::Label^  lbListViewFileName;
	private: System::Windows::Forms::PictureBox^  pbListViewImage;
	private: System::Windows::Forms::ListView^  listView;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::CheckBox^  cbAutoSave;
	private: System::Windows::Forms::Label^  lbFileName;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::PictureBox^  pb_image;
	private: System::Windows::Forms::Label^  lbMessage;
	private: System::Windows::Forms::Button^  btnCaptureImage;
	private: System::Windows::Forms::CheckBox^  cbDriverLevel;
	private: System::Windows::Forms::CheckBox^  cbServiceLevel;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker;

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		System::ComponentModel::Container ^components;

		System::String^ image_path;

		bool stopFlag;
		bool autoSave;

		int listViewItemNumber = 0;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->lbListViewFileName = (gcnew System::Windows::Forms::Label());
			this->pbListViewImage = (gcnew System::Windows::Forms::PictureBox());
			this->listView = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->cbAutoSave = (gcnew System::Windows::Forms::CheckBox());
			this->lbFileName = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->pb_image = (gcnew System::Windows::Forms::PictureBox());
			this->lbMessage = (gcnew System::Windows::Forms::Label());
			this->btnCaptureImage = (gcnew System::Windows::Forms::Button());
			this->cbDriverLevel = (gcnew System::Windows::Forms::CheckBox());
			this->cbServiceLevel = (gcnew System::Windows::Forms::CheckBox());
			this->backgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbListViewImage))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->BeginInit();
			this->SuspendLayout();
			// 
			// lbListViewFileName
			// 
			this->lbListViewFileName->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lbListViewFileName->ForeColor = System::Drawing::Color::Red;
			this->lbListViewFileName->Location = System::Drawing::Point(647, 262);
			this->lbListViewFileName->Name = L"lbListViewFileName";
			this->lbListViewFileName->Size = System::Drawing::Size(251, 23);
			this->lbListViewFileName->TabIndex = 24;
			this->lbListViewFileName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// pbListViewImage
			// 
			this->pbListViewImage->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pbListViewImage->Location = System::Drawing::Point(689, 46);
			this->pbListViewImage->Name = L"pbListViewImage";
			this->pbListViewImage->Size = System::Drawing::Size(170, 170);
			this->pbListViewImage->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbListViewImage->TabIndex = 15;
			this->pbListViewImage->TabStop = false;
			// 
			// listView
			// 
			this->listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(1) { this->columnHeader1 });
			this->listView->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->listView->FullRowSelect = true;
			this->listView->GridLines = true;
			this->listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->listView->Location = System::Drawing::Point(404, 13);
			this->listView->Name = L"listView";
			this->listView->Size = System::Drawing::Size(237, 340);
			this->listView->TabIndex = 23;
			this->listView->UseCompatibleStateImageBehavior = false;
			this->listView->View = System::Windows::Forms::View::Details;
			this->listView->SelectedIndexChanged += gcnew System::EventHandler(this, &GUI::lv_SelectedIndexChanged);
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"File Name";
			this->columnHeader1->Width = 245;
			// 
			// label1
			// 
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label1->Location = System::Drawing::Point(393, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(1, 362);
			this->label1->TabIndex = 22;
			this->label1->Text = L"label1";
			// 
			// cbAutoSave
			// 
			this->cbAutoSave->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cbAutoSave->Location = System::Drawing::Point(193, 329);
			this->cbAutoSave->Name = L"cbAutoSave";
			this->cbAutoSave->Size = System::Drawing::Size(187, 24);
			this->cbAutoSave->TabIndex = 21;
			this->cbAutoSave->Text = L"Auto Save Image";
			this->cbAutoSave->UseVisualStyleBackColor = true;
			this->cbAutoSave->CheckedChanged += gcnew System::EventHandler(this, &GUI::autoSave_checkedChanged);
			// 
			// lbFileName
			// 
			this->lbFileName->BackColor = System::Drawing::Color::White;
			this->lbFileName->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lbFileName->ForeColor = System::Drawing::Color::Red;
			this->lbFileName->Location = System::Drawing::Point(17, 91);
			this->lbFileName->Name = L"lbFileName";
			this->lbFileName->Size = System::Drawing::Size(360, 23);
			this->lbFileName->TabIndex = 20;
			this->lbFileName->Text = L"File Name : ";
			this->lbFileName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lbFileName->Visible = false;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->pb_image);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->groupBox1->Location = System::Drawing::Point(182, 124);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(200, 199);
			this->groupBox1->TabIndex = 18;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Image";
			// 
			// pb_image
			// 
			this->pb_image->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pb_image->Location = System::Drawing::Point(15, 21);
			this->pb_image->Name = L"pb_image";
			this->pb_image->Size = System::Drawing::Size(170, 170);
			this->pb_image->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_image->TabIndex = 2;
			this->pb_image->TabStop = false;
			// 
			// lbMessage
			// 
			this->lbMessage->BackColor = System::Drawing::Color::White;
			this->lbMessage->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbMessage->Font = (gcnew System::Drawing::Font(L"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lbMessage->Location = System::Drawing::Point(12, 13);
			this->lbMessage->Name = L"lbMessage";
			this->lbMessage->Size = System::Drawing::Size(370, 105);
			this->lbMessage->TabIndex = 17;
			this->lbMessage->Text = L"Welcome use capture image tool";
			this->lbMessage->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnCaptureImage
			// 
			this->btnCaptureImage->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnCaptureImage->Location = System::Drawing::Point(12, 259);
			this->btnCaptureImage->Name = L"btnCaptureImage";
			this->btnCaptureImage->Size = System::Drawing::Size(156, 56);
			this->btnCaptureImage->TabIndex = 14;
			this->btnCaptureImage->Text = L"Capture Image";
			this->btnCaptureImage->UseVisualStyleBackColor = true;
			this->btnCaptureImage->Click += gcnew System::EventHandler(this, &GUI::btnCaptureImage_Click);
			// 
			// cbDriverLevel
			// 
			this->cbDriverLevel->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cbDriverLevel->Location = System::Drawing::Point(20, 153);
			this->cbDriverLevel->Name = L"cbDriverLevel";
			this->cbDriverLevel->Size = System::Drawing::Size(147, 24);
			this->cbDriverLevel->TabIndex = 25;
			this->cbDriverLevel->Text = L"Driver  Level";
			this->cbDriverLevel->UseVisualStyleBackColor = true;
			this->cbDriverLevel->CheckedChanged += gcnew System::EventHandler(this, &GUI::driver_checkedChanged);
			// 
			// cbServiceLevel
			// 
			this->cbServiceLevel->Font = (gcnew System::Drawing::Font(L"Courier New", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cbServiceLevel->Location = System::Drawing::Point(20, 194);
			this->cbServiceLevel->Name = L"cbServiceLevel";
			this->cbServiceLevel->Size = System::Drawing::Size(147, 33);
			this->cbServiceLevel->TabIndex = 26;
			this->cbServiceLevel->Text = L"Service Level";
			this->cbServiceLevel->UseVisualStyleBackColor = true;
			this->cbServiceLevel->CheckedChanged += gcnew System::EventHandler(this, &GUI::service_checkedChanged);
			// 
			// backgroundWorker
			// 
			this->backgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &GUI::backgroundWorker_DoWork);
			this->backgroundWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &GUI::backgroundWorker_Completed);
			// 
			// GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(905, 363);
			this->Controls->Add(this->cbServiceLevel);
			this->Controls->Add(this->cbDriverLevel);
			this->Controls->Add(this->lbListViewFileName);
			this->Controls->Add(this->pbListViewImage);
			this->Controls->Add(this->listView);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cbAutoSave);
			this->Controls->Add(this->lbFileName);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->lbMessage);
			this->Controls->Add(this->btnCaptureImage);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"DongleCaptureImage (1.0)";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GUI::GUI_Closed);
			this->Load += gcnew System::EventHandler(this, &GUI::GUI_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbListViewImage))->EndInit();
			this->groupBox1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void GUI_Load(System::Object^  sender, System::EventArgs^  e) {
				 backgroundWorker->WorkerSupportsCancellation = true;

				 cbServiceLevel->Checked = true;

				 list_files();

				 checkListViewItem(image_path);
	}

	private: System::Void btnCaptureImage_Click(System::Object^  sender, System::EventArgs^  e) {
				 //printf("\n --> btnCaptureImage_Click() <-- \n");

				 HRESULT hr = S_OK;
				 BOOL	 noDevice;

				 SIZE_T unitCount = NULL;

				 if (btnCaptureImage->Text == "Capture Image") {

					 hr = RetrieveDevicePath(deviceData.DevicePath, sizeof(deviceData.DevicePath), &noDevice);

					 if (FAILED(hr)) {
						 printf("\n Unalbed to find any devices\n");
						 lbMessage->Text = "Unabled to find any devices.";
					 }
					 else {
						 if (cbDriverLevel->Checked) {
							 doStopSrv();

							 hr = OpenDevice(&deviceData, &noDevice);

							 if (FAILED(hr)) {
								 lbMessage->Text = "Unabled to find any devices.";
							 }
							 else {
								 btnCaptureImage->Text = "Stop";

								 pb_image->Image = nullptr;

								 CaptureSampleInit(deviceData.DeviceHandle);

								 if (autoSave) {
									 lbMessage->Text = "Press finger to capture and \nsave image";
								 }
								 else {
									 lbMessage->Text = "Press finger to capture image";
									 lbFileName->Visible = false;
								 }

								 backgroundWorker->RunWorkerAsync();
								 
								 cbDriverLevel->Enabled = false;
								 cbServiceLevel->Enabled = false;
							 }
						 }

						 if (cbServiceLevel->Checked) {
							 unitCount = EnumBiometricUnits();
							 printf("\n unitCount (%d)\n", (int)unitCount);

							 if (unitCount < 0) {
								 lbMessage->Text = "Unabled to find any devices.";
							 }
							 else {
								 btnCaptureImage->Text = "Stop";

								 pb_image->Image = nullptr;

								 if (autoSave) {
									 lbMessage->Text = "Press finger to capture and \nsave image";
								 }
								 else {
									 lbMessage->Text = "Press finger to capture image";
									 lbFileName->Visible = false;
								 }

								 OpenSesseion();

								 backgroundWorker->RunWorkerAsync();

								 cbDriverLevel->Enabled = false;
								 cbServiceLevel->Enabled = false;
							 }
						 }

						 stopFlag = false;
						 cbAutoSave->Enabled = false;
					 }
				 }
				 else {
					 btnCaptureImage->Text = "Capture Image";
					 backgroundWorker->CancelAsync();

					 if (cbDriverLevel->Checked) {
						 Reset(deviceData.DeviceHandle);
						 CloseDevice(&deviceData);
					 }

					 if (cbServiceLevel->Checked) {
						 CancelSession();

						 if (sample != NULL) {
							 FreeAddress(sample);
							 sample = NULL;
						 }
					 }

					 stopFlag = true;
					 cbAutoSave->Enabled = true;
					 cbDriverLevel->Enabled = true;
					 cbServiceLevel->Enabled = true;
				 }
	}

	private: System::Void backgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				 //printf("\n --> backgroundWorker_DoWork() <-- \n");
				 if (cbDriverLevel->Checked) {
					 CaptureSampleFromDriver(deviceData.DeviceHandle, &data);
				 }

				 if (cbServiceLevel->Checked) {
					 CaptureSampleFromService(&unitId, &sample, &sampleSize, &rejectDetail);
				 }
	}
	private: System::Void backgroundWorker_Completed(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
				 //printf("\n --> backgroundWorker_Completed() <-- \n");
				 HRESULT hr = S_OK;
				 BOOL	 noDevice;

				 if (cbDriverLevel->Checked) {
					 UCHAR newData[20000];
					 hr = RetrieveDevicePath(deviceData.DevicePath, sizeof(deviceData.DevicePath), &noDevice);

					 if (FAILED(hr)) {
						 lbMessage->Text = "Unabled to find any devices.";

						 btnCaptureImage->Text = "Capture Image";
						 cbAutoSave->Enabled = true;
						 cbDriverLevel->Enabled = true;
						 cbServiceLevel->Enabled = true;
					 }
					 else if (stopFlag) {
						 lbMessage->Text = "Welcome use capture image tool";
						 btnCaptureImage->Text = "Capture Image";
						 cbAutoSave->Enabled = true;
						 cbDriverLevel->Enabled = true;
						 cbServiceLevel->Enabled = true;
					 }
					 else {
						 if (data != NULL && !stopFlag) {
							 printf("\n data size: %u\n", data->PayloadSize);

							 if (data->PayloadSize.ToString() == "16408") {
								 for (int i = 0; i < FRAME_SIZE; i++) {
									 newData[i] = data->CaptureData.Data[i];
								 }

								 cli::array<Byte>^ byteArray = gcnew cli::array<Byte>(FRAME_SIZE);
								 Marshal::Copy((IntPtr)newData, byteArray, 0, FRAME_SIZE);

								 pb_image->Image = ToGrayBitmap(byteArray, FRAME_WIDTH, FRAME_HEIGHT);
							 }
							 else if (data->PayloadSize.ToString() == "16544") {
								 for (int i = 0; i < FRAME_SIZE; i++) {
									 newData[i] = data->CaptureData.Data[i + 136];
								 }

								 cli::array<Byte>^ byteArray = gcnew cli::array<Byte>(FRAME_SIZE);
								 Marshal::Copy((IntPtr)newData, byteArray, 0, FRAME_SIZE);
								 pb_image->Image = ToGrayBitmap(byteArray, FRAME_WIDTH, FRAME_HEIGHT);
							 }
							 else if (data->PayloadSize.ToString() == "8567") {
								 for (int i = 0; i < Synaptic_FRAME_SIZE; i++) {
									 newData[i] = data->CaptureData.Data[i + 136];
								 }
								 cli::array<Byte>^ byteArray = gcnew cli::array<Byte>(Synaptic_FRAME_SIZE);
								 Marshal::Copy((IntPtr)newData, byteArray, 0, Synaptic_FRAME_SIZE);
								 pb_image->Image = ToGrayBitmap(byteArray, Synaptic_FRAME_WIDTH, Synaptic_FRAME_HEIGHT);
							 }

							 if (autoSave)  {
								 saveImage();
							 }
							 backgroundWorker->RunWorkerAsync();
						 }
						 else {
							 CloseDevice(&deviceData);
						 }
					 }
				 }

				 if (cbServiceLevel->Checked) {
					 if (sample != NULL) {
						 //PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset);
						 PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset);
						 PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER));

						 DWORD width = AnsiBdbRecord->HorizontalLineLength; // Width of image in pixels
						 DWORD height = AnsiBdbRecord->VerticalLineLength; // Height of image in pixels

						 //printf("\n Image resolution: %d x %d\n", width, height);

						 PBYTE firstPixel = (PBYTE)((PBYTE)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD);

						 std::vector<uint8_t> data(width * height);
						 memcpy(&data[0], firstPixel, width * height);

						 cli::array<Byte>^ byteArray = gcnew cli::array<Byte>(width * height);
						 Marshal::Copy((IntPtr)data.data(), byteArray, 0, width * height);

						 pb_image->Image = ToGrayBitmap(byteArray, width, height);

						 lbMessage->Text = "Capture image success";

						 if (autoSave)  
						 {
							 saveImage();
						 }
						 backgroundWorker->RunWorkerAsync();
					 }
					 else if (stopFlag) {
						 lbMessage->Text = "Welcome use capture image tool";
						 btnCaptureImage->Text = "Capture Image";
						 cbAutoSave->Enabled = TRUE;
						 cbDriverLevel->Enabled = TRUE;
						 cbServiceLevel->Enabled = TRUE;
					 }
					 else {
						 lbMessage->Text = "Unabled to find any devices.";
						 btnCaptureImage->Text = "Capture Image";
						 cbAutoSave->Enabled = TRUE;
						 cbDriverLevel->Enabled = TRUE;
						 cbServiceLevel->Enabled = TRUE;
					 }
				 }
	}

	public: System::Void saveImage() {
				listView->Focus();
				if (listViewItemNumber > 0) {
					listView->Items[listViewItemNumber - 1]->Focused = false;
					listView->Items[listViewItemNumber - 1]->Selected = false;
				}
				char dateTimeBuf[100];
				char string[200];

				SYSTEMTIME sys;
				GetLocalTime(&sys);
				//printf("\n %4d/%02d/%02d %02d:%02d:%02d.%01d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
				sprintf_s(dateTimeBuf, sizeof(dateTimeBuf), "%4d%02d%02d_%02d%02d%02d%.03d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

				strcpy_s(string, sizeof(string), (char*)(void*)Marshal::StringToHGlobalAnsi(image_path));
				strcat_s(string, sizeof(string), "\\");
				strcat_s(string, sizeof(string), dateTimeBuf);
				strcat_s(string, sizeof(string), ".png");

				pb_image->Image->Save(Marshal::PtrToStringAnsi((IntPtr)string), ImageFormat::Png);

				System::String^ fileName = Path::GetFileName(Marshal::PtrToStringAnsi((IntPtr)string));
				System::String^ filePath = Marshal::PtrToStringAnsi((IntPtr)string);

				listView->Items->Add(fileName);

				lbFileName->Visible = true;
				lbFileName->Text = "File name: " + fileName;

				IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(fileName);
				char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

				IntPtr ptrToNativeFilePathString = Marshal::StringToHGlobalAnsi(filePath);
				char* nativeFilePathString = static_cast<char*>(ptrToNativeFilePathString.ToPointer());

				fileNameVector.push_back(nativeFileNameString);
				filePathVector.push_back(nativeFilePathString);

				listViewItemNumber = listViewItemNumber + 1;

				listView->Items[listViewItemNumber - 1]->Focused = true;
				listView->Items[listViewItemNumber - 1]->Selected = true;
				listView->Items[listViewItemNumber - 1]->EnsureVisible();
	}

	public: static Bitmap^ ToGrayBitmap(cli::array<Byte>^ rawValues, int width, int height) {
				Bitmap^ bmp = gcnew Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
				BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);

				int stride = bmpData->Stride;
				int offset = stride - width;
				IntPtr iptr = bmpData->Scan0;
				int scanBytes = stride * height;

				int posScan = 0, posReal = 0;

				cli::array<Byte>^ pixelValues = gcnew cli::array<Byte>(scanBytes);

				for (int x = 0; x < height; x++) {
					for (int y = 0; y < width; y++) {
						pixelValues[posScan++] = rawValues[posReal++];
					}
					posScan += offset;
				}

				Marshal::Copy(pixelValues, 0, iptr, scanBytes);
				bmp->UnlockBits(bmpData);


				ColorPalette^ temPalette;

				Bitmap^ tempBmp = gcnew Bitmap(1, 1, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				temPalette = tempBmp->Palette;

				for (int i = 0; i < 256; i++) {
					temPalette->Entries[i] = Color::FromArgb(i, i, i);
				}

				bmp->Palette = temPalette;

				return bmp;
	}

	private: void checkListViewItem(System::String^ path)
	{
				 std::vector<string> FILENAME = vector<string>();
				 std::vector<string> FILEPATH = vector<string>();

				 if (System::IO::Directory::Exists(path)) {
					 cli::array<System::String^>^ file = Directory::GetFiles(path);
					 cli::array<System::DateTime^>^ dateTime = gcnew cli::array<System::DateTime^>(file->Length);

					 if (file->Length > 0) {
						 listView->Focus();
						 /*
						 for (int x = 0; x < file->Length; x++)
						 {
						 FileInfo^ sfi = gcnew FileInfo(file->GetValue(x)->ToString());
						 dateTime[x] = sfi->LastWriteTime;
						 Console::WriteLine("name : " + sfi->Name);
						 Console::WriteLine("time : " + dateTime[x]);
						 }

						 Array::Sort(dateTime, file);
						 */

						 for (int i = 0; i < file->Length; i++) {

							 System::String^ file_name = Path::GetFileName(file[i]);
							 System::String^ file_path = file[i];

							 listView->Items->Add(file_name);

							 IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(file_name);
							 char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

							 IntPtr ptrToNativeFilePathString = Marshal::StringToHGlobalAnsi(file_path);
							 char* nativeFilePathString = static_cast<char*>(ptrToNativeFilePathString.ToPointer());

							 fileNameVector.push_back(nativeFileNameString);
							 filePathVector.push_back(nativeFilePathString);
						 }
						 listViewItemNumber = file->Length;

						 listView->Items[listViewItemNumber - 1]->Focused = true;
						 listView->Items[listViewItemNumber - 1]->Selected = true;
						 listView->Items[listViewItemNumber - 1]->EnsureVisible();
					 }
				 }
	}

	private: cli::array<System::String^>^ list_files() {
				 System::String^ DS_file = ".DS_Store";

				 if (System::IO::Directory::Exists(image_path)) {
					 cli::array<System::String^>^ file = Directory::GetFiles(image_path);
					 cli::array<System::String^>^ fileList = gcnew cli::array<System::String^>(file->Length);

					 for (int i = 0; i < file->Length; i++) {
						 if (DS_file == Path::GetFileName(file[i])) {
							 //printf("\n delete file name => %s\n", file[i]);
							 remove((char*)(void*)Marshal::StringToHGlobalAnsi(file[i]));
						 }
						 else {
							 fileList[i] = Path::GetFileName(file[i]);
						 }
					 }
					 printf("\n file length => %d\n", file->Length);
					 return fileList;
				 }
				 else {
					 return nullptr;
				 }
	}

	private: System::Void lv_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (listView->SelectedItems->Count > 0) {
					 for (int i = 0; i < listViewItemNumber; i++) {
						 if (listView->Items[i]->Selected == true) {
							 System::String^ strName = gcnew System::String(fileNameVector[i].c_str());
							 System::String^ strPath = gcnew System::String(filePathVector[i].c_str());

							 pbListViewImage->Image = Image::FromFile(strPath);
							 lbListViewFileName->Text = strName;
						 }
					 }
				 }
	}

	private: System::Void driver_checkedChanged(System::Object^  sender, System::EventArgs^  e) {
			 
				 if (cbDriverLevel->CheckState == CheckState::Checked) {
					 cbServiceLevel->Checked = FALSE;
				 }
				 else {
					 cbServiceLevel->Checked = TRUE;
				 }
	}

	private: System::Void service_checkedChanged(System::Object^  sender, System::EventArgs^  e) {
				
				 if (cbServiceLevel->CheckState == CheckState::Checked) {
					 cbDriverLevel->Checked = FALSE;
				 }
				 else {
					 cbDriverLevel->Checked = TRUE;
				 }
	}

	private: System::Void autoSave_checkedChanged(System::Object^  sender, System::EventArgs^  e) {
				 printf("\n --> autoSave_checkedChanged() <-- \n");

				 if (cbAutoSave->CheckState == CheckState::Checked) {
					 autoSave = true;
					 if (!System::IO::Directory::Exists(image_path)) {
						 System::IO::Directory::CreateDirectory(image_path);
					 }
				 }
				 else {
					 autoSave = false;
				 }
	}

	private: System::Void GUI_Closed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 if (cbDriverLevel->Checked) {
					 Reset(deviceData.DeviceHandle);
				 }

				 if (cbServiceLevel->Checked) {
					 CancelSession();

					 if (sample != NULL) {
						 FreeAddress(sample);
						 sample = NULL;
					 }
				 }
	}
};
}
