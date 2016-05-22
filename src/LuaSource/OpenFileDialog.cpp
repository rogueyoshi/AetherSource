#include "OpenFileDialog.h"

OpenFileDialog::OpenFileDialog() :
	DefaultExtension(NULL),
	Filter(NULL),
	FilterIndex(NULL),
	Flags(OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST),
	InitialDir(NULL),
	Owner(NULL),
	Title(NULL)
{
	FileName = new TCHAR[MAX_PATH];
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
