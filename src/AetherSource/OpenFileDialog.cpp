#include "OpenFileDialog.h"

OpenFileDialog::OpenFileDialog() :
	DefaultExtension(nullptr),
	Filter(nullptr),
	FilterIndex(NULL),
	Flags(OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST),
	InitialDir(nullptr),
	Owner(nullptr),
	Title(nullptr)
{
	FileName = new TCHAR[MAX_PATH];
	ZeroMemory(FileName, sizeof(FileName));
}

OpenFileDialog::~OpenFileDialog()
{
	delete FileName;
}

bool OpenFileDialog::ShowDialog()
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Owner;
	ofn.lpstrDefExt = DefaultExtension;
	ofn.lpstrFile = FileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = FilterIndex;
	ofn.lpstrInitialDir = InitialDir;
	ofn.lpstrTitle = Title;
	ofn.Flags = Flags;

	GetOpenFileName(&ofn);

	if (!_tcslen(FileName)) return false;

	return true;
}
