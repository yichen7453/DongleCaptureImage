// DongleCaptureImage.cpp: 主要專案檔。

#include "stdafx.h"
#include "GUI.h"

using namespace DongleCaptureImage;

[STAThreadAttribute]
int main(cli::array<System::String ^> ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew GUI());

	return 0;
}
